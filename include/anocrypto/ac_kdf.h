#ifndef ANO_CRYPTO_AC_KDF_H
#define ANO_CRYPTO_AC_KDF_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_HKDF_SHA256_HASH_SIZE 32u
#define ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE 8160u

/*
 * RFC 5869 HKDF-SHA-256 extract-and-expand.
 *
 * Empty inputs:
 * - ikm_len == 0 accepts ikm == NULL and extracts from an empty IKM.
 * - salt_len == 0 accepts salt == NULL and uses 32 zero octets as RFC 5869 salt.
 * - info_len == 0 accepts info == NULL and expands with empty info.
 * - out_len == 0 accepts out == NULL and succeeds after normal argument and
 *   module-state validation without deriving bytes.
 *
 * Buffer and failure contract:
 * - out_len must be at most 255 * 32 == 8160 bytes.
 * - every non-empty pointer range must be non-NULL and must not wrap uintptr_t.
 * - ikm, salt, and info may alias or overlap each other because they are read-only.
 * - a non-empty output range must not overlap any non-empty input range.
 * - the caller provides a writable output range of exactly out_len bytes.
 * - after a structurally valid output range is accepted, every failure clears
 *   all out_len bytes; no partial derived output is returned.
 * - a NULL or wrapping output range cannot be cleared and is rejected directly.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hkdf_sha256(
    const uint8_t *ikm,
    size_t ikm_len,
    const uint8_t *salt,
    size_t salt_len,
    const uint8_t *info,
    size_t info_len,
    uint8_t *out,
    size_t out_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_KDF_H */
