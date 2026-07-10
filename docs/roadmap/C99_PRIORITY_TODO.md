# AnoCrypto-C Priority TODO

Status: P1-17 draft.

## Completed P0/P1 foundation items

- C99 repository skeleton
- clean-room source manifest and exclusion log
- public C ABI header skeletons
- module lifecycle/state-machine skeleton
- startup self-test framework skeleton
- build/version identity APIs
- CMake build/install/export skeleton
- unit, negative, KAT, and install smoke test skeletons
- SHA-256 implementation
- HMAC-SHA-256 implementation
- public-source SHA-256/HMAC-SHA-256 KAT coverage
- extended chunk-boundary consistency tests
- HMAC_DRBG-SHA-256 deterministic core implementation
- public-source HMAC_DRBG-SHA-256 KAT coverage
- internal entropy adapter seam and unit tests
- in-memory HMAC CSP/key handle lifecycle slice
- engineering release evidence collection script
- install-prefix smoke log capture in the evidence script
- source archive SHA-256 generation in the evidence script
- CSP handle table exhaustion and stale-handle reuse tests
- limited module/self-test/CSP global-state lock seam and thread-safety policy draft
- POSIX concurrent CSP/HMAC smoke test
- internal platform entropy provider seam for Linux, Windows, and Apple provider branches
- platform entropy provider smoke/status tests
- internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- random-service public API design draft
- reserved public `ac_random.h` boundary with fail-closed `ano_crypto_random_*` placeholders
- HMAC_DRBG-SHA-256 boundary tests for personalization, additional input, reseed-after-generate, max request, and oversized request rejection
- CMake package naming normalized to `AnoCryptoC` / `AnoCryptoC::AnoCryptoC`
- reproducible build notes draft
- CI matrix skeleton and GitHub workflow starter
- API/ABI review checklist
- public surface policy check target
- P1-10 evidence workflow additions for reproducibility and public surface review
- customer PoC package v0 staging target
- standalone public-API core PoC example using `find_package(AnoCryptoC)`
- build-tree and installed-package PoC smoke flows
- P1-11 evidence workflow additions for PoC package smoke logs
- SDK integration guide and customer PoC package notes
- P1-12 development workflow plan
- P1-12 service scope matrix
- P1-12 differential validation strategy draft
- P1-12 block cipher/mode selection gate
- P1-12 side-channel review checklist
- P1-12 customer PoC intake questionnaire
- P1-12 evidence workflow additions for scope-review notes
- P1-13 SBOM/signing placeholder plan
- P1-13 evidence manifest policy
- P1-13 PoC package review manifest and package status records
- P1-13 evidence workflow additions for `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, and `EVIDENCE_MANIFEST.sha256`
- P1-14 thread-safety support matrix and CI checklist
- P1-14 thread-safety policy check target and evidence/PoC package records
- P1-15 SHA-384/SHA-512 reserved scope decision and KAT plan
- P1-15 reserved SHA-2 API negative test and scope policy check target
- P1-15 evidence/PoC package records for SHA-384/SHA-512 reserved scope
- P1-16 reserved public random-service boundary and fail-closed placeholder checks
- P1-17 entropy provider failure-injection seam and dedicated CI lane
- P1-17 internal startup/continuous health-test candidate skeleton and fatal state-transition tests
- P1-17 entropy/health policy, evidence, and clean PoC package boundary checks

## Immediate P1 next items

1. Review and harden CI matrix after first hosted runs on Linux, Windows, and Apple runners.
2. Add Windows-specific concurrent CSP/HMAC coverage and sanitizer lanes where supported.
3. Decide when to implement SHA-384/SHA-512 after P1-15 reserved-scope entry criteria are reviewed.
4. Select the actual entropy/noise-source boundary and review source-specific startup/continuous health tests, thresholds, restart behavior, and fatality policy.
5. Decide whether and when to expose public `ano_crypto_random_*` APIs after the P1-17 entropy/health review.
6. Add reproducible source archive timestamp normalization if byte-for-byte archives become a release requirement.
7. Decide whether to replace the public build identity struct with field getters before ABI freeze.
8. Add exported-symbol baseline comparison after symbol visibility policy is chosen.
9. Add package license/evaluation notice after legal review.
10. Select first block cipher/mode only after customer and validation-target candidate inputs are reviewed.
11. Add differential tests only after a reviewed public/reference source or tool is selected.
12. Replace SBOM/signing placeholders with reviewed generator/signing outputs after tool and key-management policy are selected.

## P2 candidates

- Block cipher/mode implementation based on reviewed customer and validation-target requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper boundary draft.
- AnoSecureKit and AnoPKI integration boundary drafts.

## P1-16 added scope

- Reserved public `ac_random.h` boundary.
- `ano_crypto_random_*` placeholders that return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` after validation and service gating.
- Random-service reserved API negative regression test.
- Random-service/DRBG/entropy boundary policy check and evidence notes.

## P1-17 added scope

- Dedicated-build entropy provider success/failure/partial-write test hook, default off.
- Negative tests for source failure, partial output, zero length, oversized request, and minimum request bounds.
- Internal startup and continuous duplicate-block health-test candidates.
- Module-fatal health failure transition, DRBG context invalidation, and sticky service-gate coverage.
- Entropy/health policy check, dedicated CI lane, evidence review records, and clean PoC SDK rebuild/scan.
- No public random implementation, public hook API, or finalized health-test claim.
