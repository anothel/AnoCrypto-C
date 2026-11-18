#ifndef ANO_CRYPTO_INTERNAL_H
#define ANO_CRYPTO_INTERNAL_H

#include <stddef.h>
#include <anocrypto/ac_status.h>

/* Internal-only state transition helper. Not installed. */
void ano_crypto_internal_module_mark_error(ano_crypto_status_t status);

/* Caller must hold the global module/CSP lock (or serialize with locking off).
 * This predicate does not acquire the lock; use it for atomic CSP admission. */
ano_crypto_status_t ano_crypto_internal_module_require_operational_unlocked(void);

/* Internal and public zeroization share one implementation. */
void ano_crypto_internal_zeroize(void *buffer, size_t buffer_len);

#endif /* ANO_CRYPTO_INTERNAL_H */
