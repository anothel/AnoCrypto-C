# P2 Validation Readiness Roadmap

Status: P2-03 internal self-test, integrity-candidate, finite-state, and recovery design freeze completed; P2-04 is next. P2 remains validation-readiness work, not automatic feature expansion.

## Fixed P2 Constraints

- Preserve C99, 14 public headers, 69 exported symbols, and `ANOCRYPTO_C_0` unless a separate explicit API/ABI gate is approved.
- Do not add algorithms merely to extend scope.
- Do not represent local, synthetic, or workflow-definition results as actual hosted execution.
- Keep P1-34 `BLOCKED_EXTERNAL` until actual artifacts are produced and reviewed.
- Do not push, tag, production-sign, timestamp, or externally release without explicit authorization.
- Mark unresolved institution-dependent decisions `EXTERNAL_CONFIRMATION_REQUIRED`.
- Normally use one numbered work item per commit.

## Sequence Overview

| Step | Status | Goal | Primary output | ABI expectation |
|---|---|---|---|---|
| P2-01 | `COMPLETE_LOCAL` | Consolidate gaps, traceability, consultation, submission inventory, and roadmap. | Current P2 pack and blocker record. | No change. |
| P2-02 | `FROZEN_INTERNAL_CANDIDATE` | Freeze candidate module boundary and one operating environment. | Boundary/environment specification, machine-readable candidate record, and fail-closed CMake/policy checks. | No change. |
| P2-03 | `FROZEN_INTERNAL_DESIGN` | Finalize self-test, integrity-candidate, finite-state, and recovery specification. | Normative design, machine-readable policy, fail-closed policy check, and implementation/test delta. | No public change; runtime integrity and on-demand transition remain unimplemented. |
| P2-04 | `NEXT` | Close entropy/provider/DRBG responsibility and evidence plan. | Entropy/DRBG specification, limits, environment policy, evidence mapping. | No change by default. |
| P2-05 | `PLANNED` | Close CSP inventory, lifecycle, and zeroization evidence. | Formal CSP and zeroization register. | No change. |
| P2-06 | `PLANNED` | Perform secure-coding and target side-channel review. | Findings register, compiler/CPU dispositions, remediation plan. | No change unless a separately approved fix requires it. |
| P2-07 | `PLANNED` | Build controlled submission-document skeleton. | Basic design, detailed design, configuration-management, test, and operator-document skeletons. | No change. |
| P2-08 | `PLANNED` | Rehearse pre-consultation evidence walkthrough and decision capture. | Mock walkthrough, prioritized questions, unresolved decision register, feedback process. | No change. |

## P2-01 — Readiness Gap Consolidation

### Deliverables

- readiness inventory and gap register;
- requirement-to-code-to-test-to-evidence matrix;
- pre-consultation questions;
- submission-document inventory;
- P2-02 through P2-08 roadmap;
- P1-34 external blocker record;
- public ABI no-change check and local regression record.

### Completion criteria

- all six minimum P2-01 documents exist and cross-reference each other;
- gaps use the agreed status taxonomy;
- no external-confirmation item is represented as resolved;
- P1-34 remains `NOT_RUN`/`UNREVIEWED`;
- local CTest, policy, install, PoC, compatibility, and evidence targets pass;
- patch/ZIP/hash artifacts reproduce the reviewed tree.

Completion record: P2-01 local engineering checks and artifacts were completed without changing public ABI. P1-34 remained blocked and was not substituted.

## P2-02 — Module Boundary and Operating Environment Freeze

Status: `FROZEN_INTERNAL_CANDIDATE`; formal acceptability remains `EXTERNAL_CONFIRMATION_REQUIRED`.

### Frozen decision

- candidate ID: `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`;
- one software shared object: `libanocrypto_c.so.0.1.0`, SONAME `libanocrypto_c.so.0`;
- Ubuntu Server 24.04.4 LTS, x86_64 little-endian, ELF/System V AMD64 ABI;
- GCC 13.3.0, CMake 3.28.3, Unix Makefiles, Release;
- generic x86-64, LTO OFF, `-march=native` forbidden;
- thread safety and Linux OS entropy provider ON;
- tests, examples, internal hooks, OpenSSL differential code, signing fixtures, and sanitizers OFF;
- static linkage and all other OS/architecture/compiler configurations deferred or unsupported for the initial candidate.

### Deliverables

- `docs/kcmvp/KCMVP_MODULE_BOUNDARY_SPEC.md`;
- `docs/kcmvp/KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`;
- `release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake`;
- `tools/check_validation_candidate_policy.cmake`;
- CMake fail-closed enforcement option `ANO_CRYPTO_ENFORCE_VALIDATION_CANDIDATE`;
- evidence integration through `VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt`.

