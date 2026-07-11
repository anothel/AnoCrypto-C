#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hmac.h>
#include <anocrypto/ac_module.h>
#include "ac_internal.h"
#include "ac_csp_internal.h"
#include "ac_hmac_internal.h"

#define ANO_CRYPTO_HMAC_CTX_MAGIC 0x4143484du /* ACHM */

typedef union ano_crypto_hmac_state_internal_u {
    ano_crypto_internal_hmac_sha256_ctx_t sha256;
    ano_crypto_internal_hmac_sha512_family_ctx_t sha512_family;
} ano_crypto_hmac_state_internal_t;

typedef struct ano_crypto_hmac_ctx_internal_s {
    uint32_t magic;
    uint32_t hash_algorithm;
    ano_crypto_hmac_state_internal_t state;
} ano_crypto_hmac_ctx_internal_t;

static int ano_crypto_hmac_algorithm_supported(
    ano_crypto_hash_algorithm_t hash_algorithm)
{
    return hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA256 ||
           hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384 ||
           hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA512;
}

static ano_crypto_status_t ano_crypto_hmac_check_ctx(
    void *ctx,
    ano_crypto_hmac_ctx_internal_t **out_ctx)
{
    ano_crypto_hmac_ctx_internal_t *internal_ctx;

    if (ctx == 0 || out_ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    internal_ctx = (ano_crypto_hmac_ctx_internal_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_HMAC_CTX_MAGIC ||
        !ano_crypto_hmac_algorithm_supported(
            (ano_crypto_hash_algorithm_t)internal_ctx->hash_algorithm)) {
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

static size_t ano_crypto_hmac_sha512_family_digest_size(
    ano_crypto_hash_algorithm_t hash_algorithm)
{
    return hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384 ?
        ANO_CRYPTO_HMAC_SHA384_MAC_SIZE : ANO_CRYPTO_HMAC_SHA512_MAC_SIZE;
}

static void ano_crypto_hmac_sha512_family_hash_init(
    ano_crypto_internal_sha512_ctx_t *ctx,
    ano_crypto_hash_algorithm_t hash_algorithm)
{
    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384) {
        ano_crypto_internal_sha384_init(ctx);
    } else {
        ano_crypto_internal_sha512_init(ctx);
    }
}

static ano_crypto_status_t ano_crypto_hmac_sha512_family_hash_final(
    ano_crypto_internal_sha512_ctx_t *ctx,
    ano_crypto_hash_algorithm_t hash_algorithm,
    uint8_t *out)
{
    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384) {
        return ano_crypto_internal_sha384_final(ctx, out);
    }
    return ano_crypto_internal_sha512_final(ctx, out);
}

ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_init(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len)
{
    uint8_t key_block[ANO_CRYPTO_HMAC_SHA512_BLOCK_SIZE];
    uint8_t hashed_key[ANO_CRYPTO_HMAC_SHA512_MAC_SIZE];
    size_t digest_len;
    size_t i;
    ano_crypto_status_t st;

    if (ctx == 0 || (key == 0 && key_len != 0u) ||
        (hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA384 &&
         hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA512)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    digest_len = ano_crypto_hmac_sha512_family_digest_size(hash_algorithm);
    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    ano_crypto_internal_zeroize(key_block, sizeof(key_block));
    ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));

    if (key_len > ANO_CRYPTO_HMAC_SHA512_BLOCK_SIZE) {
        if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384) {
            st = ano_crypto_internal_sha384_digest(key, key_len, hashed_key);
        } else {
            st = ano_crypto_internal_sha512_digest(key, key_len, hashed_key);
        }
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(hashed_key, sizeof(hashed_key));
            return st;
        }
        for (i = 0u; i < digest_len; ++i) {
            key_block[i] = hashed_key[i];
        }
    } else if (key_len != 0u) {
        for (i = 0u; i < key_len; ++i) {
            key_block[i] = key[i];
        }
    }

    for (i = 0u; i < ANO_CRYPTO_HMAC_SHA512_BLOCK_SIZE; ++i) {
        uint8_t kb = key_block[i];
        ctx->outer_pad[i] = (uint8_t)(kb ^ 0x5cu);
        key_block[i] = (uint8_t)(kb ^ 0x36u);
    }

    ctx->hash_algorithm = (uint32_t)hash_algorithm;
    ano_crypto_hmac_sha512_family_hash_init(&ctx->inner, hash_algorithm);
    st = ano_crypto_internal_sha512_update(&ctx->inner, key_block, sizeof(key_block));
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

ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_update(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    if (ctx == 0 || ctx->initialized == 0 ||
        (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    return ano_crypto_internal_sha512_update(&ctx->inner, input, input_len);
}

ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_final(
    ano_crypto_internal_hmac_sha512_family_ctx_t *ctx,
    uint8_t *out)
{
    uint8_t inner_digest[ANO_CRYPTO_HMAC_SHA512_MAC_SIZE];
    ano_crypto_internal_sha512_ctx_t outer;
    ano_crypto_hash_algorithm_t hash_algorithm;
    size_t digest_len;
    ano_crypto_status_t st;

    if (ctx == 0 || out == 0 || ctx->initialized == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    hash_algorithm = (ano_crypto_hash_algorithm_t)ctx->hash_algorithm;
    if (hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA384 &&
        hash_algorithm != ANO_CRYPTO_HASH_ALG_SHA512) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    digest_len = ano_crypto_hmac_sha512_family_digest_size(hash_algorithm);

    ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
    st = ano_crypto_hmac_sha512_family_hash_final(
        &ctx->inner, hash_algorithm, inner_digest);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return st;
    }

    ano_crypto_hmac_sha512_family_hash_init(&outer, hash_algorithm);
    st = ano_crypto_internal_sha512_update(
        &outer, ctx->outer_pad, sizeof(ctx->outer_pad));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha512_update(&outer, inner_digest, digest_len);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_hmac_sha512_family_hash_final(&outer, hash_algorithm, out);
    } else {
        ano_crypto_internal_zeroize(&outer, sizeof(outer));
    }

    ano_crypto_internal_zeroize(inner_digest, sizeof(inner_digest));
    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    return st;
}

ano_crypto_status_t ano_crypto_internal_hmac_sha512_family_compute(
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out)
{
    ano_crypto_internal_hmac_sha512_family_ctx_t ctx;
    ano_crypto_status_t st;

    if ((key == 0 && key_len != 0u) || (input == 0 && input_len != 0u) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_hmac_sha512_family_init(
        &ctx, hash_algorithm, key, key_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        return st;
    }

    st = ano_crypto_internal_hmac_sha512_family_update(&ctx, input, input_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        return st;
    }

    st = ano_crypto_internal_hmac_sha512_family_final(&ctx, out);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    }
    return st;
}

ano_crypto_status_t ano_crypto_hmac_context_size(
    ano_crypto_hash_algorithm_t hash_algorithm,
    size_t *out_size)
{
    size_t digest_len;
    ano_crypto_status_t st;

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

    internal_ctx = (ano_crypto_hmac_ctx_internal_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    internal_ctx->magic = ANO_CRYPTO_HMAC_CTX_MAGIC;
    internal_ctx->hash_algorithm = (uint32_t)hash_algorithm;

    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA256) {
        st = ano_crypto_internal_hmac_sha256_init(
            &internal_ctx->state.sha256, key, key_len);
    } else if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384 ||
               hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA512) {
        st = ano_crypto_internal_hmac_sha512_family_init(
            &internal_ctx->state.sha512_family, hash_algorithm, key, key_len);
    } else {
        st = ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }

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

    if (internal_ctx->hash_algorithm == (uint32_t)ANO_CRYPTO_HASH_ALG_SHA256) {
        return ano_crypto_internal_hmac_sha256_update(
            &internal_ctx->state.sha256, input, input_len);
    }
    return ano_crypto_internal_hmac_sha512_family_update(
        &internal_ctx->state.sha512_family, input, input_len);
}

