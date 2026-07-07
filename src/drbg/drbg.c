#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_drbg.h>
#include <anocrypto/ac_module.h>
#include "ac_drbg_internal.h"
#include "ac_hmac_internal.h"
#include "ac_internal.h"

#define ANO_CRYPTO_DRBG_CTX_MAGIC 0x41434452u /* ACDR */
#define ANO_CRYPTO_HMAC_DRBG_RESEED_INTERVAL 281474976710656ULL /* 2^48 */

/*
 * Internal context layout is intentionally private. Public callers obtain the
 * required size via ano_crypto_drbg_context_size() and pass caller-owned opaque
 * storage.
 */
typedef struct ano_crypto_drbg_ctx_internal_s {
    uint32_t magic;
    uint32_t algorithm;
    uint32_t state;
    uint32_t reserved;
    uint64_t reseed_counter;
    uint8_t key[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE];
    uint8_t value[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE];
} ano_crypto_drbg_ctx_internal_t;

static void drbg_fill(uint8_t *out, size_t out_len, uint8_t value)
{
    size_t i;

    if (out == 0) {
        return;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = value;
    }
}

static int drbg_bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    if (a == 0 || b == 0) {
        return 0;
    }

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
}

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

static int is_absent_or_valid(const uint8_t *data, size_t data_len)
{
    return data != 0 || data_len == 0u;
}

