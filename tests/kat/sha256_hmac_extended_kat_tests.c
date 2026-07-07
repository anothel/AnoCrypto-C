#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;
static int g_module_ready = 0;

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
    size_t i;

    if (got == 0 || expected == 0) {
        fprintf(stderr, "FAIL %s: null byte buffer\n", name);
        ++g_failures;
        return;
    }

    for (i = 0u; i < len; ++i) {
        if (got[i] != expected[i]) {
            fprintf(stderr, "FAIL %s: mismatch at byte %lu got 0x%02x expected 0x%02x\n",
                    name, (unsigned long)i, (unsigned int)got[i], (unsigned int)expected[i]);
            ++g_failures;
            return;
        }
    }
}

static void fill_repeat(uint8_t *out, size_t out_len, uint8_t value)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = value;
    }
}

static void fill_sequence(uint8_t *out, size_t out_len, uint8_t seed)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(seed + (uint8_t)(i * 37u) + (uint8_t)(i >> 1));
    }
}

static void ensure_module_ready(void)
{
    if (g_module_ready == 0) {
        expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
        g_module_ready = 1;
    }
}

static void sha256_digest_expect(
    const char *name,
    const uint8_t *input,
    size_t input_len,
    const uint8_t expected[32])
{
    uint8_t out[32];
    size_t written = 0u;

    expect_status(name,
                  ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                         input, input_len, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL %s: written %lu\n", name, (unsigned long)written);
        ++g_failures;
        return;
    }
    expect_bytes(name, out, expected, sizeof(out));
}

static void sha256_stream_expect(
    const char *name,
    const uint8_t *input,
    size_t input_len,
    const size_t *chunks,
    size_t chunk_count,
    const uint8_t expected[32])
{
    uint8_t ctx[256];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk_index = 0u;

    expect_status("sha256 stream context size",
                  ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL %s: invalid ctx size %lu\n", name, (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("sha256 stream init",
                  ano_crypto_hash_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256),
                  ANO_CRYPTO_OK);

    while (offset < input_len) {
        size_t chunk = chunks[chunk_index % chunk_count];
        if (chunk == 0u) {
            chunk = 1u;
        }
        if (chunk > input_len - offset) {
            chunk = input_len - offset;
        }
        expect_status("sha256 stream update",
                      ano_crypto_hash_update(ctx, input + offset, chunk),
                      ANO_CRYPTO_OK);
        offset += chunk;
        ++chunk_index;
    }

    expect_status("sha256 stream final",
                  ano_crypto_hash_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL %s: stream written %lu\n", name, (unsigned long)written);
        ++g_failures;
        return;
    }
    expect_bytes(name, out, expected, sizeof(out));
}

static void hmac_compute_expect(
    const char *name,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    const uint8_t expected[32])
{
    uint8_t out[32];
    size_t written = 0u;

    expect_status(name,
                  ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                          key, key_len,
                                          input, input_len,
                                          out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL %s: written %lu\n", name, (unsigned long)written);
        ++g_failures;
        return;
    }
    expect_bytes(name, out, expected, sizeof(out));
}

static void hmac_stream_expect(
    const char *name,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    const size_t *chunks,
    size_t chunk_count,
    const uint8_t expected[32])
{
    uint8_t ctx[512];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk_index = 0u;

    expect_status("hmac stream context size",
                  ano_crypto_hmac_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL %s: invalid ctx size %lu\n", name, (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("hmac stream init",
                  ano_crypto_hmac_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256, key, key_len),
                  ANO_CRYPTO_OK);

    while (offset < input_len) {
        size_t chunk = chunks[chunk_index % chunk_count];
        if (chunk == 0u) {
            chunk = 1u;
        }
        if (chunk > input_len - offset) {
            chunk = input_len - offset;
        }
        expect_status("hmac stream update",
                      ano_crypto_hmac_update(ctx, input + offset, chunk),
                      ANO_CRYPTO_OK);
        offset += chunk;
        ++chunk_index;
    }

    expect_status("hmac stream final",
                  ano_crypto_hmac_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL %s: stream written %lu\n", name, (unsigned long)written);
        ++g_failures;
        return;
    }
    expect_bytes(name, out, expected, sizeof(out));
}

