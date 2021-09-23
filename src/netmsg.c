/*!
 * netmsg.c - network messages for libsatoshi
 * Copyright (c) 2021, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/libsatoshi
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <satoshi/block.h>
#include <satoshi/header.h>
#include <satoshi/netaddr.h>
#include <satoshi/netmsg.h>
#include <satoshi/net.h>
#include <satoshi/tx.h>
#include <satoshi/util.h>
#include <satoshi/vector.h>
#include "impl.h"
#include "internal.h"

/*
 * Version
 */

DEFINE_SERIALIZABLE_OBJECT(btc_version, SCOPE_EXTERN)

void
btc_version_init(btc_version_t *msg) {
  msg->version = BTC_NET_PROTOCOL_VERSION;
  msg->services = BTC_NET_LOCAL_SERVICES;
  msg->time = btc_now();
  btc_netaddr_init(&msg->remote);
  btc_netaddr_init(&msg->local);
  msg->nonce = 0;
  memset(msg->agent, 0, sizeof(msg->agent));
  strcpy(msg->agent, BTC_NET_USER_AGENT);
  msg->height = 0;
  msg->no_relay = 0;
}

void
btc_version_clear(btc_version_t *msg) {
  btc_netaddr_clear(&msg->remote);
  btc_netaddr_clear(&msg->local);
}

void
btc_version_copy(btc_version_t *z, const btc_version_t *x) {
  z->version = x->version;
  z->services = x->services;
  z->time = x->time;
  btc_netaddr_copy(&z->remote, &x->remote);
  btc_netaddr_copy(&z->local, &x->local);
  z->nonce = x->nonce;
  strcpy(z->agent, x->agent);
  z->height = x->height;
  z->no_relay = x->no_relay;
}

size_t
btc_version_size(const btc_version_t *x) {
  size_t size = 0;
  size += 20;
  size += btc_smalladdr_size(&x->remote);
  size += btc_smalladdr_size(&x->local);
  size += 8;
  size += btc_string_size(x->agent);
  size += 5;
  return size;
}

uint8_t *
btc_version_write(uint8_t *zp, const btc_version_t *x) {
  zp = btc_int32_write(zp, x->version);
  zp = btc_uint64_write(zp, x->services);
  zp = btc_int64_write(zp, x->time);
  zp = btc_smalladdr_write(zp, &x->remote);
  zp = btc_smalladdr_write(zp, &x->local);
  zp = btc_uint64_write(zp, x->nonce);
  zp = btc_string_write(zp, x->agent);
  zp = btc_int32_write(zp, x->height);
  zp = btc_uint8_write(zp, x->no_relay);
  return zp;
}

int
btc_version_read(btc_version_t *z, const uint8_t **xp, size_t *xn) {
  if (!btc_int32_read(&z->version, xp, xn))
    return 0;

  if (!btc_uint64_read(&z->services, xp, xn))
    return 0;

  if (!btc_int64_read(&z->time, xp, xn))
    return 0;

  if (!btc_smalladdr_read(&z->remote, xp, xn))
    return 0;

  if (*xn > 0) {
    if (!btc_smalladdr_read(&z->local, xp, xn))
      return 0;

    if (!btc_uint64_read(&z->nonce, xp, xn))
      return 0;
  } else {
    btc_netaddr_init(&z->local);
    z->nonce = 0;
  }

  if (*xn > 0) {
    if (!btc_string_read(z->agent, xp, xn, sizeof(z->agent)))
      return 0;
  } else {
    memset(z->agent, 0, sizeof(z->agent));
  }

  if (*xn > 0) {
    if (!btc_int32_read(&z->height, xp, xn))
      return 0;
  } else {
    z->height = 0;
  }

  if (*xn > 0) {
    if (!btc_uint8_read(&z->no_relay, xp, xn))
      return 0;
  } else {
    z->no_relay = 0;
  }

  if (z->version == 10300)
    z->version = 300;

  if (z->version < 0)
    return 0;

  if (z->time < 0)
    return 0;

  if (z->height < 0)
    z->height = 0;

  return 1;
}

/*
 * Verack
 */

/* empty message */

/*
 * Ping
 */

DEFINE_SERIALIZABLE_OBJECT(btc_ping, SCOPE_EXTERN)

void
btc_ping_init(btc_ping_t *msg) {
  msg->nonce = 0;
}

void
btc_ping_clear(btc_ping_t *msg) {
  (void)msg;
}

void
btc_ping_copy(btc_ping_t *z, const btc_ping_t *x) {
  *z = *x;
}

size_t
btc_ping_size(const btc_ping_t *x) {
  return x->nonce != 0 ? 8 : 0;
}

uint8_t *
btc_ping_write(uint8_t *zp, const btc_ping_t *x) {
  if (x->nonce != 0)
    zp = btc_uint64_write(zp, x->nonce);
  return zp;
}

int
btc_ping_read(btc_ping_t *z, const uint8_t **xp, size_t *xn) {
  z->nonce = 0;

  if (*xn > 0) {
    if (!btc_uint64_read(&z->nonce, xp, xn))
      return 0;
  }

  return 1;
}

/*
 * Pong
 */

DEFINE_SERIALIZABLE_OBJECT(btc_pong, SCOPE_EXTERN)

void
btc_pong_init(btc_pong_t *msg) {
  msg->nonce = 0;
}

