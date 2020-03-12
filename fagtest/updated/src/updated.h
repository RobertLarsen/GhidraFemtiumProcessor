#ifndef _UPDATED_H
#define _UPDATED_H

#include <stdio.h>
#include <stdint.h>
#include "hmac-sha256.h"

#define VERSION "2.51c"

/* Globals */
struct updated {
  /* Command line options */
  unsigned port, uid, gid, timeout;
  char *tmpdir, *logfile, *keyfile;
  int onlysigned, onlycrypto;

  /* Run-time */
  FILE *logf;
  unsigned char *key;
  size_t key_len;
} G;

/* UPD file format */

/* Global header */
#define UPD_MAGIC ((const char[]){0x55, 0x50})
#define FLG_LZ4          (1 << 0)
#define FLG_HMAC_SHA256  (1 << 1)
#define FLG_ARC4         (1 << 2)

struct __attribute__ ((__packed__)) updhdr {
  unsigned char magic[sizeof(UPD_MAGIC)];
  enum __attribute__ ((__packed__)) {
    VER_1,
    VER_N,
  } version;
  uint8_t flags;
  uint32_t size;
};

/* Object Header */
enum objtype {
  OBJ_EXEC,
  OBJ_FILE,
  OBJ_LINK,
  OBJ_N
};

#define FLG_CRC32 (1 << 0)

#define OBJ_FLGS(hdr) ((hdr)->typeflgs >> 4)
#define OBJ_TYPE(hdr) ((hdr)->typeflgs & 0x0f)

struct __attribute__ ((__packed__)) objhdr {
  uint8_t typeflgs;
  uint32_t size;
};

#endif /* _UPDATED_H */
