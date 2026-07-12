#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_cmac.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>
#include "ac_cmac_internal.h"
#include "ac_csp_internal.h"
#include "ac_internal.h"

static int ano_crypto_cmac_context_valid(
    const ano_crypto_internal_cmac_ctx_t *ctx)
{
    return ctx != 0 &&
           ctx->magic == ANO_CRYPTO_INTERNAL_CMAC_CTX_MAGIC &&
           ctx->initialized != 0u &&
           ctx->pending_len <= ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE &&
           ctx->aes.initialized != 0u;
}

static void ano_crypto_cmac_double_block(
    const uint8_t input[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    uint8_t output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t carry = 0u;
    size_t i;

    for (i = ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE; i > 0u; --i) {
        uint8_t value = input[i - 1u];
        output[i - 1u] = (uint8_t)((uint8_t)(value << 1u) | carry);
        carry = (uint8_t)(value >> 7u);
    }
    output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE - 1u] =
        (uint8_t)(output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE - 1u] ^
                  (uint8_t)(0x87u & (uint8_t)(0u - carry)));
}

static ano_crypto_status_t ano_crypto_cmac_process_nonfinal_block(
    ano_crypto_internal_cmac_ctx_t *ctx)
{
    uint8_t block[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    size_t i;
    ano_crypto_status_t st;

    for (i = 0u; i < sizeof(block); ++i) {
        block[i] = (uint8_t)(ctx->pending[i] ^ ctx->chain[i]);
    }
    st = ano_crypto_internal_aes_encrypt_block(&ctx->aes, block, ctx->chain);
    ano_crypto_internal_zeroize(block, sizeof(block));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_cmac_uninit(ctx);
        return st;
    }

    ano_crypto_internal_zeroize(ctx->pending, sizeof(ctx->pending));
    ctx->pending_len = 0u;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_cmac_init(
    ano_crypto_internal_cmac_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len)
{
    static const uint8_t zero_block[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE] = { 0u };
    uint8_t l[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    ano_crypto_status_t st;

    if (ctx == 0 || key == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    if (key_len != 16u && key_len != 24u && key_len != 32u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_internal_aes_init(&ctx->aes, key, key_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return st;
    }

    st = ano_crypto_internal_aes_encrypt_block(&ctx->aes, zero_block, l);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(l, sizeof(l));
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
        return st;
    }

    ano_crypto_cmac_double_block(l, ctx->subkey1);
    ano_crypto_cmac_double_block(ctx->subkey1, ctx->subkey2);
    ano_crypto_internal_zeroize(l, sizeof(l));
    ctx->magic = ANO_CRYPTO_INTERNAL_CMAC_CTX_MAGIC;
    ctx->initialized = 1u;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_cmac_update(
    ano_crypto_internal_cmac_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    size_t offset = 0u;

    if (!ano_crypto_cmac_context_valid(ctx)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (input_len == 0u) {
        return ANO_CRYPTO_OK;
    }
    if (input == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    while (offset < input_len) {
        size_t available;
        size_t take;
        ano_crypto_status_t st;

        if (ctx->pending_len == ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE) {
            st = ano_crypto_cmac_process_nonfinal_block(ctx);
            if (st != ANO_CRYPTO_OK) {
                return st;
            }
        }

        available = ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE - ctx->pending_len;
        take = input_len - offset;
        if (take > available) {
            take = available;
        }
        memcpy(ctx->pending + ctx->pending_len, input + offset, take);
        ctx->pending_len += take;
        offset += take;
    }

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_cmac_final(
    ano_crypto_internal_cmac_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t last[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint8_t input_block[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    const uint8_t *subkey;
    size_t i;
    ano_crypto_status_t st;

    if (!ano_crypto_cmac_context_valid(ctx) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (ctx->pending_len == ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE) {
        memcpy(last, ctx->pending, sizeof(last));
        subkey = ctx->subkey1;
    } else {
        ano_crypto_internal_zeroize(last, sizeof(last));
        if (ctx->pending_len != 0u) {
            memcpy(last, ctx->pending, ctx->pending_len);
        }
        last[ctx->pending_len] = 0x80u;
        subkey = ctx->subkey2;
    }

    for (i = 0u; i < sizeof(last); ++i) {
        input_block[i] = (uint8_t)(last[i] ^ subkey[i] ^ ctx->chain[i]);
    }

    st = ano_crypto_internal_aes_encrypt_block(&ctx->aes, input_block, out);
    ano_crypto_internal_zeroize(last, sizeof(last));
    ano_crypto_internal_zeroize(input_block, sizeof(input_block));
    ano_crypto_internal_cmac_uninit(ctx);
    return st;
}

ano_crypto_status_t ano_crypto_internal_cmac_compute(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    ano_crypto_internal_cmac_ctx_t ctx;
    ano_crypto_status_t st;

    if (out == 0 || key == 0 || (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    st = ano_crypto_internal_cmac_init(&ctx, key, key_len);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_cmac_update(&ctx, input, input_len);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_cmac_final(&ctx, out);
    } else {
        ano_crypto_internal_cmac_uninit(&ctx);
    }
    return st;
}

void ano_crypto_internal_cmac_uninit(ano_crypto_internal_cmac_ctx_t *ctx)
{
    if (ctx != 0) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    }
}

static int ano_crypto_cmac_tags_equal(
    const uint8_t *a,
    const uint8_t *b,
    size_t len)
{
    uint8_t difference = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(a[i] ^ b[i]));
    }
    return difference == 0u;
}

ano_crypto_status_t ano_crypto_cmac_context_size(size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_size = sizeof(ano_crypto_internal_cmac_ctx_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cmac_mac_size(size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_size = ANO_CRYPTO_CMAC_TAG_SIZE;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cmac_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_csp_handle_t key_handle,
    uint32_t flags)
{
    uint8_t key_material[32];
    size_t key_material_len = 0u;
    size_t required_len = 0u;
    ano_crypto_status_t st;
    ano_crypto_internal_cmac_ctx_t *internal_ctx;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (ctx == 0 || key_handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_cmac_context_size(&required_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    internal_ctx = (ano_crypto_internal_cmac_ctx_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    if (flags != ANO_CRYPTO_CMAC_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    st = ano_crypto_internal_csp_copy_secret(
        key_handle,
        ANO_CRYPTO_KEY_TYPE_CMAC,
        key_material,
        sizeof(key_material),
        &key_material_len);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_cmac_init(
            internal_ctx, key_material, key_material_len);
    }
    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, required_len);
    }
    return st;
}

ano_crypto_status_t ano_crypto_cmac_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    return ano_crypto_internal_cmac_update(
        (ano_crypto_internal_cmac_ctx_t *)ctx, input, input_len);
}

ano_crypto_status_t ano_crypto_cmac_final(
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
    if (out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;
    if (ctx == 0 || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (out_len < ANO_CRYPTO_CMAC_TAG_SIZE) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    st = ano_crypto_internal_cmac_final(
        (ano_crypto_internal_cmac_ctx_t *)ctx, out);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, ANO_CRYPTO_CMAC_TAG_SIZE);
        return st;
    }
    *out_written = ANO_CRYPTO_CMAC_TAG_SIZE;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cmac_uninit(void *ctx, size_t ctx_len)
{
    if (ctx == 0 && ctx_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ctx != 0 && ctx_len != 0u) {
        ano_crypto_internal_zeroize(ctx, ctx_len);
    }
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_cmac_compute_with_key(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written,
    uint32_t flags)
{
    ano_crypto_internal_cmac_ctx_t ctx;
    ano_crypto_status_t st;

    if (out_written == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;
    if (input == 0 && input_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    st = ano_crypto_cmac_init(&ctx, sizeof(ctx), key_handle, flags);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_cmac_update(&ctx, input, input_len);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_cmac_final(&ctx, out, out_len, out_written);
    } else {
        ano_crypto_internal_cmac_uninit(&ctx);
    }
    return st;
}

ano_crypto_status_t ano_crypto_cmac_verify_with_key(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    const uint8_t *tag,
    size_t tag_len,
    uint32_t flags)
{
    uint8_t computed[ANO_CRYPTO_CMAC_TAG_SIZE];
    size_t written = 0u;
    ano_crypto_status_t st;
    int matches;

    if (tag == 0 || tag_len != ANO_CRYPTO_CMAC_TAG_SIZE ||
        (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(computed, sizeof(computed));
    st = ano_crypto_cmac_compute_with_key(
        key_handle, input, input_len, computed, sizeof(computed), &written, flags);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(computed, sizeof(computed));
        return st;
    }
    if (written != sizeof(computed)) {
        ano_crypto_internal_zeroize(computed, sizeof(computed));
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    matches = ano_crypto_cmac_tags_equal(computed, tag, sizeof(computed));
    ano_crypto_internal_zeroize(computed, sizeof(computed));
    return matches ? ANO_CRYPTO_OK : ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
}
