#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

#if ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE != 32u
#error "ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE must be 32"
#endif
#if ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE != 48u
#error "ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE must be 48"
#endif
#if ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE != 64u
#error "ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE must be 64"
#endif
#if ANO_CRYPTO_HASH_SHA256_BLOCK_SIZE != 64u
#error "ANO_CRYPTO_HASH_SHA256_BLOCK_SIZE must be 64"
#endif
#if ANO_CRYPTO_HASH_SHA384_BLOCK_SIZE != 128u
#error "ANO_CRYPTO_HASH_SHA384_BLOCK_SIZE must be 128"
#endif
#if ANO_CRYPTO_HASH_SHA512_BLOCK_SIZE != 128u
#error "ANO_CRYPTO_HASH_SHA512_BLOCK_SIZE must be 128"
#endif

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static void expect_size(const char *name, size_t got, size_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %lu expected %lu\n",
                name, (unsigned long)got, (unsigned long)expected);
        ++g_failures;
    }
}

static void expect_string(const char *name, const char *got, const char *expected)
{
    if (got == 0 || expected == 0 || strcmp(got, expected) != 0) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, got == 0 ? "(null)" : got, expected == 0 ? "(null)" : expected);
        ++g_failures;
    }
}

static void check_reserved_hash_algorithm(
    ano_crypto_hash_algorithm_t algorithm,
    const char *algorithm_name,
    size_t expected_digest_size)
{
    static const uint8_t msg[] = { 'a', 'b', 'c' };
    uint8_t ctx[512];
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t ctx_size = 0u;
    size_t digest_size = 0u;
    size_t mac_size = 0u;
    size_t written = 99u;

    expect_string("reserved hash algorithm name",
                  ano_crypto_hash_algorithm_name(algorithm), algorithm_name);

    expect_status("reserved hash digest size",
                  ano_crypto_hash_digest_size(algorithm, &digest_size),
                  ANO_CRYPTO_OK);
    expect_size("reserved hash digest size value", digest_size, expected_digest_size);

    expect_status("reserved hash context size",
                  ano_crypto_hash_context_size(algorithm, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL reserved hash context size for %s: %lu\n",
                algorithm_name, (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("reserved hmac mac size",
                  ano_crypto_hmac_mac_size(algorithm, &mac_size),
                  ANO_CRYPTO_OK);
    expect_size("reserved hmac mac size value", mac_size, expected_digest_size);

    written = 99u;
    expect_status("reserved hash digest returns not implemented",
                  ano_crypto_hash_digest(algorithm, msg, sizeof(msg), out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    expect_size("reserved hash digest written remains zero", written, 0u);

    expect_status("reserved hash init returns not implemented",
                  ano_crypto_hash_init(ctx, ctx_size, algorithm),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    written = 99u;
    expect_status("reserved hmac compute returns not implemented",
                  ano_crypto_hmac_compute(algorithm,
                                          msg, sizeof(msg),
                                          msg, sizeof(msg),
                                          out, sizeof(out), &written),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    expect_size("reserved hmac compute written remains zero", written, 0u);

    expect_status("reserved hmac init returns not implemented",
                  ano_crypto_hmac_init(ctx, sizeof(ctx), algorithm, msg, sizeof(msg)),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
}

int main(void)
{
    size_t out_size = 0u;

    expect_status("sha384 digest size before module init",
                  ano_crypto_hash_digest_size(ANO_CRYPTO_HASH_ALG_SHA384, &out_size),
                  ANO_CRYPTO_OK);
    expect_size("sha384 digest size before module init value",
                out_size, ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE);

    expect_status("sha512 digest size before module init",
                  ano_crypto_hash_digest_size(ANO_CRYPTO_HASH_ALG_SHA512, &out_size),
                  ANO_CRYPTO_OK);
    expect_size("sha512 digest size before module init value",
                out_size, ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    check_reserved_hash_algorithm(
        ANO_CRYPTO_HASH_ALG_SHA384,
        "SHA-384",
        ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE);
    check_reserved_hash_algorithm(
        ANO_CRYPTO_HASH_ALG_SHA512,
        "SHA-512",
        ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "sha2_reserved_api_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("sha2_reserved_api_tests: PASS");
    return 0;
}
