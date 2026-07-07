#ifndef ANO_CRYPTO_ENTROPY_INTERNAL_H
#define ANO_CRYPTO_ENTROPY_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE 0u
#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK 1u
#define ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS_RESERVED 2u

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
} ano_crypto_internal_entropy_source_t;

ano_crypto_status_t ano_crypto_internal_entropy_source_init_none(
    ano_crypto_internal_entropy_source_t *source);

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len);

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os_reserved(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len);

ano_crypto_status_t ano_crypto_internal_entropy_source_get(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len);

#endif /* ANO_CRYPTO_ENTROPY_INTERNAL_H */
