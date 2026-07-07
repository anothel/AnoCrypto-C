#ifndef ANO_CRYPTO_INTERNAL_KAT_MANIFEST_H
#define ANO_CRYPTO_INTERNAL_KAT_MANIFEST_H

#include <stddef.h>

typedef struct ano_crypto_internal_kat_manifest_entry_s {
    const char *selftest_id;
    const char *algorithm_name;
    const char *source_policy;
    const char *status;
} ano_crypto_internal_kat_manifest_entry_t;

size_t ano_crypto_internal_kat_manifest_count(void);
const ano_crypto_internal_kat_manifest_entry_t *ano_crypto_internal_kat_manifest_get(
    size_t index);

#endif /* ANO_CRYPTO_INTERNAL_KAT_MANIFEST_H */
