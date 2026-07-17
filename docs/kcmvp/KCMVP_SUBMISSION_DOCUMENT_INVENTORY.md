# KCMVP Submission Document Inventory

Status: P2-09 controlled internal skeleton, pre-consultation routing, and closeout inventory. Boundary/OE, self-test/integrity, entropy/DRBG, CSP/zeroization, and secure-coding/side-channel records are available as engineering inputs; runtime integrity implementation, exact current templates, selected-OE/external dispositions, and accepted contents remain pending or `EXTERNAL_CONFIRMATION_REQUIRED`.

## Inventory Categories

| Category | Meaning |
|---|---|
| `EXISTING_ENGINEERING_INPUT` | Useful current repository material, not yet a controlled submission document. |
| `UPDATE_AND_CONSOLIDATE` | Existing content must be reconciled into one controlled submission-facing document. |
| `CREATE_NEW` | No adequate controlled document currently exists. |
| `AFTER_CONSULTATION` | Content/template depends on institution response or selected environment. |
| `NOT_IN_SUBMISSION_PACKAGE` | Internal/test/customer artifact that should remain outside unless specifically requested through a protected channel. |

KISA public guidance identifies basic design, detailed design, configuration-management, and test documents as core submission-document categories for software modules. Exact current forms, versions, security-level requirements, and package staging must be confirmed before any external submission or P2-08 disposition.

## A. Basic Design / Module Specification

| ID | Proposed controlled document | Current source inputs | State | P2 owner/action |
|---|---|---|---|---|
| DOC-BD-001 | Module overview and product boundary | `README.md`; product-layer boundaries; `SERVICE_SCOPE_MATRIX.md` | `UPDATE_AND_CONSOLIDATE` | P2-07 basic design skeleton. |
| DOC-BD-002 | Physical/logical cryptographic boundary | `KCMVP_MODULE_BOUNDARY_SPEC.md`; candidate configuration record | `EXISTING_ENGINEERING_INPUT` | Reconcile into controlled basic design after institution confirmation. |
| DOC-BD-003 | Security level and requirement applicability | none accepted | `AFTER_CONSULTATION` | Record institution-confirmed level and applicability table. |
| DOC-BD-004 | Operating environment and configuration variants | `KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`; candidate configuration record; portability/CI docs | `EXISTING_ENGINEERING_INPUT` | Add immutable OE-01 execution records and institution-confirmed variant rules. |
| DOC-BD-005 | Roles, services, and service availability by state | public headers; service matrix; `KCMVP_FINITE_STATE_MODEL.md`; failure/recovery matrix | `EXISTING_ENGINEERING_INPUT` | Reconcile P2-03 state/service tables into controlled basic design after consultation. |
| DOC-BD-006 | Approved algorithm/service scope | `KCMVP_ALGORITHM_SCOPE.md` | `AFTER_CONSULTATION` | Confirm target algorithms/configuration; retain unsupported placeholders separately. |
| DOC-BD-007 | Ports and interfaces | 14 public headers; install/package docs | `UPDATE_AND_CONSOLIDATE` | Classify data/control/status interfaces and caller responsibilities. |
| DOC-BD-008 | Finite-state model | `module_state.c`; `KCMVP_FINITE_STATE_MODEL.md`; failure/recovery matrix | `EXISTING_ENGINEERING_INPUT` | Resolve tracked implementation deltas and convert to controlled submission form. |
| DOC-BD-009 | Cryptographic key/CSP overview | P2-05 CSP inventory, lifecycle, service matrix, key/CSP headers | `EXISTING_INTERNAL_DRAFT` | Convert the frozen internal inventory after formal classification feedback. |
| DOC-BD-010 | Self-test and integrity overview | `KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md`; KAT plans; P2-03 policy record | `EXISTING_ENGINEERING_INPUT` | Candidate selected but not implemented; revise after institution feedback and implementation. |

## B. Detailed Design

