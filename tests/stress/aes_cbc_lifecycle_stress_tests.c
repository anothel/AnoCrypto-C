#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

#define AES_LIFECYCLE_ITERATIONS 128u
#define AES_STRESS_MAX_INPUT 256u

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    return diff == 0u;
}

static int bytes_zero(const uint8_t *value, size_t len)
{
    uint8_t acc = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) acc = (uint8_t)(acc | value[i]);
    return acc == 0u;
}

static void fill_bytes(uint8_t *out, size_t len, uint32_t seed)
{
    size_t i;
    uint32_t state = seed;
    for (i = 0u; i < len; ++i) {
        state ^= state << 13u;
        state ^= state >> 17u;
        state ^= state << 5u;
        out[i] = (uint8_t)(state >> 24u);
    }
}

static int process_stream(
    void *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    unsigned int iteration)
{
    size_t offset = 0u;
    while (offset < input_len) {
        size_t blocks = 1u + (size_t)((iteration + (unsigned int)(offset / 16u)) % 5u);
        size_t chunk = blocks * 16u;
        size_t written = 0u;
        if (chunk > input_len - offset) chunk = input_len - offset;
        if (ano_crypto_cipher_update(
                ctx,
                input + offset,
                chunk,
                output + offset,
                input_len - offset,
                &written) != ANO_CRYPTO_OK || written != chunk) {
            return 0;
        }
        offset += chunk;
    }
    return 1;
}

int main(void)
{
    static const size_t key_lens[] = {16u, 24u, 32u};
    uint8_t key[32];
    uint8_t iv[16];
    uint8_t input[AES_STRESS_MAX_INPUT];
    uint8_t encrypted[AES_STRESS_MAX_INPUT];
    uint8_t recovered[AES_STRESS_MAX_INPUT];
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    unsigned int iteration;
    int result = 1;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK ||
        ano_crypto_cipher_context_size(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            &ctx_size) != ANO_CRYPTO_OK) {
        return 1;
    }
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) return 2;

    for (iteration = 0u; iteration < AES_LIFECYCLE_ITERATIONS; ++iteration) {
        const size_t key_len = key_lens[iteration % 3u];
        const size_t input_len = (size_t)(1u + (iteration % 16u)) * 16u;
        ano_crypto_csp_handle_t enc_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        ano_crypto_csp_handle_t dec_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

        fill_bytes(key, key_len, 0x11000000u ^ iteration);
        fill_bytes(iv, sizeof(iv), 0x22000000u ^ iteration);
        fill_bytes(input, input_len, 0x33000000u ^ iteration);
        memset(ctx, 0x5a, ctx_size);

        if (ano_crypto_key_import_secret(
                ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                key,
                key_len,
                ANO_CRYPTO_KEY_FLAG_NONE,
                &enc_handle) != ANO_CRYPTO_OK ||
            ano_crypto_cipher_init(
                ctx,
                ctx_size,
                ANO_CRYPTO_CIPHER_ALG_AES,
                ANO_CRYPTO_CIPHER_MODE_CBC,
                ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
                enc_handle,
                iv,
                sizeof(iv),
                ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
            ano_crypto_key_destroy(enc_handle) != ANO_CRYPTO_OK ||
            !process_stream(ctx, input, input_len, encrypted, iteration) ||
            ano_crypto_cipher_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
            !bytes_zero(ctx, ctx_size)) {
            result = 3;
            goto cleanup;
        }
        enc_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

        if (ano_crypto_key_import_secret(
                ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                key,
                key_len,
                ANO_CRYPTO_KEY_FLAG_NONE,
                &dec_handle) != ANO_CRYPTO_OK ||
            ano_crypto_cipher_init(
                ctx,
                ctx_size,
                ANO_CRYPTO_CIPHER_ALG_AES,
                ANO_CRYPTO_CIPHER_MODE_CBC,
                ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
                dec_handle,
                iv,
                sizeof(iv),
                ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
            ano_crypto_key_destroy(dec_handle) != ANO_CRYPTO_OK ||
            !process_stream(ctx, encrypted, input_len, recovered, iteration + 1u) ||
            !bytes_equal(input, recovered, input_len) ||
            ano_crypto_cipher_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
            !bytes_zero(ctx, ctx_size)) {
            result = 4;
            goto cleanup;
        }
        dec_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    }

    result = 0;
cleanup:
    if (ctx != 0) {
        (void)ano_crypto_cipher_uninit(ctx, ctx_size);
        free(ctx);
    }
    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(iv, sizeof(iv));
    (void)ano_crypto_csp_zeroize(input, sizeof(input));
    (void)ano_crypto_csp_zeroize(encrypted, sizeof(encrypted));
    (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK && result == 0) result = 5;
    if (result != 0) {
        fprintf(stderr, "AES-CBC lifecycle stress failed: stage=%d\n", result);
        return result;
    }
    printf("AES-CBC lifecycle stress passed: iterations=%u values_not_printed=1\n",
           AES_LIFECYCLE_ITERATIONS);
    return 0;
}
