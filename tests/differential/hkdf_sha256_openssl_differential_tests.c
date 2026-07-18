#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <openssl/core_names.h>
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <anocrypto/anocrypto.h>

static int openssl_hkdf_sha256(const uint8_t *ikm, size_t ikm_len,
                               const uint8_t *salt, size_t salt_len,
                               const uint8_t *info, size_t info_len,
                               uint8_t *out, size_t out_len)
{
    EVP_KDF *kdf = NULL;
    EVP_KDF_CTX *ctx = NULL;
    OSSL_PARAM params[6];
    size_t count = 0u;
    char digest[] = "SHA256";
    int mode = EVP_KDF_HKDF_MODE_EXTRACT_AND_EXPAND;
    uint8_t empty = 0u;
    int ok = 0;

    if (out_len == 0u) return 1;
    kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
    if (kdf == NULL) goto done;
    ctx = EVP_KDF_CTX_new(kdf);
    if (ctx == NULL) goto done;

    params[count++] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, digest, 0u);
    params[count++] = OSSL_PARAM_construct_int(OSSL_KDF_PARAM_MODE, &mode);
    params[count++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY,
                                                         (void *)(ikm_len == 0u ? &empty : ikm),
                                                         ikm_len);
    if (salt_len != 0u) {
        params[count++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, (void *)salt, salt_len);
    }
    if (info_len != 0u) {
        params[count++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, (void *)info, info_len);
    }
    params[count] = OSSL_PARAM_construct_end();
    ok = EVP_KDF_derive(ctx, out, out_len, params) == 1;

done:
    EVP_KDF_CTX_free(ctx);
    EVP_KDF_free(kdf);
    return ok;
}

int main(void)
{
    static const size_t lengths[] = { 0u, 1u, 31u, 32u, 33u, 255u, 8160u };
    static uint8_t got[8160];
    static uint8_t expected[8160];
    uint8_t ikm[73];
    uint8_t salt[41];
    uint8_t info[57];
    size_t i;

    for (i = 0u; i < sizeof(ikm); ++i) ikm[i] = (uint8_t)(i * 3u + 1u);
    for (i = 0u; i < sizeof(salt); ++i) salt[i] = (uint8_t)(i * 5u + 2u);
    for (i = 0u; i < sizeof(info); ++i) info[i] = (uint8_t)(i * 7u + 3u);

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) return 1;
    for (i = 0u; i < sizeof(lengths) / sizeof(lengths[0]); ++i) {
        size_t len = lengths[i];
        memset(got, 0xa5, sizeof(got));
        memset(expected, 0x5a, sizeof(expected));
        if (ano_crypto_hkdf_sha256(ikm, sizeof(ikm), salt, sizeof(salt), info, sizeof(info),
                                   len == 0u ? NULL : got, len) != ANO_CRYPTO_OK ||
            !openssl_hkdf_sha256(ikm, sizeof(ikm), salt, sizeof(salt), info, sizeof(info),
                                 len == 0u ? NULL : expected, len) ||
            (len != 0u && memcmp(got, expected, len) != 0)) {
            fprintf(stderr, "HKDF differential mismatch at length %lu\n", (unsigned long)len);
            (void)ano_crypto_module_shutdown();
            return 1;
        }
    }

    if (ano_crypto_hkdf_sha256(NULL, 0u, NULL, 0u, NULL, 0u, got, 32u) != ANO_CRYPTO_OK ||
        !openssl_hkdf_sha256(NULL, 0u, NULL, 0u, NULL, 0u, expected, 32u) ||
        memcmp(got, expected, 32u) != 0) {
        fprintf(stderr, "HKDF empty-input differential mismatch\n");
        (void)ano_crypto_module_shutdown();
        return 1;
    }

    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 1;
    puts("hkdf_sha256_openssl_differential_tests: PASS");
    return 0;
}
