# AnoCrypto-C Priority TODO

Status: P1-21 draft.

## Completed P0/P1 foundation items

- C99 repository skeleton and clean-room records.
- Public C ABI skeleton, module lifecycle, startup self-test, build/version identity, install/export package, and smoke tests.
- SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256 deterministic core with public-source KATs and boundary tests.
- Internal entropy adapter and Linux/Windows/Apple platform provider seams.
- In-memory HMAC CSP/key handle lifecycle slice and thread-safety support boundary.
- Reproducible-build notes, CI starter, API/ABI checklist, public-surface policy, evidence collection, and customer PoC package v0.
- Service-scope, differential-validation, block-cipher decision, side-channel, SBOM/signing, and evidence-manifest planning records.
- SHA-384/SHA-512 reserved scope and negative-policy checks.
- P1-16 reserved public random-service ABI boundary.
- P1-17 entropy failure injection, health-test skeleton, fatal state-transition tests, and clean PoC hook boundary.
- P1-18 OS-provider classification, status-only operational probe, and source-backed process-change guard.
- P1-19 implementation of the existing `ano_crypto_random_*` symbols using platform entropy-backed HMAC_DRBG-SHA-256.
- P1-19 random lifecycle, cross-API misuse, process/fork, provider-disabled, and failure-injection tests.
- P1-19 install/PoC smoke, CI/policy/evidence updates, and no-new-public-symbol guard.
- P1-20 random lifecycle stress, reseed-limit/failure-recovery regression, and fail-closed process-marker policy.
- P1-20 VM snapshot/clone/suspend guidance, hosted status-only review artifacts, and clean Release ABI-0 symbol-baseline automation.
- P1-21 cross-platform Win32/pthread CSP/HMAC concurrency regression and thread-platform status artifact.
- P1-21 required Linux GCC ASan/UBSan lane, sanitizer-mode guard, and concurrency/sanitizer policy check.
- P1-21 build-identity ABI-0 field-order decision and regression test without new public symbols.

## Immediate P1 next items

1. Review actual hosted Linux, Windows, and Apple random/thread artifacts and record platform-specific limitations.
2. Review the hosted Clang sanitizer runtime before making it a required lane; evaluate ThreadSanitizer separately.
3. Decide when to implement SHA-384/SHA-512 after the P1-15 entry criteria are reviewed.
4. Select a direct noise source only if required, then define source-specific startup/continuous health tests, thresholds, false-positive handling, and recovery policy.
5. Add reproducible source-archive timestamp normalization if byte-for-byte archives become a release requirement.
6. Add package license/evaluation notice after legal review.
7. Select the first block cipher/mode only after customer and validation-target candidate inputs are reviewed.
8. Add differential tests only after a reviewed public/reference source or tool is selected.
9. Replace SBOM/signing placeholders with reviewed generator/signing outputs after tool and key-management policy are selected.

## P2 candidates

- Block cipher/mode implementation based on reviewed customer and validation-target requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper boundary draft.
- AnoSecureKit and AnoPKI integration boundary drafts.

## P1-16 scope

- Reserved public `ac_random.h` boundary.
- Fail-closed `ano_crypto_random_*` placeholders.
- Reserved-boundary regression and policy/evidence notes.

## P1-17 scope

- Dedicated-build entropy provider success/failure/partial-write hook, default off.
- Source failure, partial output, zero-length, oversized-request, and minimum-request tests.
- Internal startup/continuous duplicate-block health-test candidates.
- Module-fatal health failure, DRBG invalidation, and sticky service-gate coverage.
- Entropy/health policy, CI, evidence, and clean PoC checks.

## P1-18 scope

- Internal entropy source classification, health-test ownership, and restart metadata.
- Status-only operational probe for reviewed OS-provider branches.
- Raw-noise health skeleton restricted to explicit internal engineering callbacks.
- Private process marker for source-backed DRBG state and reseed-after-process-change behavior.
- Operational tests, policy target, CI/evidence records, and clean PoC notes.

## P1-19 scope

- Existing public random symbols implemented without adding a public function.
- Fixed platform entropy -> internal adapter -> HMAC_DRBG-SHA-256 service flow.
- Caller-owned opaque context with bidirectional source-backed/explicit-entropy ownership enforcement.
- Generation failure output clearing and 65,536-byte per-request limit.
- Explicit rejection of prediction-resistance requests and cross-API DRBG contexts.
- Child/process-change generation blocked until a source-backed reseed succeeds.
- Provider-disabled and deterministic failure/partial-write test lanes.
- Install consumer and PoC lifecycle smoke without generated-byte output.
- Random-service policy/evidence/package records and clean Release ABI review.

## P1-20 scope

Random-service lifecycle and operational portability hardening:

- hosted-run status-only artifact collection and explicit review TODO;
- repeated init/generate/reseed/uninit stress;
- reseed-limit and failure recovery regression;
- VM snapshot/clone/suspend guidance;
- unsupported process-marker policy;
- clean Release symbol-baseline automation.

## P1-21 scope

Cross-platform concurrency, sanitizer, and build-identity ABI hardening:

- Win32 `CreateThread` and POSIX pthread branches in one CSP/HMAC concurrency regression;
- status-only thread backend artifact on normal hosted lanes;
- required Linux GCC AddressSanitizer/UndefinedBehaviorSanitizer lane;
- Clang sanitizer runtime and ThreadSanitizer explicitly deferred pending reviewed hosted results;
- retained build-identity ABI version 1 layout with no new public symbol;
- policy, evidence, PoC, CI, and clean-room updates;
- algorithm scope unchanged.

## P1-22 candidate

SHA-384/SHA-512 entry-gate decision:

- re-check P1-15 implementation entry criteria and public-vector provenance;
- decide implement/defer with no placeholder ambiguity;
- if approved, stage internal SHA-512 core and SHA-384 derivation with KAT/differential/evidence plan;
- otherwise choose reproducible archive normalization as the next non-algorithm hardening task.
