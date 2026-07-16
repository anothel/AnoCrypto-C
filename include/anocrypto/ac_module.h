#ifndef ANO_CRYPTO_AC_MODULE_H
#define ANO_CRYPTO_AC_MODULE_H

#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Opaque module config placeholder.
 * P0 accepts only NULL config. The layout is intentionally not public.
 */
typedef struct ano_crypto_module_config_s ano_crypto_module_config_t;

typedef enum ano_crypto_module_state_e {
    ANO_CRYPTO_MODULE_STATE_UNINITIALIZED = 0,
    ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING = 1,
    ANO_CRYPTO_MODULE_STATE_OPERATIONAL = 2,
    ANO_CRYPTO_MODULE_STATE_ERROR_STATE = 3,
    ANO_CRYPTO_MODULE_STATE_SHUTDOWN = 4
} ano_crypto_module_state_t;

/*
 * Module lifecycle.
 *
 * P2-03 policy:
 * - ano_crypto_module_init(NULL) runs the mandatory implemented startup checks.
 * - Cryptographic services require OPERATIONAL before processing.
 * - A mandatory self-test or fatal internal failure enters sticky ERROR_STATE.
 * - Orderly SHUTDOWN may be initialized again and reruns startup checks.
 * - ERROR_STATE has no in-process recovery; a fresh process load is required.
 * - The internal global-state lock protects module/self-test/CSP global state;
 *   caller-owned mutable operation contexts still require caller serialization.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_module_init(
    const ano_crypto_module_config_t *config);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_module_shutdown(void);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_module_get_state(
    ano_crypto_module_state_t *out_state);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_module_get_last_error(
    ano_crypto_status_t *out_status);

/*
 * Service gate for future cryptographic APIs.
 * Query APIs for version/build/status do not need this gate.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_module_require_operational(void);

ANO_CRYPTO_API const char *ano_crypto_module_state_name(
    ano_crypto_module_state_t state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_MODULE_H */
