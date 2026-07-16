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

/*
 * Runs the current internal self-test sequence.
 *
 * The P2-03 implementation retains build/KAT-manifest structural checks,
 * SHA-2/HMAC KATs, AES-128/192/256 and CBC-path KATs, a full-tag AES-CMAC
 * KAT, and an HMAC_DRBG-SHA-256 KAT.
 *
 * ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER is an ABI-retained legacy
 * flag name. The P2-03 runtime does not yet authenticate the installed module
 * bytes; callers must not interpret the flag as completed integrity evidence.
 * A failing implemented mandatory self-test enters ERROR_STATE.
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
