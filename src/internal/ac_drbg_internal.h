#ifndef ANO_CRYPTO_DRBG_INTERNAL_H
#define ANO_CRYPTO_DRBG_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>
#include "ac_entropy_internal.h"

ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_selftest(void);

/*
 * P1-8 internal helper for future random-service wiring. It draws entropy and
 * nonce bytes through the reviewed entropy-source seam, instantiates the
 * HMAC_DRBG-SHA-256 core, then zeroizes local seed material. This is not a
 * public random API.
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

#endif /* ANO_CRYPTO_DRBG_INTERNAL_H */
