#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_cipher.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>
#include "ac_cipher_internal.h"
#include "ac_csp_internal.h"
#include "ac_internal.h"

static int ano_crypto_cipher_algorithm_supported(
    ano_crypto_cipher_algorithm_t algorithm)
{
    return algorithm == ANO_CRYPTO_CIPHER_ALG_AES;
}

static int ano_crypto_cipher_mode_supported(ano_crypto_cipher_mode_t mode)
{
    return mode == ANO_CRYPTO_CIPHER_MODE_CBC;
}

static int ano_crypto_cipher_direction_supported(
    ano_crypto_cipher_direction_t direction)
{
    return direction == ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT ||
           direction == ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT;
}

static int ano_crypto_cipher_ranges_partially_overlap(
    const uint8_t *input,
    size_t input_len,
    const uint8_t *output)
{
    uintptr_t input_start;
    uintptr_t output_start;
    uintptr_t input_end;
    uintptr_t output_end;

    if (input == output) {
        return 0;
    }

    input_start = (uintptr_t)input;
    output_start = (uintptr_t)output;
    if (input_len > (size_t)(UINTPTR_MAX - input_start) ||
        input_len > (size_t)(UINTPTR_MAX - output_start)) {
        return 1;
    }

    input_end = input_start + (uintptr_t)input_len;
    output_end = output_start + (uintptr_t)input_len;
    return input_start < output_end && output_start < input_end;
}

