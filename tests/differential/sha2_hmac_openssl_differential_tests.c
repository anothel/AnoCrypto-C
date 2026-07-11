#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/params.h>

#include <anocrypto/anocrypto.h>

#define ANO_CRYPTO_TEST_MAX_MESSAGE_SIZE (1024u * 1024u + 8u)
#define ANO_CRYPTO_TEST_MAX_KEY_SIZE 257u
#define ANO_CRYPTO_TEST_MAX_DIGEST_SIZE ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE

typedef struct ano_crypto_test_algorithm_s {
    ano_crypto_hash_algorithm_t algorithm;
    const char *openssl_name;
    const char *display_name;
    size_t digest_size;
} ano_crypto_test_algorithm_t;

static const ano_crypto_test_algorithm_t g_algorithms[] = {
    { ANO_CRYPTO_HASH_ALG_SHA256, "SHA256", "SHA-256", ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE },
    { ANO_CRYPTO_HASH_ALG_SHA384, "SHA384", "SHA-384", ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE },
    { ANO_CRYPTO_HASH_ALG_SHA512, "SHA512", "SHA-512", ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE }
};

static const size_t g_hash_message_lengths[] = {
    0u, 1u, 2u, 3u, 7u, 15u, 16u, 31u, 32u, 47u, 48u,
    55u, 56u, 57u, 63u, 64u, 65u, 95u, 111u, 112u, 113u,
    127u, 128u, 129u, 191u, 255u, 256u, 257u, 511u, 512u,
    513u, 1023u, 1024u, 4095u, 4096u, 4097u, 65535u, 65536u,
    65537u, 1024u * 1024u
};

static const size_t g_hmac_message_lengths[] = {
    0u, 1u, 3u, 15u, 31u, 55u, 56u, 63u, 64u, 65u,
    111u, 112u, 127u, 128u, 129u, 255u, 256u, 257u, 1023u,
    4095u, 4096u, 4097u
};

static const size_t g_hmac_key_lengths[] = {
    0u, 1u, 15u, 31u, 32u, 63u, 64u, 65u, 127u, 128u, 129u, 255u, 257u
};

static const size_t g_chunk_pattern[] = {
    1u, 2u, 3u, 7u, 16u, 31u, 64u, 127u, 128u, 129u, 251u
};

static void ano_crypto_test_zeroize(void *ptr, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (len != 0u) {
        *p = 0u;
        ++p;
        --len;
    }
}

static void ano_crypto_test_fill(uint8_t *out, size_t len, uint32_t salt)
{
    uint32_t state = UINT32_C(0x9e3779b9) ^ salt;
    size_t i;

    for (i = 0u; i < len; ++i) {
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        out[i] = (uint8_t)((state >> 24) ^ (uint32_t)i ^ (salt >> (i & 7u)));
    }
}

static int ano_crypto_test_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        diff |= (uint8_t)(a[i] ^ b[i]);
    }
    return diff == 0u;
}

static int ano_crypto_test_openssl_digest(
    const char *name,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t expected_len)
{
    size_t written = 0u;

    if (EVP_Q_digest(NULL, name, NULL, input, input_len, out, &written) != 1) {
        return 0;
    }
    return written == expected_len;
}

static int ano_crypto_test_openssl_hmac(
    const char *digest_name,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t expected_len)
{
    EVP_MAC *mac = NULL;
    EVP_MAC_CTX *ctx = NULL;
    OSSL_PARAM params[2];
    char digest_param[16];
    size_t written = 0u;
    int ok = 0;

    if (strlen(digest_name) >= sizeof(digest_param)) {
        return 0;
    }
    (void)memcpy(digest_param, digest_name, strlen(digest_name) + 1u);

    mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    if (mac == NULL) {
        goto cleanup;
    }
    ctx = EVP_MAC_CTX_new(mac);
    if (ctx == NULL) {
        goto cleanup;
    }

    params[0] = OSSL_PARAM_construct_utf8_string(
        OSSL_MAC_PARAM_DIGEST,
        digest_param,
        0u);
    params[1] = OSSL_PARAM_construct_end();

    if (EVP_MAC_init(ctx, key, key_len, params) != 1) {
        goto cleanup;
    }
    if (EVP_MAC_update(ctx, input, input_len) != 1) {
        goto cleanup;
    }
    if (EVP_MAC_final(ctx, out, &written, expected_len) != 1) {
        goto cleanup;
    }
    ok = written == expected_len;

cleanup:
    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
    ano_crypto_test_zeroize(digest_param, sizeof(digest_param));
    return ok;
}

