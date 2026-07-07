#include <stddef.h>
#include <anocrypto/ac_csp.h>
#include "ac_internal.h"

void ano_crypto_internal_zeroize(void *buffer, size_t buffer_len)
{
    volatile unsigned char *p;
    size_t i;

    if (buffer == 0 || buffer_len == 0u) {
        return;
    }

    p = (volatile unsigned char *)buffer;
    for (i = 0u; i < buffer_len; ++i) {
        p[i] = 0u;
    }
}

ano_crypto_status_t ano_crypto_csp_zeroize(
    void *buffer,
    size_t buffer_len)
{
    if (buffer == 0 && buffer_len != 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(buffer, buffer_len);
    return ANO_CRYPTO_OK;
}
