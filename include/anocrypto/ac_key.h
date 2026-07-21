#ifndef ANO_CRYPTO_AC_KEY_H
#define ANO_CRYPTO_AC_KEY_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Key/CSP lifecycle surface.
 *
 * The module retains purpose-separated in-memory secret-key handles and the
 * P3-08 RSA-2048 and P3-09 P-256 public/private component-import paths. It does not export,
 * print, persist, wrap, or parse encoded key containers.
 */
typedef enum ano_crypto_key_type_e {
    ANO_CRYPTO_KEY_TYPE_SECRET_BYTES = 1,
    ANO_CRYPTO_KEY_TYPE_HMAC = 2,
    ANO_CRYPTO_KEY_TYPE_DRBG_SEED_MATERIAL = 3,
    ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER = 4,
    ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY = 5,
    ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY = 6,
    ANO_CRYPTO_KEY_TYPE_CMAC = 7
} ano_crypto_key_type_t;

#define ANO_CRYPTO_KEY_FLAG_NONE 0x00000000u
#define ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE 0x00000001u
#define ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE 0x00000002u

#define ANO_CRYPTO_KEY_MAX_SECRET_BYTES 128u

#define ANO_CRYPTO_RSA2048_MODULUS_SIZE 256u
#define ANO_CRYPTO_RSA2048_PRIME_SIZE 128u
#define ANO_CRYPTO_EC_P256_COMPONENT_SIZE 32u



/*
 * Import one RSA-2048 public or CRT private key from unsigned big-endian
 * algorithm-native components. A public key supplies only modulus/exponent.
 * A private key supplies every component. Public exponent 65537 is required.
 * Encoded key containers remain the responsibility of the caller/downstream.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_rsa_key_import(
    const uint8_t *modulus,
    size_t modulus_len,
    const uint8_t *public_exponent,
    size_t public_exponent_len,
    const uint8_t *private_exponent,
    size_t private_exponent_len,
    const uint8_t *prime1,
    size_t prime1_len,
    const uint8_t *prime2,
    size_t prime2_len,
    const uint8_t *exponent1,
    size_t exponent1_len,
    const uint8_t *exponent2,
    size_t exponent2_len,
    const uint8_t *coefficient,
    size_t coefficient_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

/*
 * Import one P-256 public or private key from fixed-width unsigned big-endian
 * affine coordinates and an optional private scalar. Public keys provide x/y
 * and no private scalar. Private keys provide x/y/d; import verifies d*G == Q.
 * SEC1, SPKI, PKCS#8, PEM, DER, and ASN.1 remain downstream-owned.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_ec_p256_key_import(
    const uint8_t *public_x,
    size_t public_x_len,
    const uint8_t *public_y,
    size_t public_y_len,
    const uint8_t *private_scalar,
    size_t private_scalar_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_import_secret(
    ano_crypto_key_type_t key_type,
    const uint8_t *key_material,
    size_t key_material_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_destroy(
    ano_crypto_csp_handle_t handle);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state);

ANO_CRYPTO_API const char *ano_crypto_key_type_name(
    ano_crypto_key_type_t key_type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_KEY_H */