void
btc_pong_clear(btc_pong_t *msg) {
  (void)msg;
}

void
btc_pong_copy(btc_pong_t *z, const btc_pong_t *x) {
  *z = *x;
}

size_t
btc_pong_size(const btc_pong_t *x) {
  (void)x;
  return 8;
}

uint8_t *
btc_pong_write(uint8_t *zp, const btc_pong_t *x) {
  return btc_uint64_write(zp, x->nonce);
}

int
btc_pong_read(btc_pong_t *z, const uint8_t **xp, size_t *xn) {
  return btc_uint64_read(&z->nonce, xp, xn);
}

/*
 * GetAddr
 */

/* empty message */

/*
 * Addr
 */

DEFINE_SERIALIZABLE_VECTOR(btc_addrs, btc_netaddr, SCOPE_EXTERN)

/*
 * Inv Item
 */

DEFINE_SERIALIZABLE_OBJECT(btc_invitem, SCOPE_EXTERN)

void
btc_invitem_init(btc_invitem_t *item) {
  item->type = 0;
  btc_hash_init(item->hash);
}

void
btc_invitem_clear(btc_invitem_t *item) {
  (void)item;
}

void
btc_invitem_copy(btc_invitem_t *z, const btc_invitem_t *x) {
  *z = *x;
}

void
btc_invitem_set(btc_invitem_t *z, uint32_t type, const uint8_t *hash) {
  z->type = type;
  btc_hash_copy(z->hash, hash);
}

size_t
btc_invitem_size(const btc_invitem_t *x) {
  (void)x;
  return 36;
}

uint8_t *
btc_invitem_write(uint8_t *zp, const btc_invitem_t *x) {
  zp = btc_uint32_write(zp, x->type);
  zp = btc_raw_write(zp, x->hash, 32);
  return zp;
}

int
btc_invitem_read(btc_invitem_t *z, const uint8_t **xp, size_t *xn) {
  if (!btc_uint32_read(&z->type, xp, xn))
    return 0;

  if (!btc_raw_read(z->hash, 32, xp, xn))
    return 0;

  return 1;
}

/*
 * Inv
 */

/* TODO: Limit at BTC_NET_MAX_INV. */
DEFINE_SERIALIZABLE_VECTOR(btc_inv, btc_invitem, SCOPE_EXTERN)

/*
 * GetData
 */

/* inherits btc_inv_t */

/*
 * NotFound
 */

/* inherits btc_inv_t */

/*
 * GetBlocks
 */

DEFINE_SERIALIZABLE_OBJECT(btc_getblocks, SCOPE_EXTERN)

void
btc_getblocks_init(btc_getblocks_t *msg) {
  msg->version = 0;
  btc_vector_init(&msg->locator);
  btc_hash_init(msg->stop);
}

void
btc_getblocks_uninit(btc_getblocks_t *msg) {
  /* TODO: Maybe add to uninit and free to vector prototype in impl.h */
  btc_vector_clear(&msg->locator);
}

void
btc_getblocks_clear(btc_getblocks_t *msg) {
  size_t i;

  for (i = 0; i < msg->locator.length; i++)
    btc_free(msg->locator.items[i]);

  btc_vector_clear(&msg->locator);
}

void
btc_getblocks_copy(btc_getblocks_t *z, const btc_getblocks_t *x) {
  z->version = x->version;
  btc_vector_copy(&z->locator, &x->locator);
  btc_hash_copy(z->stop, x->stop);
}

size_t
btc_getblocks_size(const btc_getblocks_t *x) {
  size_t size = 0;
  size += 4;
  size += btc_size_size(x->locator.length);
  size += 32 * x->locator.length;
  size += 32;
  return size;
}

uint8_t *
btc_getblocks_write(uint8_t *zp, const btc_getblocks_t *x) {
  size_t i;

  zp = btc_uint32_write(zp, x->version);
  zp = btc_size_write(zp, x->locator.length);

  for (i = 0; i < x->locator.length; i++)
    zp = btc_raw_write(zp, (uint8_t *)x->locator.items[i], 32);

  zp = btc_raw_write(zp, x->stop, 32);

  return zp;
}

int
btc_getblocks_read(btc_getblocks_t *z, const uint8_t **xp, size_t *xn) {
  size_t i, len;

  if (!btc_uint32_read(&z->version, xp, xn))
    return 0;

  if (!btc_size_read(&len, xp, xn))
    return 0;

  if (len > BTC_NET_MAX_INV)
    return 0;

  CHECK(z->locator.length == 0);

  for (i = 0; i < len; i++) {
    uint8_t *hash = (uint8_t *)btc_malloc(32);

    if (!btc_raw_read(hash, 32, xp, xn)) {
      btc_free(hash);
      return 0;
    }

    btc_vector_push(&z->locator, hash);
  }

  if (!btc_raw_read(z->stop, 32, xp, xn))
    return 0;

  return 1;
}

/*
 * GetHeaders
 */

/* inherits btc_getblocks_t */

/*
 * Hdr
 */

typedef btc_header_t btc_hdr_t;

static btc_header_t *
btc_hdr_create(void) {
  return btc_header_create();
}

static void
btc_hdr_destroy(btc_header_t *hdr) {
  btc_header_destroy(hdr);
}

