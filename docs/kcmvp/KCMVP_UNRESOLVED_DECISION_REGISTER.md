# KCMVP Unresolved Decision Register

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-DR-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_REHEARSAL` |
| Owner role | Validation program owner |
| Reviewer | `TODO` |
| Approval | `NO` |
| Policy ID | `PCR-P2-08-R1` |

| Decision ID | Linked question IDs | Decision owner | Intended recipient | Current state | Principal impact if changed | Required next action |
|---|---|---|---|---|---|---|
| DEC-001 | `PCQ-001`, `PCQ-002` | Module architect | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Boundary, build variants, evidence duplication. | Retain accepted boundary/variant interpretation before runtime integrity implementation. |
| DEC-002 | `PCQ-003`, `PCQ-003A`, `PCQ-003B`, `PCQ-031` | Operating-environment owner | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Number of OEs, hosted lanes, compiler reviews, maintenance cost. | Record exact OS/kernel/libc/CPU/compiler/VM granularity and retest triggers. |
| DEC-003 | `PCQ-006`, `PCQ-040` | Validation program owner | Testing and validation institutions | `EXTERNAL_CONFIRMATION_REQUIRED` | Required mechanisms, documents, testing depth, schedule. | Record target security level and authority split. |
| DEC-004 | `PCQ-004`, `PCQ-005`, `PCQ-007` | Product/module architect | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Submitted algorithm/service list and unsupported identifiers. | Confirm narrow scope and layer separation without API expansion. |
| DEC-005 | `PCQ-008`, `PCQ-011`, `PCQ-012`, `PCQ-013` | Module architect | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | State machine, on-demand self-test implementation, service gate, recovery. | Record accepted startup/on-demand/error-state behavior. |
| DEC-006 | `PCQ-009`, `PCQ-010`, `PCQ-013B` | Security architect and configuration manager | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Runtime integrity implementation, build injection, integrity-key custody. | Obtain mechanism and key-management disposition before code implementation. |
| DEC-007 | `PCQ-013A`, `PCQ-020A` | Self-test/entropy owner | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Conditional-test classification and test documentation. | Map each event to startup, on-demand, conditional, service validation, or not applicable. |
| DEC-008 | `PCQ-014`, `PCQ-016`, `PCQ-020B` | Entropy owner | Testing institution and entropy specialist | `EXTERNAL_CONFIRMATION_REQUIRED` | External-provider acceptance versus direct-source work. | Confirm provider responsibility and evidence; open a separate direct-source gate only if required. |
| DEC-009 | `PCQ-015`, `PCQ-018` | DRBG owner | Testing institution and entropy specialist | `EXTERNAL_CONFIRMATION_REQUIRED` | Entropy credit, instantiate construction, limits, operational reseed policy. | Retain accepted credit/limits/profile or required changes. |
| DEC-010 | `PCQ-017`, `PCQ-019`, `PCQ-020` | DRBG/process owner | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Fatal versus local errors, fork/VM/suspend restrictions, operator guidance. | Record accepted lifecycle and recovery rules. |
| DEC-011 | `PCQ-021`, `PCQ-022` | CSP owner | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Formal CSP list, in-memory-only scope, persistence/wrapping exclusions. | Retain formal classifications and scope acceptance. |
| DEC-012 | `PCQ-023`, `PCQ-024` | Zeroization reviewer | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Compiler evidence, OS/process controls, operator requirements. | Record accepted zeroization evidence and environmental controls. |
| DEC-013 | `PCQ-024A`, `PCQ-024B` | Module architect and security reviewer | Testing institution/security specialist | `EXTERNAL_CONFIRMATION_REQUIRED` | Caller-owned cleanup, fatal lock-failure behavior, potential internal redesign. | Obtain explicit disposition before changing cleanup model. |
| DEC-014 | `PCQ-025`, `PCQ-SC-001`, `PCQ-SC-002` | Secure-coding reviewer | Testing institution/side-channel specialist | `EXTERNAL_CONFIRMATION_REQUIRED` | Exact binary review and timing/cache campaign. | Define leakage model, target classes, sample sizes, tools, and reviewer competence. |
| DEC-015 | `PCQ-SC-003`, `PCQ-SC-004` | Security architect | Testing institution/side-channel specialist | `EXTERNAL_CONFIRMATION_REQUIRED` | Power/EM/fault applicability and final hardening profile. | Record applicable assessment and required/recommended controls. |
| DEC-016 | `PCQ-027`, `PCQ-029`, `PCQ-030` | Document control owner | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Official forms, traceability structure, vendor-versus-institution test content. | Retain current template/revision and document-format expectations. |
| DEC-017 | `PCQ-028`, `PCQ-032`, `PCQ-035` | Evidence/configuration manager | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | Submission artifact scope, reviewer metadata, protected delivery. | Record exact artifact list, retention fields, and approved secure channel. |
| DEC-018 | `PCQ-033`, `PCQ-034`, `PCQ-039` | Configuration manager | Testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` | SBOM scope, repository controls, change/retest policy. | Record mandatory records and change-impact categories. |
| DEC-019 | `PCQ-036`, `PCQ-037` | Product release owner | Testing/validation institution | `EXTERNAL_CONFIRMATION_REQUIRED` | External wording, signer/TSA provisioning, test-package identity. | Retain permitted claims and pre-test signing requirements. |
| DEC-020 | `PCQ-038` | Legal/product owner | Testing institution and legal counsel | `EXTERNAL_CONFIRMATION_REQUIRED` | Customer PoC distribution, notices, license, support boundary. | Obtain institution guidance and separate legal approval before broader distribution. |

## State Rules

Allowed states:

- `EXTERNAL_CONFIRMATION_REQUIRED`
- `ANSWER_RECEIVED_PENDING_REVIEW`
- `ACCEPTED_CONTROLLED_DECISION`
- `CONDITIONALLY_ACCEPTED`
- `REJECTED_REDESIGN_REQUIRED`
- `DEFERRED_OUT_OF_INITIAL_SCOPE`
- `BLOCKED_EXTERNAL`

An entry may move out of `EXTERNAL_CONFIRMATION_REQUIRED` only when the response-capture record identifies the respondent role, date, scope assumptions, referenced guidance/version or retained written answer, internal reviewer, and decision owner.

## Change Rules

- A response never edits code automatically.
- Any public API or ABI impact requires a separate API/ABI gate.
- Any new algorithm or service requires a separate scope gate.
- Any runtime integrity, entropy-source, zeroization, or side-channel implementation requires a numbered work item and tests.
- Any signing, tag, timestamp, publication, customer distribution, or external submission requires explicit authorization outside this register.
- Contradictory answers remain unresolved and must be escalated to the responsible authority identified by `PCQ-040`.