### Completion criteria and disposition

- shared linkage is fixed for the initial candidate;
- included code/state, excluded tooling/product layers, external OS dependencies, interfaces, data flows, and operator assumptions are identified;
- one exact OS/CPU/compiler/build family has an owner and evidence plan;
- non-selected environments are explicitly `DEFERRED`, `UNSUPPORTED_INITIAL_CANDIDATE`, or `BLOCKED_EXTERNAL`;
- OpenSSL, test hooks, evidence/provenance/signing tools, and customer PoC remain outside runtime boundary;
- 14 public headers, 69 exported symbols, and `ANOCRYPTO_C_0` remain unchanged;
- candidate policy and ordinary local regressions pass;
- immutable image hash, exact CPU/kernel/package capture, selected-candidate retained execution, and institution acceptance remain open.

### External dependencies

- confirmation of shared-library boundary treatment, intended security level, environment-variant granularity, OS entropy-provider responsibility, and integrity-mechanism expectations;
- actual hosted evidence remains separately blocked by P1-34 and is not closed by the P2-02 local run.

## P2-03 — Self-test, Integrity, and Error-state Specification

Status: `FROZEN_INTERNAL_DESIGN`; the selected module-integrity mechanism is `SELECTED_CANDIDATE_NOT_IMPLEMENTED` and remains `EXTERNAL_CONFIRMATION_REQUIRED`.

### Frozen internal design

- self-test policy ID: `STP-P2-03-R1`;
- finite-state policy ID: `FSM-P2-03-R1`;
- integrity candidate: `INT-HMAC-SHA256-SINGLE-SO-R1`;
- startup order: candidate integrity authentication, structural checks, then SHA-2/HMAC/AES-CBC/AES-CMAC/HMAC_DRBG KATs;
- cryptographic services remain blocked while `SELF_TEST_RUNNING`;
- fatal self-test or integrity failure enters sticky `ERROR_STATE`;
- fatal recovery requires process termination and a fresh module load;
- orderly `SHUTDOWN` may be followed by a full reinitialization and complete startup sequence;
- the existing public on-demand self-test symbol is retained, but the target state-transition behavior is a separately tracked implementation delta;
- entropy-provider and direct-noise-source conditional-test responsibility remains owned by P2-04.

### Deliverables

- `docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md`;
- `docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md`;
- `docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md`;
- `docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md`;
- `release/ANOCRYPTO_C_SELF_TEST_POLICY.cmake`;
- `tools/check_self_test_integrity_policy.cmake`;
- evidence integration through `SELF_TEST_INTEGRITY_POLICY_CHECK.txt`;
- orderly shutdown/reinitialization regression coverage.

### Completion criteria and disposition

- the legacy integrity placeholder is explicitly separated from a real module-byte integrity test;
- the selected HMAC-SHA-256 single-shared-object candidate documents canonical measurement, key ownership, deployment, evidence, and limitations;
- the finite-state and failure/recovery matrices define service availability and operator action;
- code/design differences have unique implementation and test IDs rather than being represented as completed behavior;
- public headers remain 14, exported symbols remain 69, and ABI line remains `ANOCRYPTO_C_0`;
- local policy and regression evidence may pass without advancing hosted, signing, or validation status.

### Open implementation and external dependencies

- obtain institution feedback on the selected integrity mechanism, embedded verification material, canonicalization, startup ordering, and conditional-test classification;
- implement the verifier, controlled tag/key injection, and target on-demand state transitions only after the design gate is accepted;
- add retained tests for modified/truncated/unreadable/wrong-candidate shared objects and concurrency/re-entrancy;
- enter the integrity-test key and temporary state into the P2-05 CSP inventory;
- retain actual execution on the exact P2-02 operating environment;
- keep P1-34 hosted review independently `BLOCKED_EXTERNAL`.

## P2-04 — Entropy and DRBG Evidence Closure

### Objectives

- freeze each environment's entropy provider and responsibility boundary;
- decide whether any direct noise source is in scope;
- finalize instantiate/reseed/generate limits and prediction-resistance policy;
- define fork/process/VM/container/suspend behavior;
- consolidate status-only evidence and non-disclosure rules.

### Required documents

- `docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md`;
- provider/environment responsibility matrix;
- health-test applicability and recovery matrix;
- DRBG limit/test traceability update.

### Completion criteria

