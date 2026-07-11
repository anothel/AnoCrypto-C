#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>

#ifndef ANO_CRYPTO_TEST_CMAKE_ENDIAN
#define ANO_CRYPTO_TEST_CMAKE_ENDIAN "unknown"
#endif

#define ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE 521u
#define ANO_CRYPTO_PORTABILITY_KEY_SIZE 257u
#define ANO_CRYPTO_PORTABILITY_MAX_OUTPUT ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE

static void ano_crypto_test_zeroize(void *ptr, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (len != 0u) {
        *p = 0u;
        ++p;
        --len;
    }
}

static void ano_crypto_test_fill(uint8_t *out, size_t len, uint8_t seed)
{
    size_t i;
    for (i = 0u; i < len; ++i) {
        out[i] = (uint8_t)(seed + (uint8_t)(i * 37u) + (uint8_t)(i >> 3u));
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

static const char *ano_crypto_test_runtime_endian(void)
{
    uint32_t value = UINT32_C(0x01020304);
    uint8_t bytes[4];

    (void)memcpy(bytes, &value, sizeof(bytes));
    if (bytes[0] == 0x01u && bytes[1] == 0x02u &&
        bytes[2] == 0x03u && bytes[3] == 0x04u) {
        return "big";
    }
    if (bytes[0] == 0x04u && bytes[1] == 0x03u &&
        bytes[2] == 0x02u && bytes[3] == 0x01u) {
        return "little";
    }
    return "mixed-or-unknown";
}

static int ano_crypto_test_hash_streaming(
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t output_len)
{
    static const size_t chunks[] = { 1u, 3u, 7u, 31u, 127u, 128u, 129u };
    void *ctx = NULL;
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t index = 0u;
    ano_crypto_status_t st;
    int ok = 0;

    st = ano_crypto_hash_context_size(algorithm, &ctx_size);
    if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
        return 0;
    }
    ctx = calloc(1u, ctx_size);
    if (ctx == NULL) {
        return 0;
    }
    st = ano_crypto_hash_init(ctx, ctx_size, algorithm);
    if (st != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    while (offset < input_len) {
        size_t chunk = chunks[index % (sizeof(chunks) / sizeof(chunks[0]))];
        if (chunk > input_len - offset) {
            chunk = input_len - offset;
        }
        st = ano_crypto_hash_update(ctx, input + offset, chunk);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
        offset += chunk;
        ++index;
    }
    st = ano_crypto_hash_final(ctx, out, output_len, &written);
    ok = st == ANO_CRYPTO_OK && written == output_len;

cleanup:
    ano_crypto_test_zeroize(ctx, ctx_size);
    free(ctx);
    return ok;
}

static int ano_crypto_test_algorithm(
    ano_crypto_hash_algorithm_t algorithm,
    size_t output_len,
    const uint8_t *aligned_message,
    const uint8_t *unaligned_message,
    const uint8_t *unaligned_key)
{
    uint8_t aligned_digest[ANO_CRYPTO_PORTABILITY_MAX_OUTPUT];
    uint8_t unaligned_digest[ANO_CRYPTO_PORTABILITY_MAX_OUTPUT];
    uint8_t streaming_digest[ANO_CRYPTO_PORTABILITY_MAX_OUTPUT];
    uint8_t aligned_mac[ANO_CRYPTO_PORTABILITY_MAX_OUTPUT];
    uint8_t unaligned_mac[ANO_CRYPTO_PORTABILITY_MAX_OUTPUT];
    size_t written = 0u;
    ano_crypto_status_t st;
    int ok = 0;

    ano_crypto_test_zeroize(aligned_digest, sizeof(aligned_digest));
    ano_crypto_test_zeroize(unaligned_digest, sizeof(unaligned_digest));
    ano_crypto_test_zeroize(streaming_digest, sizeof(streaming_digest));
    ano_crypto_test_zeroize(aligned_mac, sizeof(aligned_mac));
    ano_crypto_test_zeroize(unaligned_mac, sizeof(unaligned_mac));

    st = ano_crypto_hash_digest(
        algorithm,
        aligned_message,
        ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE,
        aligned_digest,
        sizeof(aligned_digest),
        &written);
    if (st != ANO_CRYPTO_OK || written != output_len) {
        goto cleanup;
    }

    written = 0u;
    st = ano_crypto_hash_digest(
        algorithm,
        unaligned_message,
        ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE,
        unaligned_digest,
        sizeof(unaligned_digest),
        &written);
    if (st != ANO_CRYPTO_OK || written != output_len ||
        !ano_crypto_test_equal(aligned_digest, unaligned_digest, output_len)) {
        goto cleanup;
    }

    if (!ano_crypto_test_hash_streaming(
            algorithm,
            unaligned_message,
            ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE,
            streaming_digest,
            output_len) ||
        !ano_crypto_test_equal(aligned_digest, streaming_digest, output_len)) {
        goto cleanup;
    }

    written = 0u;
    st = ano_crypto_hmac_compute(
        algorithm,
        unaligned_key,
        ANO_CRYPTO_PORTABILITY_KEY_SIZE,
        aligned_message,
        ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE,
        aligned_mac,
        sizeof(aligned_mac),
        &written);
    if (st != ANO_CRYPTO_OK || written != output_len) {
        goto cleanup;
    }

    written = 0u;
    st = ano_crypto_hmac_compute(
        algorithm,
        unaligned_key,
        ANO_CRYPTO_PORTABILITY_KEY_SIZE,
        unaligned_message,
        ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE,
        unaligned_mac,
        sizeof(unaligned_mac),
        &written);
    if (st != ANO_CRYPTO_OK || written != output_len ||
        !ano_crypto_test_equal(aligned_mac, unaligned_mac, output_len)) {
        goto cleanup;
    }

    ok = 1;

cleanup:
    ano_crypto_test_zeroize(aligned_digest, sizeof(aligned_digest));
    ano_crypto_test_zeroize(unaligned_digest, sizeof(unaligned_digest));
    ano_crypto_test_zeroize(streaming_digest, sizeof(streaming_digest));
    ano_crypto_test_zeroize(aligned_mac, sizeof(aligned_mac));
    ano_crypto_test_zeroize(unaligned_mac, sizeof(unaligned_mac));
    return ok;
}

int main(void)
{
    uint64_t aligned_words[(ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE + 7u) / 8u];
    uint8_t unaligned_message_storage[ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE + 1u];
    uint8_t unaligned_key_storage[ANO_CRYPTO_PORTABILITY_KEY_SIZE + 1u];
    uint8_t *aligned_message = (uint8_t *)aligned_words;
    uint8_t *unaligned_message = unaligned_message_storage + 1u;
    uint8_t *unaligned_key = unaligned_key_storage + 1u;
    const char *runtime_endian;
    ano_crypto_status_t st;
    int result = 1;

    ano_crypto_test_fill(aligned_message, ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE, 0x21u);
    (void)memcpy(
        unaligned_message,
        aligned_message,
        ANO_CRYPTO_PORTABILITY_MESSAGE_SIZE);
    ano_crypto_test_fill(unaligned_key, ANO_CRYPTO_PORTABILITY_KEY_SIZE, 0x73u);

    runtime_endian = ano_crypto_test_runtime_endian();
    if (strcmp(ANO_CRYPTO_TEST_CMAKE_ENDIAN, "unknown") != 0 &&
        strcmp(runtime_endian, ANO_CRYPTO_TEST_CMAKE_ENDIAN) != 0) {
        (void)fprintf(stderr,
            "FAIL portability endian cmake=%s runtime=%s\n",
            ANO_CRYPTO_TEST_CMAKE_ENDIAN,
            runtime_endian);
        goto cleanup;
    }

    st = ano_crypto_module_init(NULL);
    if (st != ANO_CRYPTO_OK) {
        (void)fprintf(stderr, "FAIL portability module_init status=%s\n", ano_crypto_status_name(st));
        goto cleanup;
    }

    if (!ano_crypto_test_algorithm(
            ANO_CRYPTO_HASH_ALG_SHA384,
            ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE,
            aligned_message,
            unaligned_message,
            unaligned_key) ||
        !ano_crypto_test_algorithm(
            ANO_CRYPTO_HASH_ALG_SHA512,
            ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE,
            aligned_message,
            unaligned_message,
            unaligned_key)) {
        (void)fprintf(stderr, "FAIL portability SHA-512-family alignment/streaming regression\n");
        goto shutdown;
    }

    (void)printf(
        "sha512-portability status=pass pointer_bits=%zu size_t_bits=%zu uint64_bits=%zu char_bits=%d byte_order=%s cmake_byte_order=%s unaligned_input=pass streaming_chunks=pass values_logged=no\n",
        sizeof(void *) * (size_t)CHAR_BIT,
        sizeof(size_t) * (size_t)CHAR_BIT,
        sizeof(uint64_t) * (size_t)CHAR_BIT,
        CHAR_BIT,
        runtime_endian,
        ANO_CRYPTO_TEST_CMAKE_ENDIAN);
    result = 0;

shutdown:
    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK && result == 0) {
        (void)fprintf(stderr, "FAIL portability module_shutdown status=%s\n", ano_crypto_status_name(st));
        result = 1;
    }

cleanup:
    ano_crypto_test_zeroize(aligned_words, sizeof(aligned_words));
    ano_crypto_test_zeroize(unaligned_message_storage, sizeof(unaligned_message_storage));
    ano_crypto_test_zeroize(unaligned_key_storage, sizeof(unaligned_key_storage));
    return result;
}