static void test_sha256_public_extended_vectors(void)
{
    static const uint8_t long_msg[] = {
        'a','b','c','d','b','c','d','e','c','d','e','f','d','e','f','g',
        'e','f','g','h','f','g','h','i','g','h','i','j','h','i','j','k',
        'i','j','k','l','j','k','l','m','k','l','m','n','l','m','n','o',
        'm','n','o','p','n','o','p','q'
    };
    static const uint8_t long_digest[32] = {
        0x24u, 0x8du, 0x6au, 0x61u, 0xd2u, 0x06u, 0x38u, 0xb8u,
        0xe5u, 0xc0u, 0x26u, 0x93u, 0x0cu, 0x3eu, 0x60u, 0x39u,
        0xa3u, 0x3cu, 0xe4u, 0x59u, 0x64u, 0xffu, 0x21u, 0x67u,
        0xf6u, 0xecu, 0xedu, 0xd4u, 0x19u, 0xdbu, 0x06u, 0xc1u
    };
    static const uint8_t million_a_digest[32] = {
        0xcdu, 0xc7u, 0x6eu, 0x5cu, 0x99u, 0x14u, 0xfbu, 0x92u,
        0x81u, 0xa1u, 0xc7u, 0xe2u, 0x84u, 0xd7u, 0x3eu, 0x67u,
        0xf1u, 0x80u, 0x9au, 0x48u, 0xa4u, 0x97u, 0x20u, 0x0eu,
        0x04u, 0x6du, 0x39u, 0xccu, 0xc7u, 0x11u, 0x2cu, 0xd0u
    };
    static const size_t chunks[] = { 1u, 7u, 13u, 29u };
    uint8_t chunk[1000];
    uint8_t ctx[256];
    uint8_t out[32];
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t i;

    ensure_module_ready();

    sha256_digest_expect("sha256 public long message", long_msg, sizeof(long_msg), long_digest);
    sha256_stream_expect("sha256 public long message streaming", long_msg, sizeof(long_msg),
                         chunks, sizeof(chunks) / sizeof(chunks[0]), long_digest);

    fill_repeat(chunk, sizeof(chunk), (uint8_t)'a');
    expect_status("sha256 million-a context size",
                  ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL sha256 million-a ctx size %lu\n", (unsigned long)ctx_size);
        ++g_failures;
        return;
    }
    expect_status("sha256 million-a init",
                  ano_crypto_hash_init(ctx, ctx_size, ANO_CRYPTO_HASH_ALG_SHA256),
                  ANO_CRYPTO_OK);
    for (i = 0u; i < 1000u; ++i) {
        expect_status("sha256 million-a update",
                      ano_crypto_hash_update(ctx, chunk, sizeof(chunk)),
                      ANO_CRYPTO_OK);
    }
    expect_status("sha256 million-a final",
                  ano_crypto_hash_final(ctx, out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(out)) {
        fprintf(stderr, "FAIL sha256 million-a written %lu\n", (unsigned long)written);
        ++g_failures;
        return;
    }
    expect_bytes("sha256 million-a", out, million_a_digest, sizeof(out));
}

static void test_sha256_chunk_boundary_consistency(void)
{
    static const size_t lengths[] = { 0u, 1u, 2u, 3u, 55u, 56u, 57u, 63u, 64u, 65u, 127u, 128u, 129u, 1000u };
    static const size_t chunks[] = { 1u, 2u, 5u, 17u, 31u, 64u };
    uint8_t message[1000];
    uint8_t expected[32];
    size_t i;

    ensure_module_ready();

    fill_sequence(message, sizeof(message), 0x42u);

    for (i = 0u; i < sizeof(lengths) / sizeof(lengths[0]); ++i) {
        size_t written = 0u;
        expect_status("sha256 boundary one-shot",
                      ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                             message, lengths[i], expected, sizeof(expected), &written),
                      ANO_CRYPTO_OK);
        if (written != sizeof(expected)) {
            fprintf(stderr, "FAIL sha256 boundary one-shot written len %lu\n", (unsigned long)lengths[i]);
            ++g_failures;
            continue;
        }
        sha256_stream_expect("sha256 boundary streaming",
                             message, lengths[i], chunks, sizeof(chunks) / sizeof(chunks[0]), expected);
    }
}

