# P2-07 Implementation and Test Delta

Status: `COMPLETE_LOCAL_INTERNAL_SKELETON`

Policy ID: `SDP-P2-07-R1`

## Implemented in P2-07

- 11 controlled internal submission-skeleton documents with IDs, revisions, ownership, review/approval placeholders and explicit non-authorization status;
- S0 allow-list staging manifest;
- machine-readable submission-document policy and fail-closed checker;
- staging package create/check smoke with SHA-256 manifest and sensitive/extra-file rejection;
- CMake policy and package-smoke targets/CTest integration;
- release evidence integration for the P2-07 policy result;
- current version and public-header manifest update without public API/symbol change.

## Explicitly not implemented or not completed

- official institution form/template completion;
- selected security level and final requirement applicability;
- approved secure-delivery channel and exact S2 package contents;
- runtime module integrity verifier;
- exact OE-01 execution and named external review;
- institution test results;
- actual P1-34 hosted artifacts;
- production signing, timestamp or external submission/release authorization.

## Regression expectation

- public headers: 14;
- exported symbols: 69;
- ABI line: `ANOCRYPTO_C_0`;
- local CTest registration with OpenSSL differential provider: 60 tests;
- policy, staging smoke, clean ABI, retained P1-29 compatibility, install, PoC, provenance and evidence checks must pass;
- P1-34 stays `BLOCKED_EXTERNAL`, actual hosted `NOT_RUN`, review `UNREVIEWED`, repository `UNSIGNED`, handoff `BLOCKED`, authorization `NO`.
