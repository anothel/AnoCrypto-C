# KCMVP Readiness Gap — P2 Consolidated Register

Status: P2-02 internal module-boundary and operating-environment freeze. Formal validation and external review remain pending.

## Baseline and Claims Boundary

| Item | Current value |
|---|---|
| Product | `AnoCrypto-C` |
| Work-product version | `0.1.0-p2-02-boundary-freeze` |
| Input baseline declared by project owner | `0.1.0-p1-33-release-rehearsal` at `cf0f3c425dd265c7509fb64a6123db97f71055dd` |
| Public headers | 14 |
| Exported public symbols | 69 |
| ABI line | `ANOCRYPTO_C_0` |
| P1-34 actual hosted review | `BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING` |
| Actual hosted lanes | `NOT_RUN` |
| Actual hosted review | `UNREVIEWED` |
| Repository signing | `UNSIGNED` |
| Offline signing handoff | `BLOCKED` |
| External release authorization | `NO` |

This document is an engineering readiness record. It does not establish an accepted validation scope, a completed institution review, a signed release, or authorization for external distribution.

## Status Taxonomy

| Status | Meaning |
|---|---|
| `READY_FOR_INTERNAL_REVIEW` | Implemented or documented sufficiently for project review; external acceptance is not implied. |
| `PARTIAL` | Useful implementation or evidence exists, but specification, target coverage, or retained review is incomplete. |
| `NOT_IMPLEMENTED` | Required candidate mechanism or document does not yet exist. |
| `EXTERNAL_CONFIRMATION_REQUIRED` | The correct interpretation or accepted option must be confirmed with a testing/validation institution or qualified specialist. |
| `BLOCKED_EXTERNAL` | Work cannot be completed because of an external dependency outside the repository. |
| `OUT_OF_SCOPE` | Deliberately excluded from the current candidate module or product layer. |

## Current Implementation Inventory

### Candidate module and public surface

- Candidate runtime boundary: shared `anocrypto_c` library plus 14 installed C99 headers.
- Package identity: `AnoCryptoC`; imported target: `AnoCryptoC::AnoCryptoC`.
- State model: `UNINITIALIZED -> SELF_TEST_RUNNING -> OPERATIONAL`, with sticky `ERROR_STATE` and explicit `SHUTDOWN`.
- Public service gate rejects cryptographic operations before successful startup self-test and after fatal state.
- Current ABI remains 69 exported names on `ANOCRYPTO_C_0`; opaque caller-owned contexts use size-query APIs.

### Implemented cryptographic services

- SHA-256, SHA-384, and SHA-512 one-shot and streaming.
- HMAC-SHA-256/384/512 one-shot, streaming, and HMAC-purpose key-handle paths.
- HMAC_DRBG-SHA-256 explicit-entropy deterministic core.
- Source-backed public random service using the internal platform entropy adapter and HMAC_DRBG-SHA-256.
- AES-128/192/256 primitive through CSP-backed services.
- AES-CBC without padding, complete blocks only, caller-supplied IV, exact in-place support, and partial-overlap rejection.
- AES-CMAC with AES-128/192/256, full 16-byte tag, one-shot compute/verify, and streaming compute.
- In-memory purpose-separated HMAC, block-cipher, and CMAC key handles.

### Self-test, state, entropy, and CSP assets

- Startup KAT paths for the implemented SHA-2, HMAC, HMAC_DRBG, AES, CBC, and CMAC services.
- KAT manifest and build-identity consistency checks.
- Startup and continuous duplicate-block health-test candidates for internal engineering evaluation.
- OS-provider status-only operational probe and provider-disabled failure behavior.
- Process/fork marker with reseed-required behavior for source-backed random contexts.
- Sticky fatal-state transition on selected fatal health-test failures.
- Key destruction and internal temporary/key-schedule zeroization paths.
- Limited global locking and cross-platform thread/process seams.

## Test and Evidence Inventory

The ordinary tests-enabled build currently registers 49 CTest cases when OpenSSL 3.x differential tests are available. Coverage includes:

- startup/extended KATs and API-negative tests;
- module state and cryptographic service gating;
- DRBG boundaries, reseed, entropy-helper, and failure cleanup;
- entropy adapter/provider/health/process behavior;
- AES-CBC and AES-CMAC hardening, lifecycle, concurrency, and differential tests;
- SHA-512-family representation/alignment/endian review inputs;
- CSP exhaustion, stale handle, key lifecycle, and thread-safety checks;
- current-header and retained P1-29-header compatibility consumers;
- install/build-tree PoC consumers;
- hosted artifact manifest, tagged rehearsal, provenance, signing-handoff, and sensitive-artifact negative regressions.

