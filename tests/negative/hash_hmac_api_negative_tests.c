#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

static void expect_hmac_storage_cleared(const uint8_t *ctx, size_t ctx_size)
{
    size_t i;

    for (i = 0u; i < ctx_size; ++i) {
        if (ctx[i] != 0u) {
            fputs("FAIL HMAC context storage was not cleared\n", stderr);
            ++g_failures;
            break;
        }
    }
}

static void test_hmac_shutdown_and_reinit(ano_crypto_hash_algorithm_t algorithm)
{
    static const uint8_t key[] = { 'k', 'e', 'y' };
    static const uint8_t msg[] = { 'a', 'b', 'c' };
    uint8_t out[64];
    size_t ctx_size = 0u;
    size_t mac_size = 0u;
    size_t written = 0u;
    uint8_t *ctx;

    expect_status("HMAC context size", ano_crypto_hmac_context_size(algorithm, &ctx_size),
                  ANO_CRYPTO_OK);
    expect_status("HMAC tag size", ano_crypto_hmac_mac_size(algorithm, &mac_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || mac_size == 0u || mac_size > sizeof(out)) {
        ++g_failures;
        return;
    }
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == NULL) {
        ++g_failures;
        return;
    }

    expect_status("HMAC init", ano_crypto_hmac_init(ctx, ctx_size, algorithm, key, sizeof(key)),
                  ANO_CRYPTO_OK);
    expect_status("HMAC update", ano_crypto_hmac_update(ctx, msg, sizeof(msg)), ANO_CRYPTO_OK);
    expect_status("module shutdown with active HMAC", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("HMAC update after shutdown", ano_crypto_hmac_update(ctx, msg, sizeof(msg)),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("HMAC final after shutdown", ano_crypto_hmac_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("explicit clear of abandoned HMAC", ano_crypto_csp_zeroize(ctx, ctx_size),
                  ANO_CRYPTO_OK);
    expect_hmac_storage_cleared(ctx, ctx_size);

    expect_status("module reinit for fresh HMAC", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("fresh HMAC init", ano_crypto_hmac_init(ctx, ctx_size, algorithm, key, sizeof(key)),
                  ANO_CRYPTO_OK);
    expect_status("fresh HMAC update", ano_crypto_hmac_update(ctx, msg, sizeof(msg)), ANO_CRYPTO_OK);
    expect_status("short HMAC final", ano_crypto_hmac_final(ctx, out, mac_size - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("retry HMAC final", ano_crypto_hmac_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != mac_size) {
        fputs("FAIL HMAC final tag length\n", stderr);
        ++g_failures;
    }
    expect_hmac_storage_cleared(ctx, ctx_size);
    expect_status("HMAC repeat final", ano_crypto_hmac_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_csp_zeroize(ctx, ctx_size);
    free(ctx);
}

int main(void)
{
    test_hash_negative_cases();
    test_hmac_negative_cases();
    test_hmac_shutdown_and_reinit(ANO_CRYPTO_HASH_ALG_SHA256);
    test_hmac_shutdown_and_reinit(ANO_CRYPTO_HASH_ALG_SHA384);
    test_hmac_shutdown_and_reinit(ANO_CRYPTO_HASH_ALG_SHA512);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hash_hmac_api_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("hash_hmac_api_negative_tests: PASS");
    return 0;
}