| ID | Proposed controlled document | Current source inputs | State | P2 owner/action |
|---|---|---|---|---|
| DOC-DD-001 | Module architecture and component decomposition | `src/*` layout; architecture docs | `UPDATE_AND_CONSOLIDATE` | P2-07 detailed design skeleton. |
| DOC-DD-002 | Module lifecycle/error-state design | `module_state.c`; module tests; finite-state and failure/recovery matrices | `EXISTING_ENGINEERING_INPUT` | Reconcile implementation deltas and external recovery interpretation. |
| DOC-DD-003 | Self-test, conditional-test, and integrity design | `selftest.c`; KAT manifest; P2-03 specification/policy/delta register | `EXISTING_ENGINEERING_INPUT` | Integrity design is selected but not implemented; P2-04 internal responsibility is frozen; formal conditional-test classification remains external. |
| DOC-DD-004 | SHA-2/HMAC design and API behavior | hash/HMAC source and API docs | `UPDATE_AND_CONSOLIDATE` | Map algorithm operations, buffers, state, and failure cleanup. |
| DOC-DD-005 | HMAC_DRBG and random-service design | DRBG/random/entropy docs | `UPDATE_AND_CONSOLIDATE` | P2-04 internal consolidated design exists; convert after consultation. |
| DOC-DD-006 | Entropy source/provider and health-test design | entropy adapter/operational/health docs | `AFTER_CONSULTATION` | Final ownership/tests only after source/environment confirmation. |
| DOC-DD-007 | AES/CBC/CMAC design and API behavior | AES/CBC/CMAC design/hardening docs | `UPDATE_AND_CONSOLIDATE` | Consolidate without expanding service scope. |
| DOC-DD-008 | CSP lifecycle and zeroization design | P2-05 inventory, zeroization evidence, service matrix, source and policy | `EXISTING_INTERNAL_DRAFT` | Reconcile exact OE-01 and institution evidence expectations before controlled issue. |
| DOC-DD-009 | Thread/process model | P2-02 boundary/OE matrix; thread-safety and random portability docs | `UPDATE_AND_CONSOLIDATE` | OE-01 thread/process assumptions frozen; P2-04 internal lifecycle wording is frozen; formal acceptance and exact OE evidence remain open. |
| DOC-DD-010 | Secure coding and side-channel analysis | P2-06 secure-coding review, side-channel review, findings register, compiler/CPU matrix, policy and capture | `EXISTING_INTERNAL_DRAFT` | Convert only with exact-OE and consultation dispositions retained as provisional/open. |
| DOC-DD-011 | Build/package/runtime dependency design | candidate record, CMake enforcement, boundary/OE matrix, install/PoC/provenance docs | `UPDATE_AND_CONSOLIDATE` | Convert P2-02 dependency/exclusion record into controlled detailed design. |

## C. Configuration Management

| ID | Proposed controlled document | Current source inputs | State | P2 owner/action |
|---|---|---|---|---|
| DOC-CM-001 | Configuration identification and baselines | release identity; ABI baseline; source hashes | `UPDATE_AND_CONSOLIDATE` | P2-07 controlled baseline table. |
| DOC-CM-002 | Source/change control workflow | development workflow; clean-room records | `UPDATE_AND_CONSOLIDATE` | Define roles, approvals, issue/change classification, and branch/tag rules. |
| DOC-CM-003 | Build environment and reproducibility procedure | P2-02 candidate record/CMake enforcement; reproducible-build notes | `UPDATE_AND_CONSOLIDATE` | Capture immutable OE-01 image/tool packages/commands/binary hashes and retained review. |
| DOC-CM-004 | Release creation, signing, timestamp, and authorization | release provenance/signing policies | `AFTER_CONSULTATION` | Integrate institution needs; production provisioning remains blocked. |
| DOC-CM-005 | Problem reporting and corrective action | no controlled process | `CREATE_NEW` | Define security issue intake, triage, fix, regression, and retest decision. |
| DOC-CM-006 | Document control and review history | fragmented Markdown history | `CREATE_NEW` | Add document IDs, versions, owners, reviewers, approvals, and change summaries. |
| DOC-CM-007 | Evidence/artifact retention and access control | evidence manifest policy | `UPDATE_AND_CONSOLIDATE` | Define retention, protected storage, hash verification, and disposal. |
| DOC-CM-008 | Third-party/component/license/vulnerability inventory policy | source-only SPDX and clean-room records | `AFTER_CONSULTATION` | Select scope with legal and institution input. |
| DOC-CM-009 | Maintenance/change-impact and retest policy | compatibility policy | `AFTER_CONSULTATION` | Define impact classes for source, compiler, OS, API, and security fixes. |
| DOC-CM-010 | Integrity metadata/key injection, custody, rotation, and destruction procedure | P2-03 integrity specification and policy record | `AFTER_CONSULTATION` | Create only after mechanism acceptance; no key material enters source, ordinary CI, logs, evidence, or PoC. |

