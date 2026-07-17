# P2 Validation Readiness Roadmap

Status: P2-01 through P2-09 internal validation-readiness and closeout sequence completed locally. P2-10 assisted rehearsal is also complete under `LAR-P2-10-R1`; independent human rehearsal remains `NOT_RUN`, institution contact is conditionally blocked pending named authorization, external consultation is `NOT_CONDUCTED`, institution responses are `NOT_RECEIVED`, and formal validation is `NOT_STARTED`. P2 remains validation-readiness work, not automatic feature expansion.

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
| P2-04 | `FROZEN_INTERNAL_POLICY` | Close entropy/provider/DRBG responsibility and evidence plan. | Entropy/DRBG specification, responsibility/health matrices, machine-readable policy, operational review, and implementation/test delta. | No change. |
| P2-05 | `COMPLETE_LOCAL_INTERNAL_POLICY` | Close CSP inventory, lifecycle, and zeroization evidence. | Formal CSP inventory, service/operation matrix, zeroization evidence plan, internal shutdown/fatal remediation, compiler capture, and policy check. | No change. |
| P2-06 | `COMPLETE_LOCAL_WITH_OPEN_FINDINGS` | Perform secure-coding and target side-channel review. | Secure-coding record, side-channel limitations, findings register, compiler/CPU matrix, hardening capture, and policy tests. | No change. |
| P2-07 | `COMPLETE_LOCAL_INTERNAL_SKELETON` | Build controlled submission-document skeleton. | Basic design, detailed design, configuration-management, test, and operator-document skeletons. | No change. |
| P2-08 | `COMPLETE_LOCAL_SCRIPTED` | Rehearse pre-consultation evidence walkthrough and decision capture. | Scripted walkthrough, Priority-1 owner/recipient map, 20-entry decision register, response template, action-impact routing, and status-only package smoke. | No change. |
| P2-09 | `COMPLETE_LOCAL_CLOSEOUT` | Consolidate cumulative P2 disposition, claims, blockers, exit criteria, pack index, and next-work gate. | Closeout review, status summary, open-item/exit register, controlled 17-document closeout package, policy/package checks. | No change. |
| P2-10 | `COMPLETE_LOCAL_ASSISTED` | Rehearse Priority-1 project-position answers and freeze the institution-contact decision gate. | Assisted report/transcript, response scorecard, gap register, six-precondition contact gate, and status-only package. | No change. |

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

Status: `FROZEN_INTERNAL_POLICY`; formal institution acceptance and exact OE-01 retained execution remain `EXTERNAL_CONFIRMATION_REQUIRED`/`NOT_RUN`.

### Frozen internal policy

- policy ID: `EDP-P2-04-R1`;
- provider profile: `ENT-OE01-LINUX-GETRANDOM-R1`;
- selected provider: Linux `getrandom(flags=0)` through the internal adapter;
- source classification: external conditioned RBG; health-test owner: external provider;
- no project-owned direct noise source in the initial candidate;
- generic duplicate-block health scaffolding is not applied to conditioned OS output;
- source-backed instantiate requests 32-byte entropy and a separate 16-byte nonce; reseed requests 32 bytes;
- maximum output per request is 65,536 bytes; implementation reseed interval is `2^48`; prediction resistance is unsupported;
- process-marker change requires reseed; VM/container checkpoint duplication and suspend/resume are not automatically detected; known duplicated state must be discarded and independently reinitialized;
- evidence is status/length/identifier/hash only and excludes provider output, seed, state, and generated bytes.

### Deliverables

- `docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md`;
- `docs/kcmvp/KCMVP_ENTROPY_RESPONSIBILITY_MATRIX.md`;
- `docs/kcmvp/KCMVP_ENTROPY_HEALTH_AND_RECOVERY_MATRIX.md`;
- `docs/kcmvp/P2_04_IMPLEMENTATION_TEST_DELTA.md`;
- `release/ANOCRYPTO_C_ENTROPY_DRBG_POLICY.cmake`;
- `tools/check_entropy_drbg_policy.cmake`;
- `tests/operational/entropy_drbg_policy_review.c`;
- evidence records `ENTROPY_DRBG_POLICY_CHECK.txt`, `ENTROPY_DRBG_POLICY_REVIEW.txt`, and `ENTROPY_DRBG_REVIEW_NOTES.txt`.

### Completion criteria and disposition

