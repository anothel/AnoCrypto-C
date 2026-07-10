#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ac_entropy_health_internal.h"
#include "ac_internal.h"

static int ano_crypto_internal_bytes_equal(
    const uint8_t *left,
    const uint8_t *right,
    size_t len)
{
    size_t i;
    uint8_t difference = 0u;

    for (i = 0u; i < len; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(left[i] ^ right[i]));
    }

    return difference == 0u ? 1 : 0;
}

ano_crypto_status_t ano_crypto_internal_entropy_health_init(
    ano_crypto_internal_entropy_health_t *health,
    uint32_t mode,
    size_t expected_sample_len)
{
    if (health == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (mode != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_STARTUP_DUPLICATE_BLOCK_CANDIDATE &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (expected_sample_len > ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ano_crypto_internal_zeroize(health, sizeof(*health));
    health->mode = mode;
    health->expected_sample_len = expected_sample_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_health_evaluate(
    ano_crypto_internal_entropy_health_t *health,
    const uint8_t *sample,
    size_t sample_len)
{
    size_t half_len;

    if (health == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (health->mode == ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED) {
        return ANO_CRYPTO_OK;
    }

    if (sample == 0 || sample_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (sample_len > ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (health->expected_sample_len != 0u &&
        sample_len != health->expected_sample_len) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    switch (health->mode) {
    case ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED:
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_STARTUP_DUPLICATE_BLOCK_CANDIDATE:
        if ((sample_len & 1u) != 0u || sample_len < 2u) {
            return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        }

        half_len = sample_len / 2u;
        if (ano_crypto_internal_bytes_equal(sample, sample + half_len, half_len)) {
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE:
        if (health->has_previous_sample != 0 &&
            health->previous_sample_len == sample_len &&
            ano_crypto_internal_bytes_equal(health->previous_sample, sample, sample_len)) {
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }

        ano_crypto_internal_zeroize(
            health->previous_sample,
            sizeof(health->previous_sample));
        memcpy(health->previous_sample, sample, sample_len);
        health->previous_sample_len = sample_len;
        health->has_previous_sample = 1;
        return ANO_CRYPTO_OK;

    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
}

void ano_crypto_internal_entropy_health_reset(
    ano_crypto_internal_entropy_health_t *health)
{
    if (health != 0) {
        ano_crypto_internal_zeroize(health, sizeof(*health));
    }
}
