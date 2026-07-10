#ifndef ANO_CRYPTO_ENTROPY_HEALTH_INTERNAL_H
#define ANO_CRYPTO_ENTROPY_HEALTH_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

/*
 * Internal engineering skeleton only.
 *
 * These duplicate-block candidates are disabled unless an internal entropy
 * source explicitly attaches a health-test state. They are not a substitute
 * for a reviewed noise-source-specific RCT/APT design, and they are not a
 * validation claim.
 */
#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED 0u
#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_STARTUP_DUPLICATE_BLOCK_CANDIDATE 1u
#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE 2u

#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN 64u

typedef struct ano_crypto_internal_entropy_health_s {
    uint32_t mode;
    uint32_t reserved;
    size_t expected_sample_len;
    size_t previous_sample_len;
    uint8_t previous_sample[ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN];
    int has_previous_sample;
} ano_crypto_internal_entropy_health_t;

ano_crypto_status_t ano_crypto_internal_entropy_health_init(
    ano_crypto_internal_entropy_health_t *health,
    uint32_t mode,
    size_t expected_sample_len);

ano_crypto_status_t ano_crypto_internal_entropy_health_evaluate(
    ano_crypto_internal_entropy_health_t *health,
    const uint8_t *sample,
    size_t sample_len);

void ano_crypto_internal_entropy_health_reset(
    ano_crypto_internal_entropy_health_t *health);

#endif /* ANO_CRYPTO_ENTROPY_HEALTH_INTERNAL_H */
