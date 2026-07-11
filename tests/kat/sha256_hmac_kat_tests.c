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

static void expect_bytes(const char *name, const uint8_t *got, const uint8_t *expected, size_t len)
{
    if (memcmp(got, expected, len) != 0) {
        fprintf(stderr, "FAIL %s: byte mismatch\n", name);
        ++g_failures;
    }
}

static void test_sha256_digest_api(void)
{
    static const uint8_t msg_abc[] = { 'a', 'b', 'c' };
    static const uint8_t sha256_empty[32] = {
        0xe3u, 0xb0u, 0xc4u, 0x42u, 0x98u, 0xfcu, 0x1cu, 0x14u,
        0x9au, 0xfbu, 0xf4u, 0xc8u, 0x99u, 0x6fu, 0xb9u, 0x24u,
        0x27u, 0xaeu, 0x41u, 0xe4u, 0x64u, 0x9bu, 0x93u, 0x4cu,
        0xa4u, 0x95u, 0x99u, 0x1bu, 0x78u, 0x52u, 0xb8u, 0x55u
    };
    static const uint8_t sha256_abc[32] = {
        0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
        0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
        0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
        0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu
    };
    uint8_t out[32];
    size_t written = 0u;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("sha256 empty",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         0, 0u, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL sha256 empty written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }
    expect_bytes("sha256 empty", out, sha256_empty, sizeof(out));

    memset(out, 0, sizeof(out));
    written = 0u;
    expect_status("sha256 abc",
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         msg_abc, sizeof(msg_abc), out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL sha256 abc written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }
    expect_bytes("sha256 abc", out, sha256_abc, sizeof(out));
}

static void test_sha256_streaming_api(void)
{
    static const uint8_t msg_abc[] = { 'a', 'b', 'c' };
    static const uint8_t sha256_abc[32] = {
        0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
        0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
        0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
        0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu
    };
    uint8_t ctx[256];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 0u;

    expect_status("sha256 ctx size",
                  ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL sha256 ctx size: got %lu\n", (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("sha256 init",
                  ano_crypto_hash_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256),
                  ANO_CRYPTO_OK);
    expect_status("sha256 update 1", ano_crypto_hash_update(ctx, msg_abc, 1u), ANO_CRYPTO_OK);
    expect_status("sha256 update 2", ano_crypto_hash_update(ctx, msg_abc + 1u, 2u), ANO_CRYPTO_OK);
    expect_status("sha256 final",
                  ano_crypto_hash_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL sha256 streaming written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }
    expect_bytes("sha256 streaming abc", out, sha256_abc, sizeof(out));
}

static void test_hmac_sha256_compute_api(void)
{
    static const uint8_t key_case1[20] = {
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu
    };
    static const uint8_t data_case1[] = { 'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e' };
    static const uint8_t hmac_case1[32] = {
        0xb0u, 0x34u, 0x4cu, 0x61u, 0xd8u, 0xdbu, 0x38u, 0x53u,
        0x5cu, 0xa8u, 0xafu, 0xceu, 0xafu, 0x0bu, 0xf1u, 0x2bu,
        0x88u, 0x1du, 0xc2u, 0x00u, 0xc9u, 0x83u, 0x3du, 0xa7u,
        0x26u, 0xe9u, 0x37u, 0x6cu, 0x2eu, 0x32u, 0xcfu, 0xf7u
    };
    uint8_t out[32];
    size_t written = 0u;

    expect_status("hmac case1",
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key_case1, sizeof(key_case1),
                                          data_case1, sizeof(data_case1),
                                          out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL hmac case1 written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }
    expect_bytes("hmac case1", out, hmac_case1, sizeof(out));
}

static void test_hmac_sha256_streaming_api(void)
{
    static const uint8_t key_case2[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t data_case2[] = {
        'w', 'h', 'a', 't', ' ', 'd', 'o', ' ', 'y', 'a', ' ', 'w', 'a', 'n', 't',
        ' ', 'f', 'o', 'r', ' ', 'n', 'o', 't', 'h', 'i', 'n', 'g', '?'
    };
    static const uint8_t hmac_case2[32] = {
        0x5bu, 0xdcu, 0xc1u, 0x46u, 0xbfu, 0x60u, 0x75u, 0x4eu,
        0x6au, 0x04u, 0x24u, 0x26u, 0x08u, 0x95u, 0x75u, 0xc7u,
        0x5au, 0x00u, 0x3fu, 0x08u, 0x9du, 0x27u, 0x39u, 0x83u,
        0x9du, 0xecu, 0x58u, 0xb9u, 0x64u, 0xecu, 0x38u, 0x43u
    };
    uint8_t ctx[512];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 0u;

    expect_status("hmac ctx size",
                  ano_crypto_hmac_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL hmac ctx size: got %lu\n", (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("hmac init",
                  ano_crypto_hmac_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256,
                                       key_case2, sizeof(key_case2)),
                  ANO_CRYPTO_OK);
    expect_status("hmac update 1", ano_crypto_hmac_update(ctx, data_case2, 9u), ANO_CRYPTO_OK);
    expect_status("hmac update 2", ano_crypto_hmac_update(ctx, data_case2 + 9u, sizeof(data_case2) - 9u), ANO_CRYPTO_OK);
    expect_status("hmac final", ano_crypto_hmac_final(ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL hmac streaming written: got %lu\n", (unsigned long)written);
        ++g_failures;
    }
    expect_bytes("hmac streaming case2", out, hmac_case2, sizeof(out));
}


int main(void)
{
    test_sha256_digest_api();
    test_sha256_streaming_api();
    test_hmac_sha256_compute_api();
    test_hmac_sha256_streaming_api();

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "sha256_hmac_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("sha256_hmac_kat_tests: PASS");
    return 0;
}
