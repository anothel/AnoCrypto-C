#ifndef ANO_CRYPTO_AC_HASH_H
#define ANO_CRYPTO_AC_HASH_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API for SHA-2 hash services.
 *
 * P1-22 implementation scope:
 * - SHA-256, SHA-384, and SHA-512 one-shot and streaming processing are
 *   implemented and covered by public-source startup and extended KATs.
 * - Processing functions are state-gated and require an operational module.
 * - A context is algorithm-bound after init and must not be used through a
 *   different public service family.
 * P1-23 validation scope: deterministic test-only differential and portability coverage; public ABI unchanged.
 */

#define ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE 32u
#define ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE 48u
#define ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE 64u
#define ANO_CRYPTO_HASH_SHA256_BLOCK_SIZE 64u
#define ANO_CRYPTO_HASH_SHA384_BLOCK_SIZE 128u
#define ANO_CRYPTO_HASH_SHA512_BLOCK_SIZE 128u

typedef enum ano_crypto_hash_algorithm_e {
    ANO_CRYPTO_HASH_ALG_SHA256 = 1,
    ANO_CRYPTO_HASH_ALG_SHA384 = 2,
    ANO_CRYPTO_HASH_ALG_SHA512 = 3
} ano_crypto_hash_algorithm_t;

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_context_size(
    ano_crypto_hash_algorithm_t algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_digest_size(
    ano_crypto_hash_algorithm_t algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_hash_algorithm_t algorithm);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hash_digest(
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

ANO_CRYPTO_API const char *ano_crypto_hash_algorithm_name(
    ano_crypto_hash_algorithm_t algorithm);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_HASH_H */
