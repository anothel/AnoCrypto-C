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

#define ANO_CRYPTO_SCRYPT_N 32768u
#define ANO_CRYPTO_SCRYPT_R 8u
#define ANO_CRYPTO_SCRYPT_P 1u
#define ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE 32u
/* Fixed-profile memory ceiling used by differential/resource evidence. */
#define ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE 67108864u
/* Exact caller-owned storage consumed by the fixed AnoCrypto-C implementation. */
#define ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE 33556544u

/*
 * Fixed-resource scrypt key derivation for the approved N=32768, r=8, p=1
 * profile. This is a generic primitive contract; no product framing, password
 * normalization, prompting, storage, or product workflow is implemented.
 *
 * Input contract:
 * - password and salt are exact caller-provided byte strings.
 * - empty password and empty salt are valid generic primitive inputs; product
 *   layers may impose stricter policy.
 * - NULL is accepted only for a corresponding zero-length input.
 * - password and salt may alias or overlap because they are read-only.
 *
 * Resource and buffer contract:
 * - workspace_len must be at least ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE.
 * - workspace must refer to storage obtained from malloc/calloc (or equivalent
 *   untyped allocated storage) and be aligned for uint32_t access. A caller that
 *   uses declared storage instead must declare it with a type compatible with
 *   uint32_t access; merely aligning a uint8_t array is not sufficient.
 * - workspace must not overlap any input or the 32-byte output range. Bytes
 *   beyond ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE are not read or written, although
 *   the complete caller-supplied workspace range must remain disjoint.
 * - out must reference exactly 32 writable bytes and must not overlap inputs
 *   or workspace.
 * - every non-empty pointer range must not wrap uintptr_t.
 * - the complete reviewed workspace extent is zeroized after every derivation
 *   attempt that starts using it.
 * - after a structurally valid output range is accepted, every failure clears
 *   all 32 output bytes; no partial derived output is returned.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_scrypt_derive_key(
    const uint8_t *password,
    size_t password_len,
    const uint8_t *salt,
    size_t salt_len,
    void *workspace,
    size_t workspace_len,
    uint8_t out[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_KDF_H */