static ano_crypto_status_t ano_crypto_cipher_check_context(
    void *ctx,
    ano_crypto_internal_cipher_ctx_t **out_ctx)
{
    ano_crypto_internal_cipher_ctx_t *internal_ctx;

    if (ctx == 0 || out_ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    internal_ctx = (ano_crypto_internal_cipher_ctx_t *)ctx;
    if (internal_ctx->magic != ANO_CRYPTO_INTERNAL_CIPHER_CTX_MAGIC ||
        internal_ctx->initialized == 0u ||
        internal_ctx->algorithm != (uint32_t)ANO_CRYPTO_CIPHER_ALG_AES ||
        internal_ctx->mode != (uint32_t)ANO_CRYPTO_CIPHER_MODE_CBC ||
        (internal_ctx->direction != (uint32_t)ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT &&
         internal_ctx->direction != (uint32_t)ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT) ||
        internal_ctx->aes.initialized == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_ctx = internal_ctx;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cipher_context_size(
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_size = 0u;

    if (!ano_crypto_cipher_algorithm_supported(algorithm)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
    if (!ano_crypto_cipher_mode_supported(mode)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    *out_size = sizeof(ano_crypto_internal_cipher_ctx_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cipher_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    ano_crypto_cipher_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *iv,
    size_t iv_len,
    uint32_t flags)
{
    uint8_t key_material[ANO_CRYPTO_CIPHER_AES256_KEY_SIZE];
    size_t key_material_len = 0u;
    size_t required_len = 0u;
    ano_crypto_status_t st;
    ano_crypto_internal_cipher_ctx_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx == 0 || iv == 0 || key_handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (flags != ANO_CRYPTO_CIPHER_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (!ano_crypto_cipher_algorithm_supported(algorithm)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
    if (!ano_crypto_cipher_mode_supported(mode) ||
        !ano_crypto_cipher_direction_supported(direction) ||
        iv_len != ANO_CRYPTO_CIPHER_CBC_IV_SIZE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_cipher_context_size(algorithm, mode, &required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    st = ano_crypto_internal_csp_copy_secret(
        key_handle,
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
        key_material,
        sizeof(key_material),
        &key_material_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(key_material, sizeof(key_material));
        return st;
    }
    if (key_material_len != ANO_CRYPTO_CIPHER_AES128_KEY_SIZE &&
        key_material_len != ANO_CRYPTO_CIPHER_AES192_KEY_SIZE &&
        key_material_len != ANO_CRYPTO_CIPHER_AES256_KEY_SIZE) {
        ano_crypto_internal_zeroize(key_material, sizeof(key_material));
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    internal_ctx = (ano_crypto_internal_cipher_ctx_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    st = ano_crypto_internal_aes_init(
        &internal_ctx->aes, key_material, key_material_len);
    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, required_len);
        return st;
    }

    memcpy(internal_ctx->chain, iv, ANO_CRYPTO_CIPHER_CBC_IV_SIZE);
    internal_ctx->magic = ANO_CRYPTO_INTERNAL_CIPHER_CTX_MAGIC;
    internal_ctx->algorithm = (uint32_t)algorithm;
    internal_ctx->mode = (uint32_t)mode;
    internal_ctx->direction = (uint32_t)direction;
    internal_ctx->initialized = 1u;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cipher_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    ano_crypto_status_t st;
    ano_crypto_internal_cipher_ctx_t *internal_ctx;
    size_t offset;
    uint8_t input_block[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint8_t work_block[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;

    if (input == 0 || out == 0 || input_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if ((input_len % ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (out_len < input_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    if (ano_crypto_cipher_ranges_partially_overlap(input, input_len, out)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_cipher_check_context(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    ano_crypto_internal_zeroize(input_block, sizeof(input_block));
    ano_crypto_internal_zeroize(work_block, sizeof(work_block));
    for (offset = 0u; offset < input_len; offset += ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE) {
        size_t i;
        memcpy(input_block, input + offset, sizeof(input_block));

        if (internal_ctx->direction ==
            (uint32_t)ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT) {
            for (i = 0u; i < sizeof(work_block); ++i) {
                work_block[i] = (uint8_t)(input_block[i] ^ internal_ctx->chain[i]);
            }
            st = ano_crypto_internal_aes_encrypt_block(
                &internal_ctx->aes, work_block, out + offset);
            if (st == ANO_CRYPTO_OK) {
                memcpy(internal_ctx->chain, out + offset, sizeof(internal_ctx->chain));
            }
        } else {
            st = ano_crypto_internal_aes_decrypt_block(
                &internal_ctx->aes, input_block, work_block);
            if (st == ANO_CRYPTO_OK) {
                for (i = 0u; i < sizeof(work_block); ++i) {
                    out[offset + i] =
                        (uint8_t)(work_block[i] ^ internal_ctx->chain[i]);
                }
                memcpy(internal_ctx->chain, input_block, sizeof(internal_ctx->chain));
            }
        }

        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(out, input_len);
            ano_crypto_internal_zeroize(input_block, sizeof(input_block));
            ano_crypto_internal_zeroize(work_block, sizeof(work_block));
            ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
            return st;
        }
    }

    ano_crypto_internal_zeroize(input_block, sizeof(input_block));
    ano_crypto_internal_zeroize(work_block, sizeof(work_block));
    *out_written = input_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cipher_uninit(
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

ano_crypto_status_t ano_crypto_cipher_crypt_with_key(
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    ano_crypto_cipher_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *iv,
    size_t iv_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written,
    uint32_t flags)
{
    ano_crypto_internal_cipher_ctx_t ctx;
    ano_crypto_status_t st;

    if (out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;

    ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    st = ano_crypto_cipher_init(
        &ctx,
        sizeof(ctx),
        algorithm,
        mode,
        direction,
        key_handle,
        iv,
        iv_len,
        flags);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_cipher_update(
            &ctx, input, input_len, out, out_len, out_written);
    }
    ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    return st;
}

const char *ano_crypto_cipher_algorithm_name(
    ano_crypto_cipher_algorithm_t algorithm)
{
    return algorithm == ANO_CRYPTO_CIPHER_ALG_AES ? "AES" :
        "UNKNOWN_CIPHER_ALGORITHM";
}

const char *ano_crypto_cipher_mode_name(ano_crypto_cipher_mode_t mode)
{
    return mode == ANO_CRYPTO_CIPHER_MODE_CBC ? "CBC" :
        "UNKNOWN_CIPHER_MODE";
}

const char *ano_crypto_cipher_direction_name(
    ano_crypto_cipher_direction_t direction)
{
    switch (direction) {
    case ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT:
        return "ENCRYPT";
    case ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT:
        return "DECRYPT";
    default:
        return "UNKNOWN_CIPHER_DIRECTION";
    }
}
