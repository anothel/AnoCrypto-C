#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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

static void test_hash_negative_cases(void)
{
    static const uint8_t msg[] = { 'a', 'b', 'c' };
    uint8_t ctx[256];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 123u;

    expect_status("hash digest before init",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         msg, sizeof(msg), out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("hash context size query before init",
                  ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("hash null out",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         msg, sizeof(msg), 0, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("hash null input nonzero length",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         0, 1u, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    written = 77u;
    expect_status("hash output buffer too small",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         msg, sizeof(msg), out, 31u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (written != 0u) {
        fprintf(stderr, "FAIL hash output buffer too small: written %lu\n", (unsigned long)written);
        ++g_failures;
    }

    expect_status("hash unsupported algorithm",
                  ano_crypto_hash_digest((ano_crypto_hash_algorithm_t)999u,
                                         msg, sizeof(msg), out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);

    expect_status("hash init small context",
                  ano_crypto_hash_init(ctx, ctx_size - 1u, ANO_CRYPTO_HASH_ALG_SHA256),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);

    expect_status("hash init valid",
                  ano_crypto_hash_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256),
                  ANO_CRYPTO_OK);
    expect_status("hash update null zero length",
                  ano_crypto_hash_update(ctx, 0, 0u),
                  ANO_CRYPTO_OK);
    expect_status("hash final valid",
                  ano_crypto_hash_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    expect_status("hash final after final rejected",
                  ano_crypto_hash_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
}

static void test_hmac_negative_cases(void)
{
    static const uint8_t key[] = { 'k', 'e', 'y' };
    static const uint8_t msg[] = { 'a', 'b', 'c' };
    uint8_t ctx[512];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 123u;

    expect_status("hmac context size query",
                  ano_crypto_hmac_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);

    expect_status("hmac null key nonzero length",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          0, 1u, msg, sizeof(msg), out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("hmac null input nonzero length",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key, sizeof(key), 0, 1u, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    written = 77u;
    expect_status("hmac output buffer too small",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key, sizeof(key), msg, sizeof(msg), out, 31u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (written != 0u) {
        fprintf(stderr, "FAIL hmac output buffer too small: written %lu\n", (unsigned long)written);
        ++g_failures;
    }

    expect_status("hmac unsupported hash algorithm",
                  ano_crypto_hmac_compute((ano_crypto_hash_algorithm_t)999u,
                                          key, sizeof(key), msg, sizeof(msg), out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);

    expect_status("hmac init small context",
                  ano_crypto_hmac_init(ctx, ctx_size - 1u, ANO_CRYPTO_HASH_ALG_SHA256,
                                       key, sizeof(key)),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);

    expect_status("hmac init valid",
                  ano_crypto_hmac_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256,
                                       key, sizeof(key)),
                  ANO_CRYPTO_OK);
    expect_status("hmac update null zero length",
                  ano_crypto_hmac_update(ctx, 0, 0u),
                  ANO_CRYPTO_OK);
    expect_status("hmac final valid",
                  ano_crypto_hmac_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    expect_status("hmac update after final rejected",
                  ano_crypto_hmac_update(ctx, msg, sizeof(msg)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
}

int main(void)
{
    test_hash_negative_cases();
    test_hmac_negative_cases();

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hash_hmac_api_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("hash_hmac_api_negative_tests: PASS");
    return 0;
}
