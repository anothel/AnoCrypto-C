#ifndef ANO_CRYPTO_THREAD_INTERNAL_H
#define ANO_CRYPTO_THREAD_INTERNAL_H

#include <anocrypto/ac_status.h>

/*
 * Internal process-wide lock seam.
 *
 * P1-14 policy:
 * - This lock protects anocrypto-c global module/self-test/CSP table state.
 * - Caller-provided operation contexts remain caller-owned and are not made
 *   concurrently mutable by this lock.
 * - The implementation is intentionally small and platform support must stay
 *   aligned with docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md.
 */
ano_crypto_status_t ano_crypto_internal_global_lock(void);
void ano_crypto_internal_global_unlock(void);

#endif /* ANO_CRYPTO_THREAD_INTERNAL_H */
