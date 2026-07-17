# P2-08 Implementation and Test Delta

## Scope

P2-08 adds controlled pre-consultation rehearsal documents, question ownership, evidence walkthrough, unresolved-decision capture, post-answer routing, and status-only package policy. It does not add an algorithm, public API, runtime integrity implementation, entropy source, signer, tag, institution result, or external release action.

## Added Controls

- policy `PCR-P2-08-R1`;
- scripted agenda and Priority-1 owner/recipient mapping;
- evidence classification and prohibited-material walkthrough;
- 20-entry unresolved-decision register;
- blank response-capture template;
- post-consultation action-impact classes and stop conditions;
- allowlisted status-only rehearsal package with SHA-256 manifest;
- CMake policy and package smoke tests.

## Runtime Delta

```text
Cryptographic implementation: unchanged
Public headers: 14
Exported public symbols: 69
ABI line: ANOCRYPTO_C_0
Runtime integrity: NOT_IMPLEMENTED
Exact OE-01 execution: NOT_RUN
Actual hosted lanes: NOT_RUN
Institution consultation: NOT_CONDUCTED
Institution responses: NOT_RECEIVED
External authorization: NO
```

## Required Regression

- complete CTest suite;
- P2-02 through P2-08 policy checks;
- pre-consultation rehearsal package creation and verification;
- clean Release exported-symbol baseline;
- retained P1-29 compatibility;
- install consumer and Customer PoC smoke;
- release provenance, blocked signing handoff, and evidence manifest;
- patch application and ZIP extraction/source-tree identity.

## Exit State

P2-08 completes the planned internal validation-readiness sequence P2-01 through P2-08. It does not close the external confirmation, runtime integrity, exact OE-01, P1-34 hosted, signing, legal, submission, or validation gaps.

The next work item must be selected from retained institution answers or an explicitly authorized independent blocker-remediation lane. No answer means no implementation authorization.
