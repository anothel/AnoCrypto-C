#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <anocrypto/anocrypto.h>

#define CASES 128u
#define MAX_TEXT 4096u
#define MAX_AAD 257u

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diff = 0u; size_t i;
    for (i = 0u; i < n; ++i) diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    return diff == 0u;
}

static void fill(uint8_t *out, size_t n, uint32_t seed)
{
    uint32_t x = seed; size_t i;
    for (i = 0u; i < n; ++i) { x ^= x << 13u; x ^= x >> 17u; x ^= x << 5u; out[i] = (uint8_t)(x >> 24u); }
}

static int openssl_encrypt(const uint8_t key[32], const uint8_t nonce[12],
    const uint8_t *aad, size_t aad_len, const uint8_t *in, size_t in_len,
    uint8_t *out, uint8_t tag[16])
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); int part = 0, total = 0, final_part = 0; int ok = 0;
    uint8_t dummy[1];
    if (ctx == 0 || aad_len > 0x7fffffffu || in_len > 0x7fffffffu) goto done;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), 0, 0, 0) != 1) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, 0) != 1) goto done;
    if (EVP_EncryptInit_ex(ctx, 0, 0, key, nonce) != 1) goto done;
    if (aad_len != 0u && EVP_EncryptUpdate(ctx, 0, &part, aad, (int)aad_len) != 1) goto done;
    part = 0;
    if (in_len != 0u && EVP_EncryptUpdate(ctx, out, &part, in, (int)in_len) != 1) goto done;
    total = part;
    if (EVP_EncryptFinal_ex(ctx, in_len == 0u ? dummy : out + total, &final_part) != 1) goto done;
    total += final_part;
    if ((size_t)total != in_len) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) goto done;
    ok = 1;
done:
    EVP_CIPHER_CTX_free(ctx); return ok;
}

static int openssl_decrypt(const uint8_t key[32], const uint8_t nonce[12],
    const uint8_t *aad, size_t aad_len, const uint8_t *in, size_t in_len,
    const uint8_t tag[16], uint8_t *out)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); int part = 0, total = 0, final_part = 0; int ok = 0;
    uint8_t dummy[1]; uint8_t tag_copy[16];
    memcpy(tag_copy, tag, sizeof(tag_copy));
    if (ctx == 0 || aad_len > 0x7fffffffu || in_len > 0x7fffffffu) goto done;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), 0, 0, 0) != 1) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, 0) != 1) goto done;
    if (EVP_DecryptInit_ex(ctx, 0, 0, key, nonce) != 1) goto done;
    if (aad_len != 0u && EVP_DecryptUpdate(ctx, 0, &part, aad, (int)aad_len) != 1) goto done;
    part = 0;
    if (in_len != 0u && EVP_DecryptUpdate(ctx, out, &part, in, (int)in_len) != 1) goto done;
    total = part;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag_copy) != 1) goto done;
    if (EVP_DecryptFinal_ex(ctx, in_len == 0u ? dummy : out + total, &final_part) != 1) goto done;
    total += final_part;
    if ((size_t)total != in_len) goto done;
    ok = 1;
done:
    OPENSSL_cleanse(tag_copy, sizeof(tag_copy)); EVP_CIPHER_CTX_free(ctx); return ok;
}

int main(void)
{
    uint8_t key[32], nonce[12], aad[MAX_AAD], plaintext[MAX_TEXT];
    uint8_t ref[MAX_TEXT], out[MAX_TEXT], recovered[MAX_TEXT], tag_ref[16], tag[16];
    size_t lengths[] = {0u,1u,15u,16u,17u,31u,32u,33u,255u,256u,257u,1024u,4095u,4096u};
    size_t comparisons = 0u; unsigned int c;
    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) return 1;
    for (c = 0u; c < CASES; ++c) {
        size_t text_len = lengths[c % (sizeof(lengths)/sizeof(lengths[0]))];
        size_t aad_len = lengths[(c * 5u + 3u) % (sizeof(lengths)/sizeof(lengths[0]))] % (MAX_AAD + 1u);
        uint32_t seed = 0x9e3779b9u ^ (uint32_t)(c * 0x45d9f3bu);
        ano_crypto_csp_handle_t h = ANO_CRYPTO_CSP_INVALID_HANDLE; size_t written = 0u;
        fill(key, sizeof(key), seed ^ 1u); fill(nonce, sizeof(nonce), seed ^ 2u); fill(aad, aad_len, seed ^ 3u); fill(plaintext, text_len, seed ^ 4u);
        if (!openssl_encrypt(key, nonce, aad, aad_len, plaintext, text_len, ref, tag_ref)) return 2;
        if (ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key, sizeof(key), ANO_CRYPTO_KEY_FLAG_NONE, &h) != ANO_CRYPTO_OK) return 3;
        if (ano_crypto_aes256_gcm_encrypt(h, nonce, sizeof(nonce), aad, aad_len, plaintext, text_len, out, text_len, &written, tag, sizeof(tag)) != ANO_CRYPTO_OK || written != text_len || !bytes_equal(out, ref, text_len) || !bytes_equal(tag, tag_ref, sizeof(tag))) return 4;
        comparisons += 2u;
        if (ano_crypto_aes256_gcm_decrypt(h, nonce, sizeof(nonce), aad, aad_len, ref, text_len, tag_ref, sizeof(tag_ref), recovered, text_len, &written) != ANO_CRYPTO_OK || written != text_len || !bytes_equal(recovered, plaintext, text_len)) return 5;
        if (!openssl_decrypt(key, nonce, aad, aad_len, out, text_len, tag, recovered) || !bytes_equal(recovered, plaintext, text_len)) return 6;
        comparisons += 2u;
        tag_ref[0] ^= 1u; memset(recovered, 0xa5, text_len);
        if (ano_crypto_aes256_gcm_decrypt(h, nonce, sizeof(nonce), aad, aad_len, ref, text_len, tag_ref, sizeof(tag_ref), recovered, text_len, &written) != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED || written != 0u) return 7;
        tag_ref[0] ^= 1u; comparisons += 1u;
        if (ano_crypto_key_destroy(h) != ANO_CRYPTO_OK) return 8;
    }
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 9;
    printf("AES-256-GCM OpenSSL differential comparisons: %lu (values not printed)\n", (unsigned long)comparisons);
    return comparisons == (size_t)CASES * 5u ? 0 : 10;
}
