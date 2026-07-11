# SHA-384 / SHA-512 KAT and Evidence Record

Status: P1-22 engineering record.

## Public sources

| Source ID | Source | Use |
|---|---|---|
| SRC-1001 | NIST FIPS 180-4 | SHA-384/SHA-512 algorithm definition and public example digests. |
| SRC-1002 | RFC 4231 | HMAC-SHA-384/HMAC-SHA-512 public test vectors. |

The source identifiers are recorded in `cleanroom/SOURCE_MANIFEST.md`. No third-party implementation code was imported.

## Implemented coverage

| Service | Startup self-test | Extended CTest |
|---|---|---|
| SHA-384 | Empty, `abc`, streaming path | Standard long message, million-`a`, one-shot/streaming coverage, negative boundaries. |
| SHA-512 | Empty, `abc`, streaming path | Standard long message, million-`a`, one-shot/streaming coverage, negative boundaries. |
| HMAC-SHA-384 | RFC 4231 cases 1 and 2 | Cases 3, 4, 6, and 7; streaming path; CSP key-handle path. |
| HMAC-SHA-512 | RFC 4231 cases 1 and 2 | Cases 3, 4, 6, and 7; streaming path; CSP key-handle path. |

Relevant tests:

- `tests/kat/sha512_family_kat_tests.c`
- `tests/negative/sha512_family_api_negative_tests.c`
- `tests/kat/hmac_key_handle_kat_tests.c`
- startup KATs in `src/selftest/selftest.c`
- manifest mapping in `src/selftest/kat_manifest.c`

## Logging rules

- Record test name, source ID, status, compiler/build identity, and aggregate result only.
- Do not print digest values, MAC values, HMAC key bytes, entropy, seed material, DRBG state, private keys, tokens, or customer data.
- RFC 4231 keys are public synthetic test fixtures, not operational secrets.
- Local streaming comparisons are deterministic API consistency tests, not an independent public source.

## Evidence outputs

P1-22 evidence drafts include:

- `SHA512_FAMILY_CHECK.txt`
- `SHA512_FAMILY_REVIEW_NOTES.txt`
- `ctest.log`
- `PUBLIC_SURFACE_CHECK.txt`
- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`
- install and PoC smoke logs

## Remaining validation work

Independent external-provider differential automation is not yet part of the source tree. Provider/tool selection, licensing, supported versions, result normalization, and failure triage remain P1-23 TODOs.