## D. Test Document

| ID | Proposed controlled document | Current source inputs | State | P2 owner/action |
|---|---|---|---|---|
| DOC-TS-001 | Master test strategy and environment matrix | CI matrix; differential strategy; release evidence workflow | `UPDATE_AND_CONSOLIDATE` | P2-07 test document skeleton. |
| DOC-TS-002 | Requirement-to-test traceability | P2 evidence traceability matrix | `UPDATE_AND_CONSOLIDATE` | Convert institution-confirmed requirements into test IDs. |
| DOC-TS-003 | Startup/conditional/integrity test procedure | P2-03 specification, KAT plans, implementation/test delta | `AFTER_CONSULTATION` | Convert only after integrity acceptance and runtime implementation; do not simulate mutation/institution results. |
| DOC-TS-004 | Algorithm KAT/conformance procedure | KAT tests/plans and public vector sources | `UPDATE_AND_CONSOLIDATE` | Separate vendor engineering tests from institution results. |
| DOC-TS-005 | Negative, state, misuse, and failure-injection procedure | negative/unit/stress tests | `UPDATE_AND_CONSOLIDATE` | Define expected statuses and cleanup without sensitive values. |
| DOC-TS-006 | Entropy/DRBG test procedure | entropy/DRBG tests and policies | `AFTER_CONSULTATION` | Finalize provider/direct-source responsibilities and limits. |
| DOC-TS-007 | CSP/zeroization test procedure | P2-05 lifecycle/shutdown/fatal tests, policy review, compiler capture | `EXISTING_INTERNAL_DRAFT` | Add exact OE-01 named review and institution-approved test procedure. |
| DOC-TS-008 | Portability/concurrency/sanitizer procedure | current operational/stress tests | `UPDATE_AND_CONSOLIDATE` | Limit conclusions to selected environments. |
| DOC-TS-009 | Install/package/public-boundary procedure | install/PoC/public-surface tests | `UPDATE_AND_CONSOLIDATE` | Retain clean installed-package and exclusion results. |
| DOC-TS-010 | Test result and review disposition template | evidence templates and hosted review fields | `UPDATE_AND_CONSOLIDATE` | Add reviewer, date, commit, environment, hash, disposition, limitations. |
| DOC-TS-011 | Institution test result integration record | none | `AFTER_CONSULTATION` | Do not pre-fill or simulate institution results. |

## E. Operator, Installation, and Supporting Documents

| ID | Proposed controlled document | Current source inputs | State | P2 owner/action |
|---|---|---|---|---|
| DOC-OP-001 | Installation and build guide for submitted configuration | P2-02 OE command; SDK integration guide; install smoke | `UPDATE_AND_CONSOLIDATE` | Execute and retain exact OE-01 commands, artifact hashes, SONAME/imports, and install paths. |
| DOC-OP-002 | Crypto officer/operator guidance | README; finite-state model; failure/recovery matrix; self-test/integrity spec | `UPDATE_AND_CONSOLIDATE` | P2-07 controlled guidance using P2-03 startup/fatal/restart policy and later P2-04/P2-05 inputs. |
| DOC-OP-003 | Developer API/service guide | public headers; PoC example | `UPDATE_AND_CONSOLIDATE` | Include buffer/alias/state/status rules without internal details. |
| DOC-OP-004 | Secure operation/environment assumptions | P2-02 boundary/OE; P2-04 entropy/process/VM; P2-05 caller/OS zeroization limits | `UPDATE_AND_CONSOLIDATE` | Add core-dump, swap, hibernation, crash, debugger, forced-termination, and VM controls after consultation. |
| DOC-OP-005 | Vulnerability/security reporting guide | none controlled | `CREATE_NEW` | Define contact, supported branches, embargo and patch/retest handling. |
| DOC-SUP-001 | Algorithm and public-vector source register | clean-room source manifest | `EXISTING_ENGINEERING_INPUT` | Review and extract controlled references. |
| DOC-SUP-002 | Public ABI/symbol/header baseline | `abi/*`; `release/PUBLIC_HEADERS.sha256` | `EXISTING_ENGINEERING_INPUT` | Retain as supporting configuration evidence. |
| DOC-SUP-003 | Source/provenance/SBOM package | release evidence tooling | `EXISTING_ENGINEERING_INPUT` | Provide only if requested and through approved channel. |
| DOC-SUP-004 | Hosted platform review package | hosted manifest/index model | `AFTER_CONSULTATION` | Currently blocked; no actual artifacts exist. |
| DOC-SUP-005 | Signing/timestamp/authorization package | key-free handoff policy | `AFTER_CONSULTATION` | Production values and approvals remain unprovisioned. |

