# AnoCrypto-C Priority TODO

Status: P1-24 draft.

## Completed foundation through P1-24

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

1. **P1-25 candidate: implement the approved narrow AES/CBC slice.**
   - Finalize and review `ac_cipher.h` signatures before adding public surface.
   - Implement AES-128/192/256 primitive and CBC without padding only.
   - Add FIPS 197 and NIST SP 800-38A startup/extended KATs.
   - Bind AES keys to the CSP lifecycle and add stale/destroy/concurrency tests.
   - Select and document a constant-time-oriented portable implementation strategy.
   - Add test-only differential validation, install/PoC smoke, evidence, and symbol review.
2. Review actual hosted P1-23/P1-24 Linux 32-bit, Windows, and macOS artifacts.
3. Evaluate Clang sanitizer runtime and ThreadSanitizer separately.
4. Select a direct noise source only if required, then define source-specific health tests.
5. Add package legal/evaluation notice after legal review.
6. Replace SBOM/signing placeholders after tool and key-management policy selection.

## Deferred P2 candidates

- Block-cipher/mode implementation after the P1-24 gate is approved.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper, AnoSecureKit, and AnoPKI boundary work.
