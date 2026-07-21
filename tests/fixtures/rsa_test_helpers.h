#ifndef ANO_CRYPTO_RSA_TEST_HELPERS_H
#define ANO_CRYPTO_RSA_TEST_HELPERS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "rsa2048_pkcs1_v15_kat.h"

typedef struct rsa_test_components_s {
    const uint8_t *modulus;
    size_t modulus_len;
    const uint8_t *public_exponent;
    size_t public_exponent_len;
    const uint8_t *private_exponent;
    size_t private_exponent_len;
    const uint8_t *prime1;
    size_t prime1_len;
    const uint8_t *prime2;
    size_t prime2_len;
    const uint8_t *exponent1;
    size_t exponent1_len;
    const uint8_t *exponent2;
    size_t exponent2_len;
    const uint8_t *coefficient;
    size_t coefficient_len;
} rsa_test_components_t;

static void rsa_test_public_components(rsa_test_components_t *c)
{
    memset(c, 0, sizeof(*c));
    c->modulus = RSA_KAT_N;
    c->modulus_len = sizeof(RSA_KAT_N);
    c->public_exponent = RSA_KAT_E;
    c->public_exponent_len = sizeof(RSA_KAT_E);
}

static void rsa_test_private_components(rsa_test_components_t *c)
{
    rsa_test_public_components(c);
    c->private_exponent = RSA_KAT_D;
    c->private_exponent_len = sizeof(RSA_KAT_D);
    c->prime1 = RSA_KAT_P;
    c->prime1_len = sizeof(RSA_KAT_P);
    c->prime2 = RSA_KAT_Q;
    c->prime2_len = sizeof(RSA_KAT_Q);
    c->exponent1 = RSA_KAT_DP;
    c->exponent1_len = sizeof(RSA_KAT_DP);
    c->exponent2 = RSA_KAT_DQ;
    c->exponent2_len = sizeof(RSA_KAT_DQ);
    c->coefficient = RSA_KAT_QINV;
    c->coefficient_len = sizeof(RSA_KAT_QINV);
}

static ano_crypto_status_t rsa_test_import(
    const rsa_test_components_t *c,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    if (c == 0) {
        return ano_crypto_rsa_key_import(
            0, 0u, 0, 0u, 0, 0u, 0, 0u, 0, 0u,
            0, 0u, 0, 0u, 0, 0u, flags, out_handle);
    }
    return ano_crypto_rsa_key_import(
        c->modulus, c->modulus_len,
        c->public_exponent, c->public_exponent_len,
        c->private_exponent, c->private_exponent_len,
        c->prime1, c->prime1_len,
        c->prime2, c->prime2_len,
        c->exponent1, c->exponent1_len,
        c->exponent2, c->exponent2_len,
        c->coefficient, c->coefficient_len,
        flags, out_handle);
}

#endif
