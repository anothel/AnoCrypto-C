#ifndef ANO_CRYPTO_RSA_INTERNAL_H
#define ANO_CRYPTO_RSA_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_RSA_LIMBS 64u
#define ANO_CRYPTO_RSA_HALF_LIMBS 32u
#define ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS 33u
#define ANO_CRYPTO_RSA_PUBLIC_EXPONENT 65537u
#define ANO_CRYPTO_RSA_KEY_KIND_PUBLIC 1u
#define ANO_CRYPTO_RSA_KEY_KIND_PRIVATE 2u

typedef struct ano_crypto_rsa_components_internal_s {
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
} ano_crypto_rsa_components_internal_t;

typedef struct ano_crypto_rsa_key_internal_s {
    uint32_t kind;
    uint32_t modulus_bits;
    uint32_t n[ANO_CRYPTO_RSA_LIMBS];
    uint32_t n_rr[ANO_CRYPTO_RSA_LIMBS];
    uint32_t n0_inv;
    uint32_t p[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t p_rr[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t p0_inv;
    uint32_t q[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t q_rr[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t q0_inv;
    uint32_t dp[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t dq[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t qinv[ANO_CRYPTO_RSA_HALF_LIMBS];
} ano_crypto_rsa_key_internal_t;

ano_crypto_status_t ano_crypto_internal_rsa_validate_and_prepare(
    const ano_crypto_rsa_components_internal_t *components,
    ano_crypto_rsa_key_internal_t *out_key,
    ano_crypto_key_type_t *out_type);

ano_crypto_status_t ano_crypto_internal_csp_import_rsa(
    const ano_crypto_rsa_key_internal_t *key,
    ano_crypto_key_type_t key_type,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

/* Leaves the global module/CSP lock held on success. */
ano_crypto_status_t ano_crypto_internal_csp_lock_rsa(
    ano_crypto_csp_handle_t handle,
    int require_private,
    const ano_crypto_rsa_key_internal_t **out_key);
void ano_crypto_internal_csp_unlock_rsa(void);

ano_crypto_status_t ano_crypto_internal_rsa_public(
    const ano_crypto_rsa_key_internal_t *key,
    const uint8_t input[256],
    uint8_t output[256]);

ano_crypto_status_t ano_crypto_internal_rsa_private(
    const ano_crypto_rsa_key_internal_t *key,
    const uint8_t input[256],
    const uint8_t blind[8],
    uint8_t output[256]);

#endif /* ANO_CRYPTO_RSA_INTERNAL_H */
