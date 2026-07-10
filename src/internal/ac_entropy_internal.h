#ifndef ANO_CRYPTO_ENTROPY_INTERNAL_H
#define ANO_CRYPTO_ENTROPY_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>
#include "ac_entropy_health_internal.h"

#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE 0u
#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK 1u
#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS 2u
/* Backward-compatible internal name retained for P1-4/P1-6 test wording. */
#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS_RESERVED ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS

/* Internal request ceiling. Oversized requests are rejected before output is touched. */
#define ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN 65536u

#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_OPERATION_ONLY 0u
#define ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_MODULE_FATAL 1u

typedef ano_crypto_status_t (*ano_crypto_internal_entropy_read_fn)(
    void *user_context,
    uint8_t *out,
    size_t out_len);

typedef struct ano_crypto_internal_entropy_source_s {
    uint32_t kind;
    uint32_t reserved;
    size_t min_request_len;
    ano_crypto_internal_entropy_read_fn read_fn;
    void *user_context;
    ano_crypto_internal_entropy_health_t *health;
    uint32_t health_failure_policy;
} ano_crypto_internal_entropy_source_t;

ano_crypto_status_t ano_crypto_internal_entropy_source_init_none(
    ano_crypto_internal_entropy_source_t *source);

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len);

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len);

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os_reserved(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len);

ano_crypto_status_t ano_crypto_internal_entropy_source_attach_health_test(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_health_t *health,
    uint32_t failure_policy);

ano_crypto_status_t ano_crypto_internal_entropy_source_get(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len);

int ano_crypto_internal_platform_entropy_is_available(void);

const char *ano_crypto_internal_platform_entropy_provider_name(void);

ano_crypto_status_t ano_crypto_internal_platform_entropy_get(
    uint8_t *out,
    size_t out_len);

#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
#define ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_OFF 0u
#define ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE 1u
#define ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_REPEAT 2u
#define ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL 3u
#define ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL 4u

ano_crypto_status_t ano_crypto_internal_entropy_test_hook_configure(
    uint32_t mode,
    uint8_t seed,
    size_t partial_len);

void ano_crypto_internal_entropy_test_hook_reset(void);
#endif

#endif /* ANO_CRYPTO_ENTROPY_INTERNAL_H */
