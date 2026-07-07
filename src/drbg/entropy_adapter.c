#include <stddef.h>
#include <stdint.h>
#include "ac_entropy_internal.h"
#include "ac_internal.h"

ano_crypto_status_t ano_crypto_internal_entropy_source_init_none(
    ano_crypto_internal_entropy_source_t *source)
{
    if (source == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len)
{
    if (source == 0 || read_fn == 0 || min_request_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK;
    source->min_request_len = min_request_len;
    source->read_fn = read_fn;
    source->user_context = user_context;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os_reserved(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len)
{
    if (source == 0 || min_request_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS_RESERVED;
    source->min_request_len = min_request_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_get(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_status_t st;

    if (source == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (source->min_request_len != 0u && out_len < source->min_request_len) {
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    switch (source->kind) {
    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK:
        if (source->read_fn == 0) {
            ano_crypto_internal_zeroize(out, out_len);
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        st = source->read_fn(source->user_context, out, out_len);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(out, out_len);
            return st == ANO_CRYPTO_OK ? ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED : st;
        }
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS_RESERVED:
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE:
    default:
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }
}