ano_crypto_status_t ano_crypto_hmac_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    size_t mac_len;
    ano_crypto_status_t st;
    ano_crypto_hmac_ctx_internal_t *internal_ctx;
    ano_crypto_hash_algorithm_t hash_algorithm;

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

    hash_algorithm = (ano_crypto_hash_algorithm_t)internal_ctx->hash_algorithm;
    st = ano_crypto_hmac_mac_size(hash_algorithm, &mac_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (out_len < mac_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA256) {
        st = ano_crypto_internal_hmac_sha256_final(
            &internal_ctx->state.sha256, out);
    } else {
        st = ano_crypto_internal_hmac_sha512_family_final(
            &internal_ctx->state.sha512_family, out);
    }

    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return st;
    }

    *out_written = mac_len;
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

    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA256) {
        st = ano_crypto_internal_hmac_sha256_compute(
            key, key_len, input, input_len, out);
    } else if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA384 ||
               hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA512) {
        st = ano_crypto_internal_hmac_sha512_family_compute(
            hash_algorithm, key, key_len, input, input_len, out);
    } else {
        st = ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }

    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_written = mac_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_hmac_compute_with_key(
    ano_crypto_hash_algorithm_t hash_algorithm,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    uint8_t key_material[ANO_CRYPTO_INTERNAL_CSP_MAX_SECRET_BYTES];
    size_t key_material_len = 0u;
    size_t mac_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if ((input == 0 && input_len != 0u) || out == 0 || out_written == 0) {
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

    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    st = ano_crypto_internal_csp_copy_secret(
        key_handle,
        ANO_CRYPTO_KEY_TYPE_HMAC,
        key_material,
        sizeof(key_material),
        &key_material_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(key_material, sizeof(key_material));
        return st;
    }

    if (hash_algorithm == ANO_CRYPTO_HASH_ALG_SHA256) {
        st = ano_crypto_internal_hmac_sha256_compute(
            key_material, key_material_len, input, input_len, out);
    } else {
        st = ano_crypto_internal_hmac_sha512_family_compute(
            hash_algorithm, key_material, key_material_len, input, input_len, out);
    }
    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_written = mac_len;
    return ANO_CRYPTO_OK;
}
