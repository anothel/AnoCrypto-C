#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <anocrypto/anocrypto.h>

#define CMAC_DIFFERENTIAL_CASES_PER_KEY 96u
#define CMAC_DIFFERENTIAL_PATHS_PER_CASE 6u
#define CMAC_DIFFERENTIAL_MAX_MESSAGE 2048u

static int equal_bytes(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t difference = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(a[i] ^ b[i]));
    }
    return difference == 0u;
}

static const char *cipher_name_for_key_size(size_t key_len)
{
    switch (key_len) {
    case 16u: return "AES-128-CBC";
    case 24u: return "AES-192-CBC";
    case 32u: return "AES-256-CBC";
    default: return 0;
    }
}

static int openssl_cmac(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *message,
    size_t message_len,
    uint8_t out[ANO_CRYPTO_CMAC_TAG_SIZE])
{
    EVP_MAC *mac = 0;
    EVP_MAC_CTX *ctx = 0;
    const char *cipher_name = cipher_name_for_key_size(key_len);
    OSSL_PARAM params[2];
    size_t out_len = 0u;
    int ok = 0;

    if (cipher_name == 0) {
        return 0;
    }

    mac = EVP_MAC_fetch(0, "CMAC", 0);
    if (mac == 0) {
        goto cleanup;
    }
    ctx = EVP_MAC_CTX_new(mac);
    if (ctx == 0) {
        goto cleanup;
    }

    params[0] = OSSL_PARAM_construct_utf8_string(
        OSSL_MAC_PARAM_CIPHER, (char *)cipher_name, 0u);
    params[1] = OSSL_PARAM_construct_end();

    if (EVP_MAC_init(ctx, key, key_len, params) != 1) {
        goto cleanup;
    }
    if (message_len != 0u && EVP_MAC_update(ctx, message, message_len) != 1) {
        goto cleanup;
    }
    if (EVP_MAC_final(ctx, out, &out_len, ANO_CRYPTO_CMAC_TAG_SIZE) != 1 ||
        out_len != ANO_CRYPTO_CMAC_TAG_SIZE) {
        goto cleanup;
    }
    ok = 1;

cleanup:
    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
    return ok;
}

static void fill_case(uint8_t *buffer, size_t length, uint32_t seed)
{
    uint32_t value = seed ^ 0x9e3779b9u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        value ^= value << 13u;
        value ^= value >> 17u;
        value ^= value << 5u;
        buffer[i] = (uint8_t)(value >> 24u);
    }
}

static size_t message_length_for_case(size_t case_index)
{
    static const size_t boundaries[] = {
        0u,1u,2u,3u,7u,8u,14u,15u,16u,17u,18u,30u,31u,32u,33u,
        46u,47u,48u,49u,62u,63u,64u,65u,66u,126u,127u,128u,129u,
        254u,255u,256u,257u,510u,511u,512u,513u,1023u,1024u,
        1025u,2047u,2048u
    };
    if (case_index < sizeof(boundaries) / sizeof(boundaries[0])) {
        return boundaries[case_index];
    }
    return (size_t)(((uint32_t)case_index * 97u +
                     (uint32_t)case_index * (uint32_t)case_index * 13u + 19u) %
                    (CMAC_DIFFERENTIAL_MAX_MESSAGE + 1u));
}

