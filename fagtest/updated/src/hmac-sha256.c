#include <string.h>
#include "hmac-sha256.h"
#include "sha256.h"

/* Ref: https://tools.ietf.org/html/rfc2104
 * Ref: https://en.wikipedia.org/wiki/HMAC
 *
 * HMAC(H, K, m) == H(K' ⊕ opad || H((K' ⊕ ipad) || m))
 * where K' = H(K)   if |K| > block size (64 for SHA-256)
 *          = K      otherwise
 *     ipad = 0x36
 *     opad = 0x5c
 */

#define IPAD 0x36
#define OPAD 0x5C

void hmac_sha256_init(HMAC_SHA256_CTX *ctx, const uint8_t *key,
                      size_t key_len) {
  uint8_t Kx[SHA256_BLOCK_SIZE];
  size_t i;

  /* Compute K' */
  if (key_len > SHA256_BLOCK_SIZE) {
    sha256_init(&ctx->sha);
    sha256_update(&ctx->sha, key, key_len);
    sha256_final(&ctx->sha, ctx->K);
    ctx->K_len = SHA256_DIGEST_SIZE;
  } else {
    memcpy(ctx->K, key, key_len);
    ctx->K_len = key_len;
  }

  /* K' ⊕ ipad */
  for (i = 0; i < ctx->K_len; i++) {
    Kx[i] = IPAD ^ ctx->K[i];
  }
  for (; i < SHA256_BLOCK_SIZE; i++) {
    Kx[i] = IPAD;
  }

  /* H((K' ⊕ ipad) || ... */
  sha256_init(&ctx->sha);
  sha256_update(&ctx->sha, Kx, SHA256_BLOCK_SIZE);
}

void hmac_sha256_update(HMAC_SHA256_CTX *ctx, const uint8_t *msg,
                        size_t msg_len) {
  /* ... msg ... */
  sha256_update(&ctx->sha, msg, msg_len);
}

void hmac_sha256_final(HMAC_SHA256_CTX *ctx,
                       uint8_t tag[HMAC_SHA256_TAG_SIZE]) {
  uint8_t Kx[SHA256_BLOCK_SIZE];
  size_t i;

  /* ... ) */
  sha256_final(&ctx->sha, tag);

  /* K' ⊕ opad */
  for (i = 0; i < ctx->K_len; i++) {
    Kx[i] = OPAD ^ ctx->K[i];
  }
  for (; i < SHA256_BLOCK_SIZE; i++) {
    Kx[i] = OPAD;
  }

  /* H(K' ⊕ opad || H((K' ⊕ ipad) || m)) */
  sha256_init(&ctx->sha);
  sha256_update(&ctx->sha, Kx, SHA256_BLOCK_SIZE);
  sha256_update(&ctx->sha, tag, SHA256_DIGEST_SIZE);
  sha256_final(&ctx->sha, tag);
}

void hmac_sha256(uint8_t tag[HMAC_SHA256_TAG_SIZE],
                 const uint8_t *msg, size_t msg_len,
                 const uint8_t *key, size_t key_len) {
  HMAC_SHA256_CTX ctx;

  hmac_sha256_init(&ctx, key, key_len);
  hmac_sha256_update(&ctx, msg, msg_len);
  hmac_sha256_final(&ctx, tag);
}