static btc_header_t *
btc_hdr_clone(btc_header_t *hdr) {
  return btc_header_clone(hdr);
}

static size_t
btc_hdr_size(const btc_header_t *x) {
  return btc_header_size(x) + btc_size_size(0);
}

static uint8_t *
btc_hdr_write(uint8_t *zp, const btc_header_t *x) {
  zp = btc_header_write(zp, x);
  zp = btc_size_write(zp, 0);
  return zp;
}

static int
btc_hdr_read(btc_header_t *z, const uint8_t **xp, size_t *xn) {
  size_t len;

  if (!btc_header_read(z, xp, xn))
    return 0;

  if (!btc_size_read(&len, xp, xn))
    return 0;

  return 1;
}

/*
 * Headers
 */

/* TODO: Limit at 2000. */
DEFINE_SERIALIZABLE_VECTOR(btc_headers, btc_hdr, SCOPE_EXTERN)

/*
 * SendHeaders
 */

/* empty message */

/*
 * Block
 */

/* already implemented */

/*
 * TX
 */

/* already implemented */

/*
 * Reject
 */

DEFINE_SERIALIZABLE_OBJECT(btc_reject, SCOPE_EXTERN)

void
btc_reject_init(btc_reject_t *msg) {
  memset(msg->message, 0, sizeof(msg->message));

  msg->code = BTC_REJECT_INVALID;

  memset(msg->reason, 0, sizeof(msg->reason));

  btc_hash_init(msg->hash);
}

void
btc_reject_clear(btc_reject_t *msg) {
  (void)msg;
}

void
btc_reject_copy(btc_reject_t *z, const btc_reject_t *x) {
  *z = *x;
}

void
btc_reject_set_code(btc_reject_t *z, const char *code) {
  if (strcmp(code, "malformed") == 0)
    z->code = BTC_REJECT_MALFORMED;
  else if (strcmp(code, "invalid") == 0)
    z->code = BTC_REJECT_INVALID;
  else if (strcmp(code, "obsolete") == 0)
    z->code = BTC_REJECT_OBSOLETE;
  else if (strcmp(code, "duplicate") == 0)
    z->code = BTC_REJECT_DUPLICATE;
  else if (strcmp(code, "nonstandard") == 0)
    z->code = BTC_REJECT_NONSTANDARD;
  else if (strcmp(code, "dust") == 0)
    z->code = BTC_REJECT_DUST;
  else if (strcmp(code, "insufficientfee") == 0)
    z->code = BTC_REJECT_INSUFFICIENTFEE;
  else if (strcmp(code, "checkpoint") == 0)
    z->code = BTC_REJECT_CHECKPOINT;
  else
    z->code = BTC_REJECT_INVALID;
}

const char *
btc_reject_get_code(const btc_reject_t *x) {
  switch (x->code) {
    case BTC_REJECT_MALFORMED:
      return "malformed";
    case BTC_REJECT_INVALID:
      return "invalid";
    case BTC_REJECT_OBSOLETE:
      return "obsolete";
    case BTC_REJECT_DUPLICATE:
      return "duplicate";
    case BTC_REJECT_NONSTANDARD:
      return "nonstandard";
    case BTC_REJECT_DUST:
      return "dust";
    case BTC_REJECT_INSUFFICIENTFEE:
      return "insufficientfee";
    case BTC_REJECT_CHECKPOINT:
      return "checkpoint";
    default:
      return "invalid";
  }
}

size_t
btc_reject_size(const btc_reject_t *x) {
  size_t size = 0;

  size += btc_string_size(x->message);
  size += 1;
  size += btc_string_size(x->reason);

  if (!btc_hash_is_null(x->hash))
    size += 32;

  return size;
}

uint8_t *
btc_reject_write(uint8_t *zp, const btc_reject_t *x) {
  zp = btc_string_write(zp, x->message);
  zp = btc_uint8_write(zp, x->code);
  zp = btc_string_write(zp, x->reason);

  if (!btc_hash_is_null(x->hash))
    zp = btc_raw_write(zp, x->hash, 32);

  return zp;
}

int
btc_reject_read(btc_reject_t *z, const uint8_t **xp, size_t *xn) {
  if (!btc_string_read(z->message, xp, xn, sizeof(z->message)))
    return 0;

  if (!btc_uint8_read(&z->code, xp, xn))
    return 0;

  if (!btc_string_read(z->reason, xp, xn, sizeof(z->reason)))
    return 0;

  if (strcmp(z->message, "block") == 0 || strcmp(z->message, "tx") == 0) {
    if (!btc_raw_read(z->hash, 32, xp, xn))
      return 0;
  } else {
    btc_hash_init(z->hash);
  }

  return 1;
}

/*
 * Mempool
 */

/* empty message */

/*
 * FilterLoad
 */

/* TODO */

/*
 * FilterAdd
 */

DEFINE_SERIALIZABLE_OBJECT(btc_filteradd, SCOPE_EXTERN)

void
btc_filteradd_init(btc_filteradd_t *msg) {
  memset(msg->data, 0, sizeof(msg->data));
  msg->length = 0;
}

void
btc_filteradd_clear(btc_filteradd_t *msg) {
  msg->length = 0;
}

void
btc_filteradd_copy(btc_filteradd_t *z, const btc_filteradd_t *x) {
  *z = *x;
}

