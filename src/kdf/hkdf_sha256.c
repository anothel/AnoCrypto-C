#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_kdf.h>
#include <anocrypto/ac_module.h>
#include "ac_hmac_internal.h"
#include "ac_internal.h"

static int ano_crypto_hkdf_range_valid(const void *ptr, size_t len)
{
    uintptr_t start;

    if (len == 0u) {
        return 1;
    }
    if (ptr == 0) {
        return 0;
    }

    start = (uintptr_t)ptr;
    return len <= (size_t)(UINTPTR_MAX - start);
}

static int ano_crypto_hkdf_ranges_overlap(
    const void *left,
    size_t left_len,
    const void *right,
    size_t right_len)
{
    uintptr_t left_start;
    uintptr_t right_start;
    uintptr_t left_end;
    uintptr_t right_end;

    if (left_len == 0u || right_len == 0u) {
        return 0;
    }

    left_start = (uintptr_t)left;
    right_start = (uintptr_t)right;
    left_end = left_start + (uintptr_t)left_len;
    right_end = right_start + (uintptr_t)right_len;
    return left_start < right_end && right_start < left_end;
}

static void ano_crypto_hkdf_clear_output(uint8_t *out, size_t out_len)
{
    if (out_len != 0u) {
        ano_crypto_internal_zeroize(out, out_len);
    }
}

ano_crypto_status_t ano_crypto_hkdf_sha256(
    const uint8_t *ikm,
    size_t ikm_len,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *info,
    size_t info_len,
    uint8_t *out,
    size_t out_len)
{
    uint8_t zero_salt[ANO_CRYPTO_HKDF_SHA256_HASH_SIZE];
    uint8_t prk[ANO_CRYPTO_HKDF_SHA256_HASH_SIZE];
    uint8_t block[ANO_CRYPTO_HKDF_SHA256_HASH_SIZE];
    ano_crypto_internal_hmac_sha256_ctx_t hmac_ctx;
    const uint8_t *extract_salt = salt;
    size_t extract_salt_len = salt_len;
    size_t produced = 0u;
    size_t previous_len = 0u;
    uint8_t counter = 1u;
    ano_crypto_status_t st;

    if (!ano_crypto_hkdf_range_valid(out, out_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (out_len > ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE) {
        ano_crypto_hkdf_clear_output(out, out_len);
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (!ano_crypto_hkdf_range_valid(ikm, ikm_len) ||
        !ano_crypto_hkdf_range_valid(salt, salt_len) ||
        !ano_crypto_hkdf_range_valid(info, info_len)) {
        ano_crypto_hkdf_clear_output(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (ano_crypto_hkdf_ranges_overlap(out, out_len, ikm, ikm_len) ||
        ano_crypto_hkdf_ranges_overlap(out, out_len, salt, salt_len) ||
        ano_crypto_hkdf_ranges_overlap(out, out_len, info, info_len)) {
        ano_crypto_hkdf_clear_output(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_hkdf_clear_output(out, out_len);
    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (out_len == 0u) {
        return ANO_CRYPTO_OK;
    }

    ano_crypto_internal_zeroize(zero_salt, sizeof(zero_salt));
    ano_crypto_internal_zeroize(prk, sizeof(prk));
    ano_crypto_internal_zeroize(block, sizeof(block));
    ano_crypto_internal_zeroize(&hmac_ctx, sizeof(hmac_ctx));

    if (salt_len == 0u) {
        extract_salt = zero_salt;
        extract_salt_len = sizeof(zero_salt);
    }

    st = ano_crypto_internal_hmac_sha256_compute(
        extract_salt, extract_salt_len, ikm, ikm_len, prk);
    while (st == ANO_CRYPTO_OK && produced < out_len) {
        size_t take;

        st = ano_crypto_internal_hmac_sha256_init(&hmac_ctx, prk, sizeof(prk));
        if (st == ANO_CRYPTO_OK && previous_len != 0u) {
            st = ano_crypto_internal_hmac_sha256_update(
                &hmac_ctx, block, previous_len);
        }
        if (st == ANO_CRYPTO_OK && info_len != 0u) {
            st = ano_crypto_internal_hmac_sha256_update(
                &hmac_ctx, info, info_len);
        }
        if (st == ANO_CRYPTO_OK) {
            st = ano_crypto_internal_hmac_sha256_update(
                &hmac_ctx, &counter, sizeof(counter));
        }
        if (st == ANO_CRYPTO_OK) {
            st = ano_crypto_internal_hmac_sha256_final(&hmac_ctx, block);
        }
        if (st != ANO_CRYPTO_OK) {
            break;
        }

        take = out_len - produced;
        if (take > sizeof(block)) {
            take = sizeof(block);
        }
        (void)memcpy(out + produced, block, take);
        produced += take;
        previous_len = sizeof(block);
        ++counter;
    }

    ano_crypto_internal_zeroize(&hmac_ctx, sizeof(hmac_ctx));
    ano_crypto_internal_zeroize(block, sizeof(block));
    ano_crypto_internal_zeroize(prk, sizeof(prk));
    ano_crypto_internal_zeroize(zero_salt, sizeof(zero_salt));
    if (st != ANO_CRYPTO_OK || produced != out_len) {
        ano_crypto_hkdf_clear_output(out, out_len);
        return st != ANO_CRYPTO_OK ? st : ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }
    return ANO_CRYPTO_OK;
}
