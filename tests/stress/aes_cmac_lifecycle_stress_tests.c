#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

#define CMAC_LIFECYCLE_ITERATIONS 128u
#define CMAC_MAX_MESSAGE_SIZE 1024u

static int bytes_equal(const uint8_t *left, const uint8_t *right, size_t length)
{
    uint8_t difference = 0u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(left[i] ^ right[i]));
    }
    return difference == 0u;
}

static int bytes_zero(const uint8_t *buffer, size_t length)
{
    uint8_t aggregate = 0u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        aggregate = (uint8_t)(aggregate | buffer[i]);
    }
    return aggregate == 0u;
}

static void fill_bytes(uint8_t *buffer, size_t length, uint32_t seed)
{
    uint32_t value = seed ^ 0xbb67ae85u;
    size_t i;
    for (i = 0u; i < length; ++i) {
        value ^= value << 13u;
        value ^= value >> 17u;
        value ^= value << 5u;
        buffer[i] = (uint8_t)(value >> 24u);
    }
}

static ano_crypto_status_t stream_cmac(
    void *ctx,
    const uint8_t *message,
    size_t message_len,
    uint32_t seed,
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE])
{
    size_t offset = 0u;
    size_t written = 0u;
    size_t chunk = (size_t)((seed % 29u) + 1u);
    ano_crypto_status_t st = ANO_CRYPTO_OK;

    if (message_len == 0u) {
        st = ano_crypto_cmac_update(ctx, 0, 0u);
    }
    while (st == ANO_CRYPTO_OK && offset < message_len) {
        size_t take = chunk;
        if (take > message_len - offset) {
            take = message_len - offset;
        }
        st = ano_crypto_cmac_update(ctx, message + offset, take);
        offset += take;
        chunk = (chunk * 5u + 3u) % 47u;
        if (chunk == 0u) {
            chunk = 1u;
        }
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_cmac_final(
            ctx, tag, ANO_CRYPTO_CMAC_TAG_SIZE, &written);
    }
    if (st == ANO_CRYPTO_OK && written != ANO_CRYPTO_CMAC_TAG_SIZE) {
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }
    return st;
}

