# Submission Package Index

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-IX-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Product | `AnoCrypto-C` |
| Engineering version | `0.1.0-p2-08-preconsultation-rehearsal` |
| Candidate ID | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| Owner role | Module development lead |
| Reviewer | `TODO` |
| Approval | `NO` |
| Formal template conformity | `EXTERNAL_CONFIRMATION_REQUIRED` |

## 1. Purpose

This index defines the internally controlled P2-07 document set and the package relationships required before pre-consultation. It does not authorize a submission or external distribution.

## 2. Package sections

| Package section | Controlled document | Source authority | Current disposition |
|---|---|---|---|
| Basic design | `AC-KCMVP-BD-001` | P2-02 boundary/OE, P2-03 state/self-test, P2-04 entropy, P2-05 CSP | Skeleton complete; external confirmation open. |
| Detailed design | `AC-KCMVP-DD-001` | implementation plus P2-03 through P2-06 design records | Skeleton complete; runtime integrity and exact OE review open. |
| Configuration management | `AC-KCMVP-CM-001` | release identity, compatibility, provenance, clean-room and workflow policies | Skeleton complete; production roles/signing/retention approval open. |
| Test documents | `AC-KCMVP-TS-001` | tests, traceability, evidence workflow and policy checks | Skeleton complete; institution procedures/results absent. |
| Installation/operator guidance | `AC-KCMVP-OP-001` | candidate environment, install smoke, state/recovery and operating assumptions | Skeleton complete; exact OE execution absent. |
| Traceability appendix | `AC-KCMVP-TR-001` | `KCMVP_EVIDENCE_TRACEABILITY_MATRIX.md` | Controlled reference mapping complete. |
| Reference register | `AC-KCMVP-RF-001` | public standards/guides, public vectors, repository records | Skeleton complete; full licensed standards not copied. |
| Staging/handling | `AC-KCMVP-SH-001` | clean-room, sensitive-data and evidence rules | Skeleton complete; approved delivery channel open. |
| Review checklist | `AC-KCMVP-RC-001` | all controlled documents | Ready for P2-08 mock review. |

## 3. Package status summary

- Security level: `UNSELECTED / EXTERNAL_CONFIRMATION_REQUIRED`.
- Formal algorithm scope acceptance: `EXTERNAL_CONFIRMATION_REQUIRED`.
- Exact OE-01 retained execution: `NOT_RUN`.
- Runtime integrity verifier: `NOT_IMPLEMENTED`.
- Institution test results: `NOT_RUN`.
- P1-34 actual hosted evidence: `BLOCKED_EXTERNAL`.
- Repository signing: `UNSIGNED`.
- External release/submission authorization: `NO`.

## 4. Submission staging rule

Only files enumerated in `release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt` are approved for the **internal skeleton staging area**. Source code, binaries, evidence bundles, disassembly, credentials, test hooks, customer data, and production signing material are not automatically included.
