#ifndef ANO_CRYPTO_ECDSA_INTERNAL_H
#define ANO_CRYPTO_ECDSA_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_EC_P256_LIMBS 8u
#define ANO_CRYPTO_EC_P256_KEY_KIND_PUBLIC 1u
#define ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE 2u

typedef struct ano_crypto_ec_p256_key_internal_s {
    uint32_t kind;
    uint32_t x[ANO_CRYPTO_EC_P256_LIMBS];
    uint32_t y[ANO_CRYPTO_EC_P256_LIMBS];
    uint32_t d[ANO_CRYPTO_EC_P256_LIMBS];
} ano_crypto_ec_p256_key_internal_t;

ano_crypto_status_t ano_crypto_internal_ec_p256_validate_and_prepare(
    const uint8_t *public_x,
    size_t public_x_len,
    const uint8_t *public_y,
    size_t public_y_len,
    const uint8_t *private_scalar,
    size_t private_scalar_len,
    ano_crypto_ec_p256_key_internal_t *out_key,
    ano_crypto_key_type_t *out_type);

ano_crypto_status_t ano_crypto_internal_csp_import_ec_p256(
    const ano_crypto_ec_p256_key_internal_t *key,
    ano_crypto_key_type_t key_type,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

/* Leaves the global module/CSP lock held on success. */
ano_crypto_status_t ano_crypto_internal_csp_lock_ec_p256(
    ano_crypto_csp_handle_t handle,
    int require_private,
    const ano_crypto_ec_p256_key_internal_t **out_key);
void ano_crypto_internal_csp_unlock_ec_p256(void);

ano_crypto_status_t ano_crypto_internal_ecdsa_p256_sign_digest(
    const ano_crypto_ec_p256_key_internal_t *key,
    const uint8_t digest[32],
    uint8_t signature[64]);

ano_crypto_status_t ano_crypto_internal_ecdsa_p256_verify_digest(
    const ano_crypto_ec_p256_key_internal_t *key,
    const uint8_t digest[32],
    const uint8_t signature[64]);

#endif /* ANO_CRYPTO_ECDSA_INTERNAL_H */