static int ano_crypto_test_hash_streaming(
    const ano_crypto_test_algorithm_t *algorithm,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out)
{
    void *ctx = NULL;
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk_index = 0u;
    ano_crypto_status_t st;
    int ok = 0;

    st = ano_crypto_hash_context_size(algorithm->algorithm, &ctx_size);
    if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
        return 0;
    }

    ctx = calloc(1u, ctx_size);
    if (ctx == NULL) {
        return 0;
    }

    st = ano_crypto_hash_init(ctx, ctx_size, algorithm->algorithm);
    if (st != ANO_CRYPTO_OK) {
        goto cleanup;
    }

    if (input_len == 0u) {
        st = ano_crypto_hash_update(ctx, input, 0u);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
    }

    while (offset < input_len) {
        size_t chunk = g_chunk_pattern[
            chunk_index % (sizeof(g_chunk_pattern) / sizeof(g_chunk_pattern[0]))];
        if (chunk > (input_len - offset)) {
            chunk = input_len - offset;
        }
        st = ano_crypto_hash_update(ctx, input + offset, chunk);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
        offset += chunk;
        ++chunk_index;
    }

    st = ano_crypto_hash_final(ctx, out, algorithm->digest_size, &written);
    ok = st == ANO_CRYPTO_OK && written == algorithm->digest_size;

cleanup:
    ano_crypto_test_zeroize(ctx, ctx_size);
    free(ctx);
    return ok;
}

static int ano_crypto_test_hmac_streaming(
    const ano_crypto_test_algorithm_t *algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out)
{
    void *ctx = NULL;
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk_index = 0u;
    ano_crypto_status_t st;
    int ok = 0;

    st = ano_crypto_hmac_context_size(algorithm->algorithm, &ctx_size);
    if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
        return 0;
    }

    ctx = calloc(1u, ctx_size);
    if (ctx == NULL) {
        return 0;
    }

    st = ano_crypto_hmac_init(ctx, ctx_size, algorithm->algorithm, key, key_len);
    if (st != ANO_CRYPTO_OK) {
        goto cleanup;
    }

    if (input_len == 0u) {
        st = ano_crypto_hmac_update(ctx, input, 0u);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
    }

    while (offset < input_len) {
        size_t chunk = g_chunk_pattern[
            chunk_index % (sizeof(g_chunk_pattern) / sizeof(g_chunk_pattern[0]))];
        if (chunk > (input_len - offset)) {
            chunk = input_len - offset;
        }
        st = ano_crypto_hmac_update(ctx, input + offset, chunk);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
        offset += chunk;
        ++chunk_index;
    }

    st = ano_crypto_hmac_final(ctx, out, algorithm->digest_size, &written);
    ok = st == ANO_CRYPTO_OK && written == algorithm->digest_size;

cleanup:
    ano_crypto_test_zeroize(ctx, ctx_size);
    free(ctx);
    return ok;
}