size_t
btc_filteradd_size(const btc_filteradd_t *x) {
  return btc_size_size(x->length) + x->length;
}

uint8_t *
btc_filteradd_write(uint8_t *zp, const btc_filteradd_t *x) {
  zp = btc_size_write(zp, x->length);
  zp = btc_raw_write(zp, x->data, x->length);
  return zp;
}

int
btc_filteradd_read(btc_filteradd_t *z, const uint8_t **xp, size_t *xn) {
  if (!btc_size_read(&z->length, xp, xn))
    return 0;

  if (z->length > 256)
    return 0;

  if (!btc_raw_read(z->data, z->length, xp, xn))
    return 0;

  return 1;
}

/*
 * FilterClear
 */

/* empty message */

/*
 * MerkleBlock
 */

/* TODO */

/*
 * FeeFilter
 */

DEFINE_SERIALIZABLE_OBJECT(btc_feefilter, SCOPE_EXTERN)

void
btc_feefilter_init(btc_feefilter_t *msg) {
  msg->rate = 0;
}

void
btc_feefilter_clear(btc_feefilter_t *msg) {
  msg->rate = 0;
}

void
btc_feefilter_copy(btc_feefilter_t *z, const btc_feefilter_t *x) {
  z->rate = x->rate;
}

size_t
btc_feefilter_size(const btc_feefilter_t *x) {
  (void)x;
  return 8;
}

uint8_t *
btc_feefilter_write(uint8_t *zp, const btc_feefilter_t *x) {
  return btc_int64_write(zp, x->rate);
}

int
btc_feefilter_read(btc_feefilter_t *z, const uint8_t **xp, size_t *xn) {
  return btc_int64_read(&z->rate, xp, xn);
}

/*
 * SendCmpct
 */

DEFINE_SERIALIZABLE_OBJECT(btc_sendcmpct, SCOPE_EXTERN)

void
btc_sendcmpct_init(btc_sendcmpct_t *msg) {
  msg->mode = 0;
  msg->version = 1;
}

void
btc_sendcmpct_clear(btc_sendcmpct_t *msg) {
  (void)msg;
}

void
btc_sendcmpct_copy(btc_sendcmpct_t *z, const btc_sendcmpct_t *x) {
  *z = *x;
}

size_t
btc_sendcmpct_size(const btc_sendcmpct_t *x) {
  (void)x;
  return 9;
}

uint8_t *
btc_sendcmpct_write(uint8_t *zp, const btc_sendcmpct_t *x) {
  zp = btc_uint8_write(zp, x->mode);
  zp = btc_uint64_write(zp, x->version);
  return zp;
}

int
btc_sendcmpct_read(btc_sendcmpct_t *z, const uint8_t **xp, size_t *xn) {
  if (!btc_uint8_read(&z->mode, xp, xn))
    return 0;

  if (!btc_uint64_read(&z->version, xp, xn))
    return 0;

  return 1;
}

/*
 * CmpctBlock
 */

/* TODO */

/*
 * GetBlockTxn
 */

/* TODO */

/*
 * BlockTxn
 */

/* TODO */

/*
 * Unknown
 */

DEFINE_SERIALIZABLE_OBJECT(btc_unknown, SCOPE_EXTERN)

void
btc_unknown_init(btc_unknown_t *msg) {
  msg->data = NULL;
  msg->length = 0;
}

void
btc_unknown_clear(btc_unknown_t *msg) {
  if (msg->data != NULL)
    btc_free(msg->data);

  msg->data = NULL;
  msg->length = 0;
}

void
btc_unknown_copy(btc_unknown_t *z, const btc_unknown_t *x) {
  z->data = (uint8_t *)btc_malloc(x->length);
  memcpy(z->data, x->data, x->length);
  z->length = x->length;
}

size_t
btc_unknown_size(const btc_unknown_t *x) {
  return x->length;
}

uint8_t *
btc_unknown_write(uint8_t *zp, const btc_unknown_t *x) {
  return btc_raw_write(zp, x->data, x->length);
}

int
btc_unknown_read(btc_unknown_t *z, const uint8_t **xp, size_t *xn) {
  if (z->data != NULL)
    btc_free(z->data);

  if (*xn > 0) {
    z->data = (uint8_t *)btc_malloc(*xn);
    z->length = *xn;

    memcpy(z->data, *xp, *xn);
  } else {
    z->data = NULL;
    z->length = 0;
  }

  *xp += *xn;
  *xn = 0;

  return 1;
}

/*
 * Message
 */

DEFINE_SERIALIZABLE_OBJECT(btc_msg, SCOPE_EXTERN)

void
btc_msg_init(btc_msg_t *msg) {
  msg->type = BTC_MSG_INTERNAL;
  memset(msg->cmd, 0, sizeof(msg->cmd));
  msg->body = NULL;
}

