#include <stddef.h>
#include <stdint.h>
#include "ac_internal.h"
#include "ac_sha512_internal.h"

static const uint64_t g_sha512_k[80] = {
    UINT64_C(0x428a2f98d728ae22), UINT64_C(0x7137449123ef65cd),
    UINT64_C(0xb5c0fbcfec4d3b2f), UINT64_C(0xe9b5dba58189dbbc),
    UINT64_C(0x3956c25bf348b538), UINT64_C(0x59f111f1b605d019),
    UINT64_C(0x923f82a4af194f9b), UINT64_C(0xab1c5ed5da6d8118),
    UINT64_C(0xd807aa98a3030242), UINT64_C(0x12835b0145706fbe),
    UINT64_C(0x243185be4ee4b28c), UINT64_C(0x550c7dc3d5ffb4e2),
    UINT64_C(0x72be5d74f27b896f), UINT64_C(0x80deb1fe3b1696b1),
    UINT64_C(0x9bdc06a725c71235), UINT64_C(0xc19bf174cf692694),
    UINT64_C(0xe49b69c19ef14ad2), UINT64_C(0xefbe4786384f25e3),
    UINT64_C(0x0fc19dc68b8cd5b5), UINT64_C(0x240ca1cc77ac9c65),
    UINT64_C(0x2de92c6f592b0275), UINT64_C(0x4a7484aa6ea6e483),
    UINT64_C(0x5cb0a9dcbd41fbd4), UINT64_C(0x76f988da831153b5),
    UINT64_C(0x983e5152ee66dfab), UINT64_C(0xa831c66d2db43210),
    UINT64_C(0xb00327c898fb213f), UINT64_C(0xbf597fc7beef0ee4),
    UINT64_C(0xc6e00bf33da88fc2), UINT64_C(0xd5a79147930aa725),
    UINT64_C(0x06ca6351e003826f), UINT64_C(0x142929670a0e6e70),
    UINT64_C(0x27b70a8546d22ffc), UINT64_C(0x2e1b21385c26c926),
    UINT64_C(0x4d2c6dfc5ac42aed), UINT64_C(0x53380d139d95b3df),
    UINT64_C(0x650a73548baf63de), UINT64_C(0x766a0abb3c77b2a8),
    UINT64_C(0x81c2c92e47edaee6), UINT64_C(0x92722c851482353b),
    UINT64_C(0xa2bfe8a14cf10364), UINT64_C(0xa81a664bbc423001),
    UINT64_C(0xc24b8b70d0f89791), UINT64_C(0xc76c51a30654be30),
    UINT64_C(0xd192e819d6ef5218), UINT64_C(0xd69906245565a910),
    UINT64_C(0xf40e35855771202a), UINT64_C(0x106aa07032bbd1b8),
    UINT64_C(0x19a4c116b8d2d0c8), UINT64_C(0x1e376c085141ab53),
    UINT64_C(0x2748774cdf8eeb99), UINT64_C(0x34b0bcb5e19b48a8),
    UINT64_C(0x391c0cb3c5c95a63), UINT64_C(0x4ed8aa4ae3418acb),
    UINT64_C(0x5b9cca4f7763e373), UINT64_C(0x682e6ff3d6b2b8a3),
    UINT64_C(0x748f82ee5defb2fc), UINT64_C(0x78a5636f43172f60),
    UINT64_C(0x84c87814a1f0ab72), UINT64_C(0x8cc702081a6439ec),
    UINT64_C(0x90befffa23631e28), UINT64_C(0xa4506cebde82bde9),
    UINT64_C(0xbef9a3f7b2c67915), UINT64_C(0xc67178f2e372532b),
    UINT64_C(0xca273eceea26619c), UINT64_C(0xd186b8c721c0c207),
    UINT64_C(0xeada7dd6cde0eb1e), UINT64_C(0xf57d4f7fee6ed178),
    UINT64_C(0x06f067aa72176fba), UINT64_C(0x0a637dc5a2c898a6),
    UINT64_C(0x113f9804bef90dae), UINT64_C(0x1b710b35131c471b),
    UINT64_C(0x28db77f523047d84), UINT64_C(0x32caab7b40c72493),
    UINT64_C(0x3c9ebe0a15c9bebc), UINT64_C(0x431d67c49c100d4c),
    UINT64_C(0x4cc5d4becb3e42b6), UINT64_C(0x597f299cfc657e2a),
    UINT64_C(0x5fcb6fab3ad6faec), UINT64_C(0x6c44198c4a475817)
};

static uint64_t ano_crypto_sha512_rotr(uint64_t value, unsigned int bits)
{
    return (value >> bits) | (value << (64u - bits));
}

