# KCMVP Pre-consultation Rehearsal

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-PC-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_REHEARSAL` |
| Owner role | Validation program owner |
| Reviewer | `TODO` |
| Approval | `NO` |
| Policy ID | `PCR-P2-08-R1` |
| Candidate ID | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| Work-product version | `0.1.0-p3-00-downstream-requirement-intake` |
| Scripted internal rehearsal | `COMPLETE_LOCAL_SCRIPTED` |
| Assisted desk rehearsal | `COMPLETE_LOCAL_ASSISTED` |
| P2-11 handoff | `READY_FOR_NAMED_HUMAN_EXECUTION` |
| Named reviewer / independence | `UNASSIGNED` / `NOT_RECORDED` |
| Independent live internal rehearsal | `NOT_RUN` |
| Institution-contact authorization | `NO` |
| External consultation | `NOT_CONDUCTED` |
| Institution response | `NOT_RECEIVED` |
| Submission authorization | `NO` |
| External release authorization | `NO` |

## Purpose

This document closes the internal P2-08 rehearsal step. It defines how AnoCrypto-C presents the candidate boundary, mechanisms, evidence classes, unresolved decisions, and next-action rules before contacting a testing or validation institution.

Completion of this rehearsal does **not** mean that a testing institution reviewed the module, that an answer was received, that an official template was accepted, or that submission, signing, release, or production use is authorized.

## Rehearsal Scope

The rehearsal covers:

1. product and module boundary;
2. candidate operating environment;
3. implemented algorithms and frozen public ABI;
4. module lifecycle, startup self-tests, integrity candidate, and fatal recovery;
5. entropy-provider and HMAC_DRBG responsibility;
6. CSP ownership and zeroization;
7. secure-coding and side-channel review limits;
8. local, synthetic, hosted, institution, and release evidence classification;
9. controlled submission-document skeleton and secure staging;
10. unresolved decisions, answer capture, and change-impact routing.

The rehearsal does not include source disclosure, executable delivery, production credentials, actual institution test results, actual hosted artifacts, or an implementation change authorization.

## Fixed Claims Boundary

The presenter must state at the beginning:

- AnoCrypto-C is a KCMVP candidate and readiness work product, not a validated module.
- The initial internal candidate is `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`.
- Runtime integrity remains `NOT_IMPLEMENTED`.
- Exact OE-01 execution remains `NOT_RUN`.
- P1-34 hosted lanes remain `NOT_RUN`, review remains `UNREVIEWED`, and artifacts remain 0.
- Repository signing is `UNSIGNED`; offline signing handoff is `BLOCKED`; external release authorization is `NO`.
- Local and synthetic evidence is not institution or actual hosted evidence.
- The public boundary remains 14 headers, 69 exported symbols, and `ANOCRYPTO_C_0`.

## Scripted Agenda

| Stage | Target time | Presenter objective | Required output |
|---|---:|---|---|
| R0 — Handling and claims | 5 min | Establish confidentiality, evidence classes, prohibited material, and no-authorization status. | Recipient/channel and recording rules confirmed. |
| R1 — Boundary and OE | 10 min | Explain the single shared-object boundary and OE-01 candidate. | Answers or follow-ups for `PCQ-001`, `PCQ-003`, `PCQ-006`. |
| R2 — Scope and ABI | 5 min | Show algorithm list, exclusions, 14/69/ABI freeze, and layer separation. | Scope acceptance questions routed. |
| R3 — State, self-test, integrity | 15 min | Walk through FSM, startup order, selected integrity design, and recovery limits. | Disposition for integrity mechanism and conditional-test terminology. |
| R4 — Entropy and DRBG | 15 min | Explain Linux `getrandom`, responsibility allocation, limits, fork/VM/suspend gaps. | Provider, entropy-credit, failure-state, and lifecycle questions assigned. |
| R5 — CSP and zeroization | 10 min | Explain CSP inventory, module-owned versus caller-owned cleanup, and compiler evidence limits. | Formal CSP/zeroization expectations recorded. |
| R6 — Secure coding and side channels | 10 min | Present local source/compiler review and open empirical/specialist work. | Required review depth and target evidence identified. |
| R7 — Tests and evidence | 10 min | Show traceability and evidence classifications without exposing sensitive values. | Required vendor versus institution evidence identified. |
| R8 — Documents, staging, and process | 10 min | Present controlled skeletons, secure staging, signing/claims blockers, and decision routing. | Template, artifact, channel, reviewer, and process questions assigned. |

## Priority-1 Consultation Agenda

Every Priority-1 item has an internal owner role and intended recipient. Exact answers remain blank until retained external confirmation is received.

| Agenda ID | Question IDs | Internal owner | Intended recipient | Blocking decision |
|---|---|---|---|---|
| A-01 | `PCQ-001`, `PCQ-003`, `PCQ-003A`, `PCQ-003B` | Module architect | Testing institution | Boundary and operating-environment granularity. |
| A-02 | `PCQ-006`, `PCQ-040` | Validation program owner | Testing and validation institutions | Target security level and decision authority. |
| A-03 | `PCQ-009`, `PCQ-010`, `PCQ-013B` | Security architect and configuration manager | Testing institution | Runtime integrity mechanism and integrity-key control. |
| A-04 | `PCQ-011`, `PCQ-013`, `PCQ-013A` | Module architect | Testing institution | Fatal recovery and startup/on-demand/conditional self-test classification. |
| A-05 | `PCQ-014`, `PCQ-015`, `PCQ-017`, `PCQ-020A` | Entropy/DRBG owner | Testing institution and entropy specialist | External provider acceptance, entropy credit, failure handling, conditional-test terminology. |
| A-06 | `PCQ-021`, `PCQ-023`, `PCQ-024A`, `PCQ-024B` | CSP/zeroization owner | Testing institution and security specialist | Formal CSP inventory, accepted zeroization evidence, caller-owned cleanup, fatal-lock fallback. |
| A-07 | `PCQ-SC-001`, `PCQ-SC-003`, `PCQ-SC-004` | Secure-coding reviewer | Testing institution and side-channel specialist | Exact source/binary/empirical assessment and hardening expectations. |
| A-08 | `PCQ-027`, `PCQ-028`, `PCQ-029`, `PCQ-035` | Document control and configuration manager | Testing institution | Current forms, submission artifacts, traceability format, and secure transfer channel. |
| A-09 | `PCQ-032`, `PCQ-034`, `PCQ-039` | Evidence and configuration manager | Testing institution | Reviewer metadata, configuration-management records, and post-change review rules. |
| A-10 | `PCQ-036`, `PCQ-037`, `PCQ-038` | Product release owner and legal owner | Testing/validation institution | Permitted claims, pre-test signing, and PoC distribution. |

Priority-2 and Priority-3 items remain in `KCMVP_PRECONSULTATION_QUESTIONS.md` and are asked only after the blocking decisions above are understood or when the institution requests deeper detail.

## Entry Checklist

Before a live consultation:

- [ ] Recipient institution, contact role, meeting date, confidentiality handling, and approved channel are recorded.
- [ ] The question pack has been checked against the institution's current public guidance.
- [ ] Only the internal rehearsal package is staged; no source, binary, credential, raw entropy, key, digest, MAC, tag, plaintext, ciphertext, or customer data is present.
- [ ] Each presenter knows which statements are implementation facts, local evidence, proposals, external-confirmation items, or blockers.
- [ ] The response-capture template and unresolved-decision register are ready.
- [ ] No signing, tag, push, distribution, or submission action is implied by the meeting.

## Exit Checklist

A consultation session is not complete until:

- [ ] Every answered question identifies the respondent role, date, scope assumptions, and referenced guidance/version.
- [ ] Ambiguous or verbal-only answers remain `EXTERNAL_CONFIRMATION_REQUIRED` until retained in an acceptable form.
- [ ] Each answer maps to one or more decision IDs and action classes.
- [ ] Any code or ABI impact is routed through a separate approved work item.
- [ ] Documents affected by the answer are identified.
- [ ] No local/synthetic evidence is relabeled as hosted or institution evidence.
- [ ] The project owner records the next authorized step; absence of an answer authorizes nothing.

## Rehearsal Disposition

The scripted walkthrough and P2-10 assisted desk rehearsal are complete. P2-11 provides the controlled handoff for a future named human reviewer, but reviewer identity and independence are not recorded and the live rehearsal remains `NOT_RUN`. Institution contact remains unauthorized pending completed human-review records and the project-owner decision in `KCMVP_CONSULTATION_AUTHORIZATION_RECORD.md`.

The following remain open:

- named independent internal reviewer and live rehearsal;
- named institution and secure communication channel;
- institution answers and accepted references;
- official template/security-level/boundary/integrity/entropy/CSP/side-channel decisions;
- exact OE-01 evidence;
- runtime integrity implementation;
- P1-34 hosted evidence;
- signer, TSA, approvals, and external authorization.
