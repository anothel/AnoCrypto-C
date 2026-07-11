#include <stddef.h>
#include <stdint.h>
#include "ac_aes_internal.h"
#include "ac_internal.h"

static uint8_t ano_crypto_aes_rotl8(uint8_t value, unsigned int shift)
{
    return (uint8_t)((uint8_t)(value << shift) |
                     (uint8_t)(value >> (8u - shift)));
}

static uint8_t ano_crypto_aes_xtime(uint8_t value)
{
    uint8_t high = (uint8_t)(value >> 7u);
    uint8_t mask = (uint8_t)(0u - high);
    return (uint8_t)((uint8_t)(value << 1u) ^ (uint8_t)(0x1bu & mask));
}

static uint8_t ano_crypto_aes_gf_mul(uint8_t a, uint8_t b)
{
    uint8_t result = 0u;
    unsigned int i;

    for (i = 0u; i < 8u; ++i) {
        uint8_t bit_mask = (uint8_t)(0u - (uint8_t)(b & 1u));
        result = (uint8_t)(result ^ (uint8_t)(a & bit_mask));
        a = ano_crypto_aes_xtime(a);
        b = (uint8_t)(b >> 1u);
    }

    return result;
}

static uint8_t ano_crypto_aes_gf_inverse(uint8_t value)
{
    uint8_t x2 = ano_crypto_aes_gf_mul(value, value);
    uint8_t x4 = ano_crypto_aes_gf_mul(x2, x2);
    uint8_t x8 = ano_crypto_aes_gf_mul(x4, x4);
    uint8_t x16 = ano_crypto_aes_gf_mul(x8, x8);
    uint8_t x32 = ano_crypto_aes_gf_mul(x16, x16);
    uint8_t x64 = ano_crypto_aes_gf_mul(x32, x32);
    uint8_t x128 = ano_crypto_aes_gf_mul(x64, x64);
    uint8_t inverse = ano_crypto_aes_gf_mul(x2, x4);

    inverse = ano_crypto_aes_gf_mul(inverse, x8);
    inverse = ano_crypto_aes_gf_mul(inverse, x16);
    inverse = ano_crypto_aes_gf_mul(inverse, x32);
    inverse = ano_crypto_aes_gf_mul(inverse, x64);
    inverse = ano_crypto_aes_gf_mul(inverse, x128);
    return inverse;
}

static uint8_t ano_crypto_aes_sbox(uint8_t value)
{
    uint8_t inverse = ano_crypto_aes_gf_inverse(value);
    return (uint8_t)(inverse ^
                     ano_crypto_aes_rotl8(inverse, 1u) ^
                     ano_crypto_aes_rotl8(inverse, 2u) ^
                     ano_crypto_aes_rotl8(inverse, 3u) ^
                     ano_crypto_aes_rotl8(inverse, 4u) ^
                     0x63u);
}

static uint8_t ano_crypto_aes_inverse_sbox(uint8_t value)
{
    uint8_t inverse_affine = (uint8_t)(
        ano_crypto_aes_rotl8(value, 1u) ^
        ano_crypto_aes_rotl8(value, 3u) ^
        ano_crypto_aes_rotl8(value, 6u) ^
        0x05u);
    return ano_crypto_aes_gf_inverse(inverse_affine);
}

static void ano_crypto_aes_add_round_key(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    const uint8_t *round_key)
{
    size_t i;
    for (i = 0u; i < ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE; ++i) {
        state[i] = (uint8_t)(state[i] ^ round_key[i]);
    }
}

static void ano_crypto_aes_sub_bytes(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    size_t i;
    for (i = 0u; i < ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE; ++i) {
        state[i] = ano_crypto_aes_sbox(state[i]);
    }
}

static void ano_crypto_aes_inverse_sub_bytes(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    size_t i;
    for (i = 0u; i < ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE; ++i) {
        state[i] = ano_crypto_aes_inverse_sbox(state[i]);
    }
}

static void ano_crypto_aes_shift_rows(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t tmp[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];

    tmp[0] = state[0];
    tmp[1] = state[5];
    tmp[2] = state[10];
    tmp[3] = state[15];
    tmp[4] = state[4];
    tmp[5] = state[9];
    tmp[6] = state[14];
    tmp[7] = state[3];
    tmp[8] = state[8];
    tmp[9] = state[13];
    tmp[10] = state[2];
    tmp[11] = state[7];
    tmp[12] = state[12];
    tmp[13] = state[1];
    tmp[14] = state[6];
    tmp[15] = state[11];

    {
        size_t i;
        for (i = 0u; i < sizeof(tmp); ++i) {
            state[i] = tmp[i];
        }
    }
    ano_crypto_internal_zeroize(tmp, sizeof(tmp));
}

