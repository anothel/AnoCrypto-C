#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <anocrypto/anocrypto.h>

#define AES_DIFFERENTIAL_CASES_PER_KEY 96u
#define AES_DIFFERENTIAL_MAX_BLOCKS 64u
#define AES_DIFFERENTIAL_MAX_INPUT (AES_DIFFERENTIAL_MAX_BLOCKS * 16u)

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }
    return diff == 0u;
}

static void fill_deterministic(uint8_t *out, size_t len, uint32_t seed)
{
    uint32_t state = seed;
    size_t i;
    for (i = 0u; i < len; ++i) {
        state ^= state << 13u;
        state ^= state >> 17u;
        state ^= state << 5u;
        out[i] = (uint8_t)(state >> 24u);
    }
}

static const EVP_CIPHER *cipher_for_key_len(size_t key_len)
{
    if (key_len == 16u) return EVP_aes_128_cbc();
    if (key_len == 24u) return EVP_aes_192_cbc();
    if (key_len == 32u) return EVP_aes_256_cbc();
    return 0;
}

static int openssl_crypt(
    int encrypt,
    const uint8_t *key,
    size_t key_len,
    const uint8_t iv[16],
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t *out_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = cipher_for_key_len(key_len);
    int part = 0;
    int final_part = 0;
    int ok = 0;

    if (ctx == 0 || cipher == 0 || out_len == 0 ||
        input_len > (size_t)0x7fffffff) {
        goto done;
    }
    *out_len = 0u;
    if (EVP_CipherInit_ex(ctx, cipher, 0, key, iv, encrypt) != 1) goto done;
    if (EVP_CIPHER_CTX_set_padding(ctx, 0) != 1) goto done;
    if (EVP_CipherUpdate(ctx, output, &part, input, (int)input_len) != 1) goto done;
    if (EVP_CipherFinal_ex(ctx, output + part, &final_part) != 1) goto done;
    *out_len = (size_t)(part + final_part);
    ok = 1;

done:
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

static int ano_stream_crypt(
    void *ctx,
    size_t ctx_size,
    ano_crypto_cipher_direction_t direction,
    ano_crypto_csp_handle_t handle,
    const uint8_t iv[16],
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    unsigned int case_index)
{
    size_t offset = 0u;
    int ok = 0;

    if (ano_crypto_cipher_init(
            ctx,
            ctx_size,
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            direction,
            handle,
            iv,
            16u,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK) {
        return 0;
    }

    while (offset < input_len) {
        size_t remaining_blocks = (input_len - offset) / 16u;
        size_t chunk_blocks = 1u +
            (size_t)((case_index + (unsigned int)(offset / 16u) * 5u) % 9u);
        size_t chunk;
        size_t written = 0u;
        if (chunk_blocks > remaining_blocks) chunk_blocks = remaining_blocks;
        chunk = chunk_blocks * 16u;
        if (ano_crypto_cipher_update(
                ctx,
                input + offset,
                chunk,
                output + offset,
                input_len - offset,
                &written) != ANO_CRYPTO_OK || written != chunk) {
            goto done;
        }
        offset += chunk;
    }
    ok = 1;

done:
    if (ano_crypto_cipher_uninit(ctx, ctx_size) != ANO_CRYPTO_OK) ok = 0;
    return ok;
}

static int run_case(
    size_t key_len,
    unsigned int key_index,
    unsigned int case_index,
    void *ctx,
    size_t ctx_size,
    size_t *comparisons)
{
    uint8_t key[32];
    uint8_t iv[16];
    uint8_t input[AES_DIFFERENTIAL_MAX_INPUT];
    uint8_t reference[AES_DIFFERENTIAL_MAX_INPUT];
    uint8_t output[AES_DIFFERENTIAL_MAX_INPUT];
    uint8_t recovered[AES_DIFFERENTIAL_MAX_INPUT];
    uint8_t inplace[AES_DIFFERENTIAL_MAX_INPUT];
    const size_t block_count = 1u +
        (size_t)((case_index * 29u + key_index * 11u) % AES_DIFFERENTIAL_MAX_BLOCKS);
    const size_t input_len = block_count * 16u;
    const uint32_t seed = 0x9e3779b9u ^
        ((uint32_t)key_index << 24u) ^ ((uint32_t)case_index * 0x45d9f3bu);
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    size_t reference_len = 0u;
    size_t written = 0u;
    int ok = 0;

    fill_deterministic(key, key_len, seed ^ 0x11111111u);
    fill_deterministic(iv, sizeof(iv), seed ^ 0x22222222u);
    fill_deterministic(input, input_len, seed ^ 0x33333333u);

    if (!openssl_crypt(1, key, key_len, iv, input, input_len,
                       reference, &reference_len) || reference_len != input_len) {
        goto cleanup;
    }
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
            sizeof(iv),
            input,
            input_len,
            output,
            sizeof(output),
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        written != input_len || !bytes_equal(output, reference, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    if (!ano_stream_crypt(
            ctx,
            ctx_size,
            ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
            handle,
            iv,
            input,
            input_len,
            output,
            case_index) || !bytes_equal(output, reference, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    memcpy(inplace, input, input_len);
    written = 0u;
    if (ano_crypto_cipher_crypt_with_key(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
            handle,
            iv,
            sizeof(iv),
            inplace,
            input_len,
            inplace,
            input_len,
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        written != input_len || !bytes_equal(inplace, reference, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    reference_len = 0u;
    if (!openssl_crypt(0, key, key_len, iv, reference, input_len,
                       recovered, &reference_len) || reference_len != input_len ||
        !bytes_equal(recovered, input, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    written = 0u;
    if (ano_crypto_cipher_crypt_with_key(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
            handle,
            iv,
            sizeof(iv),
            reference,
            input_len,
            recovered,
            sizeof(recovered),
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        written != input_len || !bytes_equal(recovered, input, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    if (!ano_stream_crypt(
            ctx,
            ctx_size,
            ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
            handle,
            iv,
            reference,
            input_len,
            recovered,
            case_index + 17u) || !bytes_equal(recovered, input, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;

    memcpy(inplace, reference, input_len);
    written = 0u;
    if (ano_crypto_cipher_crypt_with_key(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
            handle,
            iv,
            sizeof(iv),
            inplace,
            input_len,
            inplace,
            input_len,
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK ||
        written != input_len || !bytes_equal(inplace, input, input_len)) {
        goto cleanup;
    }
    *comparisons += 1u;
    ok = 1;

cleanup:
    if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        (void)ano_crypto_key_destroy(handle);
    }
    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(iv, sizeof(iv));
    (void)ano_crypto_csp_zeroize(input, sizeof(input));
    (void)ano_crypto_csp_zeroize(reference, sizeof(reference));
    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
    (void)ano_crypto_csp_zeroize(inplace, sizeof(inplace));
    return ok;
}

int main(void)
{
    static const size_t key_lens[] = {16u, 24u, 32u};
    void *ctx = 0;
    size_t ctx_size = 0u;
    size_t comparisons = 0u;
    unsigned int key_index;
    unsigned int case_index;
    int result = 1;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK ||
        ano_crypto_cipher_context_size(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            &ctx_size) != ANO_CRYPTO_OK) {
        return 1;
    }
    ctx = malloc(ctx_size);
    if (ctx == 0) return 2;

    for (key_index = 0u;
         key_index < (unsigned int)(sizeof(key_lens) / sizeof(key_lens[0]));
         ++key_index) {
        for (case_index = 0u;
             case_index < AES_DIFFERENTIAL_CASES_PER_KEY;
             ++case_index) {
            if (!run_case(
                    key_lens[key_index],
                    key_index,
                    case_index,
                    ctx,
                    ctx_size,
                    &comparisons)) {
                fprintf(stderr,
                        "AES-CBC differential failure: key_bits=%lu case=%u values_not_printed=1\n",
                        (unsigned long)(key_lens[key_index] * 8u),
                        case_index);
                result = 3;
                goto cleanup;
            }
        }
    }
    result = 0;

cleanup:
    if (ctx != 0) {
        (void)ano_crypto_cipher_uninit(ctx, ctx_size);
        free(ctx);
    }
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK && result == 0) result = 4;
    if (result != 0) return result;
    printf("AES-CBC OpenSSL differential comparisons passed: %lu cases_per_key=%u values_not_printed=1\n",
           (unsigned long)comparisons,
           AES_DIFFERENTIAL_CASES_PER_KEY);
    return 0;
}