void
btc_msg_clear(btc_msg_t *msg) {
  if (msg->body == NULL)
    return;

  switch (msg->type) {
    case BTC_MSG_VERSION:
      btc_version_clear((btc_version_t *)msg->body);
      break;
    case BTC_MSG_VERACK:
      msg->body = NULL;
      break;
    case BTC_MSG_PING:
      btc_ping_clear((btc_ping_t *)msg->body);
      break;
    case BTC_MSG_PONG:
      btc_pong_clear((btc_pong_t *)msg->body);
      break;
    case BTC_MSG_GETADDR:
      msg->body = NULL;
      break;
    case BTC_MSG_ADDR:
      btc_addrs_clear((btc_addrs_t *)msg->body);
      break;
    case BTC_MSG_INV:
      btc_inv_clear((btc_inv_t *)msg->body);
      break;
    case BTC_MSG_GETDATA:
      btc_inv_clear((btc_getdata_t *)msg->body);
      break;
    case BTC_MSG_NOTFOUND:
      btc_inv_clear((btc_notfound_t *)msg->body);
      break;
    case BTC_MSG_GETBLOCKS:
      btc_getblocks_clear((btc_getblocks_t *)msg->body);
      break;
    case BTC_MSG_GETHEADERS:
      btc_getblocks_clear((btc_getheaders_t *)msg->body);
      break;
    case BTC_MSG_HEADERS:
      btc_headers_clear((btc_headers_t *)msg->body);
      break;
    case BTC_MSG_SENDHEADERS:
      msg->body = NULL;
      break;
    case BTC_MSG_BLOCK:
      btc_block_clear((btc_block_t *)msg->body);
      break;
    case BTC_MSG_TX:
      btc_tx_clear((btc_tx_t *)msg->body);
      break;
    case BTC_MSG_REJECT:
      btc_reject_clear((btc_reject_t *)msg->body);
      break;
    case BTC_MSG_MEMPOOL:
      msg->body = NULL;
      break;
    case BTC_MSG_FILTERLOAD:
      /* btc_bloom_clear((btc_bloom_t *)msg->body); */
      break;
    case BTC_MSG_FILTERADD:
      btc_filteradd_clear((btc_filteradd_t *)msg->body);
      break;
    case BTC_MSG_FILTERCLEAR:
      msg->body = NULL;
      break;
    case BTC_MSG_MERKLEBLOCK:
      /* btc_merkleblock_clear((btc_merkleblock_t *)msg->body); */
      break;
    case BTC_MSG_FEEFILTER:
      btc_feefilter_clear((btc_feefilter_t *)msg->body);
      break;
    case BTC_MSG_SENDCMPCT:
      btc_sendcmpct_clear((btc_sendcmpct_t *)msg->body);
      break;
    case BTC_MSG_CMPCTBLOCK:
      /* btc_cmpctblock_clear((btc_cmpctblock_t *)msg->body); */
      break;
    case BTC_MSG_GETBLOCKTXN:
      /* btc_getblocktxn_clear((btc_getblocktxn_t *)msg->body); */
      break;
    case BTC_MSG_BLOCKTXN:
      /* btc_blocktxn_clear((btc_blocktxn_t *)msg->body); */
      break;
    case BTC_MSG_UNKNOWN:
      btc_unknown_clear((btc_unknown_t *)msg->body);
      break;
    default:
      msg->body = NULL;
      break;
  }
}

void
btc_msg_copy(btc_msg_t *z, const btc_msg_t *x) {
  *z = *x;
}

void
btc_msg_set_type(btc_msg_t *msg, enum btc_msgtype type) {
  const char *cmd;

  msg->type = type;

  switch (type) {
    case BTC_MSG_VERSION:
      cmd = "version";
      break;
    case BTC_MSG_VERACK:
      cmd = "verack";
      break;
    case BTC_MSG_PING:
      cmd = "ping";
      break;
    case BTC_MSG_PONG:
      cmd = "pong";
      break;
    case BTC_MSG_GETADDR:
      cmd = "getaddr";
      break;
    case BTC_MSG_ADDR:
      cmd = "addr";
      break;
    case BTC_MSG_INV:
      cmd = "inv";
      break;
    case BTC_MSG_GETDATA:
      cmd = "getdata";
      break;
    case BTC_MSG_NOTFOUND:
      cmd = "notfound";
      break;
    case BTC_MSG_GETBLOCKS:
      cmd = "getblocks";
      break;
    case BTC_MSG_GETHEADERS:
      cmd = "getheaders";
      break;
    case BTC_MSG_HEADERS:
      cmd = "headers";
      break;
    case BTC_MSG_SENDHEADERS:
      cmd = "sendheaders";
      break;
    case BTC_MSG_BLOCK:
      cmd = "block";
      break;
    case BTC_MSG_TX:
      cmd = "tx";
      break;
    case BTC_MSG_REJECT:
      cmd = "reject";
      break;
    case BTC_MSG_MEMPOOL:
      cmd = "mempool";
      break;
    case BTC_MSG_FILTERLOAD:
      cmd = "filterload";
      break;
    case BTC_MSG_FILTERADD:
      cmd = "filteradd";
      break;
    case BTC_MSG_FILTERCLEAR:
      cmd = "filterclear";
      break;
    case BTC_MSG_MERKLEBLOCK:
      cmd = "merkleblock";
      break;
    case BTC_MSG_FEEFILTER:
      cmd = "feefilter";
      break;
    case BTC_MSG_SENDCMPCT:
      cmd = "sendcmpct";
      break;
    case BTC_MSG_CMPCTBLOCK:
      cmd = "cmpctblock";
      break;
    case BTC_MSG_GETBLOCKTXN:
      cmd = "getblocktxn";
      break;
    case BTC_MSG_BLOCKTXN:
      cmd = "blocktxn";
      break;
    default:
      cmd = "unknown";
      break;
  }

  CHECK(strlen(cmd) <= sizeof(msg->cmd) - 1);

  strcpy(msg->cmd, cmd);
}