static void ano_crypto_aes_inverse_shift_rows(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t tmp[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];

    tmp[0] = state[0];
    tmp[1] = state[13];
    tmp[2] = state[10];
    tmp[3] = state[7];
    tmp[4] = state[4];
    tmp[5] = state[1];
    tmp[6] = state[14];
    tmp[7] = state[11];
    tmp[8] = state[8];
    tmp[9] = state[5];
    tmp[10] = state[2];
    tmp[11] = state[15];
    tmp[12] = state[12];
    tmp[13] = state[9];
    tmp[14] = state[6];
    tmp[15] = state[3];

    {
        size_t i;
        for (i = 0u; i < sizeof(tmp); ++i) {
            state[i] = tmp[i];
        }
    }
    ano_crypto_internal_zeroize(tmp, sizeof(tmp));
}

static void ano_crypto_aes_mix_columns(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    size_t column;

    for (column = 0u; column < 4u; ++column) {
        size_t offset = column * 4u;
        uint8_t a0 = state[offset];
        uint8_t a1 = state[offset + 1u];
        uint8_t a2 = state[offset + 2u];
        uint8_t a3 = state[offset + 3u];

        state[offset] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x02u) ^
                                  ano_crypto_aes_gf_mul(a1, 0x03u) ^ a2 ^ a3);
        state[offset + 1u] = (uint8_t)(a0 ^
                                      ano_crypto_aes_gf_mul(a1, 0x02u) ^
                                      ano_crypto_aes_gf_mul(a2, 0x03u) ^ a3);
        state[offset + 2u] = (uint8_t)(a0 ^ a1 ^
                                      ano_crypto_aes_gf_mul(a2, 0x02u) ^
                                      ano_crypto_aes_gf_mul(a3, 0x03u));
        state[offset + 3u] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x03u) ^
                                      a1 ^ a2 ^
                                      ano_crypto_aes_gf_mul(a3, 0x02u));
    }
}

static void ano_crypto_aes_inverse_mix_columns(
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    size_t column;

    for (column = 0u; column < 4u; ++column) {
        size_t offset = column * 4u;
        uint8_t a0 = state[offset];
        uint8_t a1 = state[offset + 1u];
        uint8_t a2 = state[offset + 2u];
        uint8_t a3 = state[offset + 3u];

        state[offset] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x0eu) ^
                                  ano_crypto_aes_gf_mul(a1, 0x0bu) ^
                                  ano_crypto_aes_gf_mul(a2, 0x0du) ^
                                  ano_crypto_aes_gf_mul(a3, 0x09u));
        state[offset + 1u] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x09u) ^
                                      ano_crypto_aes_gf_mul(a1, 0x0eu) ^
                                      ano_crypto_aes_gf_mul(a2, 0x0bu) ^
                                      ano_crypto_aes_gf_mul(a3, 0x0du));
        state[offset + 2u] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x0du) ^
                                      ano_crypto_aes_gf_mul(a1, 0x09u) ^
                                      ano_crypto_aes_gf_mul(a2, 0x0eu) ^
                                      ano_crypto_aes_gf_mul(a3, 0x0bu));
        state[offset + 3u] = (uint8_t)(ano_crypto_aes_gf_mul(a0, 0x0bu) ^
                                      ano_crypto_aes_gf_mul(a1, 0x0du) ^
                                      ano_crypto_aes_gf_mul(a2, 0x09u) ^
                                      ano_crypto_aes_gf_mul(a3, 0x0eu));
    }
}

static void ano_crypto_aes_rot_word(uint8_t word[4])
{
    uint8_t first = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = first;
}

static void ano_crypto_aes_sub_word(uint8_t word[4])
{
    word[0] = ano_crypto_aes_sbox(word[0]);
    word[1] = ano_crypto_aes_sbox(word[1]);
    word[2] = ano_crypto_aes_sbox(word[2]);
    word[3] = ano_crypto_aes_sbox(word[3]);
}

