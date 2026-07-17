#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <anocrypto/ac_drbg.h>
#include <anocrypto/ac_status.h>
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"
#include "ac_process_guard_internal.h"

static const char *source_class_name(uint32_t value)
{
    return value == ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG
        ? "external_conditioned_rbg" : "unexpected";
}

static const char *health_owner_name(uint32_t value)
{
    return value == ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER
        ? "external_provider" : "unexpected";
}

static const char *restart_policy_name(uint32_t value)
{
    return value == ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_PROVIDER_PER_CALL
        ? "provider_per_call" : "unexpected";
}

int main(void)
{
    ano_crypto_internal_entropy_source_t source;
    uint32_t source_class = 0u;
    uint32_t health_owner = 0u;
    uint32_t restart_policy = 0u;
    ano_crypto_status_t init_status;
    ano_crypto_status_t policy_status;
    ano_crypto_status_t probe_status;
    int result = 0;

    init_status = ano_crypto_internal_entropy_source_init_os(
        &source, ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE);
    policy_status = init_status == ANO_CRYPTO_OK
        ? ano_crypto_internal_entropy_source_get_policy(
              &source, &source_class, &health_owner, &restart_policy)
        : init_status;
    probe_status = policy_status == ANO_CRYPTO_OK
        ? ano_crypto_internal_entropy_source_probe_operational(&source)
        : policy_status;

    printf("policy_id=EDP-P2-04-R1\n");
    printf("candidate_id=AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1\n");
    printf("provider_profile=ENT-OE01-LINUX-GETRANDOM-R1\n");
    printf("provider=%s\n", ano_crypto_internal_platform_entropy_provider_name());
    printf("provider_available=%s\n",
           ano_crypto_internal_platform_entropy_is_available() ? "yes" : "no");
    printf("source_class=%s\n", source_class_name(source_class));
    printf("health_test_owner=%s\n", health_owner_name(health_owner));
    printf("restart_policy=%s\n", restart_policy_name(restart_policy));
    printf("direct_noise_source_in_scope=no\n");
    printf("instantiate_entropy_bytes=%u\n",
           (unsigned)ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE);
    printf("instantiate_nonce_bytes=%u\n",
           (unsigned)ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE);
    printf("reseed_entropy_bytes=%u\n",
           (unsigned)ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE);
    printf("max_request_bytes=%u\n",
           (unsigned)ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE);
    printf("reseed_interval_requests=%" PRIu64 "\n",
           (uint64_t)ANO_CRYPTO_INTERNAL_HMAC_DRBG_RESEED_INTERVAL);
    printf("prediction_resistance=unsupported\n");
    printf("process_marker_supported=%s\n",
           ano_crypto_internal_process_guard_is_supported() ? "yes" : "no");
    printf("vm_clone_detection=no\n");
    printf("suspend_resume_detection=no\n");
    printf("generated_bytes_logged=no\n");
    printf("source_init_status=%s\n", ano_crypto_status_name(init_status));
    printf("policy_query_status=%s\n", ano_crypto_status_name(policy_status));
    printf("provider_probe_status=%s\n", ano_crypto_status_name(probe_status));

    if (init_status != ANO_CRYPTO_OK || policy_status != ANO_CRYPTO_OK) {
        result = 1;
    }
    if (source_class != ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG ||
        health_owner != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER ||
        restart_policy != ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_PROVIDER_PER_CALL) {
        result = 1;
    }
#if defined(ANO_CRYPTO_EXPECT_RANDOM_AVAILABLE)
    if (probe_status != ANO_CRYPTO_OK) {
        result = 1;
    }
#endif
    return result;
}