static void test_hmac_public_extended_vectors(void)
{
    static const uint8_t hmac_case3[32] = {
        0x77u, 0x3eu, 0xa9u, 0x1eu, 0x36u, 0x80u, 0x0eu, 0x46u,
        0x85u, 0x4du, 0xb8u, 0xebu, 0xd0u, 0x91u, 0x81u, 0xa7u,
        0x29u, 0x59u, 0x09u, 0x8bu, 0x3eu, 0xf8u, 0xc1u, 0x22u,
        0xd9u, 0x63u, 0x55u, 0x14u, 0xceu, 0xd5u, 0x65u, 0xfeu
    };
    static const uint8_t hmac_case4[32] = {
        0x82u, 0x55u, 0x8au, 0x38u, 0x9au, 0x44u, 0x3cu, 0x0eu,
        0xa4u, 0xccu, 0x81u, 0x98u, 0x99u, 0xf2u, 0x08u, 0x3au,
        0x85u, 0xf0u, 0xfau, 0xa3u, 0xe5u, 0x78u, 0xf8u, 0x07u,
        0x7au, 0x2eu, 0x3fu, 0xf4u, 0x67u, 0x29u, 0x66u, 0x5bu
    };
    static const uint8_t hmac_case6[32] = {
        0x60u, 0xe4u, 0x31u, 0x59u, 0x1eu, 0xe0u, 0xb6u, 0x7fu,
        0x0du, 0x8au, 0x26u, 0xaau, 0xcbu, 0xf5u, 0xb7u, 0x7fu,
        0x8eu, 0x0bu, 0xc6u, 0x21u, 0x37u, 0x28u, 0xc5u, 0x14u,
        0x05u, 0x46u, 0x04u, 0x0fu, 0x0eu, 0xe3u, 0x7fu, 0x54u
    };
    static const uint8_t hmac_case7[32] = {
        0x9bu, 0x09u, 0xffu, 0xa7u, 0x1bu, 0x94u, 0x2fu, 0xcbu,
        0x27u, 0x63u, 0x5fu, 0xbcu, 0xd5u, 0xb0u, 0xe9u, 0x44u,
        0xbfu, 0xdcu, 0x63u, 0x64u, 0x4fu, 0x07u, 0x13u, 0x93u,
        0x8au, 0x7fu, 0x51u, 0x53u, 0x5cu, 0x3au, 0x35u, 0xe2u
    };
    static const uint8_t case6_data[] = {
        'T','e','s','t',' ','U','s','i','n','g',' ','L','a','r','g','e','r',' ',
        'T','h','a','n',' ','B','l','o','c','k','-','S','i','z','e',' ','K','e','y',
        ' ','-',' ','H','a','s','h',' ','K','e','y',' ','F','i','r','s','t'
    };
    static const uint8_t case7_data[] = {
        'T','h','i','s',' ','i','s',' ','a',' ','t','e','s','t',' ','u','s','i','n','g',' ',
        'a',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','b','l','o','c','k','-','s','i','z','e',' ',
        'k','e','y',' ','a','n','d',' ','a',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','b','l','o','c','k','-','s','i','z','e',' ',
        'd','a','t','a','.',' ','T','h','e',' ','k','e','y',' ','n','e','e','d','s',' ','t','o',' ','b','e',' ',
        'h','a','s','h','e','d',' ','b','e','f','o','r','e',' ','b','e','i','n','g',' ','u','s','e','d',' ',
        'b','y',' ','t','h','e',' ','H','M','A','C',' ','a','l','g','o','r','i','t','h','m','.'
    };
    static const size_t chunks[] = { 3u, 11u, 23u, 5u };
    uint8_t key_case3[20];
    uint8_t data_case3[50];
    uint8_t key_case4[25];
    uint8_t data_case4[50];
    uint8_t key_long[131];
    size_t i;

    ensure_module_ready();

    fill_repeat(key_case3, sizeof(key_case3), 0xaau);
    fill_repeat(data_case3, sizeof(data_case3), 0xddu);
    hmac_compute_expect("hmac rfc4231 case3", key_case3, sizeof(key_case3),
                        data_case3, sizeof(data_case3), hmac_case3);

    for (i = 0u; i < sizeof(key_case4); ++i) {
        key_case4[i] = (uint8_t)(i + 1u);
    }
    fill_repeat(data_case4, sizeof(data_case4), 0xcdu);
    hmac_compute_expect("hmac rfc4231 case4", key_case4, sizeof(key_case4),
                        data_case4, sizeof(data_case4), hmac_case4);

    fill_repeat(key_long, sizeof(key_long), 0xaau);
    hmac_compute_expect("hmac rfc4231 case6 long key", key_long, sizeof(key_long),
                        case6_data, sizeof(case6_data), hmac_case6);
    hmac_compute_expect("hmac rfc4231 case7 long key and data", key_long, sizeof(key_long),
                        case7_data, sizeof(case7_data), hmac_case7);
    hmac_stream_expect("hmac rfc4231 case7 streaming", key_long, sizeof(key_long),
                       case7_data, sizeof(case7_data), chunks, sizeof(chunks) / sizeof(chunks[0]), hmac_case7);
}

