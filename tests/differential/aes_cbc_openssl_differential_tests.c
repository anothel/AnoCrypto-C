#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <anocrypto/anocrypto.h>

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    return diff == 0u;
}

static const EVP_CIPHER *cipher_for_key_len(size_t key_len)
{
    if (key_len == 16u) return EVP_aes_128_cbc();
    if (key_len == 24u) return EVP_aes_192_cbc();
    if (key_len == 32u) return EVP_aes_256_cbc();
    return 0;
}

static int openssl_crypt(int encrypt, const uint8_t *key, size_t key_len,
                         const uint8_t iv[16], const uint8_t *input, size_t input_len,
                         uint8_t *output, size_t *out_len)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = cipher_for_key_len(key_len);
    int part = 0;
    int final_part = 0;
    int ok = 0;
    if (ctx == 0 || cipher == 0 || input_len > (size_t)0x7fffffff) goto done;
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

int main(void)
{
    static const size_t key_lens[] = {16u, 24u, 32u};
    static const size_t message_lens[] = {16u, 32u, 64u, 256u};
    uint8_t key[32];
    uint8_t iv[16];
    uint8_t input[256];
    uint8_t ano_out[256];
    uint8_t ref_out[256];
    uint8_t recovered[256];
    size_t comparisons = 0u;
    size_t k;
    size_t m;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) return 1;
    for (k = 0u; k < sizeof(key_lens)/sizeof(key_lens[0]); ++k) {
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        size_t i;
        for (i = 0u; i < key_lens[k]; ++i) key[i] = (uint8_t)(0x11u + (uint8_t)(i * 7u) + (uint8_t)k);
        for (i = 0u; i < sizeof(iv); ++i) iv[i] = (uint8_t)(0xa0u + (uint8_t)i + (uint8_t)k);
        if (ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key, key_lens[k], ANO_CRYPTO_KEY_FLAG_NONE, &handle) != ANO_CRYPTO_OK) return 2;

        for (m = 0u; m < sizeof(message_lens)/sizeof(message_lens[0]); ++m) {
            size_t written = 0u;
            size_t ref_len = 0u;
            for (i = 0u; i < message_lens[m]; ++i) input[i] = (uint8_t)(i * 13u + m * 3u + k);
            if (!openssl_crypt(1, key, key_lens[k], iv, input, message_lens[m], ref_out, &ref_len)) return 3;
            if (ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, handle, iv, sizeof(iv), input, message_lens[m], ano_out, sizeof(ano_out), &written, ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK || written != ref_len || !bytes_equal(ano_out, ref_out, ref_len)) return 4;
            ++comparisons;
            written = 0u;
            if (ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT, handle, iv, sizeof(iv), ref_out, ref_len, recovered, sizeof(recovered), &written, ANO_CRYPTO_CIPHER_FLAG_NONE) != ANO_CRYPTO_OK || written != message_lens[m] || !bytes_equal(recovered, input, message_lens[m])) return 5;
            ++comparisons;
        }
        if (ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) return 6;
    }
    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(input, sizeof(input));
    (void)ano_crypto_csp_zeroize(ano_out, sizeof(ano_out));
    (void)ano_crypto_csp_zeroize(ref_out, sizeof(ref_out));
    (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 7;
    printf("AES-CBC OpenSSL differential comparisons passed: %lu (values not printed)\n", (unsigned long)comparisons);
    return 0;
}
