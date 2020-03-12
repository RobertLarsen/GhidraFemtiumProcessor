#ifndef HMAC_SHA256_H
#define HMAC_SHA256_H

#include <stdint.h>
#include "sha256.h"
#define HMAC_SHA256_TAG_SIZE SHA256_DIGEST_SIZE

typedef struct {
  uint8_t K[SHA256_BLOCK_SIZE];
  size_t K_len;
  SHA256_CTX sha;
} HMAC_SHA256_CTX;

void hmac_sha256_init(HMAC_SHA256_CTX *ctx, const uint8_t *key, size_t key_len);
void hmac_sha256_update(HMAC_SHA256_CTX *ctx, const uint8_t buf[], size_t len);
void hmac_sha256_final(HMAC_SHA256_CTX *ctx, uint8_t tag[HMAC_SHA256_TAG_SIZE]);

void hmac_sha256(uint8_t tag[HMAC_SHA256_TAG_SIZE],
                 const uint8_t *msg, size_t msg_len,
                 const uint8_t *key, size_t key_len);

#endif /* HMAC_SHA256_H */
