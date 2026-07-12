# C99 Priority TODO

Status: P1-28 AES-CMAC implementation completed as an engineering slice.

## Completed foundation through P1-28

- C99 repository, clean-room records, stable-status API style, module lifecycle,
  self-test/error-state, build identity, and install/export package structure.
- SHA-256/384/512, HMAC-SHA-2, HMAC_DRBG-SHA-256, entropy/provider seams,
  source-backed public random lifecycle, CSP key handles, and thread-safety seams.
- Reproducible-build notes, CI/evidence/PoC workflows, SBOM/signing placeholders,
  public-surface and clean Release exported-symbol checks.
- AES-128/192/256 and CBC without padding with KAT, negative, differential,
  lifecycle, concurrency, portability, and compiler-output review artifacts.
- AES-CMAC with AES-128/192/256, full tags, dedicated CMAC-purpose handles,
  startup/extended KATs, negative tests, differential, install, PoC, evidence,
  and the reviewed 69-symbol ABI-0 baseline.

## Immediate P1 candidates

1. Review actual hosted Windows/macOS, Linux 32-bit, and big-endian artifacts.
2. Perform target-specific human review of captured AES/CMAC compiler output and
   define a documented side-channel hardening backlog without overclaiming.
3. Decide whether the next algorithm step is a no-implementation gate for
   AES-CTR, a KDF/CMAC-derived service, or a pause for packaging/legal/evidence
   maturity.
4. Strengthen automated ABI/source compatibility comparison across tagged
   releases.
5. Finalize direct-noise-source health-test ownership only after a concrete
   source and operating environment are selected.

## Deferred

- tag truncation, AES-CTR, GCM, GMAC, padding helpers, hardware dispatch;
- key export, persistence, wrapping, multi-process CSP, platform keystore;
- public-key algorithms and AnoPKI integration;
- complete SBOM/signing and customer evaluation/legal notices;
- any official-validation or completed-review claim.
