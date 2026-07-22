#ifndef ANO_CRYPTO_SHA1_INTERNAL_H
#define ANO_CRYPTO_SHA1_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_SHA1_DIGEST_SIZE 20u
#define ANO_CRYPTO_SHA1_BLOCK_SIZE 64u

typedef struct ano_crypto_internal_sha1_ctx_s {
    uint32_t state[5];
    uint64_t total_len;
    uint8_t buffer[ANO_CRYPTO_SHA1_BLOCK_SIZE];
    size_t buffer_len;
    int finalized;
} ano_crypto_internal_sha1_ctx_t;

void ano_crypto_internal_sha1_init(ano_crypto_internal_sha1_ctx_t *ctx);
ano_crypto_status_t ano_crypto_internal_sha1_update(
    ano_crypto_internal_sha1_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len);
ano_crypto_status_t ano_crypto_internal_sha1_final(
    ano_crypto_internal_sha1_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA1_DIGEST_SIZE]);
ano_crypto_status_t ano_crypto_internal_sha1_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA1_DIGEST_SIZE]);

#endif /* ANO_CRYPTO_SHA1_INTERNAL_H */
