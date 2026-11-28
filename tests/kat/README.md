# AnoCrypto-C KAT Tests

Status: P1-23 engineering draft.

Public-source coverage includes:

- SHA-256, SHA-384, and SHA-512 empty, `abc`, standard long-message, and million-`a` vectors.
- HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 RFC 4231 cases 1, 2, 3, 4, 6, and 7.
- HMAC_DRBG-SHA-256 NIST CAVP no-prediction-resistance/reseed flow.
- HMAC key-handle computation for all three SHA-2 digest variants.
- Deterministic one-shot/streaming consistency and negative API boundaries.

These tests are implementation correctness checks, not official validation evidence. Public HMAC keys and DRBG KAT inputs are synthetic fixtures, not operational secrets. Test output must not print digest values, MAC values, key material, entropy, seed material, DRBG state, or generated random bytes.

Differential tests are separate from KAT provenance and live under `tests/differential/`. They compare values in memory and log only status/provider/count metadata.

## Find a test by source file

The table maps sources in this directory to their exact CTest names when
`ANO_CRYPTO_BUILD_TESTS=ON`. It includes boundary and streaming checks
alongside standalone known-answer tests. Use these names to navigate the
existing tests. The table covers this directory and makes no claim about
complete KAT coverage or official validation. The SHA-1 entry is restricted
to OCSP CertID.

| Source file | CTest name |
| --- | --- |
| `sha256_hmac_kat_tests.c` | `ano_crypto_sha256_hmac_kat_tests` |
| `sha256_hmac_extended_kat_tests.c` | `ano_crypto_sha256_hmac_extended_kat_tests` |
| `hmac_drbg_kat_tests.c` | `ano_crypto_hmac_drbg_kat_tests` |
| `hmac_drbg_boundary_tests.c` | `ano_crypto_hmac_drbg_boundary_tests` |
| `hmac_key_handle_kat_tests.c` | `ano_crypto_hmac_key_handle_kat_tests` |
| `restricted_sha1_ocsp_certid_kat_tests.c` | `ano_crypto_restricted_sha1_ocsp_certid_kat_tests` |
| `hkdf_sha256_kat_tests.c` | `ano_crypto_hkdf_sha256_kat_tests` |
| `rsa_pkcs1_v15_kat_tests.c` | `ano_crypto_rsa_pkcs1_v15_kat_tests` |
| `ecdsa_p256_sha256_kat_tests.c` | `ano_crypto_ecdsa_p256_sha256_kat_tests` |
| `scrypt_kat_tests.c` | `ano_crypto_scrypt_kat_tests` |
| `aes256_gcm_kat_tests.c` | `ano_crypto_aes256_gcm_kat_tests` |
| `aes256_gcm_streaming_tests.c` | `ano_crypto_aes256_gcm_streaming_tests` |
| `aes_cbc_kat_tests.c` | `ano_crypto_aes_cbc_kat_tests` |
| `aes_cmac_kat_tests.c` | `ano_crypto_aes_cmac_kat_tests` |
| `sha512_family_kat_tests.c` | `ano_crypto_sha512_family_kat_tests` |

After building with tests enabled, run the exact HKDF-SHA-256 test from the
CMake build directory:

```sh
ctest -R '^ano_crypto_hkdf_sha256_kat_tests$' --output-on-failure
```
