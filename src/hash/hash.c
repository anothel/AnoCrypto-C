#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_module.h>
#include "ac_internal.h"
#include "ac_sha256_internal.h"
#include "ac_sha512_internal.h"

#define ANO_CRYPTO_HASH_CTX_MAGIC 0x41434853u /* ACHS */

typedef union ano_crypto_hash_state_internal_u {
    ano_crypto_internal_sha256_ctx_t sha256;
    ano_crypto_internal_sha512_ctx_t sha512_family;
} ano_crypto_hash_state_internal_t;

typedef struct ano_crypto_hash_ctx_internal_s {
    uint32_t magic;
    uint32_t algorithm;
    ano_crypto_hash_state_internal_t state;
} ano_crypto_hash_ctx_internal_t;

static ano_crypto_status_t ano_crypto_hash_validate_algorithm(
    ano_crypto_hash_algorithm_t algorithm)
{
    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
    case ANO_CRYPTO_HASH_ALG_SHA384:
    case ANO_CRYPTO_HASH_ALG_SHA512:
        return ANO_CRYPTO_OK;
    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
}

static ano_crypto_status_t ano_crypto_hash_check_ctx(
    void *ctx,
    ano_crypto_hash_ctx_internal_t **out_ctx)
{
    ano_crypto_hash_ctx_internal_t *internal_ctx;

    if (ctx == 0 || out_ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    internal_ctx = (ano_crypto_hash_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_HASH_CTX_MAGIC) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ano_crypto_hash_validate_algorithm(
            (ano_crypto_hash_algorithm_t)internal_ctx->algorithm) != ANO_CRYPTO_OK) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_ctx = internal_ctx;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hash_context_size(
    ano_crypto_hash_algorithm_t algorithm,
    size_t *out_size)
{
    ano_crypto_status_t st;

    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hash_validate_algorithm(algorithm);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_size = sizeof(ano_crypto_hash_ctx_internal_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hash_digest_size(
    ano_crypto_hash_algorithm_t algorithm,
    size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        *out_size = ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_HASH_ALG_SHA384:
        *out_size = ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_HASH_ALG_SHA512:
        *out_size = ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE;
        return ANO_CRYPTO_OK;
    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
}

ano_crypto_status_t ano_crypto_hash_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_hash_algorithm_t algorithm)
{
    size_t required_len;
    ano_crypto_status_t st;
    ano_crypto_hash_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hash_context_size(algorithm, &required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    internal_ctx = (ano_crypto_hash_ctx_internal_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    internal_ctx->magic = ANO_CRYPTO_HASH_CTX_MAGIC;
    internal_ctx->algorithm = (uint32_t)algorithm;

    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        ano_crypto_internal_sha256_init(&internal_ctx->state.sha256);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA384:
        ano_crypto_internal_sha384_init(&internal_ctx->state.sha512_family);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA512:
        ano_crypto_internal_sha512_init(&internal_ctx->state.sha512_family);
        break;
    default:
        ano_crypto_internal_zeroize(internal_ctx, required_len);
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hash_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len)
{
    ano_crypto_status_t st;
    ano_crypto_hash_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (input == 0 && input_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hash_check_ctx(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    switch ((ano_crypto_hash_algorithm_t)internal_ctx->algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        return ano_crypto_internal_sha256_update(
            &internal_ctx->state.sha256, input, input_len);
    case ANO_CRYPTO_HASH_ALG_SHA384:
    case ANO_CRYPTO_HASH_ALG_SHA512:
        return ano_crypto_internal_sha512_update(
            &internal_ctx->state.sha512_family, input, input_len);
    default:
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
}

ano_crypto_status_t ano_crypto_hash_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    size_t digest_len;
    ano_crypto_status_t st;
    ano_crypto_hash_ctx_internal_t *internal_ctx;
    ano_crypto_hash_algorithm_t algorithm;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out == 0 || out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_written = 0u;

    st = ano_crypto_hash_check_ctx(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    algorithm = (ano_crypto_hash_algorithm_t)internal_ctx->algorithm;
    st = ano_crypto_hash_digest_size(algorithm, &digest_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (out_len < digest_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        st = ano_crypto_internal_sha256_final(&internal_ctx->state.sha256, out);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA384:
        st = ano_crypto_internal_sha384_final(&internal_ctx->state.sha512_family, out);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA512:
        st = ano_crypto_internal_sha512_final(&internal_ctx->state.sha512_family, out);
        break;
    default:
        st = ANO_CRYPTO_ERR_INVALID_ARGUMENT;
        break;
    }

    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return st;
    }

    *out_written = digest_len;
    ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hash_digest(
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    size_t digest_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if ((input == 0 && input_len != 0u) || out == 0 || out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_written = 0u;

    st = ano_crypto_hash_digest_size(algorithm, &digest_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (out_len < digest_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        st = ano_crypto_internal_sha256_digest(input, input_len, out);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA384:
        st = ano_crypto_internal_sha384_digest(input, input_len, out);
        break;
    case ANO_CRYPTO_HASH_ALG_SHA512:
        st = ano_crypto_internal_sha512_digest(input, input_len, out);
        break;
    default:
        st = ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
        break;
    }

    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_written = digest_len;
    return ANO_CRYPTO_OK;
}

const char *ano_crypto_hash_algorithm_name(
    ano_crypto_hash_algorithm_t algorithm)
{
    switch (algorithm) {
    case ANO_CRYPTO_HASH_ALG_SHA256:
        return "SHA-256";
    case ANO_CRYPTO_HASH_ALG_SHA384:
        return "SHA-384";
    case ANO_CRYPTO_HASH_ALG_SHA512:
        return "SHA-512";
    default:
        return "UNKNOWN_HASH_ALGORITHM";
    }
}
