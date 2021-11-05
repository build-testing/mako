/*!
 * testnet.c - testnet for libsatoshi
 * Copyright (c) 2021, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/libsatoshi
 */

#include <stddef.h>
#include <stdint.h>
#include <satoshi/network.h>
#include "internal.h"

static const char *testnet_seeds[] = {
  "testnet-seed.bitcoin.jonasschnelli.ch.", /* Jonas Schnelli */
  "seed.tbtc.petertodd.org.", /* Peter Todd */
  "seed.testnet.bitcoin.sprovoost.nl.", /* Sjors Provoost */
  "testnet-seed.bluematt.me." /* Matt Corallo */
};

static const btc_checkpoint_t testnet_checkpoints[] = {
  {
    546,
    {
      0x70, 0xcb, 0x6a, 0xf7, 0xeb, 0xbc, 0xb1, 0x31,
      0x5d, 0x34, 0x14, 0x02, 0x9c, 0x55, 0x6c, 0x55,
      0xf3, 0xe2, 0xfc, 0x35, 0x3c, 0x4c, 0x90, 0x63,
      0xa7, 0x6c, 0x93, 0x2a, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    10000,
    {
      0x02, 0xa1, 0xb4, 0x3f, 0x52, 0x59, 0x1e, 0x53,
      0xb6, 0x60, 0x06, 0x91, 0x73, 0xac, 0x83, 0xb6,
      0x75, 0x79, 0x8e, 0x12, 0x59, 0x9d, 0xbb, 0x04,
      0x42, 0xb7, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    50000,
    {
      0x0c, 0x6c, 0xea, 0xbe, 0x80, 0x3c, 0xec, 0x55,
      0xba, 0x28, 0x31, 0xe4, 0x45, 0x95, 0x6d, 0x0a,
      0x43, 0xba, 0x95, 0x21, 0x74, 0x3a, 0x80, 0x2c,
      0xdd, 0xac, 0x7e, 0x07, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    90000,
    {
      0xca, 0xfc, 0x21, 0xe1, 0x7f, 0xaf, 0x90, 0x46,
      0x1a, 0x59, 0x05, 0xaa, 0x03, 0x30, 0x2c, 0x39,
      0x49, 0x12, 0x65, 0x1e, 0xd9, 0x47, 0x5a, 0xe7,
      0x11, 0x72, 0x3e, 0x0d, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    100000,
    {
      0x1e, 0x0a, 0x16, 0xbb, 0xad, 0xcc, 0xde, 0x1d,
      0x80, 0xc6, 0x65, 0x97, 0xb1, 0x93, 0x9e, 0x45,
      0xf9, 0x1b, 0x57, 0x0d, 0x29, 0xf9, 0x5f, 0xc1,
      0x58, 0x29, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    140000,
    {
      0x92, 0xc0, 0x87, 0x7b, 0x54, 0xc5, 0x56, 0x88,
      0x9b, 0x72, 0x17, 0x5c, 0xcb, 0xe0, 0xc9, 0x1a,
      0x12, 0x08, 0xf6, 0xef, 0x7e, 0xfb, 0x2c, 0x00,
      0x61, 0x01, 0x06, 0x23, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    170000,
    {
      0x50, 0x81, 0x25, 0x56, 0x0d, 0x20, 0x2b, 0x89,
      0x75, 0x78, 0x89, 0xbb, 0x0e, 0x49, 0xc7, 0x12,
      0x47, 0x7b, 0xe2, 0x04, 0x40, 0x05, 0x8f, 0x05,
      0xdb, 0x4f, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    210000,
    {
      0x32, 0x36, 0x54, 0x54, 0xb5, 0xf2, 0x9a, 0x82,
      0x6b, 0xff, 0x8a, 0xd9, 0xb0, 0x44, 0x8c, 0xad,
      0x00, 0x72, 0xfc, 0x73, 0xd5, 0x0e, 0x48, 0x2d,
      0x91, 0xa3, 0xde, 0xce, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    230000,
    {
      0xb1, 0x1a, 0x44, 0x7e, 0x62, 0x64, 0x3e, 0x0b,
      0x27, 0x40, 0x6e, 0xb0, 0xfc, 0x27, 0x0c, 0xb8,
      0x12, 0x6d, 0x7b, 0x5b, 0x70, 0x82, 0x2f, 0xb6,
      0x42, 0xd9, 0x51, 0x34, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    270000,
    {
      0x1c, 0x42, 0xb8, 0x11, 0xcf, 0x9c, 0x16, 0x39,
      0x32, 0xf6, 0xe9, 0x5e, 0xc5, 0x5b, 0xf9, 0xb5,
      0xe2, 0xcb, 0x53, 0x24, 0xe7, 0xe9, 0x30, 0x01,
      0x57, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    300000,
    {
      0xa1, 0x41, 0xbf, 0x39, 0x72, 0x42, 0x48, 0x53,
      0xf0, 0x43, 0x67, 0xb4, 0x79, 0x95, 0xe2, 0x20,
      0xe0, 0xb5, 0xa2, 0x70, 0x6e, 0x56, 0x18, 0x76,
      0x6f, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    340000,
    {
      0x67, 0xed, 0xd4, 0xd9, 0x2e, 0x40, 0x56, 0x08,
      0x10, 0x91, 0x64, 0xb1, 0x5f, 0x92, 0xb1, 0x93,
      0x37, 0x7d, 0x49, 0x32, 0x5b, 0x0e, 0xd0, 0x36,
      0x73, 0x9c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    350000,
    {
      0x59, 0x2b, 0x44, 0xbc, 0x0f, 0x7a, 0x42, 0x86,
      0xcf, 0x07, 0xea, 0xd8, 0x49, 0x71, 0x14, 0xc6,
      0x95, 0x2c, 0x1c, 0x7d, 0xea, 0x73, 0x05, 0x19,
      0x3d, 0xea, 0xcf, 0x8e, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    390000,
    {
      0xf2, 0x17, 0xe1, 0x83, 0x48, 0x4f, 0xb6, 0xd6,
      0x95, 0x60, 0x9c, 0xc7, 0x1f, 0xa2, 0xae, 0x24,
      0xc3, 0x02, 0x09, 0x43, 0x40, 0x7e, 0x01, 0x50,
      0xb2, 0x98, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    420000,
    {
      0xde, 0x9e, 0x73, 0xa3, 0xb9, 0x1f, 0xbb, 0x01,
      0x4e, 0x03, 0x6e, 0x85, 0x83, 0xa1, 0x7d, 0x6b,
      0x63, 0x8a, 0x69, 0x9a, 0xeb, 0x2d, 0xe8, 0x57,
      0x3d, 0x12, 0x58, 0x08, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    460000,
    {
      0x2e, 0x8b, 0xaa, 0xff, 0xc1, 0x07, 0xf1, 0x5c,
      0x87, 0xae, 0xbe, 0x01, 0x66, 0x4b, 0x63, 0xd0,
      0x74, 0x76, 0xaf, 0xa5, 0x3b, 0xcb, 0xad, 0xa1,
      0x28, 0x1a, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    500000,
    {
      0x06, 0xf6, 0x09, 0x22, 0xa2, 0xaa, 0xb2, 0x75,
      0x73, 0x17, 0x82, 0x0f, 0xc6, 0xff, 0xaf, 0x6a,
      0x47, 0x0e, 0x2c, 0xbb, 0x0f, 0x63, 0xa2, 0xaa,
      0xc0, 0xa7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    540000,
    {
      0x8d, 0xd0, 0xbe, 0xbf, 0xbc, 0x48, 0x78, 0xf5,
      0xaf, 0x09, 0xd3, 0xe8, 0x48, 0xdc, 0xc5, 0x78,
      0x27, 0xd2, 0xc1, 0xce, 0xbe, 0xa8, 0xec, 0x5d,
      0x8c, 0xbe, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    570000,
    {
      0x87, 0xac, 0xbd, 0x4c, 0xd3, 0xc4, 0x0e, 0xc9,
      0xbd, 0x64, 0x8f, 0x86, 0x98, 0xed, 0x22, 0x6b,
      0x31, 0x18, 0x72, 0x74, 0xc0, 0x6c, 0xc7, 0xa9,
      0xaf, 0x79, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    600000,
    {
      0x16, 0x9a, 0x05, 0xb3, 0xbb, 0x04, 0xb7, 0xd1,
      0x3a, 0xd6, 0x28, 0x91, 0x56, 0x30, 0x90, 0x0a,
      0x5e, 0xd2, 0xe8, 0x9f, 0x3a, 0x9d, 0xc6, 0x06,
      0x4f, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    630000,
    {
      0xbb, 0xbe, 0x11, 0x70, 0x35, 0x43, 0x2a, 0x6a,
      0x4e, 0xff, 0xcb, 0x29, 0x72, 0x07, 0xa0, 0x2b,
      0x03, 0x17, 0x35, 0xb4, 0x3e, 0x0d, 0x19, 0xa9,
      0x21, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    670000,
    {
      0x08, 0x0b, 0xfe, 0x75, 0xca, 0xed, 0x86, 0x24,
      0xfc, 0xfd, 0xfb, 0xc6, 0x59, 0x73, 0xc8, 0xf9,
      0x62, 0xd7, 0xbd, 0xc4, 0x95, 0xa8, 0x91, 0xf5,
      0xd1, 0x6b, 0x7d, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    700000,
    {
      0xc1, 0x4d, 0x3f, 0x6a, 0x1e, 0x7c, 0x7d, 0x66,
      0xfd, 0x94, 0x09, 0x51, 0xe4, 0x4f, 0x3c, 0x3b,
      0xe1, 0x27, 0x3b, 0xea, 0x4d, 0x2a, 0xb1, 0x78,
      0x61, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    740000,
    {
      0xb3, 0xb4, 0x23, 0xf0, 0x46, 0x2f, 0xd7, 0x8a,
      0x01, 0xe4, 0xf1, 0xa5, 0x9a, 0x27, 0x37, 0xa0,
      0x52, 0x5b, 0x5d, 0xbb, 0x9b, 0xba, 0x0b, 0x46,
      0x34, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    780000,
    {
      0x03, 0x81, 0x58, 0x2e, 0x34, 0xc3, 0x75, 0x59,
      0x64, 0xdc, 0x28, 0x13, 0xe2, 0xb3, 0x3e, 0x52,
      0x1e, 0x55, 0x96, 0x36, 0x71, 0x44, 0xe1, 0x67,
      0x08, 0x51, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    800000,
    {
      0x03, 0xb5, 0xf8, 0xab, 0x25, 0x7e, 0x02, 0x90,
      0x3f, 0x50, 0x9f, 0x5f, 0xf2, 0x93, 0x52, 0x20,
      0xee, 0xc2, 0xe7, 0x7b, 0x18, 0x19, 0x65, 0x1d,
      0x09, 0x9b, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    840000,
    {
      0xda, 0xc1, 0x64, 0x81, 0x07, 0xbd, 0x43, 0x94,
      0xe5, 0x7e, 0x40, 0x83, 0xc8, 0x6d, 0x42, 0xb5,
      0x48, 0xb1, 0xcf, 0xb1, 0x19, 0x66, 0x5f, 0x17,
      0x9e, 0xa8, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    880000,
    {
      0xff, 0x90, 0xb4, 0xbb, 0x07, 0xed, 0xed, 0x8e,
      0x96, 0x71, 0x5b, 0xf5, 0x95, 0xc0, 0x9c, 0x7d,
      0x21, 0xdd, 0x8c, 0x61, 0xb8, 0x30, 0x6f, 0xf4,
      0x87, 0x05, 0xd6, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    900000,
    {
      0x9b, 0xd8, 0xac, 0x41, 0x8b, 0xee, 0xb1, 0xa2,
      0xcf, 0x5d, 0x68, 0xc8, 0xb5, 0xc6, 0xeb, 0xaa,
      0x94, 0x7a, 0x5b, 0x76, 0x6e, 0x55, 0x24, 0x89,
      0x8d, 0x6f, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    940000,
    {
      0xc9, 0x8f, 0x16, 0x51, 0xa4, 0x75, 0xb0, 0x0d,
      0x12, 0xf8, 0xc2, 0x5e, 0xb1, 0x66, 0xee, 0x84,
      0x3a, 0xff, 0xaa, 0x90, 0x61, 0x0e, 0x36, 0xa1,
      0x9d, 0x68, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    980000,
    {
      0xcc, 0x8e, 0x97, 0x74, 0x54, 0x2d, 0x04, 0x4a,
      0x96, 0x98, 0xca, 0x23, 0x36, 0xae, 0x02, 0xd5,
      0x98, 0x71, 0x57, 0xe6, 0x76, 0xf1, 0xc7, 0x6a,
      0xa3, 0x87, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    1010000,
    {
      0x9d, 0x9f, 0xb1, 0x1a, 0xbc, 0x27, 0x12, 0xd8,
      0x03, 0x68, 0x22, 0x9e, 0x97, 0xb8, 0xd8, 0x27,
      0xb2, 0xa0, 0x7d, 0x27, 0xeb, 0x53, 0x35, 0xe5,
      0xc9, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  },
  {
    1050000,
    {
      0xd8, 0x19, 0x0c, 0xf0, 0xaf, 0x7f, 0x08, 0xe1,
      0x79, 0xca, 0xb5, 0x1d, 0x67, 0xdb, 0x0b, 0x44,
      0xb8, 0x79, 0x51, 0xa7, 0x8f, 0x7f, 0xdc, 0x31,
      0xb4, 0xa0, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  }
};

static const uint8_t testnet_genesis[] = {
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
  0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
  0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
  0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
  0x4b, 0x1e, 0x5e, 0x4a, 0xda, 0xe5, 0x49, 0x4d,
  0xff, 0xff, 0x00, 0x1d, 0x1a, 0xa4, 0xae, 0x18,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
  0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
  0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
  0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
  0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
  0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
  0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
  0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
  0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
  0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
  0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
  0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
  0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
  0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
  0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
  0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
  0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
  0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
  0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
  0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
  0xac, 0x00, 0x00, 0x00, 0x00
};

static const btc_checkpoint_t testnet_bip30[] = {
  {
    -1,
    {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
  }
};

static const btc_deployment_t testnet_deployments[] = {
  {
    /* .name = */ "csv",
    /* .bit = */ 0,
    /* .start_time = */ 1456790400, /* March 1st, 2016 */
    /* .timeout = */ 1493596800, /* May 1st, 2017 */
    /* .threshold = */ -1,
    /* .window = */ -1,
    /* .required = */ 0,
    /* .force = */ 1
  },
  {
    /* .name = */ "segwit",
    /* .bit = */ 1,
    /* .start_time = */ 1462060800, /* May 1st 2016 */
    /* .timeout = */ 1493596800, /* May 1st 2017 */
    /* .threshold = */ -1,
    /* .window = */ -1,
    /* .required = */ 1,
    /* .force = */ 0
  },
  {
    /* .name = */ "segsignal",
    /* .bit = */ 4,
    /* .start_time = */ 0xffffffff,
    /* .timeout = */ 0xffffffff,
    /* .threshold = */ 269, /* 80% */
    /* .window = */ 336, /* ~2.33 days */
    /* .required = */ 0,
    /* .force = */ 0
  },
  {
    /* .name = */ "testdummy",
    /* .bit = */ 28,
    /* .start_time = */ 1199145601, /* January 1, 2008 */
    /* .timeout = */ 1230767999, /* December 31, 2008 */
    /* .threshold = */ -1,
    /* .window = */ -1,
    /* .required = */ 0,
    /* .force = */ 1
  }
};

static const btc_network_t testnet = {
  /* .type = */ BTC_NETWORK_TESTNET,
  /* .name = */ "testnet",
  /* .seeds = */ {
    /* .items = */ testnet_seeds,
    /* .length = */ lengthof(testnet_seeds)
  },
  /* .magic = */ 0x0709110b,
  /* .port = */ 18333,
  /* .checkpoints = */ {
    /* .items = */ testnet_checkpoints,
    /* .length = */ lengthof(testnet_checkpoints)
  },
  /* .last_checkpoint */ 1050000,
  /* .halving_interval = */ 210000,
  /* .genesis = */ {
    /* .hash = */ {
      0x43, 0x49, 0x7f, 0xd7, 0xf8, 0x26, 0x95, 0x71,
      0x08, 0xf4, 0xa3, 0x0f, 0xd9, 0xce, 0xc3, 0xae,
      0xba, 0x79, 0x97, 0x20, 0x84, 0xe9, 0x0e, 0xad,
      0x01, 0xea, 0x33, 0x09, 0x00, 0x00, 0x00, 0x00
    },
    /* .header = */ {
      /* .version = */ 1,
      /* .prev_block = */ {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
      },
      /* .merkle_root = */ {
        0x3b, 0xa3, 0xed, 0xfd, 0x7a, 0x7b, 0x12, 0xb2,
        0x7a, 0xc7, 0x2c, 0x3e, 0x67, 0x76, 0x8f, 0x61,
        0x7f, 0xc8, 0x1b, 0xc3, 0x88, 0x8a, 0x51, 0x32,
        0x3a, 0x9f, 0xb8, 0xaa, 0x4b, 0x1e, 0x5e, 0x4a
      },
      /* .time = */ 1296688602,
      /* .bits = */ 486604799,
      /* .nonce = */ 414098458
    },
    /* .data = */ testnet_genesis,
    /* .length = */ sizeof(testnet_genesis)
  },
  /* .pow = */ {
    /* .limit = */ {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
    },
    /* .bits = */ 486604799,
    /* .chainwork = */ {
      0xb6, 0xf7, 0x09, 0x7d, 0xfd, 0x3c, 0x12, 0xb7,
      0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    /* .target_timespan = */ 14 * 24 * 60 * 60,
    /* .target_spacing = */ 10 * 60,
    /* .retarget_interval = */ 2016,
    /* .target_reset = */ 1,
    /* .no_retargeting = */ 0
  },
  /* .block = */ {
    /* .prune_after_height = */ 1000,
    /* .keep_blocks = */ 10000,
    /* .max_tip_age = */ 24 * 60 * 60,
    /* .slow_height = */ 950000
  },
  /* .softforks = */ {
    /* .bip30 = */ {
      /* .items = */ testnet_bip30,
      /* .length = */ 0
    },
    /* .bip34 = */ {
      21111,
      {
        0xf8, 0x8e, 0xcd, 0x99, 0x12, 0xd0, 0x0d, 0x3f,
        0x5c, 0x2a, 0x8e, 0x0f, 0x50, 0x41, 0x7d, 0x3e,
        0x41, 0x5c, 0x75, 0xb3, 0xab, 0xe5, 0x84, 0x34,
        0x6d, 0xa9, 0xb3, 0x23, 0x00, 0x00, 0x00, 0x00
      }
    },
    /* .bip65 = */ {
      581885,
      {
        0xb6, 0x1e, 0x86, 0x4f, 0xbe, 0xc4, 0x1d, 0xfa,
        0xf0, 0x9d, 0xa0, 0x5d, 0x1d, 0x76, 0xdc, 0x06,
        0x8b, 0x0d, 0xd8, 0x2e, 0xe7, 0x98, 0x2f, 0xf2,
        0x55, 0x66, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00
      }
    },
    /* .bip66 = */ {
      330776,
      {
        0x82, 0xa1, 0x4b, 0x9e, 0x5e, 0xa8, 0x1d, 0x48,
        0x32, 0xb8, 0xe2, 0xcd, 0x3c, 0x2a, 0x60, 0x92,
        0xb5, 0xa3, 0x85, 0x32, 0x85, 0xa8, 0x99, 0x5e,
        0xc4, 0xc8, 0x04, 0x21, 0x00, 0x00, 0x00, 0x00
      }
    }
  },
  /* .activation_threshold = */ 1512, /* 75% for testchains */
  /* .miner_window = */ 2016, /* pow.target_timespan / pow.target_spacing */
  /* .deployments = */ {
    /* .items = */ testnet_deployments,
    /* .length = */ lengthof(testnet_deployments)
  },
  /* .key = */ {
    /* .privkey = */ 0xef,
    /* .xpubkey = */ {
      0x043587cf, /* tpub (p2pkh or p2sh) */
      0x044a5262, /* upub (nested p2wpkh) */
      0x045f1cf6, /* vpub (native p2wpkh) */
      0x024289ef, /* Upub (nested p2wsh) */
      0x02575483  /* Vpub (native p2wsh) */
    },
    /* .xprvkey = */ {
      0x04358394, /* tprv (p2pkh or p2sh) */
      0x044a4e28, /* uprv (nested p2wpkh) */
      0x045f18bc, /* vprv (native p2wpkh) */
      0x024285b5, /* Uprv (nested p2wsh) */
      0x02575048  /* Vprv (native p2wsh) */
    },
    /* .coin_type = */ 1
  },
  /* .address = */ {
    /* .p2pkh = */ 0x6f,
    /* .p2sh = */ 0xc4,
    /* .bech32 = */ "tb"
  },
  /* .require_standard = */ 0,
  /* .rpc_port = */ 18332,
  /* .min_relay = */ 1000,
  /* .fee_rate = */ 20000,
  /* .max_fee_rate = */ 60000,
  /* .self_connect = */ 0,
  /* .request_mempool = */ 0
};

const btc_network_t *btc_testnet = &testnet;
