#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hmac.h>
#include <anocrypto/ac_module.h>
#include "ac_internal.h"
#include "ac_hmac_internal.h"

#define ANO_CRYPTO_HMAC_CTX_MAGIC 0x4143484du /* ACHM */

typedef struct ano_crypto_hmac_ctx_internal_s {
    uint32_t magic;
    uint32_t hash_algorithm;
    ano_crypto_internal_hmac_sha256_ctx_t hmac_sha256;
} ano_crypto_hmac_ctx_internal_t;

static ano_crypto_status_t ano_crypto_hmac_check_ctx(
    void *ctx,
    ano_crypto_hmac_ctx_internal_t **out_ctx)
{
    ano_crypto_hmac_ctx_internal_t *internal_ctx;

    if (ctx == 0 || out_ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    internal_ctx = (ano_crypto_hmac_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_HMAC_CTX_MAGIC) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_ctx = internal_ctx;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_hmac_sha256_init(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len)
{
    uint8_t key_block[ANO_CRYPTO_HMAC_SHA256_BLOCK_SIZE];
    uint8_t hashed_key[ANO_CRYPTO_SHA256_DIGEST_SIZE];
    size_t i;
    ano_crypto_status_t st;

    if (ctx == 0 || (key == 0 && key_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    ano_crypto_internal_zeroize(key_block, sizeof(key_block));
    ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));

    if (key_len > ANO_CRYPTO_HMAC_SHA256_BLOCK_SIZE) {
        st = ano_crypto_internal_sha256_digest(key, key_len, hashed_key);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));
            return st;
        }
        for (i = 0u; i < ANO_CRYPTO_SHA256_DIGEST_SIZE; ++i) {
            key_block[i] = hashed_key[i];
        }
    } else if (key_len != 0u) {
        for (i = 0u; i < key_len; ++i) {
            key_block[i] = key[i];
        }
    }

    for (i = 0u; i < ANO_CRYPTO_HMAC_SHA256_BLOCK_SIZE; ++i) {
        uint8_t kb = key_block[i];
        ctx->outer_pad[i] = (uint8_t)(kb ^ 0x5cu);
        key_block[i] = (uint8_t)(kb ^ 0x36u);
    }

    ano_crypto_internal_sha256_init(&ctx->inner);
    st = ano_crypto_internal_sha256_update(&ctx->inner, key_block, sizeof(key_block));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        ano_crypto_internal_zeroize(key_block, sizeof(key_block));
        ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));
        return st;
    }

    ctx->initialized = 1;
    ano_crypto_internal_zeroize(key_block, sizeof(key_block));
    ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_hmac_sha256_update(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    if (ctx == 0 || ctx->initialized == 0 || (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    return ano_crypto_internal_sha256_update(&ctx->inner, input, input_len);
}

ano_crypto_status_t ano_crypto_internal_hmac_sha256_final(
    ano_crypto_internal_hmac_sha256_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE])
{
    uint8_t inner_digest[ANO_CRYPTO_SHA256_DIGEST_SIZE];
    ano_crypto_internal_sha256_ctx_t outer;
    ano_crypto_status_t st;

    if (ctx == 0 || out == 0 || ctx->initialized == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
    st = ano_crypto_internal_sha256_final(&ctx->inner, inner_digest);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return st;
    }

    ano_crypto_internal_sha256_init(&outer);
    st = ano_crypto_internal_sha256_update(&outer, ctx->outer_pad, sizeof(ctx->outer_pad));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_update(&outer, inner_digest, sizeof(inner_digest));
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_final(&outer, out);
    } else {
        ano_crypto_internal_zeroize(&outer, sizeof(outer));
    }

    ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    return st;
}

ano_crypto_status_t ano_crypto_internal_hmac_sha256_compute(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE])
{
    ano_crypto_internal_hmac_sha256_ctx_t ctx;
    ano_crypto_status_t st;

    if ((key == 0 && key_len != 0u) || (input == 0 && input_len != 0u) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_hmac_sha256_init(&ctx, key, key_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        return st;
    }

    st = ano_crypto_internal_hmac_sha256_update(&ctx, input, input_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        return st;
    }

    st = ano_crypto_internal_hmac_sha256_final(&ctx, out);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    }
    return st;
}

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
    ano_crypto_hmac_ctx_internal_t *internal_ctx;

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

    if (hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    internal_ctx = (ano_crypto_hmac_ctx_internal_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    internal_ctx->magic = ANO_CRYPTO_HMAC_CTX_MAGIC;
    internal_ctx->hash_algorithm = (uint32_t)hash_algorithm;

    st = ano_crypto_internal_hmac_sha256_init(&internal_ctx->hmac_sha256, key, key_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return st;
    }

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hmac_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len)
{
    ano_crypto_status_t st;
    ano_crypto_hmac_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (input == 0 && input_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_hmac_check_ctx(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (internal_ctx->hash_algorithm != (uint32_t)ANO_CRYPTO_HASH_ALG_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    return ano_crypto_internal_hmac_sha256_update(&internal_ctx->hmac_sha256, input, input_len);
}

ano_crypto_status_t ano_crypto_hmac_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    ano_crypto_status_t st;
    ano_crypto_hmac_ctx_internal_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out == 0 || out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_written = 0u;

    st = ano_crypto_hmac_check_ctx(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (internal_ctx->hash_algorithm != (uint32_t)ANO_CRYPTO_HASH_ALG_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    if (out_len < ANO_CRYPTO_HMAC_SHA256_MAC_SIZE) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    st = ano_crypto_internal_hmac_sha256_final(&internal_ctx->hmac_sha256, out);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return st;
    }

    *out_written = ANO_CRYPTO_HMAC_SHA256_MAC_SIZE;
    ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
    return ANO_CRYPTO_OK;
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

    *out_written = 0u;

    st = ano_crypto_hmac_mac_size(hash_algorithm, &mac_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out_len < mac_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    if (hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA256) {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    st = ano_crypto_internal_hmac_sha256_compute(key, key_len, input, input_len, out);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_written = ANO_CRYPTO_HMAC_SHA256_MAC_SIZE;
    return ANO_CRYPTO_OK;
}
