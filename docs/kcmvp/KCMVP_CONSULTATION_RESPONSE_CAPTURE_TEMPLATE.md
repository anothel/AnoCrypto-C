# KCMVP Consultation Response Capture Template

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-RT-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_REHEARSAL` |
| Owner role | Document control owner |
| Reviewer | `TODO` |
| Approval | `NO` |
| Policy ID | `PCR-P2-08-R1` |

## Record Identity

| Field | Required value |
|---|---|
| Record ID | `RESP-YYYYMMDD-NNN` |
| Question ID(s) | `PCQ-...` |
| Related decision ID(s) | `DEC-...` |
| Institution/organization | `TODO` |
| Respondent role | `TODO` |
| Contact/meeting channel classification | `TODO` — do not place credentials or sensitive routing details here. |
| Response date and timezone | `TODO` |
| Recorder | `TODO` |
| Internal reviewer | `TODO` |
| Confidentiality classification | `PUBLIC`, `INTERNAL`, or approved protected class |

## Exact Scope Assumptions

Record the module candidate, operating environment, security-level assumption, algorithm/service scope, document revision, and any hypothetical condition used by the respondent.

```text
Candidate: AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1
Engineering version: 0.1.0-p2-09-closeout-review
Security level assumption: TODO
Question scope/conditions: TODO
```

## Response

| Field | Value |
|---|---|
| Response classification | `ACCEPTED`, `CONDITIONALLY_ACCEPTED`, `REJECTED`, `DEFERRED`, `INFORMATION_REQUESTED`, or `AMBIGUOUS` |
| Retained written answer or approved summary | `TODO` |
| Referenced guidance/form/version/section | `TODO` |
| Attachments or controlled record identifiers | `TODO` |
| Follow-up required | `YES` or `NO` |
| Follow-up owner/date | `TODO` |

Do not store keys, entropy samples, seed/DRBG state, generated random bytes, credentials, customer data, private source extracts, signatures, timestamps, or unpublished exploit detail in this record.

## Internal Interpretation

| Field | Value |
|---|---|
| Proposed decision-register state | `TODO` |
| Affected documents | `TODO` |
| Affected source/tests/tools | `TODO` |
| Public API/ABI impact | `NONE`, `POSSIBLE`, or `CONFIRMED_GATE_REQUIRED` |
| Algorithm/scope impact | `NONE`, `POSSIBLE`, or `CONFIRMED_GATE_REQUIRED` |
| Operating-environment impact | `TODO` |
| Evidence/retest impact | `TODO` |
| Estimated cost/schedule class | `LOW`, `MEDIUM`, `HIGH`, or `UNKNOWN` |
| Security/claim impact | `TODO` |

## Required Disposition

| Role | Name/identifier | Date | Disposition |
|---|---|---|---|
| Technical owner | `TODO` | `TODO` | `ACCEPT`, `REWORK`, `ESCALATE`, or `DEFER` |
| Independent reviewer | `TODO` | `TODO` | `CONCUR`, `OBJECT`, or `MORE_EVIDENCE_REQUIRED` |
| Validation program owner | `TODO` | `TODO` | `CONTROLLED_DECISION`, `PENDING`, or `NO_ACTION` |

## Resulting Work Authorization

```text
Authorized numbered work item: NONE
Authorized public API/ABI change: NO
Authorized algorithm expansion: NO
Authorized signing/tag/timestamp: NO
Authorized external submission/release/distribution: NO
```

Change the values above only through the project's explicit approval process. A captured response alone is not work or release authorization.
