# KCMVP Readiness Gap — P2 Consolidated Register

Status: P2-07 controlled internal submission-document skeleton. Runtime integrity implementation, exact operating-environment compiler/CPU review, formal CSP classification, empirical/external side-channel review, formal validation, and hosted evidence remain pending.

## Baseline and Claims Boundary

| Item | Current value |
|---|---|
| Product | `AnoCrypto-C` |
| Work-product version | `0.1.0-p2-07-submission-documents` |
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
- KAT manifest and build-identity consistency checks. These are supporting structural checks, not module-byte integrity authentication.
- P2-03 finite-state, service-availability, failure/recovery, and operator-action matrices.
- Selected integrity candidate `INT-HMAC-SHA256-SINGLE-SO-R1`, explicitly `SELECTED_CANDIDATE_NOT_IMPLEMENTED` and `EXTERNAL_CONFIRMATION_REQUIRED`.
- Startup and continuous duplicate-block health-test candidates for internal engineering evaluation.
- OS-provider status-only operational probe and provider-disabled failure behavior.
- Process/fork marker with reseed-required behavior for source-backed random contexts.
- Sticky fatal-state transition on selected fatal health-test failures.
- Formal P2-05 CSP and sensitive-intermediate inventory covering module-owned HMAC/AES/CMAC table keys, caller-owned contexts, DRBG state, and temporary copies.
- Explicit destroy plus orderly-shutdown and locked-fatal cleanup for all live module-owned CSP table entries; `ZEROIZED` tombstones are invalid for use and may be explicitly destroyed.
- Caller-owned opaque contexts remain caller-managed through documented final/uninit paths; module-wide cleanup cannot locate abandoned contexts.
- Local zeroization compiler-output capture and review procedure; exact OE-01 human disposition remains `NOT_RUN`.
- Limited global locking and cross-platform thread/process seams.

## Test and Evidence Inventory

The ordinary tests-enabled build is expected to register 60 CTest cases when OpenSSL 3.x differential tests are available, including P2-03 through P2-07 policy/operational checks. Coverage includes:

- startup/extended KATs and API-negative tests;
- module state and cryptographic service gating;
- DRBG boundaries, reseed, entropy-helper, and failure cleanup;
- entropy adapter/provider/health/process behavior;
- AES-CBC and AES-CMAC hardening, lifecycle, concurrency, and differential tests;
- SHA-512-family representation/alignment/endian review inputs;
- CSP exhaustion, stale handle, key lifecycle, thread-safety, orderly-shutdown zeroization, fatal-transition zeroization, and status-only zeroization policy checks;
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
| GAP-006 | Finite-state model | `READY_FOR_INTERNAL_REVIEW` | `KCMVP_FINITE_STATE_MODEL.md` freezes states, transitions, service availability, fatal latch, orderly reinitialization, and implementation differences. | Obtain external interpretation and implement the separately tracked on-demand transition/concurrency deltas. |
| GAP-007 | Startup algorithm KATs | `READY_FOR_INTERNAL_REVIEW` | Startup KAT implementation, structural checks, sequence specification, and external extended KAT tests exist. | Confirm accepted startup order and required submitted-service mapping; retain exact OE-01 execution. |
| GAP-008 | Module integrity test | `PARTIAL` | P2-03 selects and specifies `INT-HMAC-SHA256-SINGLE-SO-R1`, including canonical measurement, key ownership, deployment, failure, and evidence rules. Current runtime still has only placeholder/structural checks. | Obtain external confirmation, then implement controlled tag/key injection and runtime verification with mutation/truncation/path/concurrency tests. |
| GAP-009 | Conditional tests | `PARTIAL` | P2-04 freezes OS-provider versus direct-source responsibility and keeps provider operational checks distinct from raw-noise health tests. | Institution confirmation of formal conditional-test classification and triggers remains required. |
| GAP-010 | Fatal/recoverable failure policy | `READY_FOR_INTERNAL_REVIEW` | P2-03 failure/recovery matrix freezes fatal versus operation-local handling, sticky error behavior, allowed queries, process-termination recovery, and orderly reinitialization. | Reconcile remaining implementation deltas and obtain external confirmation of operator/recovery wording. |
| GAP-011 | Entropy-source responsibility | `EXTERNAL_CONFIRMATION_REQUIRED` | `EDP-P2-04-R1` selects `ENT-OE01-LINUX-GETRANDOM-R1`, classifies it as an external conditioned provider, and excludes a direct noise source from the initial candidate. | Confirm provider acceptance, entropy credit, external health ownership, and required OE evidence. |
| GAP-012 | Health-test interpretation | `PARTIAL` | P2-04 forbids applying the generic duplicate-block scaffolding to conditioned `getrandom()` output and reserves source-specific tests for any future owned direct source. | Confirm that external provider health ownership is acceptable; a future direct source needs a separate assessment and thresholds. |
| GAP-013 | DRBG limits and lifecycle | `READY_FOR_INTERNAL_REVIEW` | P2-04 freezes input/request lengths, `2^48` implementation interval, unsupported prediction resistance, process-change reseed, and VM/suspend limitations with policy/test traceability. | Confirm entropy credit, profile limits, and any stricter operational reseed rule externally. |
| GAP-014 | Fork/process behavior | `READY_FOR_INTERNAL_REVIEW` | Process-marker change rejects generation until reseed; unsupported marker branches fail closed and output is cleared. | Retain exact OE-01 execution and confirm accepted process-model wording. |
| GAP-015 | VM clone/snapshot and suspend/resume | `NOT_IMPLEMENTED` | P2-04 freezes explicit unsupported wording: duplicated/restored state must be discarded and independently reinitialized; no automatic detection is claimed. | Confirm operator guidance and whether deployment restrictions or extra evidence are required. |
| GAP-016 | CSP inventory and lifecycle | `READY_FOR_INTERNAL_REVIEW` | `CZP-P2-05-R1`, the formal inventory, service/CSP matrix, and current lifecycle design distinguish module-owned table keys, caller contexts, DRBG state, sensitive intermediates, purpose restrictions, and excluded persistence/export/wrapping. | Obtain institution confirmation of CSP classification, integrity-key treatment, and caller/module ownership wording. |
| GAP-017 | Zeroization evidence | `PARTIAL` | Explicit destroy, orderly-shutdown, and locked-fatal table cleanup are implemented and regression-tested; caller-owned cleanup boundaries and local compiler capture are documented. | Execute/review exact OE-01 output, confirm accepted evidence method, define OS/process operator controls, and resolve lock-failure cleanup expectations. |
| GAP-018 | Persistent/exported keys | `OUT_OF_SCOPE` | Public export/persistence/wrapping are rejected or absent. | Keep excluded unless a separate product and validation scope gate approves them. |
| GAP-019 | Thread-safety model | `PARTIAL` | OE-01 freezes thread safety ON with the POSIX/pthread-compatible branch; module/global state is serialized while caller-owned mutable contexts require caller serialization. | Retain OE-01 target results and formalize role/service concurrency wording. |
| GAP-020 | Secure coding review | `PARTIAL` | P2-06 source/local compiler review, controlled findings register, strict-warning/static-analysis records, hardening overlay, and ELF capture exist. | Exact OE-01 GCC 13.3.0 review, named disposition, approved timing/cache methodology, and specialist review remain open. |
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

