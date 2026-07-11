# SHA-384 / SHA-512 KAT and Evidence Record

Status: P1-23 engineering record.

## Public sources

| Source ID | Source | Use |
|---|---|---|
| SRC-1001 | NIST FIPS 180-4 | SHA-384/SHA-512 definition and public example digests. |
| SRC-1002 | RFC 4231 | HMAC-SHA-384/HMAC-SHA-512 public test vectors. |

The identifiers are recorded in `cleanroom/SOURCE_MANIFEST.md`. No third-party
implementation code was imported.

## Implemented KAT coverage

| Service | Startup self-test | Extended CTest |
|---|---|---|
| SHA-384 | Empty, `abc`, streaming path | Standard long message, million-`a`, one-shot/streaming, negative boundaries. |
| SHA-512 | Empty, `abc`, streaming path | Standard long message, million-`a`, one-shot/streaming, negative boundaries. |
| HMAC-SHA-384 | RFC 4231 cases 1 and 2 | Cases 3, 4, 6, and 7; streaming; CSP key-handle path. |
| HMAC-SHA-512 | RFC 4231 cases 1 and 2 | Cases 3, 4, 6, and 7; streaming; CSP key-handle path. |

Relevant files:

- `tests/kat/sha512_family_kat_tests.c`
- `tests/negative/sha512_family_api_negative_tests.c`
- `tests/kat/hmac_key_handle_kat_tests.c`
- startup KATs in `src/selftest/selftest.c`
- provenance mapping in `src/selftest/kat_manifest.c`

## P1-23 differential separation

`tests/differential/sha2_hmac_openssl_differential_tests.c` compares the
implemented SHA-2/HMAC services with OpenSSL 3.x over a deterministic corpus.
This is a separate engineering lane and is not recorded as a KAT source. It
must not rewrite stored public vectors or replace startup self-tests.

The differential lane covers one-shot/streaming behavior, padding/block
boundaries, long keys, larger messages, and intentionally unaligned inputs.
Only provider/version, comparison count, and status are logged.

## Evidence outputs

P1-23 evidence drafts include:

- `SHA512_FAMILY_CHECK.txt`
- `SHA512_FAMILY_REVIEW_NOTES.txt`
- `DIFFERENTIAL_PORTABILITY_CHECK.txt`
- `DIFFERENTIAL_VALIDATION.log`
- `SHA512_PORTABILITY_REVIEW.log`
- `DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt`
- `ctest.log`, install/PoC smoke, and clean export-baseline records

## Logging rules

Record test name/source ID/provider version/status/compiler identity and
aggregate counts only. Do not print digest values, MAC values, HMAC key bytes,
message contents, entropy, seed material, DRBG state, generated random bytes,
private keys, tokens, or customer data.

## Remaining review

Actual hosted 32-bit, Windows, macOS, and big-endian portability artifacts are
not implied by the workflow definitions and require explicit review.
