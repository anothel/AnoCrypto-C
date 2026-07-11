#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "ac_aes_internal.h"

#ifndef ANO_CRYPTO_TEST_CMAKE_ENDIAN
#define ANO_CRYPTO_TEST_CMAKE_ENDIAN "UNKNOWN"
#endif

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }
    return diff == 0u;
}

static int bytes_zero(const uint8_t *value, size_t len)
{
    uint8_t acc = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        acc = (uint8_t)(acc | value[i]);
    }
    return acc == 0u;
}

static const char *runtime_endian(void)
{
    const uint32_t marker = 0x01020304u;
    const uint8_t *bytes = (const uint8_t *)&marker;
    if (bytes[0] == 0x01u) return "big";
    if (bytes[0] == 0x04u) return "little";
    return "UNKNOWN";
}

static void fill_bytes(uint8_t *out, size_t len, uint32_t seed)
{
    size_t i;
    uint32_t state = seed;
    for (i = 0u; i < len; ++i) {
        state = state * 1664525u + 1013904223u;
        out[i] = (uint8_t)(state >> 24u);
    }
}

static int test_internal_context_guards(void)
{
    static const size_t key_lens[] = {16u, 24u, 32u};
    uint8_t key_storage[33];
    uint8_t input_storage[17];
    uint8_t output_storage[17];
    uint8_t recovered_storage[17];
    uint8_t *key = key_storage + 1u;
    uint8_t *input = input_storage + 1u;
    uint8_t *output = output_storage + 1u;
    uint8_t *recovered = recovered_storage + 1u;
    size_t i;

    for (i = 0u; i < sizeof(key_lens) / sizeof(key_lens[0]); ++i) {
        ano_crypto_internal_aes_ctx_t ctx;
        fill_bytes(key, key_lens[i], (uint32_t)(0x1000u + i));
        fill_bytes(input, 16u, (uint32_t)(0x2000u + i));
        if (ano_crypto_internal_aes_init(&ctx, key, key_lens[i]) != ANO_CRYPTO_OK ||
            ano_crypto_internal_aes_encrypt_block(&ctx, input, output) != ANO_CRYPTO_OK ||
            ano_crypto_internal_aes_decrypt_block(&ctx, output, recovered) != ANO_CRYPTO_OK ||
            !bytes_equal(recovered, input, 16u)) {
            return 0;
        }

        ctx.round_key_size ^= 4u;
        if (ano_crypto_internal_aes_encrypt_block(&ctx, input, output) !=
            ANO_CRYPTO_ERR_INVALID_ARGUMENT) {
            return 0;
        }

        if (ano_crypto_internal_aes_init(&ctx, key, key_lens[i]) != ANO_CRYPTO_OK ||
            ano_crypto_internal_aes_init(&ctx, 0, key_lens[i]) !=
                ANO_CRYPTO_ERR_INVALID_ARGUMENT ||
            !bytes_zero((const uint8_t *)&ctx, sizeof(ctx))) {
            return 0;
        }
        ano_crypto_internal_aes_uninit(&ctx);
    }

    (void)ano_crypto_csp_zeroize(key_storage, sizeof(key_storage));
    (void)ano_crypto_csp_zeroize(input_storage, sizeof(input_storage));
    (void)ano_crypto_csp_zeroize(output_storage, sizeof(output_storage));
    (void)ano_crypto_csp_zeroize(recovered_storage, sizeof(recovered_storage));
    return 1;
}