## P2-01 through P2-05 Disposition

P2-01 consolidated the implementation/test/evidence inventory, traceability, consultation questions, submission inventory, roadmap, blocker record, ABI check, and local regression records.

P2-02 freezes the internal candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` and its shared-library boundary/configuration. This closes the internal selection action for GAP-001 and narrows GAP-003, GAP-015, and GAP-019, but it does not establish formal boundary or environment acceptance. Immutable selected-environment evidence is still incomplete, and P1-34 remains `BLOCKED_EXTERNAL`.

All institution-dependent items remain `EXTERNAL_CONFIRMATION_REQUIRED` until a named institution or qualified reviewer supplies a retained response.


P2-03 additionally froze the internal self-test/integrity/state/recovery design without claiming implementation or acceptance of module integrity. The machine-readable policy and checker require `SELECTED_CANDIDATE_NOT_IMPLEMENTED`, `EXTERNAL_CONFIRMATION_REQUIRED`, 14 headers, 69 symbols, `ANOCRYPTO_C_0`, hosted `NOT_RUN/UNREVIEWED`, and external authorization `NO`.

P2-04 freezes `EDP-P2-04-R1` and `ENT-OE01-LINUX-GETRANDOM-R1` as internal policy only. It closes responsibility/limit documentation and local evidence wiring, but leaves provider acceptance, entropy credit, health-test allocation, exact OE-01 execution, and formal conditional-test interpretation externally open.

P2-07 freezes `SDP-P2-07-R1` and creates the controlled internal submission-document set without changing public ABI. Official form conformity, selected security level, named approval, secure delivery, exact OE-01 evidence, runtime integrity, institution results, and P1-34 hosted evidence remain open. P2-08 is next.

## External Reference Checkpoint

The following official-source checkpoints informed document categorization only:

- KISA, “암호모듈 검증” process overview and references to KS X ISO/IEC 19790 and KS X ISO/IEC 24759, checked 2026-07-14.
- KISA, public announcement for the revised “암호모듈 제출물 작성 안내서 (2025.9.)”, checked 2026-07-14; exact attachment contents and applicability remain subject to controlled review.

Exact current templates, accepted interpretations, security level, test methods, and submission package requirements remain `EXTERNAL_CONFIRMATION_REQUIRED`.
