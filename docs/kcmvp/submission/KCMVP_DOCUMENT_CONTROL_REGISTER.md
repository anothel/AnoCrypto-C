# Document Control Register

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-DC-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Configuration-management lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Status vocabulary

| Status | Meaning |
|---|---|
| `CONTROLLED_INTERNAL_SKELETON` | Structure and current engineering inputs are controlled; not externally approved. |
| `INTERNAL_REVIEW` | Named reviewers are evaluating content and evidence. |
| `EXTERNAL_CONFIRMATION_REQUIRED` | Testing/validation institution or legal/operational decision is required. |
| `APPROVED_FOR_SUBMISSION` | May be used only after named authorization and package hash binding. |
| `SUPERSEDED` | Retained for history but not current. |

## 2. Controlled document register

| ID | Revision | Owner role | Reviewer | Approval | State |
|---|---:|---|---|---|---|
| `AC-KCMVP-IX-001` | 0.1 | Module development lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-DC-001` | 0.1 | Configuration-management lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-BD-001` | 0.1 | Security architecture lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-DD-001` | 0.1 | Implementation lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-CM-001` | 0.1 | Configuration-management lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-TS-001` | 0.1 | Validation test lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-OP-001` | 0.1 | Product security/operator documentation lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-TR-001` | 0.1 | Validation readiness lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-RF-001` | 0.1 | Clean-room/compliance lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-SH-001` | 0.1 | Security operations lead | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |
| `AC-KCMVP-RC-001` | 0.1 | Independent internal reviewer | `TODO` | `NO` | `CONTROLLED_INTERNAL_SKELETON` |

## 3. Required review record

Every controlled revision must record:

- document ID and previous revision;
- change request or work-item ID;
- source commit and engineering version;
- author, reviewer, approver roles;
- UTC review and approval dates;
- affected requirement, code, test, evidence, operating environment, and customer claims;
- external feedback incorporated or still open;
- package manifest/hash updated after approval.

## 4. P2-07 change record

| Revision | Work item | Summary | Disposition |
|---|---|---|---|
| 0.1 | P2-07 | Created controlled internal skeleton and cross-reference set from P2-01 through P2-06. | Internal skeleton only; no submission approval. |


## 5. P2-08 controlled rehearsal companions

These files support pre-consultation and are not automatically part of an institution submission package.

| Document ID | File | Status | Approval |
|---|---|---|---|
| `AC-KCMVP-PC-001` | `../KCMVP_PRECONSULTATION_REHEARSAL.md` | `CONTROLLED_INTERNAL_REHEARSAL` | `NO` |
| `AC-KCMVP-EW-001` | `../KCMVP_EVIDENCE_WALKTHROUGH_SCRIPT.md` | `CONTROLLED_INTERNAL_REHEARSAL` | `NO` |
| `AC-KCMVP-DR-001` | `../KCMVP_UNRESOLVED_DECISION_REGISTER.md` | `CONTROLLED_INTERNAL_REHEARSAL` | `NO` |
| `AC-KCMVP-RT-001` | `../KCMVP_CONSULTATION_RESPONSE_CAPTURE_TEMPLATE.md` | `CONTROLLED_INTERNAL_REHEARSAL` | `NO` |
| `AC-KCMVP-AI-001` | `../KCMVP_POST_CONSULTATION_ACTION_IMPACT_MATRIX.md` | `CONTROLLED_INTERNAL_REHEARSAL` | `NO` |
