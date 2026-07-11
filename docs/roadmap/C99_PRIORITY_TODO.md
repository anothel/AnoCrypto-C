# AnoCrypto-C Priority TODO

Status: P1-23 draft.

## Completed foundation through P1-23

- C99 repository, clean-room records, public C ABI skeleton, lifecycle,
  self-test, build identity, install/export package, evidence, and PoC flows.
- SHA-256/SHA-384/SHA-512 and HMAC variants, HMAC_DRBG-SHA-256, entropy seam,
  public random lifecycle, CSP HMAC handles, thread/concurrency guardrails,
  GCC ASan/UBSan, and the 53-symbol ABI-0 baseline.
- P1-22 public-source startup/extended KATs, negative tests, CSP handle paths,
  install/PoC smoke, and SHA-512-family implementation policy.
- P1-23 deterministic OpenSSL 3.x test-only differential comparison for all
  implemented SHA-2/HMAC variants.
- P1-23 C99 representation guards, unaligned/endian portability regression,
  hosted status-artifact wiring, and a Linux GCC `-m32` workflow lane.
- P1-23 evidence and PoC checks that keep OpenSSL and differential artifacts
  outside the product SDK/runtime boundary.

## Immediate P1 next items

1. **P1-24 candidate: block-cipher/mode implementation entry gate.**
   - Reconfirm customer and validation-target candidate requirements.
   - Choose algorithm, mode, key sizes, IV/nonce policy, padding behavior, and
     allowed API surface before writing implementation code.
   - Select only public standards/vectors and record clean-room provenance.
   - Define startup KAT, extended KAT, negative, differential, CSP, install,
     PoC, and evidence expectations as one gate.
   - Decide whether the existing 53-symbol ABI can be used or a reviewed ABI
     change is required.
2. Review actual hosted P1-23 Linux 32-bit, Windows, and macOS artifacts and
   record platform-specific limitations; obtain big-endian evidence if that
   target is intended to be supported.
3. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
4. Add SHA-512-family length-counter overflow simulation only through a
   separately reviewed internal test hook.
5. Select a direct noise source only if required, then define source-specific
   health tests and recovery policy.
6. Normalize archive timestamps only if byte-for-byte reproducibility becomes
   a release requirement.
7. Add package legal/evaluation notice after legal review.
8. Replace SBOM/signing placeholders after tool and key-management policy
   selection.

## Deferred P2 candidates

- Block-cipher/mode implementation after the P1-24 gate is approved.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.