static ano_crypto_status_t stream_cmac(
    ano_crypto_csp_handle_t handle,
    const uint8_t *message,
    size_t message_len,
    size_t initial_chunk,
    uint8_t out[ANO_CRYPTO_CMAC_TAG_SIZE])
{
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    size_t offset = 0u;
    size_t chunk = initial_chunk;
    size_t written = 0u;
    ano_crypto_status_t st;

    st = ano_crypto_cmac_context_size(&ctx_size);
    if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
        return st == ANO_CRYPTO_OK ? ANO_CRYPTO_ERR_INTERNAL_ERROR : st;
    }
    ctx = (uint8_t *)calloc(1u, ctx_size);
    if (ctx == 0) {
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    st = ano_crypto_cmac_init(
        ctx, ctx_size, handle, ANO_CRYPTO_CMAC_FLAG_NONE);
    while (st == ANO_CRYPTO_OK && offset < message_len) {
        size_t take = chunk;
        if (take > message_len - offset) {
            take = message_len - offset;
        }
        st = ano_crypto_cmac_update(ctx, message + offset, take);
        offset += take;
        chunk = (chunk % 47u) + 1u;
    }
    if (message_len == 0u && st == ANO_CRYPTO_OK) {
        st = ano_crypto_cmac_update(ctx, 0, 0u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_cmac_final(
            ctx, out, ANO_CRYPTO_CMAC_TAG_SIZE, &written);
        if (st == ANO_CRYPTO_OK && written != ANO_CRYPTO_CMAC_TAG_SIZE) {
            st = ANO_CRYPTO_ERR_INTERNAL_ERROR;
        }
    }
    (void)ano_crypto_cmac_uninit(ctx, ctx_size);
    free(ctx);
    return st;
}

int main(void)
{
    static const size_t key_lengths[3] = {16u, 24u, 32u};
    uint8_t key_storage[33];
    uint8_t message_storage[CMAC_DIFFERENTIAL_MAX_MESSAGE + 1u];
    uint8_t ano_tag_storage[ANO_CRYPTO_CMAC_TAG_SIZE + 1u];
    uint8_t stream_tag_storage[ANO_CRYPTO_CMAC_TAG_SIZE + 1u];
    uint8_t reference_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t *key = key_storage + 1u;
    uint8_t *message = message_storage + 1u;
    uint8_t *ano_tag = ano_tag_storage + 1u;
    uint8_t *stream_tag = stream_tag_storage + 1u;
    size_t comparisons = 0u;
    size_t key_index;
    size_t case_index;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "CMAC differential module initialization failed: %s\n",
                ano_crypto_status_name(st));
        return 1;
    }

    for (key_index = 0u; key_index < 3u; ++key_index) {
        size_t key_len = key_lengths[key_index];
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

        fill_case(key, key_len, (uint32_t)(0x4100u + key_index));
        st = ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_CMAC, key, key_len,
            ANO_CRYPTO_KEY_FLAG_NONE, &handle);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "CMAC differential key import failed for key class %lu: %s\n",
                    (unsigned long)key_len, ano_crypto_status_name(st));
            return 1;
        }

        for (case_index = 0u;
             case_index < CMAC_DIFFERENTIAL_CASES_PER_KEY;
             ++case_index) {
            size_t message_len = message_length_for_case(case_index);
            const uint8_t *message_ptr = message_len == 0u ? 0 : message;
            size_t written = 0u;

            fill_case(message, message_len,
                      (uint32_t)(0x730000u + key_index * 4099u + case_index));
            if (!openssl_cmac(
                    key, key_len, message_ptr, message_len, reference_tag)) {
                fprintf(stderr, "OpenSSL CMAC reference failed for key class %lu case %lu length %lu\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len);
                return 1;
            }

            st = ano_crypto_cmac_compute_with_key(
                handle, message_ptr, message_len,
                ano_tag, ANO_CRYPTO_CMAC_TAG_SIZE, &written,
                ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_OK || written != ANO_CRYPTO_CMAC_TAG_SIZE ||
                !equal_bytes(ano_tag, reference_tag, ANO_CRYPTO_CMAC_TAG_SIZE)) {
                fprintf(stderr, "CMAC differential one-shot mismatch for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            st = stream_cmac(
                handle, message_ptr, message_len,
                (case_index % 17u) + 1u, stream_tag);
            if (st != ANO_CRYPTO_OK ||
                !equal_bytes(stream_tag, reference_tag, ANO_CRYPTO_CMAC_TAG_SIZE)) {
                fprintf(stderr, "CMAC differential streaming-A mismatch for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            st = stream_cmac(
                handle, message_ptr, message_len,
                (case_index % 43u) + 1u, stream_tag);
            if (st != ANO_CRYPTO_OK ||
                !equal_bytes(stream_tag, reference_tag, ANO_CRYPTO_CMAC_TAG_SIZE)) {
                fprintf(stderr, "CMAC differential streaming-B mismatch for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            st = ano_crypto_cmac_verify_with_key(
                handle, message_ptr, message_len,
                reference_tag, sizeof(reference_tag),
                ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "CMAC differential verify failure for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            memcpy(altered, reference_tag, sizeof(altered));
            altered[case_index % sizeof(altered)] ^= 0x01u;
            st = ano_crypto_cmac_verify_with_key(
                handle, message_ptr, message_len,
                altered, sizeof(altered), ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED) {
                fprintf(stderr, "CMAC differential mismatch-A status failure for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            memcpy(altered, reference_tag, sizeof(altered));
            altered[(case_index * 7u + 15u) % sizeof(altered)] ^= 0x80u;
            st = ano_crypto_cmac_verify_with_key(
                handle, message_ptr, message_len,
                altered, sizeof(altered), ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED) {
                fprintf(stderr, "CMAC differential mismatch-B status failure for key class %lu case %lu length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)case_index,
                        (unsigned long)message_len, ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;
        }

        if (ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
            return 1;
        }
    }

    (void)ano_crypto_csp_zeroize(key_storage, sizeof(key_storage));
    (void)ano_crypto_csp_zeroize(message_storage, sizeof(message_storage));
    (void)ano_crypto_csp_zeroize(ano_tag_storage, sizeof(ano_tag_storage));
    (void)ano_crypto_csp_zeroize(stream_tag_storage, sizeof(stream_tag_storage));
    (void)ano_crypto_csp_zeroize(reference_tag, sizeof(reference_tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));

    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) {
        return 1;
    }

    if (comparisons !=
        (size_t)(3u * CMAC_DIFFERENTIAL_CASES_PER_KEY *
                 CMAC_DIFFERENTIAL_PATHS_PER_CASE)) {
        fprintf(stderr, "CMAC differential comparison count mismatch\n");
        return 1;
    }

    printf("OpenSSL 3.x AES-CMAC differential validation passed: %lu comparisons (tags not printed)\n",
           (unsigned long)comparisons);
    return 0;
}