static int ano_crypto_test_hash_cases(
    uint8_t *message,
    size_t message_capacity,
    size_t *out_comparisons)
{
    uint8_t reference[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    uint8_t one_shot[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    uint8_t streaming[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    size_t algorithm_index;
    size_t length_index;

    for (algorithm_index = 0u;
         algorithm_index < (sizeof(g_algorithms) / sizeof(g_algorithms[0]));
         ++algorithm_index) {
        const ano_crypto_test_algorithm_t *algorithm = &g_algorithms[algorithm_index];

        for (length_index = 0u;
             length_index < (sizeof(g_hash_message_lengths) / sizeof(g_hash_message_lengths[0]));
             ++length_index) {
            size_t message_len = g_hash_message_lengths[length_index];
            size_t written = 0u;
            ano_crypto_status_t st;

            if (message_len > message_capacity) {
                return 0;
            }
            ano_crypto_test_fill(message, message_len,
                (uint32_t)(message_len ^ (algorithm_index * 0x10001u)));
            ano_crypto_test_zeroize(reference, sizeof(reference));
            ano_crypto_test_zeroize(one_shot, sizeof(one_shot));
            ano_crypto_test_zeroize(streaming, sizeof(streaming));

            if (!ano_crypto_test_openssl_digest(
                    algorithm->openssl_name,
                    message,
                    message_len,
                    reference,
                    algorithm->digest_size)) {
                (void)fprintf(stderr,
                    "FAIL differential provider path=digest algorithm=%s message_len=%zu\n",
                    algorithm->display_name,
                    message_len);
                return 0;
            }

            st = ano_crypto_hash_digest(
                algorithm->algorithm,
                message,
                message_len,
                one_shot,
                sizeof(one_shot),
                &written);
            if (st != ANO_CRYPTO_OK || written != algorithm->digest_size ||
                !ano_crypto_test_equal(reference, one_shot, algorithm->digest_size)) {
                (void)fprintf(stderr,
                    "FAIL differential path=hash-one-shot algorithm=%s message_len=%zu status=%s\n",
                    algorithm->display_name,
                    message_len,
                    ano_crypto_status_name(st));
                return 0;
            }
            ++(*out_comparisons);

            if (!ano_crypto_test_hash_streaming(algorithm, message, message_len, streaming) ||
                !ano_crypto_test_equal(reference, streaming, algorithm->digest_size)) {
                (void)fprintf(stderr,
                    "FAIL differential path=hash-streaming algorithm=%s message_len=%zu\n",
                    algorithm->display_name,
                    message_len);
                return 0;
            }
            ++(*out_comparisons);
        }
    }

    ano_crypto_test_zeroize(reference, sizeof(reference));
    ano_crypto_test_zeroize(one_shot, sizeof(one_shot));
    ano_crypto_test_zeroize(streaming, sizeof(streaming));
    return 1;
}

static int ano_crypto_test_hmac_cases(
    uint8_t *message,
    size_t message_capacity,
    uint8_t *key,
    size_t key_capacity,
    size_t *out_comparisons)
{
    uint8_t reference[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    uint8_t one_shot[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    uint8_t streaming[ANO_CRYPTO_TEST_MAX_DIGEST_SIZE];
    size_t algorithm_index;
    size_t key_index;
    size_t length_index;

    for (algorithm_index = 0u;
         algorithm_index < (sizeof(g_algorithms) / sizeof(g_algorithms[0]));
         ++algorithm_index) {
        const ano_crypto_test_algorithm_t *algorithm = &g_algorithms[algorithm_index];

        for (key_index = 0u;
             key_index < (sizeof(g_hmac_key_lengths) / sizeof(g_hmac_key_lengths[0]));
             ++key_index) {
            size_t key_len = g_hmac_key_lengths[key_index];

            if (key_len > key_capacity) {
                return 0;
            }
            ano_crypto_test_fill(key, key_len,
                (uint32_t)(UINT32_C(0x484d4143) ^ (uint32_t)key_len ^
                           (uint32_t)(algorithm_index * 0x10001u)));

            for (length_index = 0u;
                 length_index < (sizeof(g_hmac_message_lengths) / sizeof(g_hmac_message_lengths[0]));
                 ++length_index) {
                size_t message_len = g_hmac_message_lengths[length_index];
                size_t written = 0u;
                ano_crypto_status_t st;

                if (message_len > message_capacity) {
                    return 0;
                }
                ano_crypto_test_fill(message, message_len,
                    (uint32_t)(UINT32_C(0x4d534700) ^ (uint32_t)message_len ^
                               (uint32_t)(key_index * 257u)));
                ano_crypto_test_zeroize(reference, sizeof(reference));
                ano_crypto_test_zeroize(one_shot, sizeof(one_shot));
                ano_crypto_test_zeroize(streaming, sizeof(streaming));

                if (!ano_crypto_test_openssl_hmac(
                        algorithm->openssl_name,
                        key,
                        key_len,
                        message,
                        message_len,
                        reference,
                        algorithm->digest_size)) {
                    (void)fprintf(stderr,
                        "FAIL differential provider path=hmac algorithm=%s key_len=%zu message_len=%zu\n",
                        algorithm->display_name,
                        key_len,
                        message_len);
                    return 0;
                }

                st = ano_crypto_hmac_compute(
                    algorithm->algorithm,
                    key,
                    key_len,
                    message,
                    message_len,
                    one_shot,
                    sizeof(one_shot),
                    &written);
                if (st != ANO_CRYPTO_OK || written != algorithm->digest_size ||
                    !ano_crypto_test_equal(reference, one_shot, algorithm->digest_size)) {
                    (void)fprintf(stderr,
                        "FAIL differential path=hmac-one-shot algorithm=%s key_len=%zu message_len=%zu status=%s\n",
                        algorithm->display_name,
                        key_len,
                        message_len,
                        ano_crypto_status_name(st));
                    return 0;
                }
                ++(*out_comparisons);

                if (!ano_crypto_test_hmac_streaming(
                        algorithm,
                        key,
                        key_len,
                        message,
                        message_len,
                        streaming) ||
                    !ano_crypto_test_equal(reference, streaming, algorithm->digest_size)) {
                    (void)fprintf(stderr,
                        "FAIL differential path=hmac-streaming algorithm=%s key_len=%zu message_len=%zu\n",
                        algorithm->display_name,
                        key_len,
                        message_len);
                    return 0;
                }
                ++(*out_comparisons);
            }
        }
    }

    ano_crypto_test_zeroize(reference, sizeof(reference));
    ano_crypto_test_zeroize(one_shot, sizeof(one_shot));
    ano_crypto_test_zeroize(streaming, sizeof(streaming));
    return 1;
}

int main(void)
{
    uint8_t *message_storage = NULL;
    uint8_t *key_storage = NULL;
    uint8_t *message;
    uint8_t *key;
    size_t comparisons = 0u;
    ano_crypto_status_t st;
    int result = 1;

    message_storage = (uint8_t *)malloc(ANO_CRYPTO_TEST_MAX_MESSAGE_SIZE + 1u);
    key_storage = (uint8_t *)malloc(ANO_CRYPTO_TEST_MAX_KEY_SIZE + 1u);
    if (message_storage == NULL || key_storage == NULL) {
        (void)fprintf(stderr, "FAIL differential allocation\n");
        goto cleanup;
    }

    /* Intentionally use unaligned inputs to exercise byte-oriented load paths. */
    message = message_storage + 1u;
    key = key_storage + 1u;

    st = ano_crypto_module_init(NULL);
    if (st != ANO_CRYPTO_OK) {
        (void)fprintf(stderr, "FAIL module_init status=%s\n", ano_crypto_status_name(st));
        goto cleanup;
    }

    if (!ano_crypto_test_hash_cases(
            message,
            ANO_CRYPTO_TEST_MAX_MESSAGE_SIZE,
            &comparisons)) {
        goto shutdown;
    }
    if (!ano_crypto_test_hmac_cases(
            message,
            ANO_CRYPTO_TEST_MAX_MESSAGE_SIZE,
            key,
            ANO_CRYPTO_TEST_MAX_KEY_SIZE,
            &comparisons)) {
        goto shutdown;
    }

    (void)printf("sha2-hmac-differential status=pass provider=OpenSSL provider_version=%s comparisons=%zu values_logged=no\n",
        OpenSSL_version(OPENSSL_VERSION),
        comparisons);
    result = 0;

shutdown:
    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK && result == 0) {
        (void)fprintf(stderr, "FAIL module_shutdown status=%s\n", ano_crypto_status_name(st));
        result = 1;
    }

cleanup:
    if (message_storage != NULL) {
        ano_crypto_test_zeroize(message_storage, ANO_CRYPTO_TEST_MAX_MESSAGE_SIZE + 1u);
    }
    if (key_storage != NULL) {
        ano_crypto_test_zeroize(key_storage, ANO_CRYPTO_TEST_MAX_KEY_SIZE + 1u);
    }
    free(message_storage);
    free(key_storage);
    return result;
}