int main(void)
{
    static const size_t key_lengths[3] = {16u, 24u, 32u};
    uint8_t key[32];
    uint8_t message[CMAC_MAX_MESSAGE_SIZE];
    uint8_t one_shot_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t streaming_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    unsigned int iteration;
    int stage = 0;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK ||
        ano_crypto_cmac_context_size(&ctx_size) != ANO_CRYPTO_OK ||
        ctx_size == 0u) {
        stage = 1;
        goto cleanup;
    }
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        stage = 2;
        goto cleanup;
    }

    for (iteration = 0u; iteration < CMAC_LIFECYCLE_ITERATIONS; ++iteration) {
        size_t key_len = key_lengths[iteration % 3u];
        size_t message_len;
        const uint8_t *message_ptr;
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        size_t written = 0u;
        ano_crypto_status_t st;

        switch (iteration % 12u) {
        case 0u: message_len = 0u; break;
        case 1u: message_len = 1u; break;
        case 2u: message_len = 15u; break;
        case 3u: message_len = 16u; break;
        case 4u: message_len = 17u; break;
        case 5u: message_len = 31u; break;
        case 6u: message_len = 32u; break;
        case 7u: message_len = 33u; break;
        case 8u: message_len = 63u; break;
        case 9u: message_len = 64u; break;
        case 10u: message_len = 255u; break;
        default:
            message_len = (size_t)(((iteration * 73u) %
                (CMAC_MAX_MESSAGE_SIZE - 1u)) + 1u);
            break;
        }

        fill_bytes(key, key_len, 0x29100000u ^ iteration);
        fill_bytes(message, message_len, 0x29200000u ^ iteration);
        message_ptr = message_len == 0u ? 0 : message;
        memset(one_shot_tag, 0, sizeof(one_shot_tag));
        memset(streaming_tag, 0, sizeof(streaming_tag));
        memset(altered_tag, 0, sizeof(altered_tag));
        memset(ctx, 0, ctx_size);

        st = ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_CMAC,
            key,
            key_len,
            ANO_CRYPTO_KEY_FLAG_NONE,
            &handle);
        if (st != ANO_CRYPTO_OK) {
            stage = 3;
            goto cleanup;
        }

        st = ano_crypto_cmac_compute_with_key(
            handle,
            message_ptr,
            message_len,
            one_shot_tag,
            sizeof(one_shot_tag),
            &written,
            ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != sizeof(one_shot_tag)) {
            (void)ano_crypto_key_destroy(handle);
            stage = 4;
            goto cleanup;
        }

        st = ano_crypto_cmac_init(
            ctx, ctx_size, handle, ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            (void)ano_crypto_key_destroy(handle);
            stage = 5;
            goto cleanup;
        }

        /* Half the iterations destroy the source handle after init. The copied
         * schedule remains owned by the caller context until final/uninit. */
        if ((iteration & 1u) != 0u) {
            if (ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
                stage = 6;
                goto cleanup;
            }
            handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        }

        st = stream_cmac(
            ctx, message_ptr, message_len,
            0x29300000u ^ iteration, streaming_tag);
        if (st != ANO_CRYPTO_OK ||
            !bytes_equal(one_shot_tag, streaming_tag, sizeof(one_shot_tag)) ||
            !bytes_zero(ctx, ctx_size)) {
            if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) {
                (void)ano_crypto_key_destroy(handle);
            }
            stage = 7;
            goto cleanup;
        }

        if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            if (ano_crypto_key_import_secret(
                    ANO_CRYPTO_KEY_TYPE_CMAC,
                    key,
                    key_len,
                    ANO_CRYPTO_KEY_FLAG_NONE,
                    &handle) != ANO_CRYPTO_OK) {
                stage = 8;
                goto cleanup;
            }
        }

        st = ano_crypto_cmac_verify_with_key(
            handle,
            message_ptr,
            message_len,
            one_shot_tag,
            sizeof(one_shot_tag),
            ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            (void)ano_crypto_key_destroy(handle);
            stage = 9;
            goto cleanup;
        }

        memcpy(altered_tag, one_shot_tag, sizeof(altered_tag));
        altered_tag[iteration % ANO_CRYPTO_CMAC_TAG_SIZE] ^= 1u;
        st = ano_crypto_cmac_verify_with_key(
            handle,
            message_ptr,
            message_len,
            altered_tag,
            sizeof(altered_tag),
            ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED ||
            ano_crypto_key_destroy(handle) != ANO_CRYPTO_OK) {
            stage = 10;
            goto cleanup;
        }

        if (ano_crypto_cmac_uninit(ctx, ctx_size) != ANO_CRYPTO_OK ||
            !bytes_zero(ctx, ctx_size)) {
            stage = 11;
            goto cleanup;
        }

        (void)ano_crypto_csp_zeroize(key, sizeof(key));
        (void)ano_crypto_csp_zeroize(message, sizeof(message));
        (void)ano_crypto_csp_zeroize(one_shot_tag, sizeof(one_shot_tag));
        (void)ano_crypto_csp_zeroize(streaming_tag, sizeof(streaming_tag));
        (void)ano_crypto_csp_zeroize(altered_tag, sizeof(altered_tag));
    }

cleanup:
    if (ctx != 0) {
        (void)ano_crypto_cmac_uninit(ctx, ctx_size);
        free(ctx);
    }
    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(message, sizeof(message));
    (void)ano_crypto_csp_zeroize(one_shot_tag, sizeof(one_shot_tag));
    (void)ano_crypto_csp_zeroize(streaming_tag, sizeof(streaming_tag));
    (void)ano_crypto_csp_zeroize(altered_tag, sizeof(altered_tag));
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK && stage == 0) {
        stage = 12;
    }

    if (stage != 0) {
        fprintf(stderr, "AES-CMAC lifecycle stress failed: stage=%d\n", stage);
        return stage;
    }
    printf("AES-CMAC lifecycle stress passed: iterations=%u max_message_bytes=%u values_not_printed=1\n",
           CMAC_LIFECYCLE_ITERATIONS,
           CMAC_MAX_MESSAGE_SIZE);
    return 0;
}
