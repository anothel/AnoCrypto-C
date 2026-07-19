#include <openssl/evp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

#define CASES 64u
#define MAX_TEXT 257u
#define MAX_AAD 67u

static uint32_t state = UINT32_C(0x4a6f7921);

static uint8_t next_byte(void)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return (uint8_t)state;
}

static int openssl_encrypt(
    const uint8_t key[32], const uint8_t nonce[12],
    const uint8_t *aad, size_t aad_len,
    const uint8_t *plaintext, size_t plaintext_len,
    uint8_t *ciphertext, uint8_t tag[16])
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int out_len = 0;
    int final_len = 0;
    int ok = 0;

    if (ctx == 0) return 0;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), 0, 0, 0) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, 0) != 1 ||
        EVP_EncryptInit_ex(ctx, 0, 0, key, nonce) != 1) goto done;
    if (aad_len != 0u && EVP_EncryptUpdate(ctx, 0, &out_len, aad, (int)aad_len) != 1) goto done;
    if (plaintext_len != 0u &&
        EVP_EncryptUpdate(ctx, ciphertext, &out_len, plaintext, (int)plaintext_len) != 1) goto done;
    if ((size_t)out_len != plaintext_len) goto done;
    if (EVP_EncryptFinal_ex(ctx, ciphertext + out_len, &final_len) != 1 || final_len != 0) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) goto done;
    ok = 1;
done:
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < n; ++i) diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    return diff == 0u;
}

int main(void)
{
    size_t ctx_size = 0u;
    void *ctx = 0;
    size_t comparisons = 0u;
    size_t c;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) return 1;
    if (ano_crypto_aes256_gcm_stream_context_size(&ctx_size) != ANO_CRYPTO_OK) return 2;
    ctx = malloc(ctx_size);
    if (ctx == 0) return 3;

    for (c = 0u; c < CASES; ++c) {
        uint8_t key[32];
        uint8_t nonce[12];
        uint8_t aad[MAX_AAD];
        uint8_t plaintext[MAX_TEXT];
        uint8_t ref[MAX_TEXT];
        uint8_t out[MAX_TEXT];
        uint8_t recovered[MAX_TEXT];
        uint8_t ref_tag[16];
        uint8_t tag[16];
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        size_t aad_len = c % (MAX_AAD + 1u);
        size_t text_len = (c * 37u) % (MAX_TEXT + 1u);
        size_t i;
        size_t offset;
        size_t written;

        for (i = 0u; i < sizeof(key); ++i) key[i] = next_byte();
        for (i = 0u; i < sizeof(nonce); ++i) nonce[i] = next_byte();
        for (i = 0u; i < aad_len; ++i) aad[i] = next_byte();
        for (i = 0u; i < text_len; ++i) plaintext[i] = next_byte();
        if (!openssl_encrypt(key, nonce, aad, aad_len, plaintext, text_len, ref, ref_tag)) return 4;
        if (ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                key, sizeof(key), 0u, &handle) != ANO_CRYPTO_OK) return 5;

        if (ano_crypto_aes256_gcm_stream_init(ctx, ctx_size,
                ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, handle,
                nonce, sizeof(nonce), 0u) != ANO_CRYPTO_OK) return 6;
        if (ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, aad_len / 2u) != ANO_CRYPTO_OK ||
            ano_crypto_aes256_gcm_stream_aad_update(ctx, aad + aad_len / 2u,
                aad_len - aad_len / 2u) != ANO_CRYPTO_OK) return 7;
        offset = 0u;
        while (offset < text_len) {
            size_t take = 1u + ((offset + c * 3u) % 31u);
            if (take > text_len - offset) take = text_len - offset;
            if (ano_crypto_aes256_gcm_stream_update(ctx, plaintext + offset, take,
                    out + offset, take, &written) != ANO_CRYPTO_OK || written != take) return 8;
            offset += take;
        }
        if (text_len == 0u && ano_crypto_aes256_gcm_stream_update(
                ctx, 0, 0u, 0, 0u, &written) != ANO_CRYPTO_OK) return 9;
        if (ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)) != ANO_CRYPTO_OK) return 10;
        if (!bytes_equal(out, ref, text_len) || !bytes_equal(tag, ref_tag, sizeof(tag))) return 11;
        comparisons += 2u;

        if (ano_crypto_aes256_gcm_stream_init(ctx, ctx_size,
                ANO_CRYPTO_AEAD_DIRECTION_DECRYPT, handle,
                nonce, sizeof(nonce), 0u) != ANO_CRYPTO_OK) return 12;
        if (ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, aad_len) != ANO_CRYPTO_OK) return 13;
        offset = 0u;
        while (offset < text_len) {
            size_t take = 1u + ((offset + c * 5u) % 29u);
            if (take > text_len - offset) take = text_len - offset;
            if (ano_crypto_aes256_gcm_stream_update(ctx, ref + offset, take,
                    recovered + offset, take, &written) != ANO_CRYPTO_OK || written != take) return 14;
            offset += take;
        }
        if (ano_crypto_aes256_gcm_stream_decrypt_final(ctx, ref_tag, sizeof(ref_tag)) != ANO_CRYPTO_OK ||
            !bytes_equal(recovered, plaintext, text_len)) return 15;
        comparisons += 2u;
        if (ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) return 16;
    }

    free(ctx);
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 17;
    printf("AES-256-GCM streaming OpenSSL differential comparisons: %lu (values not printed)\n",
           (unsigned long)comparisons);
    return comparisons == (size_t)CASES * 4u ? 0 : 18;
}
