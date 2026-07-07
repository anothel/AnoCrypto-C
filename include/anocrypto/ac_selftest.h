#ifndef ANO_CRYPTO_AC_SELFTEST_H
#define ANO_CRYPTO_AC_SELFTEST_H

#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_SELFTEST_FLAG_DEFAULT 0x00000000u
#define ANO_CRYPTO_SELFTEST_FLAG_STARTUP 0x00000001u
#define ANO_CRYPTO_SELFTEST_FLAG_KAT_PLACEHOLDER 0x00000002u
#define ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER 0x00000004u

#if defined(ANO_CRYPTO_ENABLE_TEST_HOOKS)
#define ANO_CRYPTO_SELFTEST_FLAG_FORCE_FAILURE_FOR_TEST 0x80000000u
#endif

/*
 * Runs the P0 self-test framework skeleton.
 *
 * P0 does not implement cryptographic KATs yet. This function currently checks
 * framework wiring and build identity availability only.
 *
 * A self-test failure must move the module to ERROR_STATE.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_selftest_run(uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_selftest_get_last_status(
    ano_crypto_status_t *out_status);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_selftest_get_last_flags(
    uint32_t *out_flags);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_SELFTEST_H */
