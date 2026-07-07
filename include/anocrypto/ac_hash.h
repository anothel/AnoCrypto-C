#ifndef ANO_CRYPTO_AC_HASH_H
#define ANO_CRYPTO_AC_HASH_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API skeleton for hash services.
 *
 * P1-1 scope:
 * - SHA-256 processing is implemented and covered by public-source KATs.
 * - SHA-384/SHA-512 are ABI-reserved candidates and return
 *   ANO_CRYPTO_ERR_NOT_IMPLEMENTED from processing APIs until reviewed.
 * - Processing functions are state-gated and require an operational module.
 */
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
