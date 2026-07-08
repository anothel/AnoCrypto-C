# AnoCrypto-C Algorithm Scope

Status: P1-15 draft.

## Implemented candidate slice

| Area | Algorithm | Current status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-9 engineering slice | Explicit entropy-input public API, startup KAT, public vector test, boundary tests, negative tests, and internal entropy-source instantiate/reseed helper. |
| CSP/key | HMAC key import/use/destroy | Implemented P1-9 lifecycle slice | In-memory HMAC key handles only; no export, persistence, or keystore. |
| Entropy provider | Internal platform provider seam | Implemented P1-9 internal seam | Linux/Windows/Apple branches selected at build time; internal helper can seed/reseed HMAC_DRBG; no public random service API. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | ABI-reserved, not implemented | Name, digest-size, and context-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| Hash | SHA-512 | ABI-reserved, not implemented | Name, digest-size, and context-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| MAC | HMAC-SHA-384 | ABI-reserved, not implemented | MAC-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| MAC | HMAC-SHA-512 | ABI-reserved, not implemented | MAC-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |
| Random service | `ano_crypto_random_*` | Not implemented | Design draft only; no public header or ABI yet. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add additional HMAC_DRBG KAT paths: personalization string, no reseed, no additional input, and request-size boundaries.
2. Add CI matrix for Linux/Windows/Apple entropy provider branches.
3. Keep SHA-384/SHA-512 in P1-15 reserved scope until implementation/KAT/differential-validation entry criteria are ready.
4. Add reproducible build notes.
5. Add public support matrix for thread-safety and platform entropy provider behavior.
6. Perform API/ABI review before adding public random-service APIs.

## P1-9 CSP/Entropy/DRBG Notes

- HMAC key handle lifecycle is implemented as an in-memory engineering slice only.
- Internal platform entropy provider seam exists for selected OS branches.
- P1-9 retains the internal DRBG helper that draws entropy/nonce through the adapter and zeroizes local seed material.
- Public random service and validation-boundary mapping remain TODO.
- These additions are not formal validation evidence.
- Release evidence tooling captures source archive SHA-256 and installed-package smoke logs as engineering evidence inputs.

## P1-12 Scope-lock Notes

- `docs/kcmvp/SERVICE_SCOPE_MATRIX.md` is now the service-level planning companion to this algorithm scope file.
- `docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md` records allowed comparison inputs and logging limits before adding differential tests.
- `docs/design/BLOCK_CIPHER_MODE_SELECTION.md` records why block cipher/mode implementation remains deferred until customer and validation-target candidate inputs are reviewed.
- `docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md` records review questions for key-dependent code paths.
- P1-12 does not add a new algorithm implementation or a new public cryptographic API.


## P1-15 SHA-384/SHA-512 Scope Notes

- `docs/design/SHA2_384_512_SCOPE_DECISION.md` records the decision to keep SHA-384/SHA-512 processing deferred in P1-15.
- `docs/kcmvp/SHA2_384_512_KAT_PLAN.md` records planned vector classes and evidence expectations.
- `tests/negative/sha2_reserved_api_tests.c` verifies metadata queries and `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` processing behavior.
- `tools/check_sha2_scope_policy.cmake` verifies docs, tests, KAT manifest, evidence, and PoC package wiring.
- P1-15 does not add a new cryptographic algorithm implementation.
