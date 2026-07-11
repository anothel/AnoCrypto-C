#ifndef ANO_CRYPTO_PROCESS_GUARD_INTERNAL_H
#define ANO_CRYPTO_PROCESS_GUARD_INTERNAL_H

#include <stdint.h>
#include <anocrypto/ac_status.h>

/*
 * Internal process-boundary guard for source-backed DRBG state.
 *
 * The marker is an operational process identity, not entropy and not a secret.
 * It is used only to force reseeding when inherited DRBG state is observed in a
 * different process (for example after POSIX fork). P1-20 fails closed for
 * source-backed random contexts when a process marker cannot be obtained.
 */
typedef struct ano_crypto_internal_process_guard_s {
    uint64_t process_marker;
    uint32_t active;
    uint32_t reserved;
} ano_crypto_internal_process_guard_t;

uint64_t ano_crypto_internal_process_marker_current(void);

int ano_crypto_internal_process_guard_is_supported(void);

/* Pure helper for deterministic unsupported-marker policy tests. */
ano_crypto_status_t ano_crypto_internal_process_guard_init_marker(
    ano_crypto_internal_process_guard_t *guard,
    uint64_t marker);

ano_crypto_status_t ano_crypto_internal_process_guard_init(
    ano_crypto_internal_process_guard_t *guard);

ano_crypto_status_t ano_crypto_internal_process_guard_refresh(
    ano_crypto_internal_process_guard_t *guard);

ano_crypto_status_t ano_crypto_internal_process_guard_check(
    const ano_crypto_internal_process_guard_t *guard);

/* Pure helper used by deterministic internal unit tests. */
ano_crypto_status_t ano_crypto_internal_process_guard_check_marker(
    const ano_crypto_internal_process_guard_t *guard,
    uint64_t current_marker);

void ano_crypto_internal_process_guard_reset(
    ano_crypto_internal_process_guard_t *guard);

#endif /* ANO_CRYPTO_PROCESS_GUARD_INTERNAL_H */
