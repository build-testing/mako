/*!
 * util.c - utils for libsatoshi
 * Copyright (c) 2021, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/libsatoshi
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#  include <windows.h> /* SecureZeroMemory */
#endif
#include <satoshi/util.h>
#include "internal.h"

/*
 * Memory Zero
 */

void
btc_memzero(void *ptr, size_t len) {
#if defined(_WIN32) && defined(SecureZeroMemory)
  if (len > 0)
    SecureZeroMemory(ptr, len);
#elif defined(BTC_HAVE_ASM)
  if (len > 0) {
    memset(ptr, 0, len);

    __asm__ __volatile__ (
      ""
      :: "r" (ptr)
      : "memory"
    );
  }
#else
  static void *(*volatile memset_ptr)(void *, int, size_t) = memset;

  if (len > 0)
    memset_ptr(ptr, 0, len);
#endif
}

/*
 * Memory Compare
 */

int
btc_memcmp(const void *x, const void *y, size_t n) {
  /* Exposing this function is necessary to avoid a
   * particularly nasty GCC bug[1][2].
   *
   * [1] https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95189
   * [2] https://github.com/bitcoin-core/secp256k1/issues/823
   */
  const unsigned char *xp = (const unsigned char *)x;
  const unsigned char *yp = (const unsigned char *)y;
  size_t i;

  for (i = 0; i < n; i++) {
    if (xp[i] != yp[i])
      return (int)xp[i] - (int)yp[i];
  }

  return 0;
}

/*
 * Memory Equal
 */

int
btc_memequal(const void *x, const void *y, size_t n) {
  const unsigned char *xp = (const unsigned char *)x;
  const unsigned char *yp = (const unsigned char *)y;
  uint32_t z = 0;

  while (n--)
    z |= *xp++ ^ *yp++;

  return (z - 1) >> 31;
}

/*
 * Memory XOR
 */

void
btc_memxor(void *z, const void *x, size_t n) {
  const unsigned char *xp = (const unsigned char *)x;
  unsigned char *zp = (unsigned char *)z;

  while (n--)
    *zp++ ^= *xp++;
}

void
btc_memxor3(void *z, const void *x, const void *y, size_t n) {
  const unsigned char *xp = (const unsigned char *)x;
  const unsigned char *yp = (const unsigned char *)y;
  unsigned char *zp = (unsigned char *)z;

  while (n--)
    *zp++ = *xp++ ^ *yp++;
}

/*
 * Helpers
 */

static BTC_INLINE int
char2nib(int ch) {
  if (ch >= '0' && ch <= '9')
    ch -= '0';
  else if (ch >= 'A' && ch <= 'F')
    ch -= 'A' - 10;
  else if (ch >= 'a' && ch <= 'f')
    ch -= 'a' - 10;
  else
    ch = 16;

  return ch;
}

static BTC_INLINE int
nib2char(int ch) {
  if (ch <= 9)
    ch += '0';
  else
    ch += 'a' - 10;

  return ch;
}

/*
 * Hash
 */

uint8_t *
btc_hash_clone(const uint8_t *xp) {
  return (uint8_t *)memcpy(btc_malloc(32), xp, 32);
}

int
btc_hash_compare(const uint8_t *xp, const uint8_t *yp) {
  int i;

  for (i = 32 - 1; i >= 0; i--) {
    if (xp[i] != yp[i])
      return (int)xp[i] - (int)yp[i];
  }

  return 0;
}

int
btc_hash_is_null(const uint8_t *xp) {
  static const uint8_t yp[32] = {0};
  return btc_hash_equal(xp, yp);
}

int
btc_hash_import(uint8_t *zp, const char *xp) {
  int i, hi, lo;
  int j = 32;

  for (i = 0; i < 64; i += 2) {
    hi = char2nib(xp[i + 0]);

    if (hi >= 16)
      return 0;

    lo = char2nib(xp[i + 1]);

    if (lo >= 16)
      return 0;

    zp[--j] = (hi << 4) | lo;
  }

  if (xp[64] != '\0')
    return 0;

  return 1;
}

void
btc_hash_export(char *zp, const uint8_t *yp) {
  int j = 0;
  int i;

  for (i = 31; i >= 0; i--) {
    zp[j++] = nib2char(yp[i] >> 4);
    zp[j++] = nib2char(yp[i] & 15);
  }

  zp[j] = '\0';
}

/*
 * Time
 */

int64_t
btc_now(void) {
  time_t now = time(NULL);

  CHECK(now != (time_t)-1);

  return (int64_t)now;
}
