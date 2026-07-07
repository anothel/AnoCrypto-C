#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_drbg.h>
#include <anocrypto/ac_module.h>
#include "ac_internal.h"

typedef struct ano_crypto_drbg_ctx_internal_s {
    uint32_t magic;
    uint32_t algorithm;
    uint32_t state;
    uint32_t reseed_counter;
} ano_crypto_drbg_ctx_internal_t;

static ano_crypto_status_t ano_crypto_drbg_validate_algorithm(
    ano_crypto_drbg_algorithm_t algorithm)
{
    switch (algorithm) {
    case ANO_CRYPTO_DRBG_ALG_HMAC_SHA256:
    case ANO_CRYPTO_DRBG_ALG_HASH_SHA256:
        return ANO_CRYPTO_OK;
    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
}

ano_crypto_status_t ano_crypto_drbg_context_size(
    ano_crypto_drbg_algorithm_t algorithm,
    size_t *out_size)
{
    ano_crypto_status_t st;

    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_drbg_validate_algorithm(algorithm);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_size = sizeof(ano_crypto_drbg_ctx_internal_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_drbg_instantiate(
    void *ctx,
    size_t ctx_len,
    ano_crypto_drbg_algorithm_t algorithm,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags)
{
    size_t required_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || (personalization == 0 && personalization_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if ((flags & ~ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_drbg_context_size(algorithm, &required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_drbg_reseed(
    void *ctx,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || (additional_input == 0 && additional_input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if ((flags & ~ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_drbg_generate(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || out == 0 || out_len == 0u ||
        (additional_input == 0 && additional_input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if ((flags & ~ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_drbg_uninstantiate(
    void *ctx,
    size_t ctx_len)
{
    if (ctx == 0 && ctx_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx != 0 && ctx_len != 0u) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
    }

    return ANO_CRYPTO_OK;
}

const char *ano_crypto_drbg_algorithm_name(
    ano_crypto_drbg_algorithm_t algorithm)
{
    switch (algorithm) {
    case ANO_CRYPTO_DRBG_ALG_HMAC_SHA256:
        return "HMAC_DRBG-SHA-256";
    case ANO_CRYPTO_DRBG_ALG_HASH_SHA256:
        return "Hash_DRBG-SHA-256";
    default:
        return "UNKNOWN_DRBG_ALGORITHM";
    }
}

const char *ano_crypto_drbg_state_name(
    ano_crypto_drbg_state_t state)
{
    switch (state) {
    case ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED:
        return "UNINSTANTIATED";
    case ANO_CRYPTO_DRBG_STATE_INSTANTIATED:
        return "INSTANTIATED";
    case ANO_CRYPTO_DRBG_STATE_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN_DRBG_STATE";
    }
}