## F. Explicitly Excluded From Routine Submission/Customer Packages

| Item | Classification | Reason |
|---|---|---|
| OpenSSL differential provider and source | `NOT_IN_SUBMISSION_PACKAGE` | Test-only reference provider; not runtime dependency. |
| Internal entropy/random failure hooks | `NOT_IN_SUBMISSION_PACKAGE` | Dedicated tests only; disabled in Release/PoC. |
| Private source context layouts and internal headers | `NOT_IN_SUBMISSION_PACKAGE` | Not public ABI; disclose only if formally requested through protected process. |
| Compiler disassembly | `NOT_IN_SUBMISSION_PACKAGE` | Internal review input unless institution specifically requests it. |
| Synthetic hosted/tag fixtures | `NOT_IN_SUBMISSION_PACKAGE` | Policy regression only; not actual hosted or release evidence. |
| Production keys, tokens, signatures, timestamp requests/responses | `NOT_IN_SUBMISSION_PACKAGE` | Must remain outside source, CI, ordinary evidence, and PoC. |
| Integrity-test HMAC key and unprotected injection material | `NOT_IN_SUBMISSION_PACKAGE` | Controlled CSP/configuration material; never copied into ordinary documentation/evidence. |
| Customer data or internal deployment details | `NOT_IN_SUBMISSION_PACKAGE` | Outside validation evidence and prohibited by project policy. |

## External Submission Finalization Gate

P2-07 may complete the internal skeleton from reviewed P2-02 through P2-06 inputs. Do not mark the package `APPROVED_FOR_SUBMISSION` until pre-consultation or institution guidance has confirmed:

- current document templates/revisions;
- target security level and module form;
- external acceptance or required revision of OE-01 and its environment-variant granularity;
- integrity and entropy responsibilities;
- expected source/binary/test artifact package;
- confidentiality and delivery channel.
## P2-04 Entropy/DRBG controlled inputs

| Document / evidence | Current disposition | Submission action |
|---|---|---|
| `KCMVP_ENTROPY_AND_DRBG_SPEC.md` | `EXISTING_INTERNAL_DRAFT` | Convert to controlled detailed-design content only after provider/profile consultation. |
| `KCMVP_ENTROPY_RESPONSIBILITY_MATRIX.md` | `EXISTING_INTERNAL_DRAFT` | Use to assign module, OS provider, caller, operator, and institution responsibilities. |
| `KCMVP_ENTROPY_HEALTH_AND_RECOVERY_MATRIX.md` | `EXISTING_INTERNAL_DRAFT` | Reconcile formal conditional-test and fatal/recoverable classification after consultation. |
| `P2_04_IMPLEMENTATION_TEST_DELTA.md` | `ENGINEERING_CONTROL` | Track remediation and retained evidence; not an institution result. |
| `ANOCRYPTO_C_ENTROPY_DRBG_POLICY.cmake` and policy check | `ENGINEERING_CONTROL` | Preserve candidate configuration/claim drift checks. |
| `ENTROPY_DRBG_POLICY_REVIEW.txt` and policy check evidence | `LOCAL_ENGINEERING_ONLY` until exact OE-01 run | Retain identifiers/status/lengths only; never retain provider or generated bytes. |

