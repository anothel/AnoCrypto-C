#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_aead.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>
#include "ac_aes_internal.h"
#include "ac_aead_internal.h"
#include "ac_csp_internal.h"
#include "ac_internal.h"

#define ANO_CRYPTO_GCM_BLOCK_SIZE 16u
#define ANO_CRYPTO_GCM_MAX_TEXT_BYTES UINT64_C(68719476704)

static int ano_crypto_gcm_range_valid(const void *ptr, size_t len)
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

static int ano_crypto_gcm_ranges_overlap(
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

static int ano_crypto_gcm_payload_overlap_invalid(
    const uint8_t *input,
    size_t input_len,
    uint8_t *output)
{
    if (input_len == 0u || input == output) {
        return 0;
    }
    return ano_crypto_gcm_ranges_overlap(input, input_len, output, input_len);
}

static size_t ano_crypto_gcm_output_clear_len(size_t input_len, size_t output_len)
{
    return output_len < input_len ? output_len : input_len;
}

static void ano_crypto_gcm_clear(uint8_t *ptr, size_t len)
{
    if (ptr != 0 && len != 0u) {
        ano_crypto_internal_zeroize(ptr, len);
    }
}

static void ano_crypto_gcm_xor_block(
    uint8_t left[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t right[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    size_t i;
    for (i = 0u; i < ANO_CRYPTO_GCM_BLOCK_SIZE; ++i) {
        left[i] = (uint8_t)(left[i] ^ right[i]);
    }
}

static void ano_crypto_gcm_shift_right_one(
    uint8_t block[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    uint8_t carry = 0u;
    size_t i;

    for (i = 0u; i < ANO_CRYPTO_GCM_BLOCK_SIZE; ++i) {
        uint8_t next = (uint8_t)(block[i] & 1u);
        block[i] = (uint8_t)((block[i] >> 1u) | (uint8_t)(carry << 7u));
        carry = next;
    }
}

static void ano_crypto_gcm_multiply(
    uint8_t x[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    uint8_t z[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t v[ANO_CRYPTO_GCM_BLOCK_SIZE];
    size_t bit;

    ano_crypto_internal_zeroize(z, sizeof(z));
    (void)memcpy(v, h, sizeof(v));

    for (bit = 0u; bit < 128u; ++bit) {
        uint8_t x_bit = (uint8_t)(
            ((uint32_t)x[bit / 8u] >> (7u - (uint32_t)(bit % 8u))) &
            UINT32_C(1));
        uint8_t x_mask = (uint8_t)(0u - x_bit);
        uint8_t lsb = (uint8_t)(v[15] & 1u);
        uint8_t reduction_mask = (uint8_t)(0u - lsb);
        size_t i;

        for (i = 0u; i < sizeof(z); ++i) {
            z[i] = (uint8_t)(z[i] ^ (uint8_t)(v[i] & x_mask));
        }
        ano_crypto_gcm_shift_right_one(v);
        v[0] = (uint8_t)(v[0] ^ (uint8_t)(0xe1u & reduction_mask));
    }

    (void)memcpy(x, z, sizeof(z));
    ano_crypto_internal_zeroize(v, sizeof(v));
    ano_crypto_internal_zeroize(z, sizeof(z));
}

static void ano_crypto_gcm_ghash_update(
    uint8_t state[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t *input,
    size_t input_len)
{
    uint8_t block[ANO_CRYPTO_GCM_BLOCK_SIZE];
    size_t offset = 0u;

    while (input_len - offset >= ANO_CRYPTO_GCM_BLOCK_SIZE) {
        ano_crypto_gcm_xor_block(state, input + offset);
        ano_crypto_gcm_multiply(state, h);
        offset += ANO_CRYPTO_GCM_BLOCK_SIZE;
    }

    if (offset != input_len) {
        size_t remaining = input_len - offset;
        ano_crypto_internal_zeroize(block, sizeof(block));
        (void)memcpy(block, input + offset, remaining);
        ano_crypto_gcm_xor_block(state, block);
        ano_crypto_gcm_multiply(state, h);
        ano_crypto_internal_zeroize(block, sizeof(block));
    }
}

static void ano_crypto_gcm_store_u64_be(uint8_t out[8], uint64_t value)
{
    size_t i;
    for (i = 0u; i < 8u; ++i) {
        out[7u - i] = (uint8_t)(value >> (i * 8u));
    }
}

static void ano_crypto_gcm_inc32(uint8_t counter[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    size_t i = ANO_CRYPTO_GCM_BLOCK_SIZE;
    while (i > 12u) {
        --i;
        counter[i] = (uint8_t)(counter[i] + 1u);
        if (counter[i] != 0u) {
            break;
        }
    }
}

static ano_crypto_status_t ano_crypto_gcm_ctr_crypt(
    const ano_crypto_internal_aes_ctx_t *aes,
    const uint8_t j0[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t *input,
    size_t input_len,
    uint8_t *output)
{
    uint8_t counter[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t stream[ANO_CRYPTO_GCM_BLOCK_SIZE];
    size_t offset = 0u;
    ano_crypto_status_t st = ANO_CRYPTO_OK;

    (void)memcpy(counter, j0, sizeof(counter));
    ano_crypto_internal_zeroize(stream, sizeof(stream));

    while (offset < input_len) {
        size_t take = input_len - offset;
        size_t i;
        if (take > sizeof(stream)) {
            take = sizeof(stream);
        }
        ano_crypto_gcm_inc32(counter);
        st = ano_crypto_internal_aes_encrypt_block(aes, counter, stream);
        if (st != ANO_CRYPTO_OK) {
            break;
        }
        for (i = 0u; i < take; ++i) {
            output[offset + i] = (uint8_t)(input[offset + i] ^ stream[i]);
        }
        offset += take;
    }

    ano_crypto_internal_zeroize(stream, sizeof(stream));
    ano_crypto_internal_zeroize(counter, sizeof(counter));
    return st;
}

static ano_crypto_status_t ano_crypto_gcm_compute_tag(
    const ano_crypto_internal_aes_ctx_t *aes,
    const uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t j0[ANO_CRYPTO_GCM_BLOCK_SIZE],
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *ciphertext,
    size_t ciphertext_len,
    uint8_t tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE])
{
    uint8_t state[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t lengths[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t encrypted_j0[ANO_CRYPTO_GCM_BLOCK_SIZE];
    ano_crypto_status_t st;

    ano_crypto_internal_zeroize(state, sizeof(state));
    ano_crypto_internal_zeroize(lengths, sizeof(lengths));
    ano_crypto_internal_zeroize(encrypted_j0, sizeof(encrypted_j0));

    if (aad_len != 0u) {
        ano_crypto_gcm_ghash_update(state, h, aad, aad_len);
    }
    if (ciphertext_len != 0u) {
        ano_crypto_gcm_ghash_update(state, h, ciphertext, ciphertext_len);
    }
    ano_crypto_gcm_store_u64_be(lengths, ((uint64_t)aad_len) * UINT64_C(8));
    ano_crypto_gcm_store_u64_be(lengths + 8u, ((uint64_t)ciphertext_len) * UINT64_C(8));
    ano_crypto_gcm_xor_block(state, lengths);
    ano_crypto_gcm_multiply(state, h);

    st = ano_crypto_internal_aes_encrypt_block(aes, j0, encrypted_j0);
    if (st == ANO_CRYPTO_OK) {
        size_t i;
        for (i = 0u; i < ANO_CRYPTO_AES256_GCM_TAG_SIZE; ++i) {
            tag[i] = (uint8_t)(encrypted_j0[i] ^ state[i]);
        }
    }

    ano_crypto_internal_zeroize(encrypted_j0, sizeof(encrypted_j0));
    ano_crypto_internal_zeroize(lengths, sizeof(lengths));
    ano_crypto_internal_zeroize(state, sizeof(state));
    return st;
}

static int ano_crypto_gcm_tag_equal(
    const uint8_t left[ANO_CRYPTO_AES256_GCM_TAG_SIZE],
    const uint8_t right[ANO_CRYPTO_AES256_GCM_TAG_SIZE])
{
    uint8_t difference = 0u;
    size_t i;
    for (i = 0u; i < ANO_CRYPTO_AES256_GCM_TAG_SIZE; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(left[i] ^ right[i]));
    }
    return difference == 0u;
}

static ano_crypto_status_t ano_crypto_gcm_prepare_key(
    ano_crypto_csp_handle_t key_handle,
    ano_crypto_internal_aes_ctx_t *aes,
    uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    uint8_t key_material[ANO_CRYPTO_AES256_GCM_KEY_SIZE];
    uint8_t zero_block[ANO_CRYPTO_GCM_BLOCK_SIZE];
    size_t key_material_len = 0u;
    ano_crypto_status_t st;

    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    ano_crypto_internal_zeroize(zero_block, sizeof(zero_block));
    ano_crypto_internal_zeroize(aes, sizeof(*aes));
    ano_crypto_internal_zeroize(h, ANO_CRYPTO_GCM_BLOCK_SIZE);

    st = ano_crypto_internal_csp_copy_secret(
        key_handle,
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
        key_material,
        sizeof(key_material),
        &key_material_len);
    if (st == ANO_CRYPTO_OK && key_material_len != ANO_CRYPTO_AES256_GCM_KEY_SIZE) {
        st = ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_aes_init(aes, key_material, key_material_len);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_aes_encrypt_block(aes, zero_block, h);
    }

    ano_crypto_internal_zeroize(zero_block, sizeof(zero_block));
    ano_crypto_internal_zeroize(key_material, sizeof(key_material));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_aes_uninit(aes);
        ano_crypto_internal_zeroize(h, ANO_CRYPTO_GCM_BLOCK_SIZE);
    }
    return st;
}

static void ano_crypto_gcm_make_j0(
    const uint8_t nonce[ANO_CRYPTO_AES256_GCM_NONCE_SIZE],
    uint8_t j0[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    ano_crypto_internal_zeroize(j0, ANO_CRYPTO_GCM_BLOCK_SIZE);
    (void)memcpy(j0, nonce, ANO_CRYPTO_AES256_GCM_NONCE_SIZE);
    j0[15] = 1u;
}

static ano_crypto_status_t ano_crypto_gcm_validate_lengths(
    size_t aad_len,
    size_t text_len)
{
    if ((uint64_t)aad_len > UINT64_MAX / UINT64_C(8) ||
        (uint64_t)text_len > ANO_CRYPTO_GCM_MAX_TEXT_BYTES) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    return ANO_CRYPTO_OK;
}


static int ano_crypto_gcm_stream_direction_supported(
    ano_crypto_aead_direction_t direction)
{
    return direction == ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT ||
           direction == ANO_CRYPTO_AEAD_DIRECTION_DECRYPT;
}

typedef struct ano_crypto_gcm_stream_alignment_probe_s {
    uint8_t prefix;
    ano_crypto_internal_aes256_gcm_stream_ctx_t value;
} ano_crypto_gcm_stream_alignment_probe_t;

static size_t ano_crypto_gcm_stream_context_alignment(void)
{
    return offsetof(ano_crypto_gcm_stream_alignment_probe_t, value);
}

static int ano_crypto_gcm_stream_context_storage_valid(const void *ctx)
{
    size_t alignment;

    if (!ano_crypto_gcm_range_valid(
            ctx, sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t))) {
        return 0;
    }
    alignment = ano_crypto_gcm_stream_context_alignment();
    return alignment != 0u && ((uintptr_t)ctx % (uintptr_t)alignment) == 0u;
}

static int ano_crypto_gcm_stream_context_valid(
    const ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx)
{
    return ctx != 0 &&
           ctx->magic == ANO_CRYPTO_INTERNAL_GCM_STREAM_MAGIC &&
           ctx->initialized == 1u &&
           ctx->state_version == ANO_CRYPTO_INTERNAL_GCM_STREAM_STATE_VERSION &&
           (ctx->direction == (uint32_t)ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT ||
            ctx->direction == (uint32_t)ANO_CRYPTO_AEAD_DIRECTION_DECRYPT) &&
           (ctx->phase == ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_AAD ||
            ctx->phase == ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_PAYLOAD) &&
           ctx->pending_len < ANO_CRYPTO_GCM_BLOCK_SIZE &&
           ctx->stream_offset <= ANO_CRYPTO_GCM_BLOCK_SIZE &&
           ctx->aad_len <= UINT64_MAX / UINT64_C(8) &&
           ctx->text_len <= ANO_CRYPTO_GCM_MAX_TEXT_BYTES &&
           ano_crypto_internal_aes_context_is_valid(&ctx->aes);
}

static ano_crypto_status_t ano_crypto_gcm_stream_require_context(
    void *ctx,
    ano_crypto_internal_aes256_gcm_stream_ctx_t **out_ctx)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;

    if (ctx == 0 || out_ctx == 0 ||
        !ano_crypto_gcm_stream_context_storage_valid(ctx)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    internal_ctx = (ano_crypto_internal_aes256_gcm_stream_ctx_t *)ctx;
    if (!ano_crypto_gcm_stream_context_valid(internal_ctx)) {
        if (internal_ctx->magic == ANO_CRYPTO_INTERNAL_GCM_STREAM_MAGIC) {
            ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        }
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_ctx = internal_ctx;
    return ANO_CRYPTO_OK;
}

static void ano_crypto_gcm_stream_ghash_block(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx,
    const uint8_t block[ANO_CRYPTO_GCM_BLOCK_SIZE])
{
    ano_crypto_gcm_xor_block(ctx->ghash_state, block);
    ano_crypto_gcm_multiply(ctx->ghash_state, ctx->h);
}

static void ano_crypto_gcm_stream_ghash_absorb(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len)
{
    size_t offset = 0u;

    while (offset < input_len) {
        size_t available = ANO_CRYPTO_GCM_BLOCK_SIZE - ctx->pending_len;
        size_t take = input_len - offset;
        if (take > available) {
            take = available;
        }
        (void)memcpy(ctx->pending + ctx->pending_len, input + offset, take);
        ctx->pending_len += take;
        offset += take;
        if (ctx->pending_len == ANO_CRYPTO_GCM_BLOCK_SIZE) {
            ano_crypto_gcm_stream_ghash_block(ctx, ctx->pending);
            ano_crypto_internal_zeroize(ctx->pending, sizeof(ctx->pending));
            ctx->pending_len = 0u;
        }
    }
}

static void ano_crypto_gcm_stream_ghash_finish_partial(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx)
{
    if (ctx->pending_len != 0u) {
        ano_crypto_gcm_stream_ghash_block(ctx, ctx->pending);
        ano_crypto_internal_zeroize(ctx->pending, sizeof(ctx->pending));
        ctx->pending_len = 0u;
    }
}

static void ano_crypto_gcm_stream_begin_payload(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx)
{
    if (ctx->phase == ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_AAD) {
        ano_crypto_gcm_stream_ghash_finish_partial(ctx);
        ctx->phase = ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_PAYLOAD;
    }
}

static ano_crypto_status_t ano_crypto_gcm_stream_ctr_crypt(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out)
{
    size_t offset = 0u;

    while (offset < input_len) {
        size_t available;
        size_t take;
        size_t i;
        ano_crypto_status_t st;

        if (ctx->stream_offset == ANO_CRYPTO_GCM_BLOCK_SIZE) {
            ano_crypto_gcm_inc32(ctx->counter);
            st = ano_crypto_internal_aes_encrypt_block(
                &ctx->aes, ctx->counter, ctx->stream);
            if (st != ANO_CRYPTO_OK) {
                return st;
            }
            ctx->stream_offset = 0u;
        }
        available = ANO_CRYPTO_GCM_BLOCK_SIZE - ctx->stream_offset;
        take = input_len - offset;
        if (take > available) {
            take = available;
        }
        for (i = 0u; i < take; ++i) {
            out[offset + i] = (uint8_t)(
                input[offset + i] ^ ctx->stream[ctx->stream_offset + i]);
        }
        ctx->stream_offset += take;
        offset += take;
    }
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_gcm_stream_compute_tag(
    ano_crypto_internal_aes256_gcm_stream_ctx_t *ctx,
    uint8_t tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE])
{
    uint8_t lengths[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t encrypted_j0[ANO_CRYPTO_GCM_BLOCK_SIZE];
    ano_crypto_status_t st;
    size_t i;

    ano_crypto_gcm_stream_begin_payload(ctx);
    ano_crypto_gcm_stream_ghash_finish_partial(ctx);
    ano_crypto_internal_zeroize(lengths, sizeof(lengths));
    ano_crypto_internal_zeroize(encrypted_j0, sizeof(encrypted_j0));
    ano_crypto_gcm_store_u64_be(lengths, ctx->aad_len * UINT64_C(8));
    ano_crypto_gcm_store_u64_be(lengths + 8u, ctx->text_len * UINT64_C(8));
    ano_crypto_gcm_stream_ghash_block(ctx, lengths);
    st = ano_crypto_internal_aes_encrypt_block(&ctx->aes, ctx->j0, encrypted_j0);
    if (st == ANO_CRYPTO_OK) {
        for (i = 0u; i < ANO_CRYPTO_AES256_GCM_TAG_SIZE; ++i) {
            tag[i] = (uint8_t)(encrypted_j0[i] ^ ctx->ghash_state[i]);
        }
    }
    ano_crypto_internal_zeroize(encrypted_j0, sizeof(encrypted_j0));
    ano_crypto_internal_zeroize(lengths, sizeof(lengths));
    return st;
}

static int ano_crypto_gcm_stream_add_length(
    uint64_t current,
    size_t additional,
    uint64_t maximum,
    uint64_t *out_total)
{
    uint64_t add;

#if SIZE_MAX > UINT64_MAX
    if (additional > (size_t)UINT64_MAX) {
        return 0;
    }
#endif
    add = (uint64_t)additional;
    if (current > maximum || add > maximum - current) {
        return 0;
    }
    *out_total = current + add;
    return 1;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_context_size(size_t *out_size)
{
    if (out_size == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_size = sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_aead_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    uint32_t flags)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;
    size_t required_len = sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t);
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (ctx == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ctx_len < required_len) {
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    if (!ano_crypto_gcm_stream_context_storage_valid(ctx) ||
        !ano_crypto_gcm_range_valid(nonce, nonce_len) ||
        ano_crypto_gcm_ranges_overlap(ctx, required_len, nonce, nonce_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    internal_ctx = (ano_crypto_internal_aes256_gcm_stream_ctx_t *)ctx;
    ano_crypto_internal_zeroize(internal_ctx, required_len);
    if (!ano_crypto_gcm_stream_direction_supported(direction) ||
        key_handle == ANO_CRYPTO_CSP_INVALID_HANDLE ||
        nonce_len != ANO_CRYPTO_AES256_GCM_NONCE_SIZE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (flags != ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_gcm_prepare_key(key_handle, &internal_ctx->aes, internal_ctx->h);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(internal_ctx, required_len);
        return st;
    }
    ano_crypto_gcm_make_j0(nonce, internal_ctx->j0);
    (void)memcpy(internal_ctx->counter, internal_ctx->j0, sizeof(internal_ctx->counter));
    internal_ctx->stream_offset = ANO_CRYPTO_GCM_BLOCK_SIZE;
    internal_ctx->direction = (uint32_t)direction;
    internal_ctx->phase = ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_AAD;
    internal_ctx->state_version = ANO_CRYPTO_INTERNAL_GCM_STREAM_STATE_VERSION;
    internal_ctx->initialized = 1u;
    internal_ctx->magic = ANO_CRYPTO_INTERNAL_GCM_STREAM_MAGIC;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_aad_update(
    void *ctx,
    const uint8_t *aad,
    size_t aad_len)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;
    uint64_t new_len;
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    st = ano_crypto_gcm_stream_require_context(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (internal_ctx->phase != ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_AAD) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (!ano_crypto_gcm_range_valid(aad, aad_len) ||
        ano_crypto_gcm_ranges_overlap(
            ctx, sizeof(*internal_ctx), aad, aad_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (aad_len == 0u) {
        return ANO_CRYPTO_OK;
    }
    if (!ano_crypto_gcm_stream_add_length(
            internal_ctx->aad_len, aad_len, UINT64_MAX / UINT64_C(8), &new_len)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    ano_crypto_gcm_stream_ghash_absorb(internal_ctx, aad, aad_len);
    internal_ctx->aad_len = new_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;
    uint64_t new_len;
    size_t clear_len;
    ano_crypto_status_t st;

    if (!ano_crypto_gcm_range_valid(out, out_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    clear_len = ano_crypto_gcm_output_clear_len(input_len, out_len);
    if (out_written == 0 ||
        !ano_crypto_gcm_range_valid(out_written, sizeof(*out_written))) {
        ano_crypto_gcm_clear(out, clear_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if ((ctx != 0 && ano_crypto_gcm_ranges_overlap(
            ctx, sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t), out, out_len)) ||
        (ctx != 0 && ano_crypto_gcm_ranges_overlap(
            ctx, sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t),
            out_written, sizeof(*out_written))) ||
        ano_crypto_gcm_ranges_overlap(out, input_len, out_written, sizeof(*out_written))) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (!ano_crypto_gcm_range_valid(input, input_len) ||
        out_len < input_len ||
        ano_crypto_gcm_payload_overlap_invalid(input, input_len, out) ||
        (ctx != 0 && ano_crypto_gcm_ranges_overlap(
            ctx, sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t), input, input_len)) ||
        ano_crypto_gcm_ranges_overlap(
            input, input_len, out_written, sizeof(*out_written))) {
        ano_crypto_gcm_clear(out, clear_len);
        return out_len < input_len ?
            ANO_CRYPTO_ERR_BUFFER_TOO_SMALL : ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(out, clear_len);
        return st;
    }
    st = ano_crypto_gcm_stream_require_context(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(out, clear_len);
        return st;
    }
    if (input_len == 0u) {
        return ANO_CRYPTO_OK;
    }
    if (!ano_crypto_gcm_stream_add_length(
            internal_ctx->text_len, input_len, ANO_CRYPTO_GCM_MAX_TEXT_BYTES, &new_len)) {
        ano_crypto_gcm_clear(out, input_len);
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ano_crypto_gcm_stream_begin_payload(internal_ctx);
    if (internal_ctx->direction == (uint32_t)ANO_CRYPTO_AEAD_DIRECTION_DECRYPT) {
        ano_crypto_gcm_stream_ghash_absorb(internal_ctx, input, input_len);
        st = ano_crypto_gcm_stream_ctr_crypt(internal_ctx, input, input_len, out);
    } else {
        st = ano_crypto_gcm_stream_ctr_crypt(internal_ctx, input, input_len, out);
        if (st == ANO_CRYPTO_OK) {
            ano_crypto_gcm_stream_ghash_absorb(internal_ctx, out, input_len);
        }
    }
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(out, input_len);
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return st;
    }
    internal_ctx->text_len = new_len;
    *out_written = input_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_encrypt_final(
    void *ctx,
    uint8_t *tag,
    size_t tag_len)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;
    uint8_t computed[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
    size_t clear_len;
    ano_crypto_status_t st;

    clear_len = ano_crypto_gcm_range_valid(tag, tag_len) ?
        (tag_len < ANO_CRYPTO_AES256_GCM_TAG_SIZE ?
            tag_len : ANO_CRYPTO_AES256_GCM_TAG_SIZE) : 0u;
    st = ano_crypto_gcm_stream_require_context(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(tag, clear_len);
        return st;
    }
    if (internal_ctx->direction != (uint32_t)ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT) {
        ano_crypto_gcm_clear(tag, clear_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (!ano_crypto_gcm_range_valid(tag, tag_len) ||
        tag_len != ANO_CRYPTO_AES256_GCM_TAG_SIZE ||
        ano_crypto_gcm_ranges_overlap(ctx, sizeof(*internal_ctx), tag, tag_len)) {
        ano_crypto_gcm_clear(tag, clear_len);
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return tag_len < ANO_CRYPTO_AES256_GCM_TAG_SIZE ?
            ANO_CRYPTO_ERR_BUFFER_TOO_SMALL : ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_module_require_operational();
    if (st == ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(computed, sizeof(computed));
        st = ano_crypto_gcm_stream_compute_tag(internal_ctx, computed);
        if (st == ANO_CRYPTO_OK) {
            (void)memcpy(tag, computed, sizeof(computed));
        }
        ano_crypto_internal_zeroize(computed, sizeof(computed));
    }
    ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE);
    }
    return st;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_decrypt_final(
    void *ctx,
    const uint8_t *tag,
    size_t tag_len)
{
    ano_crypto_internal_aes256_gcm_stream_ctx_t *internal_ctx;
    uint8_t expected[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
    ano_crypto_status_t st;

    st = ano_crypto_gcm_stream_require_context(ctx, &internal_ctx);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (internal_ctx->direction != (uint32_t)ANO_CRYPTO_AEAD_DIRECTION_DECRYPT) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (!ano_crypto_gcm_range_valid(tag, tag_len) ||
        tag_len != ANO_CRYPTO_AES256_GCM_TAG_SIZE ||
        ano_crypto_gcm_ranges_overlap(ctx, sizeof(*internal_ctx), tag, tag_len)) {
        ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_module_require_operational();
    ano_crypto_internal_zeroize(expected, sizeof(expected));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_gcm_stream_compute_tag(internal_ctx, expected);
    }
    if (st == ANO_CRYPTO_OK && !ano_crypto_gcm_tag_equal(expected, tag)) {
        st = ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
    }
    ano_crypto_internal_zeroize(expected, sizeof(expected));
    ano_crypto_internal_zeroize(internal_ctx, sizeof(*internal_ctx));
    return st;
}

ano_crypto_status_t ano_crypto_aes256_gcm_stream_uninit(void *ctx, size_t ctx_len)
{
    const size_t required_len = sizeof(ano_crypto_internal_aes256_gcm_stream_ctx_t);

    if (ctx == 0) {
        return ctx_len == 0u ? ANO_CRYPTO_OK : ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (!ano_crypto_gcm_range_valid(ctx, ctx_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ctx_len < required_len) {
        if (ctx_len != 0u) {
            ano_crypto_internal_zeroize(ctx, ctx_len);
        }
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    ano_crypto_internal_zeroize(ctx, required_len);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_encrypt(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *plaintext,
    size_t plaintext_len,
    uint8_t *ciphertext,
    size_t ciphertext_len,
    size_t *out_written,
    uint8_t *tag,
    size_t tag_len)
{
    ano_crypto_internal_aes_ctx_t aes;
    uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t j0[ANO_CRYPTO_GCM_BLOCK_SIZE];
    size_t output_clear_len;
    size_t tag_clear_len;
    ano_crypto_status_t st;

    if (!ano_crypto_gcm_range_valid(ciphertext, ciphertext_len) ||
        !ano_crypto_gcm_range_valid(tag, tag_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    output_clear_len = ano_crypto_gcm_output_clear_len(plaintext_len, ciphertext_len);
    tag_clear_len = tag_len < ANO_CRYPTO_AES256_GCM_TAG_SIZE ?
        tag_len : ANO_CRYPTO_AES256_GCM_TAG_SIZE;
    if (out_written == 0) {
        ano_crypto_gcm_clear(ciphertext, output_clear_len);
        ano_crypto_gcm_clear(tag, tag_clear_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;

    if (!ano_crypto_gcm_range_valid(nonce, nonce_len) ||
        !ano_crypto_gcm_range_valid(aad, aad_len) ||
        !ano_crypto_gcm_range_valid(plaintext, plaintext_len) ||
        nonce_len != ANO_CRYPTO_AES256_GCM_NONCE_SIZE ||
        tag_len != ANO_CRYPTO_AES256_GCM_TAG_SIZE ||
        ciphertext_len < plaintext_len ||
        key_handle == ANO_CRYPTO_CSP_INVALID_HANDLE ||
        ano_crypto_gcm_payload_overlap_invalid(plaintext, plaintext_len, ciphertext) ||
        ano_crypto_gcm_ranges_overlap(ciphertext, plaintext_len, nonce, nonce_len) ||
        ano_crypto_gcm_ranges_overlap(ciphertext, plaintext_len, aad, aad_len) ||
        ano_crypto_gcm_ranges_overlap(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE, nonce, nonce_len) ||
        ano_crypto_gcm_ranges_overlap(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE, aad, aad_len) ||
        ano_crypto_gcm_ranges_overlap(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE, plaintext, plaintext_len) ||
        ano_crypto_gcm_ranges_overlap(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE, ciphertext, plaintext_len)) {
        ano_crypto_gcm_clear(ciphertext, output_clear_len);
        ano_crypto_gcm_clear(tag, tag_clear_len);
        return ciphertext_len < plaintext_len ?
            ANO_CRYPTO_ERR_BUFFER_TOO_SMALL : ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_gcm_validate_lengths(aad_len, plaintext_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(ciphertext, plaintext_len);
        ano_crypto_gcm_clear(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE);
        return st;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(ciphertext, plaintext_len);
        ano_crypto_gcm_clear(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE);
        return st;
    }

    ano_crypto_internal_zeroize(&aes, sizeof(aes));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_zeroize(j0, sizeof(j0));
    st = ano_crypto_gcm_prepare_key(key_handle, &aes, h);
    if (st == ANO_CRYPTO_OK) {
        ano_crypto_gcm_make_j0(nonce, j0);
        st = ano_crypto_gcm_ctr_crypt(&aes, j0, plaintext, plaintext_len, ciphertext);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_gcm_compute_tag(
            &aes, h, j0, aad, aad_len, ciphertext, plaintext_len, tag);
    }

    ano_crypto_internal_zeroize(j0, sizeof(j0));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_aes_uninit(&aes);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(ciphertext, plaintext_len);
        ano_crypto_gcm_clear(tag, ANO_CRYPTO_AES256_GCM_TAG_SIZE);
        return st;
    }

    *out_written = plaintext_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_aes256_gcm_decrypt(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *ciphertext,
    size_t ciphertext_len,
    const uint8_t *tag,
    size_t tag_len,
    uint8_t *plaintext,
    size_t plaintext_len,
    size_t *out_written)
{
    ano_crypto_internal_aes_ctx_t aes;
    uint8_t h[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t j0[ANO_CRYPTO_GCM_BLOCK_SIZE];
    uint8_t expected_tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
    size_t output_clear_len;
    ano_crypto_status_t st;

    if (!ano_crypto_gcm_range_valid(plaintext, plaintext_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    output_clear_len = ano_crypto_gcm_output_clear_len(ciphertext_len, plaintext_len);
    if (out_written == 0) {
        ano_crypto_gcm_clear(plaintext, output_clear_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;

    if (!ano_crypto_gcm_range_valid(nonce, nonce_len) ||
        !ano_crypto_gcm_range_valid(aad, aad_len) ||
        !ano_crypto_gcm_range_valid(ciphertext, ciphertext_len) ||
        !ano_crypto_gcm_range_valid(tag, tag_len) ||
        nonce_len != ANO_CRYPTO_AES256_GCM_NONCE_SIZE ||
        tag_len != ANO_CRYPTO_AES256_GCM_TAG_SIZE ||
        plaintext_len < ciphertext_len ||
        key_handle == ANO_CRYPTO_CSP_INVALID_HANDLE ||
        ano_crypto_gcm_payload_overlap_invalid(ciphertext, ciphertext_len, plaintext) ||
        ano_crypto_gcm_ranges_overlap(plaintext, ciphertext_len, nonce, nonce_len) ||
        ano_crypto_gcm_ranges_overlap(plaintext, ciphertext_len, aad, aad_len) ||
        ano_crypto_gcm_ranges_overlap(plaintext, ciphertext_len, tag, tag_len)) {
        ano_crypto_gcm_clear(plaintext, output_clear_len);
        return plaintext_len < ciphertext_len ?
            ANO_CRYPTO_ERR_BUFFER_TOO_SMALL : ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_gcm_validate_lengths(aad_len, ciphertext_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(plaintext, ciphertext_len);
        return st;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(plaintext, ciphertext_len);
        return st;
    }

    ano_crypto_internal_zeroize(&aes, sizeof(aes));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_zeroize(j0, sizeof(j0));
    ano_crypto_internal_zeroize(expected_tag, sizeof(expected_tag));
    st = ano_crypto_gcm_prepare_key(key_handle, &aes, h);
    if (st == ANO_CRYPTO_OK) {
        ano_crypto_gcm_make_j0(nonce, j0);
        st = ano_crypto_gcm_compute_tag(
            &aes, h, j0, aad, aad_len, ciphertext, ciphertext_len, expected_tag);
    }
    if (st == ANO_CRYPTO_OK && !ano_crypto_gcm_tag_equal(expected_tag, tag)) {
        st = ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_gcm_ctr_crypt(&aes, j0, ciphertext, ciphertext_len, plaintext);
    }

    ano_crypto_internal_zeroize(expected_tag, sizeof(expected_tag));
    ano_crypto_internal_zeroize(j0, sizeof(j0));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_aes_uninit(&aes);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_gcm_clear(plaintext, ciphertext_len);
        return st;
    }

    *out_written = ciphertext_len;
    return ANO_CRYPTO_OK;
}
