#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_module.h>
#include <anocrypto/ac_random.h>
#include "ac_internal.h"

static int random_absent_or_valid(const uint8_t *data, size_t data_len)
{
    return data != 0 || data_len == 0u;
}

static ano_crypto_status_t random_reject_flags(uint32_t flags)
{
    if (flags == ANO_CRYPTO_RANDOM_FLAG_NONE) {
        return ANO_CRYPTO_OK;
    }

    return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
}

int ano_crypto_random_is_available(void)
{
    return 0;
}

const char *ano_crypto_random_service_name(void)
{
    return "AnoCrypto-C random service reserved";
}

ano_crypto_status_t ano_crypto_random_context_size(size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_size = (size_t)ANO_CRYPTO_RANDOM_SERVICE_RESERVED;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_random_init(
    void *ctx,
    size_t ctx_len,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags)
{
    ano_crypto_status_t st;

    if (ctx == 0 || ctx_len == 0u || !random_absent_or_valid(personalization, personalization_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = random_reject_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    ano_crypto_internal_zeroize(ctx, ctx_len);
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_random_generate(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_status_t st;

    if (ctx == 0 || out == 0 || out_len == 0u ||
        !random_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = random_reject_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
        return st;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
        return st;
    }

    ano_crypto_internal_zeroize(out, out_len);
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_random_reseed(
    void *ctx,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_status_t st;

    if (ctx == 0 || !random_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = random_reject_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_random_uninit(
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
