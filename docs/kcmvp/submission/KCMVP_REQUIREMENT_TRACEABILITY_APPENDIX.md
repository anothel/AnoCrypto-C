# Requirement Traceability Appendix

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-TR-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Validation readiness lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Source matrix

The authoritative engineering mapping remains `../KCMVP_EVIDENCE_TRACEABILITY_MATRIX.md`. This appendix defines how that matrix is incorporated into a controlled submission package without rewriting provisional status as compliance.

## 2. Required controlled columns

| Field | Required content |
|---|---|
| Requirement ID | Institution/standard requirement identifier after licensed review |
| Internal trace ID | Existing `TR-*` identifier |
| Applicability | applicable, not applicable with rationale, or externally confirmable |
| Basic/detailed design reference | controlled section/document ID |
| Code reference | file/function/component without sensitive source disclosure in routine package |
| Test procedure/result | vendor and institution result classes kept distinct |
| Evidence hash | exact file and SHA-256 |
| Status | READY, PARTIAL, NOT_IMPLEMENTED, NOT_RUN, BLOCKED_EXTERNAL, EXTERNAL_CONFIRMATION_REQUIRED |
| Reviewer disposition | named reviewer/date/limitations |

## 3. Current P2-07 trace status

- Boundary/OE, state/self-test design, entropy/DRBG, CSP/zeroization and secure-coding inputs are mapped.
- Submission skeleton documents now provide controlled design/configuration/test/operator destinations.
- Runtime integrity, exact OE-01, formal requirement applicability, institution results, hosted review and external authorization remain open.
- Public ABI remains 14 headers, 69 symbols and `ANOCRYPTO_C_0`.

## 4. Prohibited traceability shortcuts

- Do not equate a code path with an externally accepted requirement.
- Do not mark workflow definitions or synthetic fixtures as actual execution.
- Do not use local PASS as institution PASS.
- Do not fill unknown standard clauses from memory; use licensed/current source review.
- Do not close `EXTERNAL_CONFIRMATION_REQUIRED` without retained feedback and change control.