void
btc_msg_set_cmd(btc_msg_t *msg, const char *cmd) {
  if (strcmp(cmd, "version") == 0)
    msg->type = BTC_MSG_VERSION;
  else if (strcmp(cmd, "verack") == 0)
    msg->type = BTC_MSG_VERACK;
  else if (strcmp(cmd, "ping") == 0)
    msg->type = BTC_MSG_PING;
  else if (strcmp(cmd, "pong") == 0)
    msg->type = BTC_MSG_PONG;
  else if (strcmp(cmd, "getaddr") == 0)
    msg->type = BTC_MSG_GETADDR;
  else if (strcmp(cmd, "addr") == 0)
    msg->type = BTC_MSG_ADDR;
  else if (strcmp(cmd, "inv") == 0)
    msg->type = BTC_MSG_INV;
  else if (strcmp(cmd, "getdata") == 0)
    msg->type = BTC_MSG_GETDATA;
  else if (strcmp(cmd, "notfound") == 0)
    msg->type = BTC_MSG_NOTFOUND;
  else if (strcmp(cmd, "getblocks") == 0)
    msg->type = BTC_MSG_GETBLOCKS;
  else if (strcmp(cmd, "getheaders") == 0)
    msg->type = BTC_MSG_GETHEADERS;
  else if (strcmp(cmd, "headers") == 0)
    msg->type = BTC_MSG_HEADERS;
  else if (strcmp(cmd, "sendheaders") == 0)
    msg->type = BTC_MSG_SENDHEADERS;
  else if (strcmp(cmd, "block") == 0)
    msg->type = BTC_MSG_BLOCK;
  else if (strcmp(cmd, "tx") == 0)
    msg->type = BTC_MSG_TX;
  else if (strcmp(cmd, "reject") == 0)
    msg->type = BTC_MSG_REJECT;
  else if (strcmp(cmd, "mempool") == 0)
    msg->type = BTC_MSG_MEMPOOL;
  else if (strcmp(cmd, "filterload") == 0)
    msg->type = BTC_MSG_FILTERLOAD;
  else if (strcmp(cmd, "filteradd") == 0)
    msg->type = BTC_MSG_FILTERADD;
  else if (strcmp(cmd, "filterclear") == 0)
    msg->type = BTC_MSG_FILTERCLEAR;
  else if (strcmp(cmd, "merkleblock") == 0)
    msg->type = BTC_MSG_MERKLEBLOCK;
  else if (strcmp(cmd, "feefilter") == 0)
    msg->type = BTC_MSG_FEEFILTER;
  else if (strcmp(cmd, "sendcmpct") == 0)
    msg->type = BTC_MSG_SENDCMPCT;
  else if (strcmp(cmd, "cmpctblock") == 0)
    msg->type = BTC_MSG_CMPCTBLOCK;
  else if (strcmp(cmd, "getblocktxn") == 0)
    msg->type = BTC_MSG_GETBLOCKTXN;
  else if (strcmp(cmd, "blocktxn") == 0)
    msg->type = BTC_MSG_BLOCKTXN;
  else
    msg->type = BTC_MSG_UNKNOWN;

  CHECK(strlen(cmd) <= sizeof(msg->cmd) - 1);

  strcpy(msg->cmd, cmd);
}

void
btc_msg_alloc(btc_msg_t *msg) {
  switch (msg->type) {
    case BTC_MSG_VERSION:
      msg->body = btc_version_create();
      break;
    case BTC_MSG_VERACK:
      msg->body = NULL;
      break;
    case BTC_MSG_PING:
      msg->body = btc_ping_create();
      break;
    case BTC_MSG_PONG:
      msg->body = btc_pong_create();
      break;
    case BTC_MSG_GETADDR:
      msg->body = NULL;
      break;
    case BTC_MSG_ADDR:
      msg->body = btc_addrs_create();
      break;
    case BTC_MSG_INV:
      msg->body = btc_inv_create();
      break;
    case BTC_MSG_GETDATA:
      msg->body = btc_inv_create();
      break;
    case BTC_MSG_NOTFOUND:
      msg->body = btc_inv_create();
      break;
    case BTC_MSG_GETBLOCKS:
      msg->body = btc_getblocks_create();
      break;
    case BTC_MSG_GETHEADERS:
      msg->body = btc_getblocks_create();
      break;
    case BTC_MSG_HEADERS:
      msg->body = btc_headers_create();
      break;
    case BTC_MSG_SENDHEADERS:
      msg->body = NULL;
      break;
    case BTC_MSG_BLOCK:
      msg->body = btc_block_create();
      break;
    case BTC_MSG_TX:
      msg->body = btc_tx_create();
      break;
    case BTC_MSG_REJECT:
      msg->body = btc_reject_create();
      break;
    case BTC_MSG_MEMPOOL:
      msg->body = NULL;
      break;
    case BTC_MSG_FILTERLOAD:
      /* msg->body = btc_bloom_create(); */
      break;
    case BTC_MSG_FILTERADD:
      msg->body = btc_filteradd_create();
      break;
    case BTC_MSG_FILTERCLEAR:
      msg->body = NULL;
      break;
    case BTC_MSG_MERKLEBLOCK:
      /* msg->body = btc_merkleblock_create(); */
      break;
    case BTC_MSG_FEEFILTER:
      msg->body = btc_feefilter_create();
      break;
    case BTC_MSG_SENDCMPCT:
      msg->body = btc_sendcmpct_create();
      break;
    case BTC_MSG_CMPCTBLOCK:
      /* msg->body = btc_cmpctblock_create(); */
      break;
    case BTC_MSG_GETBLOCKTXN:
      /* msg->body = btc_getblocktxn_create(); */
      break;
    case BTC_MSG_BLOCKTXN:
      /* msg->body = btc_blocktxn_create(); */
      break;
    case BTC_MSG_UNKNOWN:
      msg->body = btc_unknown_create();
      break;
    default:
      msg->body = NULL;
      break;
  }
}

