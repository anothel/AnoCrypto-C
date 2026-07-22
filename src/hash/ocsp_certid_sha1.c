#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_ocsp_certid_hash.h>
#include <anocrypto/ac_module.h>
#include "ac_internal.h"
#include "ac_sha1_internal.h"

static int range_valid(const void *pointer, size_t length)
{
    uintptr_t start;

    if (length == 0u) {
        return 1;
    }
    if (pointer == 0) {
        return 0;
    }
    start = (uintptr_t)pointer;
    return length <= (size_t)(UINTPTR_MAX - start);
}

static int ranges_overlap(
    const void *left,
    size_t left_length,
    const void *right,
    size_t right_length)
{
    uintptr_t left_start;
    uintptr_t right_start;
    uintptr_t left_end;
    uintptr_t right_end;

    if (left_length == 0u || right_length == 0u) {
        return 0;
    }
    left_start = (uintptr_t)left;
    right_start = (uintptr_t)right;
    left_end = left_start + (uintptr_t)left_length;
    right_end = right_start + (uintptr_t)right_length;
    return left_start < right_end && right_start < left_end;
}

static void clear_output(uint8_t *output, size_t output_length)
{
    size_t clear_length = output_length;

    if (clear_length > ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE) {
        clear_length = ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE;
    }
    if (output != 0 && clear_length != 0u && range_valid(output, clear_length)) {
        ano_crypto_internal_zeroize(output, clear_length);
    }
}

ano_crypto_status_t ano_crypto_ocsp_certid_sha1_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written)
{
    ano_crypto_status_t status;

    if (out_written != 0 && range_valid(out_written, sizeof(*out_written))) {
        *out_written = 0u;
    }

    if (!range_valid(input, input_len) ||
        !range_valid(out, out_len) ||
        !range_valid(out_written, out_written == 0 ? 0u : sizeof(*out_written))) {
        clear_output(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (out == 0 || out_written == 0) {
        clear_output(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (out_len < ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE) {
        clear_output(out, out_len);
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    if (ranges_overlap(input, input_len, out, ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE) ||
        ranges_overlap(input, input_len, out_written, sizeof(*out_written)) ||
        ranges_overlap(out, ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE,
                       out_written, sizeof(*out_written))) {
        clear_output(out, out_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    clear_output(out, out_len);
    status = ano_crypto_module_require_operational();
    if (status != ANO_CRYPTO_OK) {
        return status;
    }

    status = ano_crypto_internal_sha1_digest(input, input_len, out);
    if (status != ANO_CRYPTO_OK) {
        clear_output(out, out_len);
        return status;
    }

    *out_written = ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE;
    return ANO_CRYPTO_OK;
}
