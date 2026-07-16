# C99 Priority TODO

Status: P2-02 module-boundary and operating-environment freeze completed; P2-03 is next.

## Completed Through P1-33

- C99 repository, clean-room records, stable status/API conventions, module lifecycle, startup self-test/error-state, build identity, install/export package, and customer PoC.
- SHA-256/384/512, HMAC-SHA-2, HMAC_DRBG-SHA-256, platform-backed random service, AES-128/192/256 CBC without padding, AES-CMAC full tag, and purpose-separated in-memory CSP handles.
- KAT, negative, lifecycle, concurrency, sanitizer, differential, portability, install, compatibility, source-provenance, source-SPDX, signing-handoff, and clean rehearsal engineering assets.
- Public boundary frozen at 14 headers, 69 exported symbols, and `ANOCRYPTO_C_0`.

## P1-34 External Blocker — Still Incomplete

```text
Status: BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING
Run: 29256531912
Commit: cf0f3c425dd265c7509fb64a6123db97f71055dd
Jobs reaching runner: 0
Artifacts: 0
Actual hosted lanes: NOT_RUN
Actual hosted review: UNREVIEWED
```

Do not substitute local or synthetic evidence. Resume only after billing is resolved and actual artifacts are independently reviewed. See `docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md`.

## Completed P2-01

- consolidated current implementation, tests, evidence, packaging, and release-state inventory;
- created requirement-to-code-to-test-to-evidence traceability;
- classified readiness gaps with explicit external-confirmation and blocked states;
- prepared prioritized pre-consultation questions;
- inventoried submission documents by existing/update/new/after-consultation state;
- defined P2-02 through P2-08 completion criteria and dependencies;
- retained P1-34 blocker and release/signing boundaries;
- preserved public ABI with no algorithm or public API addition.

## Completed P2-02

- froze `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` as one internal shared-library candidate;
- documented physical/logical boundary, runtime source areas, data flows, exclusions, dependencies, and operator assumptions;
- selected Ubuntu Server 24.04.4 LTS x86_64, GCC 13.3.0, CMake 3.28.3, Release/Unix Makefiles;
- froze thread safety and Linux OS entropy ON with tests/examples/hooks/OpenSSL/signing fixtures/sanitizers/LTO OFF;
- classified all other environments as deferred, unsupported for the initial candidate, or externally blocked;
- added machine-readable candidate metadata, fail-closed CMake enforcement, policy/evidence checks, and unchanged-ABI verification;
- retained formal boundary/environment acceptance, immutable candidate execution, and P1-34 hosted review as unresolved.

## Next Sequence

1. **P2-03 — Self-test, integrity, finite-state, and recovery specification**
2. **P2-04 — Entropy/provider/DRBG responsibility and evidence closure**
3. **P2-05 — CSP inventory and zeroization evidence closure**
4. **P2-06 — Secure coding and target-specific side-channel review**
5. **P2-07 — Controlled submission-document skeleton**
6. **P2-08 — Pre-consultation rehearsal and decision capture**

Detailed deliverables and exit criteria are in `docs/roadmap/P2_VALIDATION_READINESS_ROADMAP.md`.

## Deferred Until Explicit Scope Gate

- AES-CTR/GCM/GMAC, generic AEAD, padding helpers, and tag truncation;
- KDF public APIs and public-key algorithms;
- key export, persistence, wrapping, multi-process CSP, and platform keystore;
- hardware acceleration/dispatch and C++ convenience wrapper priority;
- production signer/TSA provisioning, actual tag/signature/timestamp, and external release;
- any statement that formal validation or institution review is complete.
