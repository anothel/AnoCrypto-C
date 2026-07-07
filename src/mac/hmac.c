#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hmac.h>
#include <anocrypto/ac_module.h>

typedef struct ano_crypto_hmac_ctx_internal_s {
    uint32_t magic;
    uint32_t hash_algorithm;
    uint64_t processed_bytes;
} ano_crypto_hmac_ctx_internal_t;

ano_crypto_status_t ano_crypto_hmac_context_size(
    ano_crypto_hash_algorithm_t hash_algorithm,
    size_t *out_size)
{
    ano_crypto_status_t st;
    size_t digest_len;

    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hash_digest_size(hash_algorithm, &digest_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    (void)digest_len;
    *out_size = sizeof(ano_crypto_hmac_ctx_internal_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hmac_mac_size(
    ano_crypto_hash_algorithm_t hash_algorithm,
    size_t *out_size)
{
    return ano_crypto_hash_digest_size(hash_algorithm, out_size);
}

ano_crypto_status_t ano_crypto_hmac_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len)
{
    size_t required_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || (key == 0 && key_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hmac_context_size(hash_algorithm, &required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_hmac_update(
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

ano_crypto_status_t ano_crypto_hmac_final(
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

ano_crypto_status_t ano_crypto_hmac_compute(
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    size_t mac_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if ((key == 0 && key_len != 0u) || (input == 0 && input_len != 0u) ||
        out == 0 || out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hmac_mac_size(hash_algorithm, &mac_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out_len < mac_len) {
        *out_written = 0u;
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    *out_written = 0u;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}