- provider/source/health-test responsibility is explicit and machine checked;
- no generic engineering skeleton is represented as a finalized raw-noise health test;
- lifecycle limits, failure/recovery, fork/process, VM/container, and suspend assumptions are explicit;
- implementation/test gaps and institution questions are retained rather than promoted to PASS;
- local policy, CTest, install, PoC, compatibility, provenance, and evidence regressions pass;
- public surface remains 14 headers, 69 symbols, and `ANOCRYPTO_C_0`;
- P1-34 remains independently `BLOCKED_EXTERNAL`.

### Open external dependencies

- institution/entropy-specialist acceptance of OS-provider responsibility and supporting evidence;
- entropy-credit and separate entropy/nonce request interpretation;
- provider-failure fatality, stricter operational reseed policy, and conditional-test classification;
- exact OE-01 run and hosted review;
- any future direct-noise source requires a separate scope/design/assessment gate.

## P2-05 — CSP and Zeroization Evidence Closure

Status: `COMPLETE_LOCAL_INTERNAL_POLICY`; policy ID `CZP-P2-05-R1`. Formal CSP classification, accepted evidence method, and exact OE-01 compiler review remain external or `NOT_RUN`.

### Frozen internal policy and remediation

- conservatively enumerate module-owned table keys, caller-owned opaque contexts, DRBG Key/V state, and sensitive intermediates;
- distinguish module-owned storage from caller-owned memory and external OS/provider copies;
- purpose-separate HMAC, block-cipher, and CMAC handles; persistence, export, wrapping, platform keystore, and multi-process sharing remain outside the initial candidate;
- explicit destroy clears the full slot and marks it `DESTROYED`; orderly shutdown and locked fatal transition clear every live module-owned slot and mark it `ZEROIZED`;
- a `ZEROIZED` tombstone is invalid for cryptographic use and may be explicitly destroyed once; slot reuse receives a new generation;
- caller-owned contexts are not retained by the module and require their documented final/uninit paths; abandoned or forcibly terminated caller memory is not claimed cleared;
- the volatile-byte-store primitive and relevant call sites are captured for local compiler review, but exact OE-01 human disposition remains `NOT_RUN`;
- if global lock acquisition fails, fatal state is latched but safe table traversal is not attempted; the residual risk is deferred to P2-06/external review.

### Deliverables

- `docs/kcmvp/KCMVP_CSP_INVENTORY.md`;
- `docs/kcmvp/KCMVP_ZEROIZATION_EVIDENCE.md`;
- `docs/kcmvp/KCMVP_SERVICE_CSP_OPERATION_MATRIX.md`;
- `docs/kcmvp/P2_05_IMPLEMENTATION_TEST_DELTA.md`;
- `release/ANOCRYPTO_C_CSP_ZEROIZATION_POLICY.cmake`;
- `tools/check_csp_zeroization_policy.cmake`;
- `tools/capture_zeroization_compiler_review.cmake`;
- shutdown/fatal zeroization regressions and status-only operational review.

### Completion disposition

- all currently implemented CSPs and sensitive intermediates have owners, lifetimes, cleanup paths, limitations, and evidence references;
- module-owned shutdown/fatal cleanup gap is remediated without public declaration or symbol growth;
- local policy, CTest, clean ABI, install, PoC, compatibility, compiler capture, provenance, and evidence checks are required for the final artifact;
- public surface remains 14 headers, 69 symbols, and `ANOCRYPTO_C_0`;
- formal classification, exact OE-01 compiler/CPU review, OS/process memory controls, and integrity-key realization remain open.

### External dependencies

- accepted CSP classification and zeroization-evidence expectations;
- exact OE-01 GCC 13.3.0 build and named human disposition;
- operator controls for core dumps, swap, hibernation, crash telemetry, debugger access, and VM snapshots;
- decision on emergency cleanup expectations when locking or process integrity is already compromised.

## P2-06 — Secure Coding and Side-channel Review

Status: `COMPLETE_LOCAL_WITH_OPEN_FINDINGS`; formal/selected-OE disposition remains pending.

### Completed outputs

