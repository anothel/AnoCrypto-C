# SHA-384 / SHA-512 KAT Plan

Status: P1-15 draft.

## Purpose

This document records the KAT and evidence plan for SHA-384, SHA-512, HMAC-SHA-384, and HMAC-SHA-512 before implementation is added. It is a planning artifact only.

## Current P1-15 state

| Algorithm | Public API state | Processing state | KAT state | Evidence state |
|---|---|---|---|---|
| SHA-384 | Algorithm ID, name, digest size, context size reserved | Not implemented | Planned only | Scope check and reserved-API negative test |
| SHA-512 | Algorithm ID, name, digest size, context size reserved | Not implemented | Planned only | Scope check and reserved-API negative test |
| HMAC-SHA-384 | Inherits SHA-384 MAC size metadata | Not implemented | Planned only | Scope check and reserved-API negative test |
| HMAC-SHA-512 | Inherits SHA-512 MAC size metadata | Not implemented | Planned only | Scope check and reserved-API negative test |

## Candidate public vector sources

| Source ID | Candidate source | Intended use | Current status |
|---|---|---|---|
| SRC-1001 | NIST FIPS 180-4 Secure Hash Standard already recorded in clean-room manifest | SHA-384/SHA-512 algorithm specification and public example digests | Recorded, but SHA-384/SHA-512 processing not implemented |
| SRC-1002 | RFC 4231 already recorded in clean-room manifest | HMAC-SHA-384/HMAC-SHA-512 public test vectors | Recorded, but HMAC-SHA-384/HMAC-SHA-512 processing not implemented |
| TBD | Independently selected public/reference comparison source or tool | Differential validation after implementation | TODO |

## Planned vector classes

| Class | SHA-384 | SHA-512 | HMAC-SHA-384 | HMAC-SHA-512 |
|---|---:|---:|---:|---:|
| Empty message digest | Planned | Planned | Not applicable | Not applicable |
| `abc` digest | Planned | Planned | Not applicable | Not applicable |
| Multi-block message digest | Planned | Planned | Not applicable | Not applicable |
| Million-character digest | Planned | Planned | Not applicable | Not applicable |
| RFC 4231 short key | Not applicable | Not applicable | Planned | Planned |
| RFC 4231 long key | Not applicable | Not applicable | Planned | Planned |
| Streaming chunk-boundary consistency | Planned | Planned | Planned | Planned |
| Negative output-size tests | Planned | Planned | Planned | Planned |

## Logging and package rules

- Do not log HMAC key bytes, entropy bytes, nonce material, DRBG state, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details.
- Public RFC HMAC keys are synthetic test-vector inputs only and must not be described as operational secrets.
- Local chunk-boundary fixtures must be labeled deterministic API consistency tests, not public KAT sources.
- Evidence should record source IDs, test names, and status, not sensitive values.

## Required code updates when implementation starts

1. Add internal SHA-512-family context and compression code under `src/hash/` and `src/internal/`.
2. Keep public context layout opaque and update only size-query behavior if needed.
3. Add startup self-test entries or a documented startup-scope rationale.
4. Add `tests/kat` executables for SHA-384/SHA-512 and HMAC variants.
5. Update `src/selftest/kat_manifest.c` statuses from `p1-15-reserved-kat-plan` to implemented or extended-test statuses.
6. Update `docs/kcmvp/SERVICE_SCOPE_MATRIX.md` and `docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md`.
7. Update `evidence/release-template/README.md` and release evidence workflow notes.
8. Keep the public-surface and SHA-2 scope policy checks passing.

## P1-15 evidence files

P1-15 evidence drafts should include:

- `SHA2_SCOPE_CHECK.txt`
- `SHA2_SCOPE_REVIEW_NOTES.txt`
- `ctest.log` with `sha2_reserved_api_tests`
- `PUBLIC_SURFACE_CHECK.txt`
- `THREAD_SAFETY_CHECK.txt`
- Existing install-smoke and PoC package smoke logs

These files are engineering review inputs and should be reviewed before external sharing.
