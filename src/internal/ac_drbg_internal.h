#ifndef ANO_CRYPTO_DRBG_INTERNAL_H
#define ANO_CRYPTO_DRBG_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>
#include "ac_entropy_internal.h"

#define ANO_CRYPTO_INTERNAL_HMAC_DRBG_RESEED_INTERVAL 281474976710656ULL /* 2^48 */

ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_selftest(void);

/*
 * Internal source-backed helper used by the public random-service implementation.
 * It draws entropy and nonce bytes through the reviewed entropy-source seam,
 * instantiates HMAC_DRBG-SHA-256, initializes the process guard, and zeroizes
 * local seed material. The provider and process metadata remain private.
 */
ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
    void *ctx,
    size_t ctx_len,
    const ano_crypto_internal_entropy_source_t *source,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags);

ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
    void *ctx,
    const ano_crypto_internal_entropy_source_t *source,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

/* Generate only from a context created through the entropy-source helper. */
ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_generate_source_backed(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

#if defined(ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS)
/* Dedicated-test-build hook. Never compiled into release or PoC builds. */
ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_test_set_reseed_counter(
    void *ctx,
    uint64_t reseed_counter);
#endif

#endif /* ANO_CRYPTO_DRBG_INTERNAL_H */