Existing engineering evidence assets include source/archive hashes, SPDX 2.3 source inventory, public-header and symbol baselines, install and PoC smoke logs, compiler-output captures, unsigned provenance, offline-signing handoff records, and synthetic tagged-release rehearsal. They are not substitutes for actual hosted results, institution test results, or formal review disposition.

## Consolidated Readiness Gap Register

| ID | Area | Current status | Existing basis | Primary gap / closure condition |
|---|---|---|---|---|
| GAP-001 | Candidate physical/logical boundary | `READY_FOR_INTERNAL_REVIEW` | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` freezes one shared-object runtime boundary, source areas, state, interfaces, data flows, external dependencies, exclusions, and operator assumptions. | Obtain institution confirmation of the dynamic-library boundary and treatment of caller-owned contexts/OS dependencies. |
| GAP-002 | Security level and validation target | `EXTERNAL_CONFIRMATION_REQUIRED` | Software-module candidate assumptions exist. | Confirm target security level, accepted module form, and applicable requirement interpretation before submission drafting. |
| GAP-003 | Operating environments | `PARTIAL` | Initial OE-01 is frozen to Ubuntu Server 24.04.4 LTS x86_64, GCC 13.3.0, CMake 3.28.3, Release shared library; all other environments are deferred or unsupported for the initial candidate. | Capture immutable image/CPU/kernel/package identifiers, execute on the selected candidate, and confirm variant/retest treatment externally. |
| GAP-004 | Algorithm/service target scope | `EXTERNAL_CONFIRMATION_REQUIRED` | Implemented scope is narrow and recorded. | Confirm which implemented services are in the initial validation target and whether any identifiers/placeholders must be excluded from the submitted configuration. |
| GAP-005 | Public API/ABI control | `READY_FOR_INTERNAL_REVIEW` | 14 headers, 69 symbols, retained P1-29 headers, normalized compatibility checks. | Institution acceptance and selected-platform ABI review remain external. |
| GAP-006 | Finite-state model | `PARTIAL` | State gate and sticky fatal state are implemented and tested. | Produce a normative state/transition/service-availability specification and recovery/operator-action matrix in P2-03. |
| GAP-007 | Startup algorithm KATs | `READY_FOR_INTERNAL_REVIEW` | Startup KAT implementation and external extended KAT tests exist. | Map every submitted service and configuration to accepted startup/conditional tests. |
| GAP-008 | Module integrity test | `NOT_IMPLEMENTED` | Current integrity flag is a placeholder plus build/KAT-manifest consistency checks. | Select, implement, and document an accepted integrity mechanism after consultation. |
| GAP-009 | Conditional tests | `PARTIAL` | Key-purpose checks, DRBG lifecycle checks, and failure paths exist. | Determine required conditional-test set and trigger points for the selected scope. |
| GAP-010 | Fatal/recoverable failure policy | `PARTIAL` | Sticky fatal state exists; ordinary errors remain service-local. | Freeze exact classification, restart requirements, and allowed status queries in P2-03. |
| GAP-011 | Entropy-source responsibility | `EXTERNAL_CONFIRMATION_REQUIRED` | OS-provider adapter and external-provider health ownership are documented. | Confirm whether the OS provider can remain outside direct noise-source testing and what evidence is required for each environment. |
| GAP-012 | Health-test interpretation | `PARTIAL` | Duplicate-block engineering skeleton and fatal/local policy wiring exist. | Select source-specific tests, thresholds, startup/continuous placement, and recovery rules only for an owned direct noise source, if required. |
| GAP-013 | DRBG limits and lifecycle | `PARTIAL` | Instantiate/reseed/generate limits and reseed-required tests exist. | Consolidate exact limits, prediction-resistance policy, process-change behavior, and operator guidance in P2-04. |
| GAP-014 | Fork/process behavior | `PARTIAL` | Process marker and reseed-required behavior are implemented. | Retain actual target results and confirm unsupported-marker policy. |
| GAP-015 | VM clone/snapshot and suspend/resume | `NOT_IMPLEMENTED` | P2-02 explicitly excludes automatic detection and treats cloned/restored contexts as unsupported unless discarded and reinitialized by the consuming environment. | Confirm operator guidance and evidence expectations in P2-04; do not claim automatic protection. |
| GAP-016 | CSP inventory and lifecycle | `PARTIAL` | Purpose-separated in-memory handles and destruction paths exist. | Create a formal CSP inventory, state/ownership table, and service-by-service lifecycle in P2-05. |
| GAP-017 | Zeroization evidence | `PARTIAL` | Explicit zeroize paths and negative/lifecycle tests exist. | Retain target/compiler-specific evidence and define what is guaranteed at module/process/OS boundaries. |
| GAP-018 | Persistent/exported keys | `OUT_OF_SCOPE` | Public export/persistence/wrapping are rejected or absent. | Keep excluded unless a separate product and validation scope gate approves them. |
| GAP-019 | Thread-safety model | `PARTIAL` | OE-01 freezes thread safety ON with the POSIX/pthread-compatible branch; module/global state is serialized while caller-owned mutable contexts require caller serialization. | Retain OE-01 target results and formalize role/service concurrency wording. |
| GAP-020 | Secure coding review | `PARTIAL` | Warnings, sanitizers, negative tests, policy scripts, and clean-room records exist. | Establish review checklist completion, findings register, and disposition process in P2-06. |
| GAP-021 | Side-channel review | `PARTIAL` | Table-free AES and compiler-output captures exist. | Complete target/compiler/CPU-specific human review and define external-assessment backlog. |
| GAP-022 | Local test evidence | `READY_FOR_INTERNAL_REVIEW` | Repeatable CTest, policy, install, PoC, compatibility, and evidence tooling exist. | Retain reviewer/date/disposition for the selected release candidate. |
| GAP-023 | Actual hosted evidence | `BLOCKED_EXTERNAL` | Workflow and manifest/review model exist. | Resolve GitHub Actions billing, run on a known commit, hash/download, and perform human review. P1-34 remains incomplete. |
| GAP-024 | 32-bit and big-endian target evidence | `BLOCKED_EXTERNAL` | Workflow definitions and cross-toolchain seams exist. | No actual hosted runner result exists; confirm whether either environment is in initial scope. |
| GAP-025 | Reproducibility | `PARTIAL` | Clean rehearsal, source binding, and notes exist. | Decide source and binary reproducibility claims and retain an actual tagged candidate run. |
| GAP-026 | Submission documents | `PARTIAL` | Many engineering design/evidence fragments exist. | Convert them into controlled basic design, detailed design, configuration-management, and test documents in P2-07. |
| GAP-027 | Configuration management | `PARTIAL` | Git/CMake identity, source manifest, ABI baseline, provenance, and hash tooling exist. | Define controlled baselines, roles, change control, build environment, release records, and archival procedure. |
| GAP-028 | Operator/installation guidance | `PARTIAL` | SDK integration and PoC notes exist. | Produce submitted-configuration installation, startup, error, and secure-operation guidance. |
| GAP-029 | Source/component inventory | `PARTIAL` | Exact source inventory and source-only SPDX record exist. | Select dependency, license, vulnerability, toolchain/runtime scope with legal and institution input. |
| GAP-030 | Signing and release authorization | `BLOCKED_EXTERNAL` | Key-free handoff and verifier policy exist. | Provision signer/TSA/roles, complete actual hosted review, create an approved tag, and obtain explicit release authorization. |
| GAP-031 | Customer evaluation terms | `NOT_IMPLEMENTED` | PoC technical boundary is documented. | Legal review and approved evaluation/support terms are required before broader distribution. |
| GAP-032 | Product-layer separation | `READY_FOR_INTERNAL_REVIEW` | AnoCrypto-C, AnoSecureKit, AnoPKI, and optional wrapper boundaries are documented. | Reconfirm during consultation so application/PKI workflow is not pulled into the module scope unintentionally. |

## P2-01 and P2-02 Disposition

P2-01 consolidated the implementation/test/evidence inventory, traceability, consultation questions, submission inventory, roadmap, blocker record, ABI check, and local regression records.

P2-02 freezes the internal candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` and its shared-library boundary/configuration. This closes the internal selection action for GAP-001 and narrows GAP-003, GAP-015, and GAP-019, but it does not establish formal boundary or environment acceptance. Immutable selected-environment evidence is still incomplete, and P1-34 remains `BLOCKED_EXTERNAL`.

All institution-dependent items remain `EXTERNAL_CONFIRMATION_REQUIRED` until a named institution or qualified reviewer supplies a retained response.

## External Reference Checkpoint

The following official-source checkpoints informed document categorization only:

- KISA, “암호모듈 검증” process overview and references to KS X ISO/IEC 19790 and KS X ISO/IEC 24759, checked 2026-07-14.
- KISA, public announcement for the revised “암호모듈 제출물 작성 안내서 (2025.9.)”, checked 2026-07-14; exact attachment contents and applicability remain subject to controlled review.

Exact current templates, accepted interpretations, security level, test methods, and submission package requirements remain `EXTERNAL_CONFIRMATION_REQUIRED`.
