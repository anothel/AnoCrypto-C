#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_kdf.h>
#include <anocrypto/ac_module.h>
#include "ac_hmac_internal.h"
#include "ac_internal.h"

#define ANO_CRYPTO_SCRYPT_BLOCK_WORDS (32u * ANO_CRYPTO_SCRYPT_R)
#define ANO_CRYPTO_SCRYPT_V_WORDS (ANO_CRYPTO_SCRYPT_BLOCK_WORDS * ANO_CRYPTO_SCRYPT_N)
#define ANO_CRYPTO_SCRYPT_SCRATCH_WORDS (ANO_CRYPTO_SCRYPT_BLOCK_WORDS + 16u)
#define ANO_CRYPTO_SCRYPT_TOTAL_WORDS \
    (ANO_CRYPTO_SCRYPT_V_WORDS + ANO_CRYPTO_SCRYPT_BLOCK_WORDS + ANO_CRYPTO_SCRYPT_SCRATCH_WORDS)

typedef struct ano_crypto_scrypt_alignment_probe {
    uint8_t prefix;
    uint32_t word;
} ano_crypto_scrypt_alignment_probe_t;

#define ANO_CRYPTO_SCRYPT_WORD_ALIGNMENT \
    offsetof(ano_crypto_scrypt_alignment_probe_t, word)

#if ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE != (ANO_CRYPTO_SCRYPT_TOTAL_WORDS * 4u)
#error ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE does not match the fixed profile layout
#endif

#if ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE > ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE
#error ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE exceeds the fixed profile memory ceiling
#endif

static uint32_t ano_crypto_scrypt_rotl32(uint32_t value, unsigned int shift)
{
    return (uint32_t)((value << shift) | (value >> (32u - shift)));
}

static uint32_t ano_crypto_scrypt_load32_le(const uint8_t input[4])
{
    return ((uint32_t)input[0]) |
           ((uint32_t)input[1] << 8) |
           ((uint32_t)input[2] << 16) |
           ((uint32_t)input[3] << 24);
}

static void ano_crypto_scrypt_store32_le(uint8_t output[4], uint32_t value)
{
    output[0] = (uint8_t)value;
    output[1] = (uint8_t)(value >> 8);
    output[2] = (uint8_t)(value >> 16);
    output[3] = (uint8_t)(value >> 24);
}

static void ano_crypto_scrypt_store32_be(uint8_t output[4], uint32_t value)
{
    output[0] = (uint8_t)(value >> 24);
    output[1] = (uint8_t)(value >> 16);
    output[2] = (uint8_t)(value >> 8);
    output[3] = (uint8_t)value;
}

static int ano_crypto_scrypt_range_valid(const void *ptr, size_t len)
{
    uintptr_t start;

    if (len == 0u) {
        return 1;
    }
    if (ptr == 0) {
        return 0;
    }
    start = (uintptr_t)ptr;
    return len <= (size_t)(UINTPTR_MAX - start);
}

static int ano_crypto_scrypt_ranges_overlap(
    const void *left,
    size_t left_len,
    const void *right,
    size_t right_len)
{
    uintptr_t left_start;
    uintptr_t right_start;
    uintptr_t left_end;
    uintptr_t right_end;

    if (left_len == 0u || right_len == 0u) {
        return 0;
    }
    left_start = (uintptr_t)left;
    right_start = (uintptr_t)right;
    left_end = left_start + (uintptr_t)left_len;
    right_end = right_start + (uintptr_t)right_len;
    return left_start < right_end && right_start < left_end;
}

