#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static void test_gate_before_init(void)
{
    uint8_t out[64];
    size_t written = 123u;
    uint8_t ctx[64];
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    const uint8_t key[1] = {0u};

    expect_status("hash before init",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         0, 0u, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("hmac before init",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key, sizeof(key), 0, 0u,
                                          out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("drbg before init",
                  ano_crypto_drbg_generate(ctx, out, sizeof(out), 0, 0u,
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("key import before init",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                                               key, sizeof(key),
                                               ANO_CRYPTO_KEY_FLAG_NONE,
                                               &handle),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
}

static void test_query_processing_and_not_implemented_after_init(void)
{
    uint8_t out[64];
    uint8_t ctx[256];
    uint8_t entropy[32];
    uint8_t nonce[16];
    size_t size = 0u;
    size_t written = 0u;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    const uint8_t key[1] = {0u};
    size_t i;

    for (i = 0u; i < sizeof(entropy); ++i) {
        entropy[i] = (uint8_t)i;
    }
    for (i = 0u; i < sizeof(nonce); ++i) {
        nonce[i] = (uint8_t)(0xa0u + (uint8_t)i);
    }

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("hash digest size",
                  ano_crypto_hash_digest_size(ANO_CRYPTO_HASH_ALG_SHA256, &size),
                  ANO_CRYPTO_OK);
    if (size != 32u) {
        fprintf(stderr, "FAIL hash digest size: got %lu\n", (unsigned long)size);
        ++g_failures;
    }

    expect_status("hmac mac size",
                  ano_crypto_hmac_mac_size(ANO_CRYPTO_HASH_ALG_SHA512, &size),
                  ANO_CRYPTO_OK);
    if (size != 64u) {
        fprintf(stderr, "FAIL hmac mac size: got %lu\n", (unsigned long)size);
        ++g_failures;
    }

    expect_status("drbg ctx size",
                  ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &size),
                  ANO_CRYPTO_OK);
    if (size == 0u || size > sizeof(ctx)) {
        fprintf(stderr, "FAIL drbg ctx size: got %lu\n", (unsigned long)size);
        ++g_failures;
    }

    expect_status("sha256 after init",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         0, 0u, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != 32u) {
        fprintf(stderr, "FAIL sha256 written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }

    written = 0u;
    expect_status("hmac sha256 after init",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key, sizeof(key), 0, 0u,
                                          out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != 32u) {
        fprintf(stderr, "FAIL hmac sha256 written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }

    expect_status("sha512 still not implemented",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA512,
                                         0, 0u, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    expect_status("hmac drbg instantiate after init",
                  ano_crypto_drbg_instantiate(ctx, sizeof(ctx),
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("hash drbg still not implemented",
                  ano_crypto_drbg_instantiate(ctx, sizeof(ctx),
                                              ANO_CRYPTO_DRBG_ALG_HASH_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    expect_status("key import after init",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                                               key, sizeof(key),
                                               ANO_CRYPTO_KEY_FLAG_NONE,
                                               &handle),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
}

static void test_zeroize_helper(void)
{
    uint8_t secret_like_buffer[4] = {1u, 2u, 3u, 4u};
    expect_status("zeroize", ano_crypto_csp_zeroize(secret_like_buffer, sizeof(secret_like_buffer)), ANO_CRYPTO_OK);
    if (memcmp(secret_like_buffer, "\0\0\0\0", sizeof(secret_like_buffer)) != 0) {
        fprintf(stderr, "FAIL zeroize did not clear buffer\n");
        ++g_failures;
    }
}

int main(void)
{
    test_gate_before_init();
    test_query_processing_and_not_implemented_after_init();
    test_zeroize_helper();

    if (g_failures != 0) {
        fprintf(stderr, "crypto_service_gate_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("crypto_service_gate_tests: PASS");
    return 0;
}
