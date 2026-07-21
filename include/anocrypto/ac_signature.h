#ifndef ANO_CRYPTO_AC_SIGNATURE_H
#define ANO_CRYPTO_AC_SIGNATURE_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_RSA2048_SIGNATURE_SIZE 256u
#define ANO_CRYPTO_ECDSA_P256_SIGNATURE_SIZE 64u

typedef enum ano_crypto_signature_algorithm_e {
    ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256 = 1,
    ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384 = 2,
    ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512 = 3,
    ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256 = 4
} ano_crypto_signature_algorithm_t;

/*
 * Message-based RSA PKCS#1 v1.5 and ECDSA P-256 signature service.
 *
 * RSA-2048/e=65537 is the P3-08 profile; P-256/SHA-256 is the P3-09 profile.
 * The module owns SHA-2 hashing, DigestInfo encoding, EMSA-PKCS1-v1_5,
 * the RSA private/public primitive, private-operation blinding, and the
 * post-operation public fault check. PEM, DER, ASN.1, PKCS#8, SPKI, CSR,
 * certificate, CRL, and OCSP processing remain outside this API.
 *
 * ECDSA P-256 uses SHA-256 and emits a fixed 64-byte raw r || s signature;
 * ASN.1 DER conversion remains downstream-owned. The nonce is generated with
 * RFC 6979 HMAC-SHA-256.
 *
 * Size query: out_signature == NULL and out_signature_len == 0 succeeds and
 * writes the algorithm-specific size to out_written. On every other failure,
 * out_written is zero and
 * any caller-provided output range is cleared. Exact input/output overlap and
 * partial overlap are rejected.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_signature_sign(
    ano_crypto_csp_handle_t private_key,
    ano_crypto_signature_algorithm_t algorithm,
    const uint8_t *message,
    size_t message_len,
    uint8_t *out_signature,
    size_t out_signature_len,
    size_t *out_written);

/*
 * Verification accepts only the exact algorithm-specific signature length and
 * returns ANO_CRYPTO_ERR_AUTHENTICATION_FAILED for malformed or mathematically
 * invalid signatures. Public or private handles may be used for verification.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_signature_verify(
    ano_crypto_csp_handle_t key,
    ano_crypto_signature_algorithm_t algorithm,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *signature,
    size_t signature_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_SIGNATURE_H */
