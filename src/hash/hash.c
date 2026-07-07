#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_module.h>

typedef struct ano_crypto_hash_ctx_internal_s {
    uint32_t magic;
    uint32_t algorithm;
    uint64_t processed_bytes;
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
        *out_size = 32u;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_HASH_ALG_SHA384:
        *out_size = 48u;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_HASH_ALG_SHA512:
        *out_size = 64u;
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

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_hash_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_hash_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || out == 0 || out_written == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_written = 0u;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
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

    st = ano_crypto_hash_digest_size(algorithm, &digest_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out_len < digest_len) {
        *out_written = 0u;
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    *out_written = 0u;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
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
