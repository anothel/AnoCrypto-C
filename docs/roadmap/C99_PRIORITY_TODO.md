# AnoCrypto-C Priority TODO

Status: P1-25 implementation slice completed; P1-26 hardening candidate.

## Completed foundation through P1-25

- C99 repository, clean-room records, public C ABI skeleton, lifecycle,
  self-test, build identity, install/export package, evidence, and PoC flows.
- SHA-256/SHA-384/SHA-512 and HMAC variants, HMAC_DRBG-SHA-256, entropy seam,
  public random lifecycle, CSP HMAC handles, thread/concurrency guardrails,
  GCC ASan/UBSan, and the prior 53-symbol ABI-0 baseline.
- P1-22 public-source startup/extended KATs, negative tests, CSP handle paths,
  install/PoC smoke, and SHA-512-family implementation policy.
- P1-23 deterministic OpenSSL 3.x test-only differential comparison for all
  implemented SHA-2/HMAC variants.
- P1-23 C99 representation guards, unaligned/endian portability regression,
  hosted status-artifact wiring, and a Linux GCC `-m32` workflow lane.
- P1-23 evidence and PoC checks that keep OpenSSL and differential artifacts
  outside the product SDK/runtime boundary.

## Immediate P1 next items

1. **P1-26 candidate: harden the implemented AES/CBC slice without widening the public API.**
   - Finalize and review `ac_cipher.h` signatures before adding public surface.
   - Implement AES-128/192/256 primitive and CBC without padding only.
   - Add FIPS 197 and NIST SP 800-38A startup/extended KATs.
   - Bind AES keys to the CSP lifecycle and add stale/destroy/concurrency tests.
   - Select and document a constant-time-oriented portable implementation strategy.
   - Add test-only differential validation, install/PoC smoke, evidence, and symbol review.
2. Review actual hosted P1-23 through P1-25 Linux 32-bit, Windows, and macOS artifacts.
3. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
4. Select a direct noise source only if required, then define source-specific health tests.
5. Add package legal/evaluation notice after legal review.
6. Replace SBOM/signing placeholders after tool and key-management policy selection.

## Deferred P2 candidates

- CTR/AEAD/padding-helper/CMAC/GMAC scope only after a separate decision gate.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.


## P1-25 placement

P1-25 implements the narrow block-cipher slice approved by P1-24. It adds AES-128/192/256, CBC without padding, CSP-backed keys, startup/extended KATs, negative and OpenSSL test-only differential coverage, install/PoC smoke, evidence records, and a reviewed 61-symbol ABI-0 export baseline.

P1-25 does not add padding, CTR, GCM, CMAC/GMAC, hardware acceleration, or domestic algorithms. Compiler-output and target-specific side-channel review remain TODO.

## Planned next sequence

| Step | Candidate name | Scope |
|---|---|---|
| P1-25 | AES/CBC implementation | Completed with CSP key lifecycle, startup/extended KATs, negative/differential tests, package/evidence wiring, and 61-symbol baseline. |
| P1-26 | AES implementation hardening | Candidate: compiler-output review workflow, larger differential corpus, 32-bit/big-endian hosted evidence, and target-specific side-channel backlog without widening the public API. |