static void test_hmac_chunk_boundary_consistency(void)
{
    static const size_t key_lengths[] = { 0u, 1u, 20u, 64u, 65u, 130u };
    static const size_t data_lengths[] = { 0u, 1u, 3u, 63u, 64u, 65u, 129u, 257u };
    static const size_t chunks[] = { 1u, 8u, 19u, 64u };
    uint8_t key[130];
    uint8_t data[257];
    uint8_t expected[32];
    size_t i;
    size_t j;

    ensure_module_ready();

    fill_sequence(key, sizeof(key), 0x11u);
    fill_sequence(data, sizeof(data), 0x73u);

    for (i = 0u; i < sizeof(key_lengths) / sizeof(key_lengths[0]); ++i) {
        for (j = 0u; j < sizeof(data_lengths) / sizeof(data_lengths[0]); ++j) {
            size_t written = 0u;
            expect_status("hmac boundary one-shot",
                          ano_crypto_hmac_compute(ANO_CRYPTO_HASH_ALG_SHA256,
                                                  key, key_lengths[i],
                                                  data, data_lengths[j],
                                                  expected, sizeof(expected), &written),
                          ANO_CRYPTO_OK);
            if (written != sizeof(expected)) {
                fprintf(stderr, "FAIL hmac boundary one-shot written key %lu data %lu\n",
                        (unsigned long)key_lengths[i], (unsigned long)data_lengths[j]);
                ++g_failures;
                continue;
            }
            hmac_stream_expect("hmac boundary streaming",
                               key, key_lengths[i], data, data_lengths[j],
                               chunks, sizeof(chunks) / sizeof(chunks[0]), expected);
        }
    }
}

int main(void)
{
    test_sha256_public_extended_vectors();
    test_sha256_chunk_boundary_consistency();
    test_hmac_public_extended_vectors();
    test_hmac_chunk_boundary_consistency();

    if (g_module_ready != 0) {
        expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    }

    if (g_failures != 0) {
        fprintf(stderr, "sha256_hmac_extended_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("sha256_hmac_extended_kat_tests: PASS");
    return 0;
}
