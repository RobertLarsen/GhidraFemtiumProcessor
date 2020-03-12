#define _GNU_SOURCE /* asprintf */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lz4.h"
#include "arc4.h"
#include "crc32.h"
#include "misc.h"
#include "updated.h"

#include "handler.h"

/* Globals */
struct updhdr HDR;
struct {
  char *path;
  int fd;
} TMP = {NULL, 0};

/* Forward declarations */
void unlink_tmpfile(void);
void upd_recv(void);
void upd_verify(void);
void upd_process(void);
void upd_exec(struct objhdr *hdr);
void upd_file(struct objhdr *hdr);
void upd_link(struct objhdr *hdr);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ERR(fmt, ...)                                       \
  do {                                                      \
    fprintf(stderr, "Error: " fmt "\n", ## __VA_ARGS__);    \
    exit(EXIT_FAILURE);                                     \
  } while (0)

void handler() {
  char buf[0x100];

  if (-1 == asprintf(&TMP.path, "%s/updated-XXXXXX", G.tmpdir)) {
  ERRTMP:
    ERR("Code 1000");
  }

  TMP.fd = mkstemp(TMP.path);
  if (-1 == TMP.fd) {
    goto ERRTMP;
  }

  /* Unlink tmpfile on exit */
  atexit(unlink_tmpfile);

  upd_recv();

  if (HDR.flags & FLG_HMAC_SHA256) {
    upd_verify();
  } else if (G.onlysigned) {
    ERR("Code 1011");
  }

  if (!(HDR.flags & FLG_ARC4) && G.onlycrypto) {
    ERR("Code 1012");
  }

  upd_process();

  puts("OK");
}

struct io_ctx {
  struct rc4_ctx rc4;
  uint8_t buf[0x1000];
  size_t i, n;
};

void lz4_write(const uint8_t *buf, unsigned int len, void *p) {
  (void)p;

  if (!writen(TMP.fd, buf, len)) {
    ERR("Code 1003");
  }
}

uint8_t lz4_read(void *p) {
  struct io_ctx *ctx = p;
  int ret;

  /* Read more data if buffer is empty */
  if (ctx->i == ctx->n) {
    ret = do_read(STDIN_FILENO, ctx->buf, sizeof(ctx->buf));
    if (0 >= ret) {
      ERR("Code 2018");
    }
    ctx->n = ret;
    ctx->i = 0;
  }

  /* Maybe decrypt */
  if (HDR.flags & FLG_ARC4) {
    ctx->buf[ctx->i] ^= rc4_next(&ctx->rc4);
  }

  return ctx->buf[ctx->i++];
}

void upd_recv(void) {
  struct io_ctx ctx;
  int ret;

  if (!readn(STDIN_FILENO, &HDR, sizeof(HDR))) {
    ERR("Code 2000");
  }

  if (memcmp(HDR.magic, UPD_MAGIC, sizeof(UPD_MAGIC))) {
    ERR("Code 1001");
  }

  if (HDR.flags & FLG_ARC4) {
    rc4_init(&ctx.rc4, G.key, G.key_len);
  }

  if (!writen(TMP.fd, &HDR, sizeof(HDR))) {
    ERR("Code 2001");
  }

  /* Check for key if needed */
  if (HDR.flags & (FLG_HMAC_SHA256 | FLG_ARC4)) {
    if (!G.keyfile) {
      ERR("Code 3000");
    }
  }

  if (HDR.flags & FLG_LZ4) {

    /* Trigger buffer refill on first read */
    ctx.i = ctx.n = 0;

    /* Un-compress data and maybe decrypt */
    if (!unlz4(lz4_read, lz4_write, &ctx)) {
      ERR("Code 1002");
    }

  } else {
    for (;;) {

      /* Read data */
      ret = do_read(STDIN_FILENO, ctx.buf, sizeof(ctx.buf));
      if (-1 == ret) {
        ERR("Code 2002");
      }
      if (0 == ret) {
        break;
      }
      /* Maybe decrypt */
      if (HDR.flags & FLG_ARC4) {
        for (ctx.i = 0; ctx.i < (size_t)ret; ctx.i++) {
          ctx.buf[ctx.i] ^= rc4_next(&ctx.rc4);
        }
      }
      /* Write */
      if (!writen(TMP.fd, ctx.buf, ret)) {
        ERR("Code 2003");
      }

    }
  }

  if (-1 == lseek(TMP.fd, sizeof(HDR), SEEK_SET)) {
    ERR("Code 2004");
  }

}

void upd_verify(void) {
  unsigned char sig[HMAC_SHA256_TAG_SIZE], tag[HMAC_SHA256_TAG_SIZE],
    buf[0x1000];
  HMAC_SHA256_CTX ctx;
  int ret;
  size_t i;
  off_t saved_pos;

  hmac_sha256_init(&ctx, G.key, G.key_len);

  if (!readn(TMP.fd, sig, sizeof(sig))) {
    ERR("Code 2005");
  }
  saved_pos = lseek(TMP.fd, 0, SEEK_CUR);

  i = 0;
  while (i < HDR.size) {
    ret = do_read(TMP.fd, buf, MIN(sizeof(buf), HDR.size - i));
    if (0 >= ret) {
      ERR("Code 2006");
    }
    hmac_sha256_update(&ctx, buf, ret);
    i += ret;
  }

  hmac_sha256_final(&ctx, tag);

  if (memcmp(sig, tag, sizeof(sig))) {
    ERR("Code 1004");
  }

  lseek(TMP.fd, saved_pos, SEEK_SET);
}

void upd_process(void) {
  struct objhdr obj;
  int ret;

  for (;;) {
    ret = do_read(TMP.fd, &obj, sizeof(obj));
    if (-1 == ret) {
      ERR("Code 2007");
    }
    if (0 == ret) {
      break;
    }

    switch (OBJ_TYPE(&obj)) {

    case OBJ_EXEC:
      upd_exec(&obj);
      break;

    case OBJ_FILE:
      upd_file(&obj);
      break;

    case OBJ_LINK:
      upd_link(&obj);
      break;

    default:
      ERR("Code 1005");
    }
  }
}

void upd_exec(struct objhdr *obj) {
  char c, cmd[0x1000];
  unsigned int crc;
  size_t i;

  i = 0;
  do {
    if (!readn(TMP.fd, &c, 1)) {
      ERR("Code 2008");
    }
    cmd[i++] = c;
  } while(c);

  printf("EXEC: %s\n", cmd);

  if (OBJ_FLGS(obj) & FLG_CRC32) {
    if (!readn(TMP.fd, &crc, sizeof(crc))) {
      ERR("Code 2019");
    }

    if (crc != crc32(cmd, obj->size)) {
      ERR("Code 1006");
    }
  }

  fflush(stdout);
  fflush(stderr);
  if (!fork()) {
    setgid(G.gid);
    setuid(G.uid);
    system(cmd);
    _exit(0);
  }

  wait(NULL);
}

void upd_file(struct objhdr *obj) {
  char c, dst[0x100];
  unsigned char buf[0x1000];
  unsigned int crc, crc_check;
  int fd, ret;
  uint32_t mode;
  size_t i, saved_i;
  off_t saved_pos;

  i = 0;
  do {
    if (!readn(TMP.fd, &c, 1)) {
      ERR("Code 2009");
    }
    dst[i++] = c;
  } while(c);

  if (!readn(TMP.fd, &mode, sizeof(mode))) {
    ERR("Code 2010");
  }
  i += sizeof(mode);

  printf("FILE: %s (0%03o, %luB)\n", dst, mode, obj->size - i);

  if (OBJ_FLGS(obj) & FLG_CRC32) {
    saved_pos = lseek(TMP.fd, 0, SEEK_CUR);
    saved_i = i;

    crc_check = crc32(dst, strlen(dst) + 1);
    crc_check = crc32x(crc_check, &mode, sizeof(mode));
    while (i < obj->size) {
      ret = do_read(TMP.fd, buf, MIN(sizeof(buf), obj->size - i));
      if (0 >= ret) {
        ERR("Code 2011");
      }
      crc_check = crc32x(crc_check, buf, ret);
      i += ret;
    }

    if (!readn(TMP.fd, &crc, sizeof(crc))) {
      ERR("Code 2012");
    }

    if (crc != crc_check) {
      ERR("Code 1007");
    }

    i = saved_i;
    lseek(TMP.fd, saved_pos, SEEK_SET);
  }

  fd = open(dst, O_WRONLY | O_TRUNC | O_CREAT, mode);
  fchown(fd, G.uid, G.gid);
  if (-1 == fd) {
    ERR("Code 1008");
  }

  while (i < obj->size) {
    ret = do_read(TMP.fd, buf, MIN(sizeof(buf), obj->size - i));
    if (0 >= ret) {
      ERR("Code 2013");
    }
    if (!writen(fd, buf, ret)) {
      ERR("Code 2014");
    }
    i += ret;
  }

  /* skip CRC */
  if (OBJ_FLGS(obj) & FLG_CRC32) {
    lseek(TMP.fd, sizeof(crc), SEEK_CUR);
  }
}

void upd_link(struct objhdr *obj) {
  char c, buf[0x1000], *src, *dst;
  unsigned int crc, crc_check;
  size_t i;

  src = buf;
  i = 0;
  do {
    if (!readn(TMP.fd, &c, 1)) {
      ERR("Code 2015");
    }
    src[i++] = c;
  } while(c);

  dst = &src[i];
  i = 0;
  do {
    if (!readn(TMP.fd, &c, 1)) {
      ERR("Code 2016");
    }
    dst[i++] = c;
  } while(c);

  printf("LINK: %s -> %s\n", dst, src);

  if (obj->size != strlen(src) + 1 + strlen(dst) + 1) {
    ERR("Code 1009-%u-%lu", obj->size, strlen(src) + 1 + strlen(dst) + 1);
  }

  if (OBJ_FLGS(obj) & FLG_CRC32) {
    if (!readn(TMP.fd, &crc, sizeof(crc))) {
      ERR("Code 2017");
    }

    crc_check = crc32(src, strlen(src) + 1);
    crc_check = crc32x(crc_check, dst, strlen(dst) + 1);
    if (crc != crc_check) {
      ERR("Code 1010");
    }
  }

  symlink(src, dst);
  chown(dst, G.uid, G.gid);
}

void unlink_tmpfile(void) {
  close(TMP.fd);
  unlink(TMP.path);
}