ano_crypto_status_t ano_crypto_internal_aes_init(
    ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len)
{
    size_t nk;
    size_t nr;
    size_t total_words;
    size_t word_index;
    uint8_t rcon = 0x01u;

    if (ctx == 0 || key == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (key_len != 16u && key_len != 24u && key_len != 32u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    nk = key_len / 4u;
    nr = nk + 6u;
    total_words = 4u * (nr + 1u);
    ano_crypto_internal_zeroize(ctx, sizeof(*ctx));

    for (word_index = 0u; word_index < key_len; ++word_index) {
        ctx->round_keys[word_index] = key[word_index];
    }

    for (word_index = nk; word_index < total_words; ++word_index) {
        uint8_t temp[4];
        size_t previous_offset = (word_index - 1u) * 4u;
        size_t source_offset = (word_index - nk) * 4u;
        size_t output_offset = word_index * 4u;
        size_t i;

        for (i = 0u; i < 4u; ++i) {
            temp[i] = ctx->round_keys[previous_offset + i];
        }

        if ((word_index % nk) == 0u) {
            ano_crypto_aes_rot_word(temp);
            ano_crypto_aes_sub_word(temp);
            temp[0] = (uint8_t)(temp[0] ^ rcon);
            rcon = ano_crypto_aes_xtime(rcon);
        } else if (nk > 6u && (word_index % nk) == 4u) {
            ano_crypto_aes_sub_word(temp);
        }

        for (i = 0u; i < 4u; ++i) {
            ctx->round_keys[output_offset + i] =
                (uint8_t)(ctx->round_keys[source_offset + i] ^ temp[i]);
        }
        ano_crypto_internal_zeroize(temp, sizeof(temp));
    }

    ctx->rounds = (uint32_t)nr;
    ctx->initialized = 1u;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_aes_encrypt_block(
    const ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t input[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    uint8_t output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint32_t round;
    size_t i;

    if (ctx == 0 || input == 0 || output == 0 || ctx->initialized == 0u ||
        (ctx->rounds != 10u && ctx->rounds != 12u && ctx->rounds != 14u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < sizeof(state); ++i) {
        state[i] = input[i];
    }

    ano_crypto_aes_add_round_key(state, ctx->round_keys);
    for (round = 1u; round < ctx->rounds; ++round) {
        ano_crypto_aes_sub_bytes(state);
        ano_crypto_aes_shift_rows(state);
        ano_crypto_aes_mix_columns(state);
        ano_crypto_aes_add_round_key(
            state, ctx->round_keys + ((size_t)round * ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE));
    }
    ano_crypto_aes_sub_bytes(state);
    ano_crypto_aes_shift_rows(state);
    ano_crypto_aes_add_round_key(
        state, ctx->round_keys + ((size_t)ctx->rounds * ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE));

    for (i = 0u; i < sizeof(state); ++i) {
        output[i] = state[i];
    }
    ano_crypto_internal_zeroize(state, sizeof(state));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_aes_decrypt_block(
    const ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t input[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    uint8_t output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE])
{
    uint8_t state[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint32_t round;
    size_t i;

    if (ctx == 0 || input == 0 || output == 0 || ctx->initialized == 0u ||
        (ctx->rounds != 10u && ctx->rounds != 12u && ctx->rounds != 14u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < sizeof(state); ++i) {
        state[i] = input[i];
    }

    ano_crypto_aes_add_round_key(
        state, ctx->round_keys + ((size_t)ctx->rounds * ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE));
    for (round = ctx->rounds; round > 1u; --round) {
        ano_crypto_aes_inverse_shift_rows(state);
        ano_crypto_aes_inverse_sub_bytes(state);
        ano_crypto_aes_add_round_key(
            state, ctx->round_keys + ((size_t)(round - 1u) * ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE));
        ano_crypto_aes_inverse_mix_columns(state);
    }
    ano_crypto_aes_inverse_shift_rows(state);
    ano_crypto_aes_inverse_sub_bytes(state);
    ano_crypto_aes_add_round_key(state, ctx->round_keys);

    for (i = 0u; i < sizeof(state); ++i) {
        output[i] = state[i];
    }
    ano_crypto_internal_zeroize(state, sizeof(state));
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_aes_uninit(ano_crypto_internal_aes_ctx_t *ctx)
{
    if (ctx != 0) {
        ano_crypto_internal_zeroize(ctx, sizeof(*ctx));
    }
}