size_t
btc_msg_size(const btc_msg_t *x) {
  switch (x->type) {
    case BTC_MSG_VERSION:
      return btc_version_size((btc_version_t *)x->body);
    case BTC_MSG_VERACK:
      return 0;
    case BTC_MSG_PING:
      return btc_ping_size((btc_ping_t *)x->body);
    case BTC_MSG_PONG:
      return btc_pong_size((btc_pong_t *)x->body);
    case BTC_MSG_GETADDR:
      return 0;
    case BTC_MSG_ADDR:
      return btc_addrs_size((btc_addrs_t *)x->body);
    case BTC_MSG_INV:
      return btc_inv_size((btc_inv_t *)x->body);
    case BTC_MSG_GETDATA:
      return btc_inv_size((btc_getdata_t *)x->body);
    case BTC_MSG_NOTFOUND:
      return btc_inv_size((btc_notfound_t *)x->body);
    case BTC_MSG_GETBLOCKS:
      return btc_getblocks_size((btc_getblocks_t *)x->body);
    case BTC_MSG_GETHEADERS:
      return btc_getblocks_size((btc_getheaders_t *)x->body);
    case BTC_MSG_HEADERS:
      return btc_headers_size((btc_headers_t *)x->body);
    case BTC_MSG_SENDHEADERS:
      return 0;
    case BTC_MSG_BLOCK:
      return btc_block_size((btc_block_t *)x->body);
    case BTC_MSG_TX:
      return btc_tx_size((btc_tx_t *)x->body);
    case BTC_MSG_REJECT:
      return btc_reject_size((btc_reject_t *)x->body);
    case BTC_MSG_MEMPOOL:
      return 0;
    case BTC_MSG_FILTERLOAD:
      /* return btc_bloom_size((btc_bloom_t *)x->body); */
      return 0;
    case BTC_MSG_FILTERADD:
      return btc_filteradd_size((btc_filteradd_t *)x->body);
    case BTC_MSG_FILTERCLEAR:
      return 0;
    case BTC_MSG_MERKLEBLOCK:
      /* return btc_merkleblock_size((btc_merkleblock_t *)x->body); */
      return 0;
    case BTC_MSG_FEEFILTER:
      return btc_feefilter_size((btc_feefilter_t *)x->body);
    case BTC_MSG_SENDCMPCT:
      return btc_sendcmpct_size((btc_sendcmpct_t *)x->body);
    case BTC_MSG_CMPCTBLOCK:
      /* return btc_cmpctblock_size((btc_cmpctblock_t *)x->body); */
      return 0;
    case BTC_MSG_GETBLOCKTXN:
      /* return btc_getblocktxn_size((btc_getblocktxn_t *)x->body); */
      return 0;
    case BTC_MSG_BLOCKTXN:
      /* return btc_blocktxn_size((btc_blocktxn_t *)x->body); */
      return 0;
    case BTC_MSG_UNKNOWN:
      return btc_unknown_size((btc_unknown_t *)x->body);
    default:
      return 0;
  }
}