static ano_crypto_status_t reject_unsupported_flags(uint32_t flags)
{
    if (flags == ANO_CRYPTO_DRBG_FLAG_NONE) {
        return ANO_CRYPTO_OK;
    }

    /* Prediction resistance needs an entropy adapter and is intentionally TODO. */
    if ((flags & ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
}

static ano_crypto_status_t validate_entropy_nonce(
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *nonce,
    size_t nonce_len)
{
    if (entropy_input == 0 || entropy_input_len < ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE) {
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    if (nonce == 0 || nonce_len < ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t validate_reseed_entropy(
    const uint8_t *entropy_input,
    size_t entropy_input_len)
{
    if (entropy_input == 0 || entropy_input_len < ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE) {
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t hmac_sha256_pieces(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *p0,
    size_t p0_len,
    const uint8_t *p1,
    size_t p1_len,
    const uint8_t *p2,
    size_t p2_len,
    const uint8_t *p3,
    size_t p3_len,
    const uint8_t *p4,
    size_t p4_len,
    uint8_t out[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE])
{
    ano_crypto_internal_hmac_sha256_ctx_t hmac;
    ano_crypto_status_t st;

    if ((key == 0 && key_len != 0u) ||
        (p0 == 0 && p0_len != 0u) ||
        (p1 == 0 && p1_len != 0u) ||
        (p2 == 0 && p2_len != 0u) ||
        (p3 == 0 && p3_len != 0u) ||
        (p4 == 0 && p4_len != 0u) ||
        out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_hmac_sha256_init(&hmac, key, key_len);
    if (st == ANO_CRYPTO_OK && p0_len != 0u) {
        st = ano_crypto_internal_hmac_sha256_update(&hmac, p0, p0_len);
    }
    if (st == ANO_CRYPTO_OK && p1_len != 0u) {
        st = ano_crypto_internal_hmac_sha256_update(&hmac, p1, p1_len);
    }
    if (st == ANO_CRYPTO_OK && p2_len != 0u) {
        st = ano_crypto_internal_hmac_sha256_update(&hmac, p2, p2_len);
    }
    if (st == ANO_CRYPTO_OK && p3_len != 0u) {
        st = ano_crypto_internal_hmac_sha256_update(&hmac, p3, p3_len);
    }
    if (st == ANO_CRYPTO_OK && p4_len != 0u) {
        st = ano_crypto_internal_hmac_sha256_update(&hmac, p4, p4_len);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_final(&hmac, out);
    } else {
        ano_crypto_internal_zeroize(&hmac, sizeof(hmac));
    }

    return st;
}

static ano_crypto_status_t hmac_drbg_update(
    ano_crypto_drbg_ctx_internal_t *ctx,
    const uint8_t *p0,
    size_t p0_len,
    const uint8_t *p1,
    size_t p1_len,
    const uint8_t *p2,
    size_t p2_len)
{
    static const uint8_t sep0 = 0x00u;
    static const uint8_t sep1 = 0x01u;
    uint8_t next_key[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE];
    uint8_t next_value[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE];
    ano_crypto_status_t st;
    int provided_data_present;

    if (ctx == 0 || (p0 == 0 && p0_len != 0u) ||
        (p1 == 0 && p1_len != 0u) || (p2 == 0 && p2_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    provided_data_present = (p0_len != 0u || p1_len != 0u || p2_len != 0u) ? 1 : 0;

    st = hmac_sha256_pieces(
        ctx->key, sizeof(ctx->key),
        ctx->value, sizeof(ctx->value),
        &sep0, sizeof(sep0),
        p0, p0_len,
        p1, p1_len,
        p2, p2_len,
        next_key);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(next_key, sizeof(next_key));
        return st;
    }

    st = ano_crypto_internal_hmac_sha256_compute(
        next_key, sizeof(next_key), ctx->value, sizeof(ctx->value), next_value);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(next_key, sizeof(next_key));
        ano_crypto_internal_zeroize(next_value, sizeof(next_value));
        return st;
    }

    memcpy(ctx->key, next_key, sizeof(ctx->key));
    memcpy(ctx->value, next_value, sizeof(ctx->value));

    if (provided_data_present != 0) {
        st = hmac_sha256_pieces(
            ctx->key, sizeof(ctx->key),
            ctx->value, sizeof(ctx->value),
            &sep1, sizeof(sep1),
            p0, p0_len,
            p1, p1_len,
            p2, p2_len,
            next_key);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(next_key, sizeof(next_key));
            ano_crypto_internal_zeroize(next_value, sizeof(next_value));
            return st;
        }

        st = ano_crypto_internal_hmac_sha256_compute(
            next_key, sizeof(next_key), ctx->value, sizeof(ctx->value), next_value);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(next_key, sizeof(next_key));
            ano_crypto_internal_zeroize(next_value, sizeof(next_value));
            return st;
        }

        memcpy(ctx->key, next_key, sizeof(ctx->key));
        memcpy(ctx->value, next_value, sizeof(ctx->value));
    }

    ano_crypto_internal_zeroize(next_key, sizeof(next_key));
    ano_crypto_internal_zeroize(next_value, sizeof(next_value));
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t hmac_drbg_instantiate_core(
    ano_crypto_drbg_ctx_internal_t *ctx,
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *personalization,
    size_t personalization_len)
{
    ano_crypto_status_t st;

    if (ctx == 0 || !is_absent_or_valid(personalization, personalization_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = validate_entropy_nonce(entropy_input, entropy_input_len, nonce, nonce_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    ctx->magic = ANO_CRYPTO_DRBG_CTX_MAGIC;
    ctx->algorithm = (uint32_t)ANO_CRYPTO_DRBG_ALG_HMAC_SHA256;
    ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED;
    ctx->reseed_counter = 0u;
    drbg_fill(ctx->key, sizeof(ctx->key), 0x00u);
    drbg_fill(ctx->value, sizeof(ctx->value), 0x01u);

    st = hmac_drbg_update(
        ctx,
        entropy_input, entropy_input_len,
        nonce, nonce_len,
        personalization, personalization_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return st;
    }

    ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_INSTANTIATED;
    ctx->reseed_counter = 1u;
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t hmac_drbg_reseed_core(
    ano_crypto_drbg_ctx_internal_t *ctx,
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *additional_input,
    size_t additional_input_len)
{
    ano_crypto_status_t st;

    if (ctx == 0 || !is_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx->magic != ANO_CRYPTO_DRBG_CTX_MAGIC ||
        ctx->algorithm != (uint32_t)ANO_CRYPTO_DRBG_ALG_HMAC_SHA256) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx->state != (uint32_t)ANO_CRYPTO_DRBG_STATE_INSTANTIATED) {
        return ANO_CRYPTO_ERR_NOT_INITIALIZED;
    }

    st = validate_reseed_entropy(entropy_input, entropy_input_len);
    if (st != ANO_CRYPTO_OK) {
        ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_ERROR;
        return st;
    }

    st = hmac_drbg_update(ctx, entropy_input, entropy_input_len, additional_input, additional_input_len, 0, 0u);
    if (st != ANO_CRYPTO_OK) {
        ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_ERROR;
        return st;
    }

    ctx->reseed_counter = 1u;
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t hmac_drbg_generate_core(
    ano_crypto_drbg_ctx_internal_t *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len)
{
    size_t produced;
    ano_crypto_status_t st;

    if (ctx == 0 || out == 0 || out_len == 0u ||
        !is_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (out_len > ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (ctx->magic != ANO_CRYPTO_DRBG_CTX_MAGIC ||
        ctx->algorithm != (uint32_t)ANO_CRYPTO_DRBG_ALG_HMAC_SHA256) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx->state != (uint32_t)ANO_CRYPTO_DRBG_STATE_INSTANTIATED) {
        return ANO_CRYPTO_ERR_NOT_INITIALIZED;
    }

    if (ctx->reseed_counter > ANO_CRYPTO_HMAC_DRBG_RESEED_INTERVAL) {
        return ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED;
    }

    if (additional_input_len != 0u) {
        st = hmac_drbg_update(ctx, additional_input, additional_input_len, 0, 0u, 0, 0u);
        if (st != ANO_CRYPTO_OK) {
            ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_ERROR;
            return st;
        }
    }

    produced = 0u;
    while (produced < out_len) {
        size_t remaining;
        size_t copy_len;

        st = ano_crypto_internal_hmac_sha256_compute(
            ctx->key, sizeof(ctx->key), ctx->value, sizeof(ctx->value), ctx->value);
        if (st != ANO_CRYPTO_OK) {
            ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_ERROR;
            return st;
        }

        remaining = out_len - produced;
        copy_len = remaining < sizeof(ctx->value) ? remaining : sizeof(ctx->value);
        memcpy(out + produced, ctx->value, copy_len);
        produced += copy_len;
    }

    st = hmac_drbg_update(ctx, additional_input, additional_input_len, 0, 0u, 0, 0u);
    if (st != ANO_CRYPTO_OK) {
        ctx->state = (uint32_t)ANO_CRYPTO_DRBG_STATE_ERROR;
        return st;
    }

    ctx->reseed_counter += 1u;
    return ANO_CRYPTO_OK;
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
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *nonce,
    size_t nonce_len,
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

    if (ctx == 0 || !is_absent_or_valid(personalization, personalization_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = reject_unsupported_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    st = ano_crypto_drbg_context_size(algorithm, &required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    if (algorithm != ANO_CRYPTO_DRBG_ALG_HMAC_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return hmac_drbg_instantiate_core(
        (ano_crypto_drbg_ctx_internal_t *)ctx,
        entropy_input, entropy_input_len,
        nonce, nonce_len,
        personalization, personalization_len);
}

ano_crypto_status_t ano_crypto_drbg_reseed(
    void *ctx,
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags)
{
    ano_crypto_status_t st;
    ano_crypto_drbg_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || !is_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = reject_unsupported_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    internal_ctx = (ano_crypto_drbg_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_DRBG_CTX_MAGIC) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (internal_ctx->algorithm != (uint32_t)ANO_CRYPTO_DRBG_ALG_HMAC_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return hmac_drbg_reseed_core(internal_ctx, entropy_input, entropy_input_len, additional_input, additional_input_len);
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
    ano_crypto_drbg_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || out == 0 || out_len == 0u ||
        !is_absent_or_valid(additional_input, additional_input_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = reject_unsupported_flags(flags);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    internal_ctx = (ano_crypto_drbg_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_DRBG_CTX_MAGIC) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (internal_ctx->algorithm != (uint32_t)ANO_CRYPTO_DRBG_ALG_HMAC_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return hmac_drbg_generate_core(internal_ctx, out, out_len, additional_input, additional_input_len);
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

ano_crypto_status_t ano_crypto_drbg_get_state(
    const void *ctx,
    ano_crypto_drbg_state_t *out_state)
{
    const ano_crypto_drbg_ctx_internal_t *internal_ctx;

    if (out_state == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx == 0) {
        *out_state = ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED;
        return ANO_CRYPTO_OK;
    }

    internal_ctx = (const ano_crypto_drbg_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_DRBG_CTX_MAGIC) {
        *out_state = ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED;
        return ANO_CRYPTO_OK;
    }

    switch ((ano_crypto_drbg_state_t)internal_ctx->state) {
    case ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED:
    case ANO_CRYPTO_DRBG_STATE_INSTANTIATED:
    case ANO_CRYPTO_DRBG_STATE_ERROR:
        *out_state = (ano_crypto_drbg_state_t)internal_ctx->state;
        return ANO_CRYPTO_OK;
    default:
        *out_state = ANO_CRYPTO_DRBG_STATE_ERROR;
        return ANO_CRYPTO_OK;
    }
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

ano_crypto_status_t ano_crypto_internal_hmac_drbg_sha256_selftest(void)
{
    static const uint8_t entropy_input[32] = {
        0x05u, 0xacu, 0x9fu, 0xc4u, 0xc6u, 0x2au, 0x02u, 0xe3u,
        0xf9u, 0x08u, 0x40u, 0xdau, 0x56u, 0x16u, 0x21u, 0x8cu,
        0x6du, 0xe5u, 0x74u, 0x3du, 0x66u, 0xb8u, 0xe0u, 0xfbu,
        0xf8u, 0x33u, 0x75u, 0x9cu, 0x59u, 0x28u, 0xb5u, 0x3du
    };
    static const uint8_t nonce[16] = {
        0x2bu, 0x89u, 0xa1u, 0x79u, 0x04u, 0x92u, 0x2eu, 0xd8u,
        0xf0u, 0x17u, 0xa6u, 0x30u, 0x44u, 0x84u, 0x85u, 0x45u
    };
    static const uint8_t entropy_input_reseed[32] = {
        0x27u, 0x91u, 0x12u, 0x6bu, 0x8bu, 0x52u, 0xeeu, 0x1fu,
        0xd9u, 0x39u, 0x2au, 0x0au, 0x13u, 0xe0u, 0x08u, 0x3bu,
        0xedu, 0x41u, 0x86u, 0xdcu, 0x64u, 0x9bu, 0x73u, 0x96u,
        0x07u, 0xacu, 0x70u, 0xecu, 0x8du, 0xceu, 0xcfu, 0x9bu
    };
    static const uint8_t additional_input_reseed[32] = {
        0x43u, 0xbau, 0xc1u, 0x3bu, 0xaeu, 0x71u, 0x50u, 0x92u,
        0xcfu, 0x7eu, 0xb2u, 0x80u, 0xa2u, 0xe1u, 0x0au, 0x96u,
        0x2fu, 0xafu, 0x72u, 0x33u, 0xc4u, 0x14u, 0x12u, 0xf6u,
        0x9bu, 0xc7u, 0x4au, 0x35u, 0xa5u, 0x84u, 0xe5u, 0x4cu
    };
    static const uint8_t additional_input_1[32] = {
        0x3fu, 0x2fu, 0xedu, 0x4bu, 0x68u, 0xd5u, 0x06u, 0xecu,
        0xefu, 0xa2u, 0x1fu, 0x3fu, 0x5bu, 0xb9u, 0x07u, 0xbeu,
        0xb0u, 0xf1u, 0x7du, 0xbcu, 0x30u, 0xf6u, 0xffu, 0xbbu,
        0xa5u, 0xe5u, 0x86u, 0x14u, 0x08u, 0xc5u, 0x3au, 0x1eu
    };
    static const uint8_t additional_input_2[32] = {
        0x52u, 0x90u, 0x30u, 0xdfu, 0x50u, 0xf4u, 0x10u, 0x98u,
        0x5fu, 0xdeu, 0x06u, 0x8du, 0xf8u, 0x2bu, 0x93u, 0x5eu,
        0xc2u, 0x3du, 0x83u, 0x9cu, 0xb4u, 0xb2u, 0x69u, 0x41u,
        0x4cu, 0x0eu, 0xdeu, 0x6cu, 0xffu, 0xeau, 0x5bu, 0x68u
    };
    static const uint8_t expected[128] = {
        0x02u, 0xddu, 0xffu, 0x51u, 0x73u, 0xdau, 0x2fu, 0xcfu,
        0xfau, 0x10u, 0x21u, 0x5bu, 0x03u, 0x0du, 0x66u, 0x0du,
        0x61u, 0x17u, 0x9eu, 0x61u, 0xecu, 0xc2u, 0x26u, 0x09u,
        0xb1u, 0x15u, 0x1au, 0x75u, 0xf1u, 0xcbu, 0xcbu, 0xb4u,
        0x36u, 0x3cu, 0x3au, 0x89u, 0x29u, 0x9bu, 0x4bu, 0x63u,
        0xacu, 0xa5u, 0xe5u, 0x81u, 0xe7u, 0x3cu, 0x86u, 0x04u,
        0x91u, 0x01u, 0x0au, 0xa3u, 0x5du, 0xe3u, 0x33u, 0x7cu,
        0xc6u, 0xc0u, 0x9eu, 0xbeu, 0xc8u, 0xc9u, 0x1au, 0x62u,
        0x87u, 0x58u, 0x6fu, 0x3au, 0x74u, 0xd9u, 0x69u, 0x4bu,
        0x46u, 0x2du, 0x27u, 0x20u, 0xeau, 0x2eu, 0x11u, 0xbbu,
        0xd0u, 0x2au, 0xf3u, 0x3au, 0xdeu, 0xfbu, 0x4au, 0x16u,
        0xe6u, 0xb3u, 0x70u, 0xfau, 0x0eu, 0xffu, 0xd5u, 0x7du,
        0x60u, 0x75u, 0x47u, 0xbdu, 0xcfu, 0xbbu, 0x78u, 0x31u,
        0xf5u, 0x4du, 0xe7u, 0x07u, 0x3au, 0xd2u, 0xa7u, 0xdau,
        0x98u, 0x7au, 0x00u, 0x16u, 0xa8u, 0x2fu, 0xa9u, 0x58u,
        0x77u, 0x9au, 0x16u, 0x86u, 0x74u, 0xb5u, 0x65u, 0x24u
    };
    ano_crypto_drbg_ctx_internal_t ctx;
    uint8_t generated[128];
    ano_crypto_status_t st;

    st = hmac_drbg_instantiate_core(
        &ctx, entropy_input, sizeof(entropy_input), nonce, sizeof(nonce), 0, 0u);
    if (st == ANO_CRYPTO_OK) {
        st = hmac_drbg_reseed_core(
            &ctx,
            entropy_input_reseed, sizeof(entropy_input_reseed),
            additional_input_reseed, sizeof(additional_input_reseed));
    }
    if (st == ANO_CRYPTO_OK) {
        st = hmac_drbg_generate_core(
            &ctx, generated, sizeof(generated), additional_input_1, sizeof(additional_input_1));
    }
    if (st == ANO_CRYPTO_OK) {
        st = hmac_drbg_generate_core(
            &ctx, generated, sizeof(generated), additional_input_2, sizeof(additional_input_2));
    }

    if (st != ANO_CRYPTO_OK || !drbg_bytes_equal(generated, expected, sizeof(expected))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(generated, sizeof(generated));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    ano_crypto_internal_zeroize(generated, sizeof(generated));
    return ANO_CRYPTO_OK;
}
