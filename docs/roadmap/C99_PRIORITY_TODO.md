# AnoCrypto-C Priority TODO

Status: P1-22 draft.

## Completed foundation through P1-22

- C99 repository, clean-room records, public C ABI skeleton, lifecycle, self-test, build identity, install/export package, and evidence/PoC workflows.
- SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, entropy-provider seam, public random lifecycle, CSP HMAC key handles, thread-safety/concurrency guardrails, sanitizer lane, and 53-symbol ABI baseline.
- P1-22 SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 implementation using existing public identifiers.
- P1-22 startup and extended public-source KATs, negative tests, CSP key-handle coverage, install/PoC smoke, policy/evidence, and clean-room updates.

## Immediate P1 next items

1. **P1-23 candidate: SHA-512-family differential validation and implementation hardening.**
   - Select an independently reviewed public/reference provider or tool.
   - Add automated one-shot/streaming comparison over deterministic public/non-secret inputs.
   - Add boundary lengths around 111/112/127/128/129 bytes and larger deterministic messages.
   - Record provider/version/license and status-only mismatch evidence.
   - Review 64-bit portability/performance and compiler output on hosted Linux, Windows, and Apple builds.
2. Review actual hosted random/thread/SHA-512-family artifacts and record platform-specific limitations.
3. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
4. Select a direct noise source only if required, then define source-specific health tests and recovery policy.
5. Normalize archive timestamps only if byte-for-byte reproducibility becomes a release requirement.
6. Add package legal/evaluation notice after legal review.
7. Select the first block cipher/mode only after customer and validation-target candidate inputs are reviewed.
8. Replace SBOM/signing placeholders after tool and key-management policy selection.

## Deferred P2 candidates

- Block cipher/mode implementation based on reviewed requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.
