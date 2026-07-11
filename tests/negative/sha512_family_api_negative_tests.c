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

static void expect_size(const char *name, size_t got, size_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %lu expected %lu\n",
                name, (unsigned long)got, (unsigned long)expected);
        ++g_failures;
    }
}

static void check_algorithm(
    ano_crypto_hash_algorithm_t algorithm,
    size_t expected_digest_size)
{
    static const uint8_t msg[] = { 'a', 'b', 'c' };
    static const uint8_t key[] = { 'k', 'e', 'y' };
    uint8_t hash_ctx[512];
    uint8_t hmac_ctx[512];
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t hash_ctx_size = 0u;
    size_t hmac_ctx_size = 0u;
    size_t written = 77u;

    expect_status("hash context size", ano_crypto_hash_context_size(algorithm, &hash_ctx_size), ANO_CRYPTO_OK);
    expect_status("hmac context size", ano_crypto_hmac_context_size(algorithm, &hmac_ctx_size), ANO_CRYPTO_OK);
    if (hash_ctx_size == 0u || hash_ctx_size > sizeof(hash_ctx) ||
        hmac_ctx_size == 0u || hmac_ctx_size > sizeof(hmac_ctx)) {
        fprintf(stderr, "FAIL context size bounds\n");
        ++g_failures;
        return;
    }

    memset(out, 0xa5, sizeof(out));
    written = 77u;
    expect_status("hash short output",
                  ano_crypto_hash_digest(algorithm, msg, sizeof(msg),
                                         out, expected_digest_size - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_size("hash short output written", written, 0u);
    if (out[0] != 0xa5u) {
        fprintf(stderr, "FAIL hash short output modified caller buffer\n");
        ++g_failures;
    }

    expect_status("hash init short context",
                  ano_crypto_hash_init(hash_ctx, hash_ctx_size - 1u, algorithm),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("hash init", ano_crypto_hash_init(hash_ctx, hash_ctx_size, algorithm), ANO_CRYPTO_OK);
    expect_status("hash update null nonzero", ano_crypto_hash_update(hash_ctx, 0, 1u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("hash update", ano_crypto_hash_update(hash_ctx, msg, sizeof(msg)), ANO_CRYPTO_OK);
    written = 0u;
    expect_status("hash final short output",
                  ano_crypto_hash_final(hash_ctx, out, expected_digest_size - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("hash final retry", ano_crypto_hash_final(hash_ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
    expect_size("hash final written", written, expected_digest_size);
    expect_status("hash final after final", ano_crypto_hash_final(hash_ctx, out, sizeof(out), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    memset(out, 0xa5, sizeof(out));
    written = 77u;
    expect_status("hmac short output",
                  ano_crypto_hmac_compute(algorithm, key, sizeof(key), msg, sizeof(msg),
                                          out, expected_digest_size - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_size("hmac short output written", written, 0u);
    if (out[0] != 0xa5u) {
        fprintf(stderr, "FAIL hmac short output modified caller buffer\n");
        ++g_failures;
    }

    expect_status("hmac init short context",
                  ano_crypto_hmac_init(hmac_ctx, hmac_ctx_size - 1u, algorithm, key, sizeof(key)),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("hmac init", ano_crypto_hmac_init(hmac_ctx, hmac_ctx_size, algorithm, key, sizeof(key)), ANO_CRYPTO_OK);
    expect_status("hmac update null nonzero", ano_crypto_hmac_update(hmac_ctx, 0, 1u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("hmac update", ano_crypto_hmac_update(hmac_ctx, msg, sizeof(msg)), ANO_CRYPTO_OK);
    written = 0u;
    expect_status("hmac final short output",
                  ano_crypto_hmac_final(hmac_ctx, out, expected_digest_size - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("hmac final retry", ano_crypto_hmac_final(hmac_ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
    expect_size("hmac final written", written, expected_digest_size);
    expect_status("hmac final after final", ano_crypto_hmac_final(hmac_ctx, out, sizeof(out), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("hash context rejected by hmac update",
                  ano_crypto_hash_init(hash_ctx, hash_ctx_size, algorithm),
                  ANO_CRYPTO_OK);
    expect_status("cross API hash as hmac",
                  ano_crypto_hmac_update(hash_ctx, msg, sizeof(msg)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_csp_zeroize(hash_ctx, sizeof(hash_ctx));

    expect_status("hmac context rejected by hash update",
                  ano_crypto_hmac_init(hmac_ctx, hmac_ctx_size, algorithm, key, sizeof(key)),
                  ANO_CRYPTO_OK);
    expect_status("cross API hmac as hash",
                  ano_crypto_hash_update(hmac_ctx, msg, sizeof(msg)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_csp_zeroize(hmac_ctx, sizeof(hmac_ctx));
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
}

int main(void)
{
    size_t digest_size = 0u;

    expect_status("sha384 size before init",
                  ano_crypto_hash_digest_size(ANO_CRYPTO_HASH_ALG_SHA384, &digest_size),
                  ANO_CRYPTO_OK);
    expect_size("sha384 size", digest_size, ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE);
    expect_status("sha512 size before init",
                  ano_crypto_hash_digest_size(ANO_CRYPTO_HASH_ALG_SHA512, &digest_size),
                  ANO_CRYPTO_OK);
    expect_size("sha512 size", digest_size, ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    check_algorithm(ANO_CRYPTO_HASH_ALG_SHA384, ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE);
    check_algorithm(ANO_CRYPTO_HASH_ALG_SHA512, ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE);
    expect_status("unsupported hash", ano_crypto_hash_context_size((ano_crypto_hash_algorithm_t)999u, &digest_size), ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);
    expect_status("unsupported hmac", ano_crypto_hmac_context_size((ano_crypto_hash_algorithm_t)999u, &digest_size), ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "sha512_family_api_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("sha512_family_api_negative_tests: PASS");
    return 0;
}
