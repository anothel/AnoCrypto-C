# AnoCrypto-C Algorithm Scope

Status: P1-20 draft.

## Implemented candidate slice

| Area | Algorithm | Current status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-9 engineering slice | Explicit entropy-input public API, startup KAT, public vector test, boundary tests, negative tests, and internal entropy-source instantiate/reseed helper. |
| CSP/key | HMAC key import/use/destroy | Implemented P1-9 lifecycle slice | In-memory HMAC key handles only; no export, persistence, or keystore. |
| Entropy provider | Internal platform provider seam | Implemented P1-9 internal seam | Linux/Windows/Apple branches selected at build time; internal helper seeds/reseeds HMAC_DRBG and P1-19 exposes the existing public random-service lifecycle on reviewed branches. |
| Random service | HMAC_DRBG-SHA-256 over platform entropy | Implemented P1-19 lifecycle slice | Existing `ano_crypto_random_*` symbols, caller-owned opaque context, manual reseed, request limit, provider-disabled behavior, process-change reseed requirement, and output clearing on random-service failures. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | ABI-reserved, not implemented | Name, digest-size, and context-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| Hash | SHA-512 | ABI-reserved, not implemented | Name, digest-size, and context-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| MAC | HMAC-SHA-384 | ABI-reserved, not implemented | MAC-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| MAC | HMAC-SHA-512 | ABI-reserved, not implemented | MAC-size metadata query only; processing returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add additional HMAC_DRBG KAT paths: personalization string, no reseed, no additional input, and request-size boundaries.
2. Add CI matrix for Linux/Windows/Apple entropy provider branches.
3. Keep SHA-384/SHA-512 in P1-15 reserved scope until implementation/KAT/differential-validation entry criteria are ready.
4. Add reproducible build notes.
5. Add public support matrix for thread-safety and platform entropy provider behavior.
6. Maintain P1-19 public random-service evidence, provider-disabled behavior, process-change reseed handling, and explicit lifecycle limitations.

## P1-9 CSP/Entropy/DRBG Notes

- HMAC key handle lifecycle is implemented as an in-memory engineering slice only.
- Internal platform entropy provider seam exists for selected OS branches.
- P1-9 retains the internal DRBG helper that draws entropy/nonce through the adapter and zeroizes local seed material.
- P1-16 originally reserved the public random boundary; P1-19 implements the existing symbols while retaining explicit lifecycle and platform limitations.
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


## P1-16 Random-Service Boundary Notes

- `include/anocrypto/ac_random.h` was introduced as the reserved public random-service boundary in P1-16.
- P1-19 changes `src/drbg/random.c` to implement the existing lifecycle over source-backed HMAC_DRBG-SHA-256; provider-disabled builds remain fail-closed with `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`.
- `tests/unit/random_service_tests.c` verifies lifecycle, provider-disabled behavior, output zeroization, cross-API context rejection, and POSIX fork/reseed behavior.
- `docs/design/PUBLIC_RANDOM_API_DECISION.md` and `docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md` record the decision and evidence rules.
- P1-16 introduced the reserved boundary; P1-19 implements the existing public symbols using the current HMAC_DRBG-SHA-256 core.

## P1-17 Entropy Testability Notes

- No new public cryptographic algorithm or public random-generation service is added.
- Entropy failure injection and health-test state remain internal engineering seams.
- Health failure maps to the existing `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`; no public status enum expansion is made.
- Final entropy source, health-test requirements, sample boundary, thresholds, restart handling, and validation mapping remain TODO / confirmation required.

## P1-19 entropy operational scope note

P1-19 does not add an algorithm or widen the current SHA-256/HMAC-SHA-256/HMAC_DRBG-SHA-256 processing slice. It adds internal source classification, status-only provider probing, and process-change reseed behavior for source-backed DRBG state. Direct noise-source selection, finalized startup/continuous tests, public random generation, and validation mapping remain outside the completed scope.

## P1-19 public random-service scope

P1-19 does not add a new algorithm. It exposes the existing HMAC_DRBG-SHA-256 implementation through `ano_crypto_random_*` using reviewed platform provider branches. Prediction resistance, direct raw-noise source integration, VM snapshot/restore detection, and final validation mapping remain outside the completed scope.
