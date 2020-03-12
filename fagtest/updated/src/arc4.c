#include <string.h>
#include "arc4.h"

void rc4_init(struct rc4_ctx *ctx, const unsigned char *key, size_t key_len){
  unsigned char i, j, t;
  i = 0;
  do {
    ctx->S[i] = i;
  } while (++i);

  i = j = 0;
  do {
    j += ctx->S[i] + key[i % key_len];
    t = ctx->S[i];
    ctx->S[i] = ctx->S[j];
    ctx->S[j] = t;
  } while(++i);

  ctx->i = 0;
  ctx->j = 0;
}

unsigned char rc4_next(struct rc4_ctx *ctx){
  unsigned char t;

  ctx->i++;
  ctx->j += ctx->S[ctx->i];
  t = ctx->S[ctx->i];
  ctx->S[ctx->i] = ctx->S[ctx->j];
  ctx->S[ctx->j] = t;
  return ctx->S[(ctx->S[ctx->i] + ctx->S[ctx->j]) & 0xff];
}
