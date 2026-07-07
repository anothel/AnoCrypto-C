#ifndef ANO_CRYPTO_INTERNAL_H
#define ANO_CRYPTO_INTERNAL_H

#include <stddef.h>
#include <anocrypto/ac_status.h>

/* Internal-only state transition helper. Not installed. */
void ano_crypto_internal_module_mark_error(ano_crypto_status_t status);

/* Internal and public zeroization share one implementation. */
void ano_crypto_internal_zeroize(void *buffer, size_t buffer_len);

#endif /* ANO_CRYPTO_INTERNAL_H */
