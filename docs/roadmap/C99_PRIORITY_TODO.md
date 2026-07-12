# C99 Priority TODO

Status: P1-29 AES-CMAC hardening completed as an engineering slice.

## Completed foundation through P1-29

- C99 repository, clean-room records, stable-status API style, module lifecycle,
  self-test/error-state, build identity, and install/export package structure.
- SHA-256/384/512, HMAC-SHA-2, HMAC_DRBG-SHA-256, entropy/provider seams,
  source-backed public random lifecycle, CSP key handles, and thread-safety seams.
- Reproducible-build notes, CI/evidence/PoC workflows, SBOM/signing placeholders,
  public-surface and clean Release exported-symbol checks.
- AES-128/192/256 and CBC without padding with KAT, negative, differential,
  lifecycle, concurrency, portability, and compiler-output review artifacts.
- AES-CMAC with AES-128/192/256, full tags, dedicated CMAC-purpose handles,
  startup/extended KATs, install/PoC/evidence integration, and a 69-symbol ABI-0
  baseline.
- P1-29 CMAC private-state integrity guards, fail-closed reinitialization,
  bounded cleanup and alias rules, 128-iteration lifecycle stress, expanded
  1,728-comparison differential coverage, shared-handle concurrency regression,
  and target-specific compiler-output capture.

## Immediate P1 candidates

1. Review and retain actual hosted Windows/macOS, Linux 32-bit, and big-endian
   artifacts instead of treating workflow presence as execution evidence.
2. Perform target-specific human review of captured AES and CMAC compiler output
   and record the remaining timing/cache/fault backlog without overclaiming.
3. Strengthen automated ABI/source compatibility comparison across tagged
   releases, including public type size/alignment and semantic compatibility.
4. Decide whether the next algorithm step is a no-implementation gate for a
   narrowly scoped KDF, AES-CTR, or a pause for packaging/legal/evidence
   maturity. CTR must not proceed without a counter-ownership policy.
5. Finalize direct-noise-source health-test ownership only after a concrete
   source and operating environment are selected.

## Deferred

- tag truncation, AES-CTR, GCM, GMAC, padding helpers, hardware dispatch;
- key export, persistence, wrapping, multi-process CSP, platform keystore;
- public-key algorithms and AnoPKI integration;
- complete SBOM/signing and customer evaluation/legal notices;
- any official-validation or completed-review claim.


## P1-30 Release Compatibility Baseline

Completed engineering scope:

- central release identity and expected-tag metadata;
- exact P1-29 public-header and 69-symbol baseline;
- normalized public source-surface comparison;
- current-header and P1-29-header consumers against P1-30;
- installed CMake package exact/incompatible version probes;
- hosted artifact manifest create/verify/tamper tests;
- status-only hosted bundle index/review workflow;
- evidence and PoC compatibility boundaries with no public ABI growth.

Remaining TODO:

- review actual Linux/macOS/Windows hosted compatibility artifacts;
- review actual Linux 32-bit and s390x big-endian artifacts;
- verify the expected tag and commit in a tagged Git workflow;
- select signing/provenance policy before external release use;
- decide P1-31 scope after hosted artifact disposition, without implying completed validation.
