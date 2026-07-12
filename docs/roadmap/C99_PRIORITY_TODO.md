# C99 Priority TODO

Status: P1-31 unsigned release provenance and source-SBOM gate completed as an engineering slice.

## Completed foundation through P1-31

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
- P1-30 central release identity, retained P1-29 public-header/symbol baseline, old/current-header consumers, strict tag/commit/HEAD fixture checks, and hashed hosted-artifact manifests.
- P1-31 exact-source archive/file hash binding, SPDX 2.3 JSON source inventory, unsigned signing boundary, authorization TODO record, provenance regression tests, hosted provenance lane, and PoC exclusion checks.

## Immediate P1 candidates

1. Review and retain actual hosted Windows/macOS, Linux 32-bit, and big-endian
   artifacts instead of treating workflow presence as execution evidence.
2. Perform target-specific human review of captured AES and CMAC compiler output
   and record the remaining timing/cache/fault backlog without overclaiming.
3. Select and approve an actual detached-signature/verification mechanism, signer identity, key custody, timestamping, and release authorization process without storing credentials in the repository.
4. Expand source inventory into a reviewed dependency/component, license, and vulnerability scope only after tool, privacy, and legal requirements are selected.
5. Strengthen automated ABI/source compatibility comparison across tagged releases, including public type size/alignment and semantic compatibility.
6. Decide whether the next algorithm step is a no-implementation gate for a
   narrowly scoped KDF, AES-CTR, or a pause for packaging/legal/evidence
   maturity. CTR must not proceed without a counter-ownership policy.
7. Finalize direct-noise-source health-test ownership only after a concrete
   source and operating environment are selected.

## Deferred

- tag truncation, AES-CTR, GCM, GMAC, padding helpers, hardware dispatch;
- key export, persistence, wrapping, multi-process CSP, platform keystore;
- public-key algorithms and AnoPKI integration;
- dependency/component SBOM scope, vulnerability/license review, actual signing/verification, signer authorization, timestamping, and customer evaluation/legal notices;
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
- retain actual hosted artifact reviewer/date disposition;
- approve an actual signing/verifier and external-release authorization procedure before distribution.

## P1-31 Release Provenance and Source SBOM Gate

Completed engineering scope:

- deterministic SPDX 2.3 JSON source inventory for the exact source-archive file list;
- SHA-256 binding of the source archive, source files, public-header manifest, ABI-0 symbol list, source SBOM, and hosted disposition;
- exact bundle-manifest coverage, path-safety, source-tree, file-count, and package-verification-code checks;
- explicit `UNSIGNED`, authorization `TODO`, and `External release authorized: NO` records;
- negative regression for tampering, unlisted artifacts, unsafe paths, false signed/approved claims, wrong tag metadata, and incomplete hosted disposition;
- hosted `release-provenance|Linux` bundle/index wiring;
- customer PoC exclusion of the internal source archive, source SBOM, signing request, authorization record, and release tooling;
- unchanged 14-header, 69-symbol `ANOCRYPTO_C_0` public surface.

Remaining TODO:

- review real hosted Windows/macOS/Linux 32-bit/s390x/differential/compiler/compatibility artifacts and record reviewer/date disposition;
- verify the real expected tag, commit, and checkout in the release workflow;
- validate SPDX syntax with the selected downstream consumer where required;
- select dependency, license, and vulnerability inventory scope;
- approve detached-signature format/tool, signer identity, key custody, timestamping, revocation, and verification retention;
- record legal/product external-release authorization;
- decide P1-32 only after the above gaps are ranked; no new public algorithm is implied.