- no generic health-test skeleton is represented as a finalized direct-source test;
- every selected environment has a provider evidence path;
- unsupported lifecycle scenarios are explicit;
- public API remains unchanged unless a separate gate approves otherwise.

### External dependencies

- institution/entropy-specialist confirmation of OS-provider and direct-source expectations.

## P2-05 — CSP and Zeroization Evidence Closure

### Objectives

- enumerate all CSPs and sensitive intermediates;
- record creation, storage, use, duplication, restriction, destruction, and zeroization paths;
- define key-purpose and handle-state rules;
- retain compiler/optimization and process/OS boundary limitations.

### Required documents

- `docs/kcmvp/KCMVP_CSP_INVENTORY.md`;
- `docs/kcmvp/KCMVP_ZEROIZATION_EVIDENCE.md`;
- service/CSP/operation matrix;
- target/compiler review plan and findings.

### Completion criteria

- every CSP has a code path, owner, lifetime, zeroization path, and evidence reference;
- export/persistence/wrapping remain explicitly outside scope;
- zeroization wording is limited to what code and selected environments support;
- required remediation is tracked without silent ABI growth.

### External dependencies

- accepted CSP classification and zeroization evidence expectations.

## P2-06 — Secure Coding and Side-channel Review

### Objectives

- run a controlled secure-coding review over the frozen scope;
- review warnings, sanitizers, integer/length/alias/failure paths, and sensitive-output rules;
- perform target/compiler/CPU-specific review of AES, CMAC compare, HMAC/DRBG, and zeroization output;
- maintain a findings and limitations register.

### Required documents

- `docs/security/SECURE_CODING_REVIEW_P2.md`;
- `docs/security/SIDE_CHANNEL_REVIEW_FINDINGS_P2.md`;
- compiler/target disposition records;
- remediation and external-review backlog.

### Completion criteria

- findings have severity, evidence, owner, disposition, and regression requirement;
- unresolved timing/cache/fault items are not hidden or overstated;
- any code fix is separately reviewed and retested;
- no broad leakage-resistance claim is introduced.

### External dependencies

- selected targets from P2-02 and, where needed, independent specialist review.

## P2-07 — Submission-document Skeleton

### Objectives

- create controlled basic design, detailed design, configuration-management, test, and operator-document structures;
- populate only reviewed P2-02 through P2-06 content;
- add document control, change history, ownership, and approval fields;
- define submission package staging and protected-material handling.

### Required documents

At minimum:

- module basic design;
- module detailed design;
- configuration-management plan;
- test plan/procedure/result templates;
- installation/operator guidance;
- requirement/traceability appendix;
- controlled source/reference register.

### Completion criteria

- current institution templates/revisions are confirmed;
- all provisional sections are marked clearly;
- engineering evidence is distinguished from institution results;
- internal/test/customer-only artifacts are not copied into the submission package by default.

### External dependencies

- current forms, secure delivery channel, and package-content guidance.

## P2-08 — Pre-consultation Rehearsal

### Objectives

- walk through boundary, state, entropy, CSP, tests, and evidence as a reviewer would;
- prioritize questions that block design versus questions that only affect packaging;
- record unresolved decisions and expected cost/schedule impact;
- define how institution feedback becomes controlled project decisions.

### Required documents

- `docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md`;
- evidence walkthrough script and checklist;
- decision register and answer-capture template;
- post-consultation action/impact matrix.

### Completion criteria

- every high-priority question has an owner and intended recipient;
- sample evidence can be shown without exposing prohibited material;
- no synthetic/local record is presented as actual hosted or institution evidence;
- next implementation/document step is selected from retained answers.

## Parallel Lane — P1-34 Actual Hosted Artifact Review

P1-34 remains separate and incomplete:

```text
Status: BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING
Run: 29256531912
Baseline commit: cf0f3c425dd265c7509fb64a6123db97f71055dd
Jobs reaching runner: 0
Artifacts: 0
Actual hosted lanes: NOT_RUN
Actual hosted review: UNREVIEWED
```

Resume only after billing is resolved. Then run at a known commit, download and hash actual artifacts, verify platform/compiler/ref identity, perform named human review, and retain date/disposition/limitations. Local P2 verification never closes P1-34.

## Deferred Feature Gate

The following remain deferred unless a separate customer/validation scope decision approves them:

- AES-CTR, GCM, GMAC, generic AEAD, padding helpers, and tag truncation;
- KDF public APIs and public-key algorithms;
- key export, persistence, wrapping, multi-process sharing, and platform keystore;
- hardware acceleration/dispatch;
- C++ convenience wrapper work ahead of readiness closure.