static uint64_t ano_crypto_sha512_load_be64(const uint8_t *input)
{
    return ((uint64_t)input[0] << 56) |
           ((uint64_t)input[1] << 48) |
           ((uint64_t)input[2] << 40) |
           ((uint64_t)input[3] << 32) |
           ((uint64_t)input[4] << 24) |
           ((uint64_t)input[5] << 16) |
           ((uint64_t)input[6] << 8) |
           (uint64_t)input[7];
}

static void ano_crypto_sha512_store_be64(uint8_t *out, uint64_t value)
{
    out[0] = (uint8_t)(value >> 56);
    out[1] = (uint8_t)(value >> 48);
    out[2] = (uint8_t)(value >> 40);
    out[3] = (uint8_t)(value >> 32);
    out[4] = (uint8_t)(value >> 24);
    out[5] = (uint8_t)(value >> 16);
    out[6] = (uint8_t)(value >> 8);
    out[7] = (uint8_t)value;
}

static void ano_crypto_sha512_compress(
    ano_crypto_internal_sha512_ctx_t *ctx,
    const uint8_t block[ANO_CRYPTO_SHA512_BLOCK_SIZE])
{
    uint64_t w[80];
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
    uint64_t e;
    uint64_t f;
    uint64_t g;
    uint64_t h;
    size_t i;

    for (i = 0u; i < 16u; ++i) {
        w[i] = ano_crypto_sha512_load_be64(block + (i * 8u));
    }
    for (i = 16u; i < 80u; ++i) {
        uint64_t s0 = ano_crypto_sha512_rotr(w[i - 15u], 1u) ^
                      ano_crypto_sha512_rotr(w[i - 15u], 8u) ^
                      (w[i - 15u] >> 7u);
        uint64_t s1 = ano_crypto_sha512_rotr(w[i - 2u], 19u) ^
                      ano_crypto_sha512_rotr(w[i - 2u], 61u) ^
                      (w[i - 2u] >> 6u);
        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0u; i < 80u; ++i) {
        uint64_t sum1 = ano_crypto_sha512_rotr(e, 14u) ^
                        ano_crypto_sha512_rotr(e, 18u) ^
                        ano_crypto_sha512_rotr(e, 41u);
        uint64_t choose = (e & f) ^ ((~e) & g);
        uint64_t temp1 = h + sum1 + choose + g_sha512_k[i] + w[i];
        uint64_t sum0 = ano_crypto_sha512_rotr(a, 28u) ^
                        ano_crypto_sha512_rotr(a, 34u) ^
                        ano_crypto_sha512_rotr(a, 39u);
        uint64_t majority = (a & b) ^ (a & c) ^ (b & c);
        uint64_t temp2 = sum0 + majority;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;

    ano_crypto_internal_zeroize(w, sizeof(w));
}

static void ano_crypto_sha512_init_common(
    ano_crypto_internal_sha512_ctx_t *ctx,
    const uint64_t initial_state[8])
{
    size_t i;

    if (ctx == 0 || initial_state == 0) {
        return;
    }

    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    for (i = 0u; i < 8u; ++i) {
        ctx->state[i] = initial_state[i];
    }
    ctx->initialized = 1u;
}

void ano_crypto_internal_sha384_init(ano_crypto_internal_sha512_ctx_t *ctx)
{
    static const uint64_t initial_state[8] = {
        UINT64_C(0xcbbb9d5dc1059ed8), UINT64_C(0x629a292a367cd507),
        UINT64_C(0x9159015a3070dd17), UINT64_C(0x152fecd8f70e5939),
        UINT64_C(0x67332667ffc00b31), UINT64_C(0x8eb44a8768581511),
        UINT64_C(0xdb0c2e0d64f98fa7), UINT64_C(0x47b5481dbefa4fa4)
    };

    ano_crypto_sha512_init_common(ctx, initial_state);
}

void ano_crypto_internal_sha512_init(ano_crypto_internal_sha512_ctx_t *ctx)
{
    static const uint64_t initial_state[8] = {
        UINT64_C(0x6a09e667f3bcc908), UINT64_C(0xbb67ae8584caa73b),
        UINT64_C(0x3c6ef372fe94f82b), UINT64_C(0xa54ff53a5f1d36f1),
        UINT64_C(0x510e527fade682d1), UINT64_C(0x9b05688c2b3e6c1f),
        UINT64_C(0x1f83d9abfb41bd6b), UINT64_C(0x5be0cd19137e2179)
    };

    ano_crypto_sha512_init_common(ctx, initial_state);
}

static ano_crypto_status_t ano_crypto_sha512_add_length(
    ano_crypto_internal_sha512_ctx_t *ctx,
    size_t input_len)
{
    uint64_t next_low;
    uint64_t next_high;

    if (ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    next_low = ctx->total_bytes_low + (uint64_t)input_len;
    next_high = ctx->total_bytes_high;
    if (next_low < ctx->total_bytes_low) {
        if (next_high == UINT64_MAX) {
            return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        }
        next_high += UINT64_C(1);
    }

    /* SHA-512 encodes a 128-bit bit length, so the byte count must fit in 125 bits. */
    if ((next_high & UINT64_C(0xe000000000000000)) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ctx->total_bytes_low = next_low;
    ctx->total_bytes_high = next_high;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_sha512_update(
    ano_crypto_internal_sha512_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    size_t offset = 0u;
    ano_crypto_status_t st;

    if (ctx == 0 || ctx->initialized == 0u ||
        (input == 0 && input_len != 0u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_sha512_add_length(ctx, input_len);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (ctx->buffer_len != 0u) {
        size_t needed = ANO_CRYPTO_SHA512_BLOCK_SIZE - ctx->buffer_len;
        size_t take = input_len < needed ? input_len : needed;
        size_t i;

        for (i = 0u; i < take; ++i) {
            ctx->buffer[ctx->buffer_len + i] = input[i];
        }
        ctx->buffer_len += take;
        offset += take;
        if (ctx->buffer_len == ANO_CRYPTO_SHA512_BLOCK_SIZE) {
            ano_crypto_sha512_compress(ctx, ctx->buffer);
            ctx->buffer_len = 0u;
            ano_crypto_internal_zeroize(ctx->buffer, sizeof(ctx->buffer));
        }
    }

    while ((input_len - offset) >= ANO_CRYPTO_SHA512_BLOCK_SIZE) {
        ano_crypto_sha512_compress(ctx, input + offset);
        offset += ANO_CRYPTO_SHA512_BLOCK_SIZE;
    }

    if (offset < input_len) {
        size_t remaining = input_len - offset;
        size_t i;
        for (i = 0u; i < remaining; ++i) {
            ctx->buffer[i] = input[offset + i];
        }
        ctx->buffer_len = remaining;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_sha512_final_common(
    ano_crypto_internal_sha512_ctx_t *ctx,
    uint8_t *out,
    size_t digest_len)
{
    uint64_t bit_length_high;
    uint64_t bit_length_low;
    size_t i;
    size_t word_count;

    if (ctx == 0 || out == 0 || ctx->initialized == 0u ||
        (digest_len != ANO_CRYPTO_SHA384_DIGEST_SIZE &&
         digest_len != ANO_CRYPTO_SHA512_DIGEST_SIZE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    bit_length_high = (ctx->total_bytes_high << 3u) |
                      (ctx->total_bytes_low >> 61u);
    bit_length_low = ctx->total_bytes_low << 3u;

    ctx->buffer[ctx->buffer_len++] = 0x80u;
    if (ctx->buffer_len > 112u) {
        while (ctx->buffer_len < ANO_CRYPTO_SHA512_BLOCK_SIZE) {
            ctx->buffer[ctx->buffer_len++] = 0u;
        }
        ano_crypto_sha512_compress(ctx, ctx->buffer);
        ctx->buffer_len = 0u;
        ano_crypto_internal_zeroize(ctx->buffer, sizeof(ctx->buffer));
    }

    while (ctx->buffer_len < 112u) {
        ctx->buffer[ctx->buffer_len++] = 0u;
    }
    ano_crypto_sha512_store_be64(ctx->buffer + 112u, bit_length_high);
    ano_crypto_sha512_store_be64(ctx->buffer + 120u, bit_length_low);
    ano_crypto_sha512_compress(ctx, ctx->buffer);

    word_count = digest_len / 8u;
    for (i = 0u; i < word_count; ++i) {
        ano_crypto_sha512_store_be64(out + (i * 8u), ctx->state[i]);
    }

    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_sha384_final(
    ano_crypto_internal_sha512_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA384_DIGEST_SIZE])
{
    return ano_crypto_sha512_final_common(ctx, out, ANO_CRYPTO_SHA384_DIGEST_SIZE);
}

ano_crypto_status_t ano_crypto_internal_sha512_final(
    ano_crypto_internal_sha512_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_SHA512_DIGEST_SIZE])
{
    return ano_crypto_sha512_final_common(ctx, out, ANO_CRYPTO_SHA512_DIGEST_SIZE);
}

ano_crypto_status_t ano_crypto_internal_sha384_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA384_DIGEST_SIZE])
{
    ano_crypto_internal_sha512_ctx_t ctx;
    ano_crypto_status_t st;

    if ((input == 0 && input_len != 0u) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_sha384_init(&ctx);
    st = ano_crypto_internal_sha512_update(&ctx, input, input_len);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha384_final(&ctx, out);
    } else {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    }
    return st;
}

ano_crypto_status_t ano_crypto_internal_sha512_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_SHA512_DIGEST_SIZE])
{
    ano_crypto_internal_sha512_ctx_t ctx;
    ano_crypto_status_t st;

    if ((input == 0 && input_len != 0u) || out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_sha512_init(&ctx);
    st = ano_crypto_internal_sha512_update(&ctx, input, input_len);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha512_final(&ctx, out);
    } else {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
    }
    return st;
}
