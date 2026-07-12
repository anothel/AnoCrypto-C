#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "ac_cmac_internal.h"

#define CMAC_SENTINEL_BYTES 32u

typedef union cmac_alias_storage_u {
    size_t written;
    uint8_t bytes[32];
} cmac_alias_storage_t;

static int bytes_equal(const uint8_t *left, const uint8_t *right, size_t length)
{
    uint8_t difference = 0u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(left[i] ^ right[i]));
    }
    return difference == 0u;
}

static int bytes_zero(const uint8_t *buffer, size_t length)
{
    uint8_t aggregate = 0u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        aggregate = (uint8_t)(aggregate | buffer[i]);
    }
    return aggregate == 0u;
}

static int bytes_value(const uint8_t *buffer, size_t length, uint8_t value)
{
    size_t i;
    for (i = 0u; i < length; ++i) {
        if (buffer[i] != value) {
            return 0;
        }
    }
    return 1;
}

static void fill_bytes(uint8_t *buffer, size_t length, uint32_t seed)
{
    uint32_t value = seed ^ 0x6a09e667u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        value ^= value << 13u;
        value ^= value >> 17u;
        value ^= value << 5u;
        buffer[i] = (uint8_t)(value >> 24u);
    }
}

static int import_cmac_key(
    const uint8_t *key,
    size_t key_len,
    ano_crypto_csp_handle_t *out_handle)
{
    return ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_CMAC,
        key,
        key_len,
        ANO_CRYPTO_KEY_FLAG_NONE,
        out_handle) == ANO_CRYPTO_OK;
}

