#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_drbg.h>
#include <anocrypto/ac_module.h>
#include <anocrypto/ac_random.h>
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"
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

static ano_crypto_status_t random_init_os_source(
    ano_crypto_internal_entropy_source_t *source)
{
    if (source == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (!ano_crypto_internal_platform_entropy_is_available()) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return ano_crypto_internal_entropy_source_init_os(
        source,
        ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE);
}

int ano_crypto_random_is_available(void)
{
    return ano_crypto_internal_platform_entropy_is_available();
}

const char *ano_crypto_random_service_name(void)
{
    return "AnoCrypto-C HMAC_DRBG-SHA-256 random service";
}

ano_crypto_status_t ano_crypto_random_context_size(size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_size = 0u;
    if (!ano_crypto_internal_platform_entropy_is_available()) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return ano_crypto_drbg_context_size(
        ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
        out_size);
}

ano_crypto_status_t ano_crypto_random_init(
    void *ctx,
    size_t ctx_len,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags)
{
    ano_crypto_internal_entropy_source_t source;
    size_t required_len = 0u;
    ano_crypto_status_t st;

    if (ctx == 0 || ctx_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (!random_absent_or_valid(personalization, personalization_len)) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = random_reject_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return st;
    }

    st = ano_crypto_random_context_size(&required_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return st;
    }
    if (ctx_len < required_len) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return st;
    }

    st = random_init_os_source(&source);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
        return st;
    }

    st = ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
        ctx,
        ctx_len,
        &source,
        personalization,
        personalization_len,
        ANO_CRYPTO_DRBG_FLAG_NONE);
    ano_crypto_internal_zeroize(&source, sizeof(source));
    return st;
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

    if (out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx == 0 || !random_absent_or_valid(additional_input, additional_input_len)) {
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = random_reject_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
        return st;
    }

    if (out_len > ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE) {
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
        return st;
    }

    st = ano_crypto_internal_hmac_drbg_sha256_generate_source_backed(
        ctx,
        out,
        out_len,
        additional_input,
        additional_input_len,
        ANO_CRYPTO_DRBG_FLAG_NONE);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
    }
    return st;
}

ano_crypto_status_t ano_crypto_random_reseed(
    void *ctx,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_internal_entropy_source_t source;
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

    st = random_init_os_source(&source);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    st = ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
        ctx,
        &source,
        additional_input,
        additional_input_len,
        ANO_CRYPTO_DRBG_FLAG_NONE);
    ano_crypto_internal_zeroize(&source, sizeof(source));
    return st;
}

ano_crypto_status_t ano_crypto_random_uninit(
    void *ctx,
    size_t ctx_len)
{
    return ano_crypto_drbg_uninstantiate(ctx, ctx_len);
}
