#ifndef ANO_CRYPTO_HMAC_INTERNAL_H
#define ANO_CRYPTO_HMAC_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_status.h>
#include "ac_sha256_internal.h"
#include "ac_sha512_internal.h"

#define ANO_CRYPTO_HMAC_SHA256_MAC_SIZE ANO_CRYPTO_SHA256_DIGEST_SIZE
#define ANO_CRYPTO_HMAC_SHA384_MAC_SIZE ANO_CRYPTO_SHA384_DIGEST_SIZE
#define ANO_CRYPTO_HMAC_SHA512_MAC_SIZE ANO_CRYPTO_SHA512_DIGEST_SIZE
#define ANO_CRYPTO_HMAC_SHA256_BLOCK_SIZE ANO_CRYPTO_SHA256_BLOCK_SIZE
#define ANO_CRYPTO_HMAC_SHA512_BLOCK_SIZE ANO_CRYPTO_SHA512_BLOCK_SIZE

typedef struct ano_crypto_internal_hmac_sha256_ctx_s {
    ano_crypto_internal_sha256_ctx_t inner;
    uint8_t outer_pad[ANO_CRYPTO_HMAC_SHA256_BLOCK_SIZE];
    int initialized;
} ano_crypto_internal_hmac_sha256_ctx_t;

typedef struct ano_crypto_internal_hmac_sha512_family_ctx_s {
    ano_crypto_internal_sha512_ctx_t inner;
    uint8_t outer_pad[ANO_CRYPTO_HMAC_SHA512_BLOCK_SIZE];
    uint32_t hash_algorithm;
    int initialized;
} ano_crypto_internal_hmac_sha512_family_ctx_t;

ano_crypto_status_t ano_crypto_internal_hmac_sha256_init(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len);
ano_crypto_status_t ano_crypto_internal_hmac_sha256_update(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len);
ano_crypto_status_t ano_crypto_internal_hmac_sha256_final(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE]);
ano_crypto_status_t ano_crypto_internal_hmac_sha256_compute(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE]);

ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_init(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len);
ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_update(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len);
ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_final(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    uint8_t *out);
ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_compute(
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out);

#endif /* ANO_CRYPTO_HMAC_INTERNAL_H */
