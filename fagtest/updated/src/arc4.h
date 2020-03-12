#ifndef _ARC4_H
#define _ARC4_H

#include <stdlib.h>

struct rc4_ctx {
  unsigned char S[0x100], i, j;
};

void rc4_init(struct rc4_ctx *ctx, const unsigned char *key, size_t key_len);
unsigned char rc4_next(struct rc4_ctx *ctx);

#endif /* _ARC4_H */
