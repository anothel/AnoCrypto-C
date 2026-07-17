# KCMVP Named Reviewer Live-Rehearsal Execution Guide

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-HRH-003` |
| Status | `CONTROLLED_EXECUTION_GUIDE` |
| Policy ID | `HRH-P2-11-R1` |
| Session state | `NOT_RUN` |

## Before the session

- Verify the P2-11 handoff-package SHA-256.
- Complete reviewer identity, role, conflict statement, and expertise limits.
- Record the project owner and minutes owner.
- Confirm that no source, binary, credential, signature, timestamp, customer data, or institution response is present.
- Confirm the claims boundary and P1-34 hosted blocker.
- Use only the frozen pack; record every supplemental document separately.

## Review method

For each agenda, the presenter gives the narrow project answer and evidence path. The reviewer then challenges scope, unsupported claims, missing evidence, operator assumptions, and external dependencies.

Use one disposition per agenda:

- `ACCEPTED_FOR_INTERNAL_PRESENTATION`;
- `ACCEPTED_WITH_REWORDING`;
- `OBJECTION_REQUIRES_REMEDIATION`;
- `BLOCKED_MISSING_EVIDENCE`;
- `REFER_TO_SPECIALIST_OR_INSTITUTION`.

## Required ten-agenda execution table

| Agenda | Required challenge focus | Reviewer disposition | Objection/defect IDs |
|---|---|---|---|
| `A-01` | Candidate module boundary, loader, OE variation, exclusions. | `NOT_REVIEWED` | — |
| `A-02` | Security level, authority, and decision ownership. | `NOT_REVIEWED` | — |
| `A-03` | Integrity candidate versus unimplemented runtime verifier. | `NOT_REVIEWED` | — |
| `A-04` | Startup/on-demand/conditional tests and fatal recovery. | `NOT_REVIEWED` | — |
| `A-05` | OS entropy provider, health-test allocation, VM/suspend policy. | `NOT_REVIEWED` | — |
| `A-06` | CSP classification, zeroization evidence, caller-owned memory. | `NOT_REVIEWED` | — |
| `A-07` | Exact-OE compiler review, timing/cache, power/EM/fault gaps. | `NOT_REVIEWED` | — |
| `A-08` | Official forms, staging contents, secure transfer channel. | `NOT_REVIEWED` | — |
| `A-09` | Traceability, reviewer metadata, hosted evidence blocker. | `NOT_REVIEWED` | — |
| `A-10` | Claims, unsigned state, PoC limits, signing and release gate. | `NOT_REVIEWED` | — |

## Reviewer challenge prompts

For every answer ask:

1. What is implemented now?
2. What evidence proves only that statement?
3. What is local or synthetic rather than actual hosted or institution evidence?
4. What remains unimplemented, not run, externally blocked, or confirmation-required?
5. What would change if the institution rejects the current assumption?
6. Does the wording exceed the evidence?

## Session close

The reviewer must state one recommendation and list all material objections. Blank or inferred approvals are not permitted. The project owner records the separate contact decision only after the minutes and objection register are complete.