static int run_public_case(size_t key_len, uint32_t seed)
{
    uint8_t key_storage[33];
    uint8_t iv_storage[17];
    uint8_t input_storage[257];
    uint8_t encrypted_storage[257];
    uint8_t recovered_storage[257];
    uint8_t inplace_storage[257];
    uint8_t *key = key_storage + 1u;
    uint8_t *iv = iv_storage + 1u;
    uint8_t *input = input_storage + 1u;
    uint8_t *encrypted = encrypted_storage + 1u;
    uint8_t *recovered = recovered_storage + 1u;
    uint8_t *inplace = inplace_storage + 1u;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    size_t offset = 0u;
    size_t written = 0u;
    const size_t input_len = 256u;
    int ok = 0;

    fill_bytes(key, key_len, seed ^ 0x11111111u);
    fill_bytes(iv, 16u, seed ^ 0x22222222u);
    fill_bytes(input, input_len, seed ^ 0x33333333u);

    if (ano_crypto_cipher_context_size(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            &ctx_size) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) goto cleanup;
    memset(ctx, 0xa5, ctx_size);

    if (ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
            key,
            key_len,
            ANO_CRYPTO_KEY_FLAG_NONE,
            &handle) != ANO_CRYPTO_OK ||
        ano_crypto_cipher_init(
            ctx,
            ctx_size,
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
            handle,
            iv,
            16u,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    while (offset < input_len) {
        size_t chunk = (((offset / 16u) % 5u) + 1u) * 16u;
        if (chunk > input_len - offset) chunk = input_len - offset;
        written = 0u;
        if (ano_crypto_cipher_update(
                ctx,
                input + offset,
                chunk,
                encrypted + offset,
                input_len - offset,
                &written) != ANO_CRYPTO_OK || written != chunk) {
            goto cleanup;
        }
        offset += chunk;
    }
    if (ano_crypto_cipher_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
        !bytes_zero(ctx, ctx_size)) {
        goto cleanup;
    }

    if (ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
            key,
            key_len,
            ANO_CRYPTO_KEY_FLAG_NONE,
            &handle) != ANO_CRYPTO_OK ||
        ano_crypto_cipher_init(
            ctx,
            ctx_size,
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
            handle,
            iv,
            16u,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    offset = 0u;
    while (offset < input_len) {
        size_t chunk = (((offset / 16u) % 7u) + 1u) * 16u;
        if (chunk > input_len - offset) chunk = input_len - offset;
        written = 0u;
        if (ano_crypto_cipher_update(
                ctx,
                encrypted + offset,
                chunk,
                recovered + offset,
                input_len - offset,
                &written) != ANO_CRYPTO_OK || written != chunk) {
            goto cleanup;
        }
        offset += chunk;
    }
    if (!bytes_equal(recovered, input, input_len) ||
        ano_crypto_cipher_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
        !bytes_zero(ctx, ctx_size)) {
        goto cleanup;
    }

    memcpy(inplace, input, input_len);
    if (ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
            key,
            key_len,
            ANO_CRYPTO_KEY_FLAG_NONE,
            &handle) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    written = 0u;
    if (ano_crypto_cipher_crypt_with_key(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
            handle,
            iv,
            16u,
            inplace,
            input_len,
            inplace,
            input_len,
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        written != input_len ||
        !bytes_equal(inplace, encrypted, input_len)) {
        goto cleanup;
    }
    ok = 1;

cleanup:
    if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        (void)ano_crypto_key_destroy(handle);
    }
    if (ctx != 0) {
        (void)ano_crypto_cipher_uninit(ctx, ctx_size);
        free(ctx);
    }
    (void)ano_crypto_csp_zeroize(key_storage, sizeof(key_storage));
    (void)ano_crypto_csp_zeroize(iv_storage, sizeof(iv_storage));
    (void)ano_crypto_csp_zeroize(input_storage, sizeof(input_storage));
    (void)ano_crypto_csp_zeroize(encrypted_storage, sizeof(encrypted_storage));
    (void)ano_crypto_csp_zeroize(recovered_storage, sizeof(recovered_storage));
    (void)ano_crypto_csp_zeroize(inplace_storage, sizeof(inplace_storage));
    return ok;
}

int main(void)
{
    static const size_t key_lens[] = {16u, 24u, 32u};
    size_t i;
    const char *endian = runtime_endian();

    if (CHAR_BIT != 8 ||
        strcmp(endian, ANO_CRYPTO_TEST_CMAKE_ENDIAN) != 0 ||
        !test_internal_context_guards()) {
        fprintf(stderr, "AES internal hardening review failed\n");
        return 1;
    }
    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) return 2;
    for (i = 0u; i < sizeof(key_lens) / sizeof(key_lens[0]); ++i) {
        if (!run_public_case(key_lens[i], (uint32_t)(0x5a000000u + i))) {
            fprintf(stderr, "AES public hardening review failed for key length %lu\n",
                    (unsigned long)key_lens[i]);
            return 3;
        }
    }
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 4;

    printf("AES-CBC hardening review passed: pointer_bits=%lu size_t_bits=%lu endian=%s cases=%lu values_not_printed=1\n",
           (unsigned long)(sizeof(void *) * CHAR_BIT),
           (unsigned long)(sizeof(size_t) * CHAR_BIT),
           endian,
           (unsigned long)(sizeof(key_lens) / sizeof(key_lens[0])));
    return 0;
}
