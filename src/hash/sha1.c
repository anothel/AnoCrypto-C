#include "ac_sha1_internal.h"
#include "ac_internal.h"

#include <string.h>

#define SHA1_MAX_INPUT_BYTES (UINT64_MAX / 8u)

static uint32_t rol32(uint32_t value, unsigned int shift)
{
    return (uint32_t)((value << shift) | (value >> (32u - shift)));
}

static uint32_t load_be32(const uint8_t *input)
{
    return ((uint32_t)input[0] << 24) |
           ((uint32_t)input[1] << 16) |
           ((uint32_t)input[2] << 8) |
           (uint32_t)input[3];
}

static void store_be32(uint8_t *output, uint32_t value)
{
    output[0] = (uint8_t)(value >> 24);
    output[1] = (uint8_t)(value >> 16);
    output[2] = (uint8_t)(value >> 8);
    output[3] = (uint8_t)value;
}

static void store_be64(uint8_t *output, uint64_t value)
{
    output[0] = (uint8_t)(value >> 56);
    output[1] = (uint8_t)(value >> 48);
    output[2] = (uint8_t)(value >> 40);
    output[3] = (uint8_t)(value >> 32);
    output[4] = (uint8_t)(value >> 24);
    output[5] = (uint8_t)(value >> 16);
    output[6] = (uint8_t)(value >> 8);
    output[7] = (uint8_t)value;
}

static void sha1_transform(
    ano_crypto_internal_sha1_ctx_t *ctx,
    const uint8_t block[ANO_CRYPTO_SHA1_BLOCK_SIZE])
{
    uint32_t schedule[80];
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    size_t index;

    for (index = 0u; index < 16u; ++index) {
        schedule[index] = load_be32(block + (index * 4u));
    }
    for (index = 16u; index < 80u; ++index) {
        schedule[index] = rol32(
            schedule[index - 3u] ^ schedule[index - 8u] ^
            schedule[index - 14u] ^ schedule[index - 16u],
            1u);
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];

    for (index = 0u; index < 80u; ++index) {
        uint32_t function_value;
        uint32_t constant;
        uint32_t temporary;

        if (index < 20u) {
            function_value = (b & c) | ((~b) & d);
            constant = UINT32_C(0x5a827999);
        } else if (index < 40u) {
            function_value = b ^ c ^ d;
            constant = UINT32_C(0x6ed9eba1);
        } else if (index < 60u) {
            function_value = (b & c) | (b & d) | (c & d);
            constant = UINT32_C(0x8f1bbcdc);
        } else {
            function_value = b ^ c ^ d;
            constant = UINT32_C(0xca62c1d6);
        }

        temporary = rol32(a, 5u) + function_value + e + constant + schedule[index];
        e = d;
        d = c;
        c = rol32(b, 30u);
        b = a;
        a = temporary;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;

    ano_crypto_internal_zeroize(schedule, sizeof(schedule));
}

void ano_crypto_internal_sha1_init(ano_crypto_internal_sha1_ctx_t *ctx)
{
    if (ctx == 0) {
        return;
    }

    ctx->state[0] = UINT32_C(0x67452301);
    ctx->state[1] = UINT32_C(0xefcdab89);
    ctx->state[2] = UINT32_C(0x98badcfe);
    ctx->state[3] = UINT32_C(0x10325476);
    ctx->state[4] = UINT32_C(0xc3d2e1f0);
    ctx->total_len = 0u;
    ctx->buffer_len = 0u;
    ctx->finalized = 0;
    (void)memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

ano_crypto_status_t ano_crypto_internal_sha1_update(
    ano_crypto_internal_sha1_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    size_t offset = 0u;

    if (ctx == 0 || (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ctx->finalized != 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (input_len == 0u) {
        return ANO_CRYPTO_OK;
    }
    if (ctx->total_len > SHA1_MAX_INPUT_BYTES ||
        (uint64_t)input_len > (SHA1_MAX_INPUT_BYTES - ctx->total_len)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ctx->total_len += (uint64_t)input_len;

    if (ctx->buffer_len != 0u) {
        size_t needed = ANO_CRYPTO_SHA1_BLOCK_SIZE - ctx->buffer_len;
        if (needed > input_len) {
            needed = input_len;
        }
        (void)memcpy(ctx->buffer + ctx->buffer_len, input, needed);
        ctx->buffer_len += needed;
        offset += needed;
        if (ctx->buffer_len == ANO_CRYPTO_SHA1_BLOCK_SIZE) {
            sha1_transform(ctx, ctx->buffer);
            ano_crypto_internal_zeroize(ctx->buffer, sizeof(ctx->buffer));
            ctx->buffer_len = 0u;
        }
    }

    while ((input_len - offset) >= ANO_CRYPTO_SHA1_BLOCK_SIZE) {
        sha1_transform(ctx, input + offset);
        offset += ANO_CRYPTO_SHA1_BLOCK_SIZE;
    }

    if (offset < input_len) {
        ctx->buffer_len = input_len - offset;
        (void)memcpy(ctx->buffer, input + offset, ctx->buffer_len);
    }

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_sha1_final(
    ano_crypto_internal_sha1_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA1_DIGEST_SIZE])
{
    uint64_t bit_len;
    size_t index;

    if (ctx == 0 || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ctx->finalized != 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    bit_len = ctx->total_len * UINT64_C(8);
    ctx->buffer[ctx->buffer_len] = 0x80u;
    ++ctx->buffer_len;

    if (ctx->buffer_len > 56u) {
        while (ctx->buffer_len < ANO_CRYPTO_SHA1_BLOCK_SIZE) {
            ctx->buffer[ctx->buffer_len] = 0u;
            ++ctx->buffer_len;
        }
        sha1_transform(ctx, ctx->buffer);
        ano_crypto_internal_zeroize(ctx->buffer, sizeof(ctx->buffer));
        ctx->buffer_len = 0u;
    }

    while (ctx->buffer_len < 56u) {
        ctx->buffer[ctx->buffer_len] = 0u;
        ++ctx->buffer_len;
    }
    store_be64(ctx->buffer + 56u, bit_len);
    sha1_transform(ctx, ctx->buffer);

    for (index = 0u; index < 5u; ++index) {
        store_be32(out + (index * 4u), ctx->state[index]);
    }

    ctx->finalized = 1;
    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_sha1_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA1_DIGEST_SIZE])
{
    ano_crypto_internal_sha1_ctx_t ctx;
    ano_crypto_status_t status;

    if ((input == 0 && input_len != 0u) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_sha1_init(&ctx);
    status = ano_crypto_internal_sha1_update(&ctx, input, input_len);
    if (status == ANO_CRYPTO_OK) {
        status = ano_crypto_internal_sha1_final(&ctx, out);
    }
    if (status != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    }
    return status;
}