static void ano_crypto_scrypt_salsa20_8(uint32_t block[16])
{
    uint32_t x[16];
    size_t i;

    (void)memcpy(x, block, sizeof(x));
    for (i = 0u; i < 8u; i += 2u) {
        x[4] ^= ano_crypto_scrypt_rotl32(x[0] + x[12], 7u);
        x[8] ^= ano_crypto_scrypt_rotl32(x[4] + x[0], 9u);
        x[12] ^= ano_crypto_scrypt_rotl32(x[8] + x[4], 13u);
        x[0] ^= ano_crypto_scrypt_rotl32(x[12] + x[8], 18u);
        x[9] ^= ano_crypto_scrypt_rotl32(x[5] + x[1], 7u);
        x[13] ^= ano_crypto_scrypt_rotl32(x[9] + x[5], 9u);
        x[1] ^= ano_crypto_scrypt_rotl32(x[13] + x[9], 13u);
        x[5] ^= ano_crypto_scrypt_rotl32(x[1] + x[13], 18u);
        x[14] ^= ano_crypto_scrypt_rotl32(x[10] + x[6], 7u);
        x[2] ^= ano_crypto_scrypt_rotl32(x[14] + x[10], 9u);
        x[6] ^= ano_crypto_scrypt_rotl32(x[2] + x[14], 13u);
        x[10] ^= ano_crypto_scrypt_rotl32(x[6] + x[2], 18u);
        x[3] ^= ano_crypto_scrypt_rotl32(x[15] + x[11], 7u);
        x[7] ^= ano_crypto_scrypt_rotl32(x[3] + x[15], 9u);
        x[11] ^= ano_crypto_scrypt_rotl32(x[7] + x[3], 13u);
        x[15] ^= ano_crypto_scrypt_rotl32(x[11] + x[7], 18u);

        x[1] ^= ano_crypto_scrypt_rotl32(x[0] + x[3], 7u);
        x[2] ^= ano_crypto_scrypt_rotl32(x[1] + x[0], 9u);
        x[3] ^= ano_crypto_scrypt_rotl32(x[2] + x[1], 13u);
        x[0] ^= ano_crypto_scrypt_rotl32(x[3] + x[2], 18u);
        x[6] ^= ano_crypto_scrypt_rotl32(x[5] + x[4], 7u);
        x[7] ^= ano_crypto_scrypt_rotl32(x[6] + x[5], 9u);
        x[4] ^= ano_crypto_scrypt_rotl32(x[7] + x[6], 13u);
        x[5] ^= ano_crypto_scrypt_rotl32(x[4] + x[7], 18u);
        x[11] ^= ano_crypto_scrypt_rotl32(x[10] + x[9], 7u);
        x[8] ^= ano_crypto_scrypt_rotl32(x[11] + x[10], 9u);
        x[9] ^= ano_crypto_scrypt_rotl32(x[8] + x[11], 13u);
        x[10] ^= ano_crypto_scrypt_rotl32(x[9] + x[8], 18u);
        x[12] ^= ano_crypto_scrypt_rotl32(x[15] + x[14], 7u);
        x[13] ^= ano_crypto_scrypt_rotl32(x[12] + x[15], 9u);
        x[14] ^= ano_crypto_scrypt_rotl32(x[13] + x[12], 13u);
        x[15] ^= ano_crypto_scrypt_rotl32(x[14] + x[13], 18u);
    }
    for (i = 0u; i < 16u; ++i) {
        block[i] += x[i];
    }
    ano_crypto_internal_zeroize(x, sizeof(x));
}

static void ano_crypto_scrypt_blockmix(uint32_t *block, uint32_t *scratch)
{
    uint32_t *y = scratch;
    uint32_t *x = scratch + ANO_CRYPTO_SCRYPT_BLOCK_WORDS;
    size_t i;
    size_t j;

    (void)memcpy(x, block + ANO_CRYPTO_SCRYPT_BLOCK_WORDS - 16u, 16u * sizeof(uint32_t));
    for (i = 0u; i < (2u * ANO_CRYPTO_SCRYPT_R); ++i) {
        const uint32_t *source = block + (i * 16u);
        for (j = 0u; j < 16u; ++j) {
            x[j] ^= source[j];
        }
        ano_crypto_scrypt_salsa20_8(x);
        (void)memcpy(y + (i * 16u), x, 16u * sizeof(uint32_t));
    }
    for (i = 0u; i < ANO_CRYPTO_SCRYPT_R; ++i) {
        (void)memcpy(block + (i * 16u), y + ((2u * i) * 16u), 16u * sizeof(uint32_t));
        (void)memcpy(block + ((i + ANO_CRYPTO_SCRYPT_R) * 16u),
                     y + ((2u * i + 1u) * 16u), 16u * sizeof(uint32_t));
    }
}

static ano_crypto_status_t ano_crypto_scrypt_pbkdf2_sha256_c1(
    const uint8_t *password,
    size_t password_len,
    const uint8_t *salt,
    size_t salt_len,
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_internal_hmac_sha256_ctx_t hmac;
    uint8_t digest[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE];
    uint8_t index_be[4];
    uint32_t block_index = 1u;
    size_t produced = 0u;
    ano_crypto_status_t st = ANO_CRYPTO_OK;

    while (produced < out_len) {
        size_t take = out_len - produced;
        if (take > sizeof(digest)) {
            take = sizeof(digest);
        }
        ano_crypto_scrypt_store32_be(index_be, block_index);
        st = ano_crypto_internal_hmac_sha256_init(&hmac, password, password_len);
        if (st == ANO_CRYPTO_OK && salt_len != 0u) {
            st = ano_crypto_internal_hmac_sha256_update(&hmac, salt, salt_len);
        }
        if (st == ANO_CRYPTO_OK) {
            st = ano_crypto_internal_hmac_sha256_update(&hmac, index_be, sizeof(index_be));
        }
        if (st == ANO_CRYPTO_OK) {
            st = ano_crypto_internal_hmac_sha256_final(&hmac, digest);
        }
        if (st != ANO_CRYPTO_OK) {
            break;
        }
        (void)memcpy(out + produced, digest, take);
        produced += take;
        if (block_index == UINT32_MAX && produced < out_len) {
            st = ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
            break;
        }
        ++block_index;
    }
    ano_crypto_internal_zeroize(&hmac, sizeof(hmac));
    ano_crypto_internal_zeroize(digest, sizeof(digest));
    ano_crypto_internal_zeroize(index_be, sizeof(index_be));
    return st;
}