uint8_t *
btc_msg_write(uint8_t *zp, const btc_msg_t *x) {
  switch (x->type) {
    case BTC_MSG_VERSION:
      return btc_version_write(zp, (btc_version_t *)x->body);
    case BTC_MSG_VERACK:
      return zp;
    case BTC_MSG_PING:
      return btc_ping_write(zp, (btc_ping_t *)x->body);
    case BTC_MSG_PONG:
      return btc_pong_write(zp, (btc_pong_t *)x->body);
    case BTC_MSG_GETADDR:
      return zp;
    case BTC_MSG_ADDR:
      return btc_addrs_write(zp, (btc_addrs_t *)x->body);
    case BTC_MSG_INV:
      return btc_inv_write(zp, (btc_inv_t *)x->body);
    case BTC_MSG_GETDATA:
      return btc_inv_write(zp, (btc_getdata_t *)x->body);
    case BTC_MSG_NOTFOUND:
      return btc_inv_write(zp, (btc_notfound_t *)x->body);
    case BTC_MSG_GETBLOCKS:
      return btc_getblocks_write(zp, (btc_getblocks_t *)x->body);
    case BTC_MSG_GETHEADERS:
      return btc_getblocks_write(zp, (btc_getheaders_t *)x->body);
    case BTC_MSG_HEADERS:
      return btc_headers_write(zp, (btc_headers_t *)x->body);
    case BTC_MSG_SENDHEADERS:
      return zp;
    case BTC_MSG_BLOCK:
      return btc_block_write(zp, (btc_block_t *)x->body);
    case BTC_MSG_TX:
      return btc_tx_write(zp, (btc_tx_t *)x->body);
    case BTC_MSG_REJECT:
      return btc_reject_write(zp, (btc_reject_t *)x->body);
    case BTC_MSG_MEMPOOL:
      return zp;
    case BTC_MSG_FILTERLOAD:
      /* return btc_bloom_write(zp, (btc_bloom_t *)x->body); */
      return zp;
    case BTC_MSG_FILTERADD:
      return btc_filteradd_write(zp, (btc_filteradd_t *)x->body);
    case BTC_MSG_FILTERCLEAR:
      return zp;
    case BTC_MSG_MERKLEBLOCK:
      /* return btc_merkleblock_write(zp, (btc_merkleblock_t *)x->body); */
      return zp;
    case BTC_MSG_FEEFILTER:
      return btc_feefilter_write(zp, (btc_feefilter_t *)x->body);
    case BTC_MSG_SENDCMPCT:
      return btc_sendcmpct_write(zp, (btc_sendcmpct_t *)x->body);
    case BTC_MSG_CMPCTBLOCK:
      /* return btc_cmpctblock_write(zp, (btc_cmpctblock_t *)x->body); */
      return zp;
    case BTC_MSG_GETBLOCKTXN:
      /* return btc_getblocktxn_write(zp, (btc_getblocktxn_t *)x->body); */
      return zp;
    case BTC_MSG_BLOCKTXN:
      /* return btc_blocktxn_write(zp, (btc_blocktxn_t *)x->body); */
      return zp;
    case BTC_MSG_UNKNOWN:
      return btc_unknown_write(zp, (btc_unknown_t *)x->body);
    default:
      return zp;
  }
}

int
btc_msg_read(btc_msg_t *z, const uint8_t **xp, size_t *xn) {
  switch (z->type) {
    case BTC_MSG_VERSION:
      return btc_version_read((btc_version_t *)z->body, xp, xn);
    case BTC_MSG_VERACK:
      return 1;
    case BTC_MSG_PING:
      return btc_ping_read((btc_ping_t *)z->body, xp, xn);
    case BTC_MSG_PONG:
      return btc_pong_read((btc_pong_t *)z->body, xp, xn);
    case BTC_MSG_GETADDR:
      return 1;
    case BTC_MSG_ADDR:
      return btc_addrs_read((btc_addrs_t *)z->body, xp, xn);
    case BTC_MSG_INV:
      return btc_inv_read((btc_inv_t *)z->body, xp, xn);
    case BTC_MSG_GETDATA:
      return btc_inv_read((btc_getdata_t *)z->body, xp, xn);
    case BTC_MSG_NOTFOUND:
      return btc_inv_read((btc_notfound_t *)z->body, xp, xn);
    case BTC_MSG_GETBLOCKS:
      return btc_getblocks_read((btc_getblocks_t *)z->body, xp, xn);
    case BTC_MSG_GETHEADERS:
      return btc_getblocks_read((btc_getheaders_t *)z->body, xp, xn);
    case BTC_MSG_HEADERS:
      return btc_headers_read((btc_headers_t *)z->body, xp, xn);
    case BTC_MSG_SENDHEADERS:
      return 1;
    case BTC_MSG_BLOCK:
      return btc_block_read((btc_block_t *)z->body, xp, xn);
    case BTC_MSG_TX:
      return btc_tx_read((btc_tx_t *)z->body, xp, xn);
    case BTC_MSG_REJECT:
      return btc_reject_read((btc_reject_t *)z->body, xp, xn);
    case BTC_MSG_MEMPOOL:
      return 1;
    case BTC_MSG_FILTERLOAD:
      /* return btc_bloom_read((btc_bloom_t *)z->body, xp, xn); */
      return 1;
    case BTC_MSG_FILTERADD:
      return btc_filteradd_read((btc_filteradd_t *)z->body, xp, xn);
    case BTC_MSG_FILTERCLEAR:
      return 1;
    case BTC_MSG_MERKLEBLOCK:
      /* return btc_merkleblock_read((btc_merkleblock_t *)z->body, xp, xn); */
      return 1;
    case BTC_MSG_FEEFILTER:
      return btc_feefilter_read((btc_feefilter_t *)z->body, xp, xn);
    case BTC_MSG_SENDCMPCT:
      return btc_sendcmpct_read((btc_sendcmpct_t *)z->body, xp, xn);
    case BTC_MSG_CMPCTBLOCK:
      /* return btc_cmpctblock_read((btc_cmpctblock_t *)z->body, xp, xn); */
      return 1;
    case BTC_MSG_GETBLOCKTXN:
      /* return btc_getblocktxn_read((btc_getblocktxn_t *)z->body, xp, xn); */
      return 1;
    case BTC_MSG_BLOCKTXN:
      /* return btc_blocktxn_read((btc_blocktxn_t *)z->body, xp, xn); */
      return 1;
    case BTC_MSG_UNKNOWN:
      return btc_unknown_read((btc_unknown_t *)z->body, xp, xn);
    default:
      return 0;
  }
}
