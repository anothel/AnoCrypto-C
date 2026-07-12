#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <anocrypto/anocrypto.h>

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

int main(void)
{
    static const size_t key_lengths[3] = {16u, 24u, 32u};
    static const size_t message_lengths[] = {
        0u,1u,2u,7u,15u,16u,17u,31u,32u,33u,47u,48u,49u,63u,64u,65u,
        127u,128u,129u,255u,256u,257u,511u,512u
    };
    uint8_t key[32];
    uint8_t message[512];
    uint8_t ano_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t stream_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t reference_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
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
             case_index < sizeof(message_lengths) / sizeof(message_lengths[0]);
             ++case_index) {
            size_t message_len = message_lengths[case_index];
            const uint8_t *message_ptr = message_len == 0u ? 0 : message;
            size_t written = 0u;
            size_t ctx_size = 0u;
            uint8_t *ctx;
            size_t offset = 0u;
            size_t chunk = 1u;

            fill_case(message, message_len,
                      (uint32_t)(0x730000u + key_index * 257u + case_index));
            if (!openssl_cmac(key, key_len, message_ptr, message_len, reference_tag)) {
                fprintf(stderr, "OpenSSL CMAC reference failed for key class %lu message length %lu\n",
                        (unsigned long)key_len, (unsigned long)message_len);
                return 1;
            }

            st = ano_crypto_cmac_compute_with_key(
                handle, message_ptr, message_len,
                ano_tag, sizeof(ano_tag), &written, ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_OK || written != sizeof(ano_tag) ||
                !equal_bytes(ano_tag, reference_tag, sizeof(ano_tag))) {
                fprintf(stderr, "CMAC differential one-shot mismatch for key class %lu message length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)message_len,
                        ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            st = ano_crypto_cmac_context_size(&ctx_size);
            if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
                return 1;
            }
            ctx = (uint8_t *)calloc(1u, ctx_size);
            if (ctx == 0) {
                return 1;
            }
            st = ano_crypto_cmac_init(ctx, ctx_size, handle, ANO_CRYPTO_CMAC_FLAG_NONE);
            while (st == ANO_CRYPTO_OK && offset < message_len) {
                size_t take = chunk;
                if (take > message_len - offset) {
                    take = message_len - offset;
                }
                st = ano_crypto_cmac_update(ctx, message + offset, take);
                offset += take;
                chunk = (chunk % 19u) + 1u;
            }
            if (message_len == 0u && st == ANO_CRYPTO_OK) {
                st = ano_crypto_cmac_update(ctx, 0, 0u);
            }
            if (st == ANO_CRYPTO_OK) {
                written = 0u;
                st = ano_crypto_cmac_final(
                    ctx, stream_tag, sizeof(stream_tag), &written);
            }
            (void)ano_crypto_cmac_uninit(ctx, ctx_size);
            free(ctx);
            if (st != ANO_CRYPTO_OK || written != sizeof(stream_tag) ||
                !equal_bytes(stream_tag, reference_tag, sizeof(stream_tag))) {
                fprintf(stderr, "CMAC differential streaming mismatch for key class %lu message length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)message_len,
                        ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            st = ano_crypto_cmac_verify_with_key(
                handle, message_ptr, message_len,
                reference_tag, sizeof(reference_tag), ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "CMAC differential verify failure for key class %lu message length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)message_len,
                        ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;

            memcpy(altered, reference_tag, sizeof(altered));
            altered[case_index % sizeof(altered)] ^= 0x01u;
            st = ano_crypto_cmac_verify_with_key(
                handle, message_ptr, message_len,
                altered, sizeof(altered), ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED) {
                fprintf(stderr, "CMAC differential mismatch-status failure for key class %lu message length %lu status %s\n",
                        (unsigned long)key_len, (unsigned long)message_len,
                        ano_crypto_status_name(st));
                return 1;
            }
            ++comparisons;
        }

        if (ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
            return 1;
        }
    }

    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(message, sizeof(message));
    (void)ano_crypto_csp_zeroize(ano_tag, sizeof(ano_tag));
    (void)ano_crypto_csp_zeroize(stream_tag, sizeof(stream_tag));
    (void)ano_crypto_csp_zeroize(reference_tag, sizeof(reference_tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));

    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) {
        return 1;
    }

    printf("OpenSSL 3.x AES-CMAC differential validation passed: %lu comparisons (tags not printed)\n",
           (unsigned long)comparisons);
    return 0;
}
