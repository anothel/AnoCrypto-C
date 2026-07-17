# C99 Priority TODO

Status: P2-01 through P2-08 internal readiness sequence completed locally. External consultation remains `NOT_CONDUCTED`; institution responses remain `NOT_RECEIVED`.

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

## Completed P2-03

- froze self-test policy `STP-P2-03-R1` and finite-state policy `FSM-P2-03-R1`;
- selected `INT-HMAC-SHA256-SINGLE-SO-R1` as an internal candidate without claiming implementation or external acceptance;
- documented startup order, conditional-test ownership, service availability, fatal/recoverable failures, orderly reinitialization, and process-termination recovery;
- recorded runtime integrity, on-demand state transition, controlled key/tag injection, tamper tests, and exact OE-01 execution as open implementation/evidence work;
- added machine-readable policy, fail-closed checker, evidence integration, and orderly reinitialization regression;
- retained 14 headers, 69 symbols, `ANOCRYPTO_C_0`, and all P1-34/release blockers.

## Completed P2-04

- froze `EDP-P2-04-R1` and Linux `getrandom(flags=0)` external-conditioned-provider responsibility for OE-01;
- recorded entropy/nonce/reseed lengths, request/reseed limits, unsupported prediction resistance, and process/VM/suspend behavior;
- separated OS-provider responsibility from any future project-owned direct-noise-source health tests;
- added policy, operational review, responsibility/recovery matrices, evidence integration, and status-only safeguards;
- retained provider acceptance, entropy credit, exact OE-01 run, and formal conditional-test interpretation as external/open.

## Completed P2-05

- froze `CZP-P2-05-R1` and a conservative inventory of module-owned keys, caller-owned contexts, DRBG state, and sensitive intermediates;
- remediated module-owned CSP cleanup on orderly shutdown and locked fatal transition without public ABI growth;
- defined `ZEROIZED` tombstones, explicit destroy, generation-based stale-handle rejection, and caller-owned uninit responsibility;
- added shutdown/fatal/reinitialization regressions, policy checks, and local compiler-output capture;
- retained exact OE-01 review, formal CSP classification, OS/process memory controls, integrity-key realization, and lock-failure expectations as open.

## Completed P2-06

- froze secure-coding policy `SCP-P2-06-R1` and build profile `SCB-P2-06-LINUX-ELF-R1`;
- recorded source review, local GCC/Clang diagnostics, side-channel limitations, findings, and compiler/CPU dispositions;
- fixed the internal CSP `INVALID` switch coverage gap without public ABI change;
- enabled Linux GNU/Clang Release hardening and captured ELF hardening/library hashes;
- retained exact OE-01, named human disposition, statistical timing/cache, power/EM/fault, runtime integrity, and hosted evidence as open;
- preserved 14 headers, 69 symbols, `ANOCRYPTO_C_0`, and all release/signing blockers.

## Completed P2-07 and P2-08

- P2-07 created the controlled internal submission-document skeleton under `SDP-P2-07-R1`.
- P2-08 created the scripted rehearsal, evidence walkthrough, Priority-1 ownership map, 20-entry unresolved-decision register, blank response template, action-impact routing, policy checker, and status-only package smoke under `PCR-P2-08-R1`.
- External consultation is `NOT_CONDUCTED`; institution responses are `NOT_RECEIVED`; no implementation, ABI, algorithm, signing, submission, or release action is authorized by rehearsal completion.

## Next Selection Gate

1. Conduct an approved internal live rehearsal or external pre-consultation through a controlled channel.
2. Capture retained answers and map them to `DEC-*` entries and `ACT-*` classes.
3. Open the next numbered work item only after explicit authorization.
4. Resume **P1-34** independently only after GitHub billing is resolved.

## Deferred Until Explicit Scope Gate

- AES-CTR/GCM/GMAC, generic AEAD, padding helpers, and tag truncation;
- KDF public APIs and public-key algorithms;
- key export, persistence, wrapping, multi-process CSP, and platform keystore;
- hardware acceleration/dispatch and C++ convenience wrapper priority;
- production signer/TSA provisioning, actual tag/signature/timestamp, and external release;
- any statement that formal validation or institution review is complete.