- policy ID `SCP-P2-06-R1` and build-hardening profile `SCB-P2-06-LINUX-ELF-R1`;
- controlled source-review record, side-channel limitations, findings register, and compiler/CPU review matrix;
- local GCC analyzer and Clang strict/static-analysis review with a documented false positive and one fixed internal enum-switch gap;
- Linux GNU/Clang Release overlay using FORTIFY 3, strong stack protector, stack-clash protection, full RELRO/BIND_NOW, and non-executable stack;
- ELF hardening/library-hash capture, source-policy check, and status-only CMAC verify operational review;
- explicit prohibition on universal constant-time, side-channel, power/EM, fault-resistance, audit, validation, or hosted-review claims;
- unchanged 14-header, 69-symbol `ANOCRYPTO_C_0` public boundary.

### Open findings and dependencies

- exact immutable OE-01 GCC 13.3.0 compiler/disassembly/hardening capture and named reviewer/date/disposition are `NOT_RUN`;
- approved statistical timing/cache methodology and execution remain `EXTERNAL_CONFIRMATION_REQUIRED`;
- power, EM, transient-execution, and active-fault work remains `NOT_EVALUATED`;
- runtime module integrity remains unimplemented and CSP cleanup after lock acquisition failure remains externally reviewable;
- P1-34 actual hosted evidence remains independently `BLOCKED_EXTERNAL`.

### Exit disposition

P2-06 is complete as an internal engineering review and backlog consolidation step. It does not close target-specific or institution-dependent findings. P2-07 may use the retained P2-06 records while preserving their provisional status.

## P2-07 — Submission-document Skeleton

Status: `COMPLETE_LOCAL_INTERNAL_SKELETON`; policy `SDP-P2-07-R1`. Formal template conformity and submission authorization remain external/unapproved.

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

- controlled skeleton files have document IDs, revisions, owners, reviewer/approval placeholders, status, and cross-references;
- all provisional sections are marked clearly;
- engineering evidence is distinguished from institution results;
- internal/test/customer-only artifacts are excluded from the S0 staging manifest;
- policy checks preserve 14 headers, 69 symbols, `ANOCRYPTO_C_0`, P1-34 blocker state, `UNSIGNED/BLOCKED/NO`, and runtime-integrity `NOT_IMPLEMENTED` status.

Completion record: `SDP-P2-07-R1` and the controlled internal skeleton set are complete locally. Official forms, security level, secure delivery, exact package contents, and submission authorization remain open.

### External dependencies

- current forms/revision acceptance, security level, secure delivery channel, and package-content guidance;
- exact OE-01 and institution-result integration after P2-08 decisions.

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
- answer capture and change-impact routing are controlled;
- the next implementation/document step is selected only from retained answers or explicit authorization.

Completion record: `PCR-P2-08-R1` is complete as a scripted local rehearsal. Priority-1 agendas: 10; unresolved decisions: 20; external consultation: `NOT_CONDUCTED`; institution responses: `NOT_RECEIVED`; live independent internal rehearsal: `NOT_RUN`.

## P2-09 — Cumulative Closeout Review

### Deliverables

- cumulative P2-01 through P2-08 disposition and go/no-go review;
- claims/status summary;
- open-item and exit-criteria register;
- controlled pre-consultation closeout pack index and allowlist;
- next-work authority/selection gate;
- policy and package smoke checks.

Completion record: `P2C-P2-09-R1` is `COMPLETE_LOCAL_CLOSEOUT`. The status-only pack is ready for internal review. External consultation remains `NOT_CONDUCTED`; institution responses remain `NOT_RECEIVED`; formal validation remains `NOT_STARTED`; submission, next implementation, and external release authorization remain `NO`.

### Post-P2-09 gate

The next numbered work item must be selected from retained institution answers or an explicitly authorized independent blocker-remediation lane. Runtime integrity implementation, exact OE-01 execution, side-channel specialist work, official document conversion, signing, submission, or release do not begin automatically. See `P2_CLOSEOUT_AND_NEXT_WORK_GATE.md`.

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
## P2-10 — Assisted Pre-consultation Rehearsal

Status: `COMPLETE_LOCAL_ASSISTED`; independent human rehearsal is `NOT_RUN` and external consultation is `NOT_CONDUCTED`.

### Deliverables

- assisted rehearsal report and compact project-position transcript;
- ten-agenda response scorecard;
- nine-entry rehearsal gap/remediation register;
- six-precondition institution-contact decision gate;
- machine-readable policy and status-only package smoke.

### Exit rule

P2-10 authorizes preparation for a named human internal rehearsal only. Institution contact requires named authorization and all six retained preconditions. It does not authorize implementation, formal submission, signing, timestamping, publication, or release.
