#ifndef ANO_CRYPTO_AC_SHA512_INTERNAL_H
#define ANO_CRYPTO_AC_SHA512_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_SHA384_DIGEST_SIZE 48u
#define ANO_CRYPTO_SHA512_DIGEST_SIZE 64u
#define ANO_CRYPTO_SHA512_BLOCK_SIZE 128u

typedef struct ano_crypto_internal_sha512_ctx_s {
    uint64_t state[8];
    uint64_t total_bytes_high;
    uint64_t total_bytes_low;
    uint8_t buffer[ANO_CRYPTO_SHA512_BLOCK_SIZE];
    size_t buffer_len;
    uint32_t initialized;
} ano_crypto_internal_sha512_ctx_t;

void ano_crypto_internal_sha384_init(ano_crypto_internal_sha512_ctx_t *ctx);
void ano_crypto_internal_sha512_init(ano_crypto_internal_sha512_ctx_t *ctx);

ano_crypto_status_t ano_crypto_internal_sha512_update(
    ano_crypto_internal_sha512_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len);

ano_crypto_status_t ano_crypto_internal_sha384_final(
    ano_crypto_internal_sha512_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA384_DIGEST_SIZE]);

ano_crypto_status_t ano_crypto_internal_sha512_final(
    ano_crypto_internal_sha512_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA512_DIGEST_SIZE]);

ano_crypto_status_t ano_crypto_internal_sha384_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA384_DIGEST_SIZE]);

ano_crypto_status_t ano_crypto_internal_sha512_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA512_DIGEST_SIZE]);

#endif /* ANO_CRYPTO_AC_SHA512_INTERNAL_H */
