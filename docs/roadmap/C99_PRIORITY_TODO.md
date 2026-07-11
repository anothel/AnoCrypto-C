# AnoCrypto-C Priority TODO

Status: P1-26 AES/CBC hardening completed; P1-27 scope decision pending.

## Completed foundation through P1-26

- C99 repository, clean-room records, stable public C ABI skeleton, lifecycle,
  self-test, build identity, install/export package, evidence, and PoC flows.
- SHA-256/SHA-384/SHA-512 and HMAC variants, HMAC_DRBG-SHA-256, entropy seam,
  public random lifecycle, CSP HMAC/AES handles, thread/concurrency guardrails,
  GCC ASan/UBSan, and explicit exported-symbol baselines.
- P1-23 deterministic OpenSSL 3.x test-only differential comparison and C99
  representation, unaligned/endian, and Linux 32-bit workflow guardrails.
- P1-24 reviewed block-cipher entry gate.
- P1-25 AES-128/192/256 primitive, CBC without padding, CSP lifecycle,
  startup/extended KATs, negative/differential tests, install/PoC/evidence,
  and the reviewed 61-symbol ABI-0 baseline.
- P1-26 internal AES-context integrity and fail-closed reinitialization,
  128-iteration lifecycle stress, variable-chunk/unaligned regression,
  CSP HMAC/AES concurrency coverage, expanded OpenSSL AES-CBC differential
  corpus, and compiler-output capture for manual review.

## Immediate P1 next items

1. Review actual hosted P1-23 through P1-26 Windows, macOS, Linux 32-bit, and
   s390x big-endian artifacts; record toolchain, commit, artifact hash,
   reviewer, limitations, and disposition.
2. Manually review retained GCC/Clang AES compiler-output artifacts for each
   intended target and optimization profile. Do not infer a general
   leakage-resistance claim from the capture workflow.
3. Choose one P1-27 direction through a separate decision gate:
   - narrow AES-CTR with counter construction/exhaustion and overlap policy;
   - CMAC with key-purpose and CSP service-scope policy; or
   - pause algorithm expansion and focus on hosted evidence, legal package
     notice, SBOM/signing tooling, and target-specific review.
4. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
5. Select a direct noise source only if required, then define source-specific
   health tests.
6. Add package legal/evaluation notice after legal review.
7. Replace SBOM/signing placeholders after tool and key-management policy
   selection.

## Deferred P2 candidates

- AEAD, padding helpers, GMAC, or broader mode suites only after separate gates.
- KDF/MAC expansion beyond a reviewed requirement.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.

## P1-26 placement

P1-26 hardens the P1-25 AES/CBC implementation without changing public
functions, enum values, public layouts, or the 61-symbol baseline. It adds
internal context validation, fail-closed reinitialization, longer lifecycle and
concurrency regression, expanded test-only differential comparison, and
compiler-output artifacts for human review.

P1-26 does not add CTR, GCM, padding, CMAC/GMAC, hardware acceleration, or a
new algorithm. It does not claim that the portable implementation is constant
time on every compiler/CPU, and hosted workflow definitions are not recorded as
successful evidence until their artifacts are reviewed.

## Planned next sequence

| Step | Candidate name | Scope |
|---|---|---|
| P1-25 | AES/CBC implementation | Completed with CSP key lifecycle, KAT/negative/differential tests, package/evidence wiring, and 61-symbol baseline. |
| P1-26 | AES/CBC hardening | Completed without public ABI widening; lifecycle, portability, concurrency, expanded differential, and compiler-output capture. |
| P1-27 | Scope decision gate | Select narrow AES-CTR, CMAC, or a non-algorithm evidence/productization step after hosted/manual review. |
