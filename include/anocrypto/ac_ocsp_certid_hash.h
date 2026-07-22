#ifndef ANO_CRYPTO_AC_OCSP_CERTID_HASH_H
#define ANO_CRYPTO_AC_OCSP_CERTID_HASH_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE 20u

/*
 * Restricted SHA-1 interoperability service for OCSP CertID issuer-name and
 * issuer-key hashing only. This is intentionally not a generic hash algorithm,
 * streaming service, signature digest, HMAC, KDF, or password primitive.
 *
 * The input is the exact byte sequence selected by the AnoPKI OCSP codec:
 * - DER-encoded issuer Name for issuerNameHash; or
 * - subjectPublicKey BIT STRING contents, excluding the unused-bit-count octet,
 *   for issuerKeyHash.
 *
 * NULL input is permitted only when input_len is zero. Input/output overlap and
 * overlap with out_written are rejected. On every failure, out_written is zero
 * when addressable and up to the first 20 output bytes are cleared.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_ocsp_certid_sha1_digest(
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_OCSP_CERTID_HASH_H */
