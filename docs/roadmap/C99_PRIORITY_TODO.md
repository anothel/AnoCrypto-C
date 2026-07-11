# AnoCrypto-C Priority TODO

Status: P1-27 CMAC entry gate completed; P1-28 implementation decision is ready for review.

## Completed foundation through P1-27

- C99 repository, clean-room records, stable public C ABI skeleton, lifecycle,
  self-test, build identity, install/export package, evidence, and PoC flows.
- SHA-256/SHA-384/SHA-512 and HMAC variants, HMAC_DRBG-SHA-256, entropy seam,
  public random lifecycle, CSP HMAC/AES handles, thread/concurrency guardrails,
  GCC ASan/UBSan, and explicit exported-symbol baselines.
- Deterministic OpenSSL 3.x test-only differential comparison and C99
  representation, unaligned/endian, Linux 32-bit, and big-endian workflow
  guardrails.
- AES-128/192/256 primitive and CBC without padding with CSP lifecycle,
  startup/extended KAT, negative/differential tests, install/PoC/evidence,
  lifecycle/concurrency regression, compiler-output capture, and the reviewed
  61-symbol ABI-0 baseline.
- P1-27 decision selecting a narrow AES-CMAC candidate before AES-CTR, with a
  dedicated key-purpose requirement, full-tag-only API proposal, public-vector
  KAT/evidence plan, and automated no-implementation/unchanged-ABI checks.

## Immediate P1 next items

1. Review and approve or revise the P1-27 CMAC API and dedicated key-purpose
   decision before adding public declarations.
2. If approved, implement P1-28 as one narrow slice:
   - dedicated CMAC key type or equally explicit usage authorization;
   - AES-128/192/256 CMAC;
   - full 16-byte tags only;
   - streaming compute, one-shot compute, and one-shot verification;
   - startup and extended public-source KATs;
   - negative, differential, install, PoC, evidence, and symbol-delta checks.
3. Review actual hosted Windows, macOS, Linux 32-bit, and s390x big-endian
   artifacts; record toolchain, commit, artifact hash, reviewer, limitations,
   and disposition.
4. Manually review retained GCC/Clang AES compiler-output artifacts for each
   intended target and optimization profile. Do not infer a general
   leakage-resistance claim from the capture workflow.
5. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
6. Add package legal/evaluation notice only after legal review.
7. Replace SBOM/signing placeholders after tool and key-management policy
   selection.

## Deferred P1/P2 candidates

- AES-CTR requires a separate counter construction, uniqueness, exhaustion,
  clone/snapshot, and recovery gate.
- AEAD, padding helpers, GMAC, or broader mode suites require separate gates.
- KDF expansion beyond a reviewed requirement.
- Direct noise source selection and source-specific health tests.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.

## P1-27 placement

P1-27 is a scope and API/evidence gate. It adds no CMAC implementation, public
header, enum value, function, or exported symbol. It retains the P1-26 AES/CBC
implementation and the 61-symbol baseline.

The gate chooses CMAC before CTR because the current core already contains a
narrow confidentiality mode, while CTR would introduce counter ownership and
reuse risks that require a separate policy decision. The CMAC decision does not
approve a protocol construction or replace AEAD.

## Planned next sequence

| Step | Candidate name | Scope |
|---|---|---|
| P1-26 | AES/CBC hardening | Completed without public ABI widening. |
| P1-27 | CMAC entry gate | Completed with selection, API/key-purpose proposal, KAT/evidence plan, package/evidence notes, and unchanged 61-symbol baseline. |
| P1-28 | AES-CMAC implementation | Candidate: dedicated-purpose CSP handle, AES-128/192/256, full tags, compute/verify, KAT/negative/differential/install/PoC/evidence, reviewed symbol delta. |