int main(void)
{
    uint8_t key_storage[33];
    uint8_t message_storage[258];
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t second_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t overlap[64];
    uint8_t *ctx_storage = 0;
    uint8_t *ctx;
    size_t ctx_size = 0u;
    size_t written = 0u;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_internal_cmac_ctx_t *internal_ctx;
    cmac_alias_storage_t alias_storage;
    ano_crypto_status_t st;
    size_t i;
    int stage = 0;

    fill_bytes(key_storage, sizeof(key_storage), 0x2901u);
    fill_bytes(message_storage, sizeof(message_storage), 0x2902u);
    memset(tag, 0, sizeof(tag));
    memset(second_tag, 0, sizeof(second_tag));
    memset(altered, 0, sizeof(altered));
    memset(overlap, 0x5au, sizeof(overlap));
    memset(&alias_storage, 0, sizeof(alias_storage));

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_context_size(&ctx_size) != ANO_CRYPTO_OK ||
        ctx_size != sizeof(ano_crypto_internal_cmac_ctx_t)) {
        stage = 1;
        goto cleanup;
    }

    ctx_storage = (uint8_t *)malloc(ctx_size + CMAC_SENTINEL_BYTES);
    if (ctx_storage == 0) {
        stage = 2;
        goto cleanup;
    }
    ctx = ctx_storage;
    internal_ctx = (ano_crypto_internal_cmac_ctx_t *)ctx;

    if (!import_cmac_key(key_storage + 1u, 32u, &handle)) {
        stage = 3;
        goto cleanup;
    }

    /* A streaming context owns its copied schedule after successful init. */
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
        stage = 4;
        goto cleanup;
    }
    handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    if (ano_crypto_cmac_update(ctx, message_storage + 1u, 257u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written) != ANO_CRYPTO_OK ||
        written != sizeof(tag) || !bytes_zero(ctx, ctx_size)) {
        stage = 5;
        goto cleanup;
    }

    if (!import_cmac_key(key_storage + 1u, 32u, &handle)) {
        stage = 6;
        goto cleanup;
    }

    /* Context/input and context/output aliasing are rejected before state use. */
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_update(ctx, ctx + 1u, 1u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_update(ctx, message_storage + 1u, 31u) != ANO_CRYPTO_OK) {
        stage = 7;
        goto cleanup;
    }
    written = 91u;
    if (ano_crypto_cmac_final(
            ctx, ctx + 1u, ANO_CRYPTO_CMAC_TAG_SIZE, &written) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_final(ctx, second_tag, sizeof(second_tag), &written) !=
            ANO_CRYPTO_OK ||
        written != sizeof(second_tag)) {
        stage = 8;
        goto cleanup;
    }

    /* Output and output-length storage must not overlap; rejected calls preserve
     * the valid context so the caller can retry with independent storage. */
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_update(ctx, message_storage + 1u, 17u) != ANO_CRYPTO_OK) {
        stage = 9;
        goto cleanup;
    }
    alias_storage.written = 73u;
    if (ano_crypto_cmac_final(
            ctx,
            alias_storage.bytes,
            ANO_CRYPTO_CMAC_TAG_SIZE,
            &alias_storage.written) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_final(ctx, second_tag, sizeof(second_tag), &written) !=
            ANO_CRYPTO_OK) {
        stage = 10;
        goto cleanup;
    }

    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_update(ctx, message_storage + 1u, 15u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_final(
            ctx, second_tag, sizeof(second_tag), (size_t *)ctx) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_final(ctx, second_tag, sizeof(second_tag), &written) !=
            ANO_CRYPTO_OK) {
        stage = 11;
        goto cleanup;
    }

    /* Failed reinitialization after size validation invalidates the earlier state. */
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_init(
            ctx, ctx_size, ANO_CRYPTO_CSP_INVALID_HANDLE, 0u) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        !bytes_zero(ctx, ctx_size) ||
        ano_crypto_cmac_update(ctx, message_storage + 1u, 1u) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT) {
        stage = 12;
        goto cleanup;
    }

    /* Internal state-version and AES schedule guards invalidate marked CMAC
     * contexts rather than retaining damaged secret-dependent state. */
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK) {
        stage = 13;
        goto cleanup;
    }
    internal_ctx->state_version ^= 1u;
    if (ano_crypto_cmac_update(ctx, 0, 0u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        !bytes_zero(ctx, ctx_size)) {
        stage = 14;
        goto cleanup;
    }

    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK) {
        stage = 15;
        goto cleanup;
    }
    internal_ctx->aes.round_key_size ^= 4u;
    if (ano_crypto_cmac_update(ctx, 0, 0u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        !bytes_zero(ctx, ctx_size)) {
        stage = 16;
        goto cleanup;
    }

    /* uninit clears the queried context extent, not unrelated trailing bytes. */
    memset(ctx_storage, 0xa5, ctx_size + CMAC_SENTINEL_BYTES);
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_uninit(ctx, ctx_size + CMAC_SENTINEL_BYTES) != ANO_CRYPTO_OK ||
        !bytes_zero(ctx, ctx_size) ||
        !bytes_value(ctx + ctx_size, CMAC_SENTINEL_BYTES, 0xa5u)) {
        stage = 17;
        goto cleanup;
    }

    memset(ctx_storage, 0xa5, ctx_size + CMAC_SENTINEL_BYTES);
    if (ano_crypto_cmac_init(ctx, ctx_size, handle, 0u) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_uninit(ctx, ctx_size - 1u) !=
            ANO_CRYPTO_ERR_BUFFER_TOO_SMALL ||
        !bytes_zero(ctx, ctx_size - 1u) ||
        ano_crypto_cmac_update(ctx, 0, 0u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
        !bytes_zero(ctx, ctx_size) ||
        !bytes_value(ctx + ctx_size, CMAC_SENTINEL_BYTES, 0xa5u)) {
        stage = 18;
        goto cleanup;
    }

    /* One-shot overlap is rejected and every mismatch position maps to the same
     * public authentication-failure status without printing tag material. */
    if (ano_crypto_cmac_compute_with_key(
            handle, overlap, 32u, overlap + 8u, ANO_CRYPTO_CMAC_TAG_SIZE,
            &written, 0u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_compute_with_key(
            handle, overlap, 32u, tag, ANO_CRYPTO_CMAC_TAG_SIZE,
            (size_t *)(void *)(overlap + 8u), 0u) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_verify_with_key(
            handle, overlap, 32u, overlap + 8u, ANO_CRYPTO_CMAC_TAG_SIZE,
            0u) != ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
        ano_crypto_cmac_compute_with_key(
            handle, message_storage + 1u, 257u, tag, sizeof(tag),
            &written, 0u) != ANO_CRYPTO_OK) {
        stage = 19;
        goto cleanup;
    }
    for (i = 0u; i < sizeof(tag); ++i) {
        memcpy(altered, tag, sizeof(altered));
        altered[i] ^= 1u;
        st = ano_crypto_cmac_verify_with_key(
            handle, message_storage + 1u, 257u,
            altered, sizeof(altered), 0u);
        if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED) {
            stage = 20;
            goto cleanup;
        }
    }
    if (!ano_crypto_internal_cmac_tags_equal(tag, tag, sizeof(tag)) ||
        ano_crypto_internal_cmac_tags_equal(tag, altered, sizeof(tag)) ||
        !bytes_equal(tag, tag, sizeof(tag))) {
        stage = 21;
        goto cleanup;
    }

cleanup:
    if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        (void)ano_crypto_key_destroy(handle);
    }
    if (ctx_storage != 0) {
        (void)ano_crypto_cmac_uninit(ctx_storage, ctx_size);
        free(ctx_storage);
    }
    (void)ano_crypto_csp_zeroize(key_storage, sizeof(key_storage));
    (void)ano_crypto_csp_zeroize(message_storage, sizeof(message_storage));
    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    (void)ano_crypto_csp_zeroize(second_tag, sizeof(second_tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));
    (void)ano_crypto_csp_zeroize(overlap, sizeof(overlap));
    (void)ano_crypto_csp_zeroize(&alias_storage, sizeof(alias_storage));
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK && stage == 0) {
        stage = 22;
    }

    if (stage != 0) {
        fprintf(stderr, "AES-CMAC hardening review failed: stage=%d\n", stage);
        return stage;
    }
    printf("AES-CMAC hardening review passed: context_guard=1 alias_guard=1 bounded_uninit=1 mismatch_positions=%u values_not_printed=1\n",
           (unsigned int)ANO_CRYPTO_CMAC_TAG_SIZE);
    return 0;
}