Open submission dependencies remain provider acceptance, entropy credit, health-test ownership, exact OE-01 evidence, and institution-approved lifecycle terminology.


## P2-05 CSP/zeroization controlled inputs

| Document / evidence | Current disposition | Submission action |
|---|---|---|
| `KCMVP_CSP_INVENTORY.md` | `EXISTING_INTERNAL_DRAFT` | Confirm formal CSP classifications and integrity-key treatment before controlled issue. |
| `KCMVP_SERVICE_CSP_OPERATION_MATRIX.md` | `EXISTING_INTERNAL_DRAFT` | Convert ownership/lifetime/cleanup paths into detailed design and operator guidance. |
| `KCMVP_ZEROIZATION_EVIDENCE.md` | `EXISTING_INTERNAL_DRAFT` | Retain limitations; add exact OE-01 reviewer/hash/disposition and institution-approved method. |
| `P2_05_IMPLEMENTATION_TEST_DELTA.md` | `ENGINEERING_CONTROL` | Track shutdown/fatal remediation and unresolved lock/caller/OS boundaries; not an institution result. |
| `ANOCRYPTO_C_CSP_ZEROIZATION_POLICY.cmake` and checker | `ENGINEERING_CONTROL` | Preserve 14/69/ABI/blocker/claim drift controls. |
| shutdown/fatal/policy CTest logs | `LOCAL_ENGINEERING_ONLY` | Use as vendor engineering inputs; do not label as institution evidence. |
| `ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt` and disassembly | `LOCAL_ENGINEERING_ONLY` | Internal manual-review input; exact OE-01 named review remains `NOT_RUN`. |

Open dependencies remain formal CSP classification, accepted zeroization evidence, exact OE-01 review, operator controls for OS/process copies, integrity-key implementation, and lock-failure cleanup expectations.

## P2-06 Secure-coding/side-channel controlled inputs

| Document / evidence | Current disposition | Submission action |
|---|---|---|
| `KCMVP_SECURE_CODING_REVIEW.md`; `KCMVP_SECURITY_FINDINGS_REGISTER.md`; `P2_06_IMPLEMENTATION_TEST_DELTA.md` | `EXISTING_INTERNAL_DRAFT` | Convert findings only with target, reviewer, date, evidence hash, and disposition. |
| `KCMVP_SIDE_CHANNEL_REVIEW.md`; `KCMVP_COMPILER_CPU_REVIEW_MATRIX.md` | `EXISTING_INTERNAL_DRAFT` | Preserve source/local-compiler limitations; add exact OE-01 and any required empirical results after confirmation. |
| `ANOCRYPTO_C_SECURE_CODING_POLICY.cmake` and checker | `ENGINEERING_CONTROL` | Preserve hardening, ABI, claims, hosted, and release-state drift controls. |
| `SECURE_CODING_COMPILER_CPU_REVIEW.txt`; `ELF_HARDENING_REVIEW.txt` | `LOCAL_ENGINEERING_ONLY` | Do not treat as exact OE-01 or formal side-channel evidence. |
| `SECURE_CODING_POLICY_REVIEW.txt` | `LOCAL_ENGINEERING_ONLY` | Status-only fixed-tag-path review; no timing or leakage conclusion. |

Open dependencies remain exact OE-01 compiler/CPU review, named disposition, accepted timing/cache method if required, power/EM/fault applicability, runtime integrity, and institution acceptance.

## P2-07 Controlled Skeleton Mapping

The controlled internal set is under `docs/kcmvp/submission/` and uses policy `SDP-P2-07-R1`. Basic design, detailed design, configuration management, test, operator, traceability, reference, staging, document-control, package-index, and review-checklist files are now `CONTROLLED_INTERNAL_SKELETON`. Official forms, security level, exact OE-01 evidence, runtime integrity, institution results, secure delivery, signing, and authorization remain open.


## P2-08 Pre-consultation Mapping

The P2-08 rehearsal set is an internal control layer, not an additional institution submission category. It links the controlled P2-07 skeleton to the question pack, evidence walkthrough, 20 unresolved decisions, blank response records, and `ACT-*` change routing. External consultation remains `NOT_CONDUCTED`; no document is approved for submission by this mapping.