static ano_crypto_status_t ano_crypto_scrypt_romix(uint8_t *b, uint32_t *v, uint32_t *block, uint32_t *scratch)
{
    size_t i;
    size_t j;

    for (i = 0u; i < ANO_CRYPTO_SCRYPT_BLOCK_WORDS; ++i) {
        block[i] = ano_crypto_scrypt_load32_le(b + (i * 4u));
    }
    for (i = 0u; i < ANO_CRYPTO_SCRYPT_N; ++i) {
        (void)memcpy(v + (i * ANO_CRYPTO_SCRYPT_BLOCK_WORDS), block,
                     ANO_CRYPTO_SCRYPT_BLOCK_WORDS * sizeof(uint32_t));
        ano_crypto_scrypt_blockmix(block, scratch);
    }
    for (i = 0u; i < ANO_CRYPTO_SCRYPT_N; ++i) {
        uint32_t selector = block[(2u * ANO_CRYPTO_SCRYPT_R - 1u) * 16u];
        size_t selected = (size_t)(selector & (ANO_CRYPTO_SCRYPT_N - 1u));
        const uint32_t *selected_block = v + (selected * ANO_CRYPTO_SCRYPT_BLOCK_WORDS);
        for (j = 0u; j < ANO_CRYPTO_SCRYPT_BLOCK_WORDS; ++j) {
            block[j] ^= selected_block[j];
        }
        ano_crypto_scrypt_blockmix(block, scratch);
    }
    for (i = 0u; i < ANO_CRYPTO_SCRYPT_BLOCK_WORDS; ++i) {
        ano_crypto_scrypt_store32_le(b + (i * 4u), block[i]);
    }
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_scrypt_derive_key(
    const uint8_t *password,
    size_t password_len,
    const uint8_t *salt,
    size_t salt_len,
    void *workspace,
    size_t workspace_len,
    uint8_t out[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE])
{
    uint32_t *words;
    uint32_t *v;
    uint32_t *block;
    uint32_t *scratch;
    uint8_t *b;
    ano_crypto_status_t st;

    if (!ano_crypto_scrypt_range_valid(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    ano_crypto_internal_zeroize(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE);
    if (!ano_crypto_scrypt_range_valid(password, password_len) ||
        !ano_crypto_scrypt_range_valid(salt, salt_len) ||
        !ano_crypto_scrypt_range_valid(workspace, workspace_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (workspace_len < ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    if (((uintptr_t)workspace % (uintptr_t)ANO_CRYPTO_SCRYPT_WORD_ALIGNMENT) != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ano_crypto_scrypt_ranges_overlap(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE, password, password_len) ||
        ano_crypto_scrypt_ranges_overlap(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE, salt, salt_len) ||
        ano_crypto_scrypt_ranges_overlap(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE, workspace, workspace_len) ||
        ano_crypto_scrypt_ranges_overlap(workspace, workspace_len, password, password_len) ||
        ano_crypto_scrypt_ranges_overlap(workspace, workspace_len, salt, salt_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    words = (uint32_t *)workspace;
    v = words;
    block = v + ANO_CRYPTO_SCRYPT_V_WORDS;
    scratch = block + ANO_CRYPTO_SCRYPT_BLOCK_WORDS;
    b = (uint8_t *)block;
    ano_crypto_internal_zeroize(workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);

    st = ano_crypto_scrypt_pbkdf2_sha256_c1(
        password, password_len, salt, salt_len, b,
        ANO_CRYPTO_SCRYPT_BLOCK_WORDS * sizeof(uint32_t));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_scrypt_romix(b, v, block, scratch);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_scrypt_pbkdf2_sha256_c1(
            password, password_len, b,
            ANO_CRYPTO_SCRYPT_BLOCK_WORDS * sizeof(uint32_t),
            out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE);
    }

    ano_crypto_internal_zeroize(workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE);
    }
    return st;
}
