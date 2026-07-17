# KCMVP Evidence Walkthrough Script

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-EW-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_REHEARSAL` |
| Owner role | Evidence owner |
| Reviewer | `TODO` |
| Approval | `NO` |
| Policy ID | `PCR-P2-08-R1` |

## Evidence Classification Legend

| Class | Meaning | Presentation rule |
|---|---|---|
| `IMPLEMENTATION_FACT` | Directly supported by current source and public surface. | Show path/identifier and status; disclose source only through an approved protected channel. |
| `LOCAL_ENGINEERING_ONLY` | Produced locally on a non-OE-01 or otherwise internal environment. | Never call hosted, institution, or formal validation evidence. |
| `SYNTHETIC_POLICY_ONLY` | Generated to test a policy or failure mode. | Never present as actual release, hosted, signing, or institution activity. |
| `ACTUAL_HOSTED` | Artifact from an assigned hosted runner and retained review. | Currently unavailable: `NOT_RUN` and `UNREVIEWED`. |
| `INSTITUTION_RESULT` | Result issued or retained from the testing/validation process. | Currently unavailable: `NOT_RECEIVED`. |
| `DESIGN_CANDIDATE` | Selected internal proposal awaiting implementation or external confirmation. | Explicitly state limitations and alternatives. |
| `BLOCKED_EXTERNAL` | Closure depends on an external condition. | State the blocker and resume condition; do not substitute local evidence. |

## Prohibited Presentation Material

Do not place in ordinary rehearsal packages, screen shares, chat, meeting notes, or evidence extracts:

- key, expanded key schedule, HMAC pad, CMAC subkey, entropy, nonce, seed, DRBG V/K, generated random bytes;
- IV, plaintext, ciphertext, digest, MAC, tag, private signing material, signature, timestamp request/response;
- access tokens, production infrastructure details, customer data, or unpublished exploit instructions;
- internal source or disassembly unless the approved recipient/channel and exact need are established.

## Station W0 — Identity, Claims, and Blockers

**Presenter statement**

- Product: AnoCrypto-C, C99 private commercial candidate SDK.
- Version: `0.1.0-p3-01-downstream-requirement-consolidation`.
- Candidate: `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`.
- Public boundary: 14 headers, 69 symbols, `ANOCRYPTO_C_0`.
- Formal validation/institution review: not completed.
- Hosted status: P1-34 `BLOCKED_EXTERNAL`; lanes `NOT_RUN`; review `UNREVIEWED`; artifacts 0.
- Signing/release: `UNSIGNED`, handoff `BLOCKED`, external authorization `NO`.

**Controlled references**

- `README.md`
- `docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md`
- `docs/kcmvp/KCMVP_READINESS_GAP_P2.md`

**Stop condition**: stop if any attendee interprets local or synthetic results as formal, hosted, signed, released, or validated.

## Station W1 — Boundary and Operating Environment

**Show**

- shared-object boundary and exclusions;
- candidate OE-01 configuration and fail-closed build enforcement;
- deferred Windows/macOS/static/32-bit/big-endian/other-CPU environments.

**Controlled references**

- `docs/kcmvp/KCMVP_MODULE_BOUNDARY_SPEC.md`
- `docs/kcmvp/KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`
- `release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake`

**Ask**: agenda A-01 and A-02.

**Do not claim**: OE-01 execution or institution acceptance.

## Station W2 — Algorithm, Service, and ABI Scope

**Show**

- SHA-256/384/512;
- HMAC-SHA-256/384/512;
- HMAC_DRBG-SHA-256 and public random service;
- AES-128/192/256 CBC without padding;
- AES-CMAC full tag;
- in-memory purpose-separated HMAC, block-cipher, and CMAC CSP handles;
- unchanged 14-header/69-symbol `ANOCRYPTO_C_0` boundary.

**Controlled references**

- `docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md`
- `docs/kcmvp/SERVICE_SCOPE_MATRIX.md`
- `docs/design/API_ABI_REVIEW_CHECKLIST.md`

**Do not show**: digest/MAC/tag/ciphertext values from tests.

## Station W3 — State, Self-tests, Integrity, and Recovery

**Show**

- finite-state model and service gate;
- startup KAT coverage;
- selected `INT-HMAC-SHA256-SINGLE-SO-R1` design;
- runtime integrity status `NOT_IMPLEMENTED`;
- fatal recovery proposal: terminate process and fresh load;
- current on-demand self-test implementation delta.

**Controlled references**

- `docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md`
- `docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md`
- `docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md`
- `docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md`

**Ask**: agenda A-03 and A-04.

## Station W4 — Entropy and DRBG

**Show**

- `ENT-OE01-LINUX-GETRANDOM-R1` external conditioned-provider model;
- 32-byte entropy and 16-byte nonce inputs at instantiate;
- 32-byte reseed input, 65,536-byte request maximum, and `2^48` reseed interval;
- no prediction resistance;
- process-marker/fork handling;
- VM clone/snapshot and suspend/resume limitations;
- status-only evidence rule.

**Controlled references**

- `docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md`
- `docs/kcmvp/KCMVP_ENTROPY_RESPONSIBILITY_MATRIX.md`
- `docs/kcmvp/KCMVP_ENTROPY_HEALTH_AND_RECOVERY_MATRIX.md`

**Ask**: agenda A-05.

**Do not show**: provider output, entropy, nonce, seed, V/K, or generated bytes.

## Station W5 — CSP and Zeroization

**Show**

- conservative CSP inventory;
- module-owned versus caller-owned storage;
- shutdown and locked fatal-transition table wipe;
- tombstone state and stale-generation rejection;
- compiler-output evidence limitations;
- process/OS residual-copy limitations.

**Controlled references**

- `docs/kcmvp/KCMVP_CSP_INVENTORY.md`
- `docs/kcmvp/KCMVP_SERVICE_CSP_OPERATION_MATRIX.md`
- `docs/kcmvp/KCMVP_ZEROIZATION_EVIDENCE.md`
- `docs/kcmvp/P2_05_IMPLEMENTATION_TEST_DELTA.md`

**Ask**: agenda A-06.

## Station W6 — Secure Coding and Side Channels

**Show**

- local source, warnings, analyzer, compiler-output, and ELF-hardening review;
- table-free portable AES source profile;
- fixed 16-byte CMAC comparison candidate;
- public-length-dependent work in hash/HMAC/DRBG paths;
- exact OE-01, empirical timing/cache, power/EM/fault, and transient-execution gaps.

**Controlled references**

- `docs/kcmvp/KCMVP_SECURE_CODING_REVIEW.md`
- `docs/kcmvp/KCMVP_SIDE_CHANNEL_REVIEW.md`
- `docs/kcmvp/KCMVP_SECURITY_FINDINGS_REGISTER.md`
- `docs/kcmvp/KCMVP_COMPILER_CPU_REVIEW_MATRIX.md`

**Ask**: agenda A-07.

**Do not claim**: universal constant-time behavior or certified side-channel resistance.

## Station W7 — Test and Evidence Traceability

**Show by metadata only**

- requirement-to-code-to-test-to-evidence paths;
- KAT, negative, lifecycle, concurrency, sanitizer, differential, package, ABI, provenance, and policy categories;
- exact evidence class and limitations;
- P1-34 blocker record.

**Controlled references**

- `docs/kcmvp/KCMVP_EVIDENCE_TRACEABILITY_MATRIX.md`
- `docs/kcmvp/RELEASE_EVIDENCE_WORKFLOW.md`
- `docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md`

**Safe sample fields**

- algorithm/service identifier;
- test name and PASS/FAIL status;
- compiler/platform identifier;
- artifact path, size, SHA-256, commit, reviewer role, date, and disposition;
- no secret or cryptographic result values.

## Station W8 — Submission Documents, Staging, and Decision Capture

**Show**

- controlled internal submission skeleton and document-control register;
- S0 allowlist staging and SHA-256 manifest;
- pre-consultation response template and unresolved-decision register;
- post-consultation action-impact matrix;
- signing, claims, and release blockers.

**Controlled references**

- `docs/kcmvp/submission/KCMVP_SUBMISSION_PACKAGE_INDEX.md`
- `docs/kcmvp/submission/KCMVP_DOCUMENT_CONTROL_REGISTER.md`
- `docs/kcmvp/submission/KCMVP_SUBMISSION_STAGING_AND_HANDLING.md`
- `docs/kcmvp/KCMVP_CONSULTATION_RESPONSE_CAPTURE_TEMPLATE.md`
- `docs/kcmvp/KCMVP_UNRESOLVED_DECISION_REGISTER.md`
- `docs/kcmvp/KCMVP_POST_CONSULTATION_ACTION_IMPACT_MATRIX.md`

**Ask**: agenda A-08 through A-10.

## Walkthrough Completion Record

| Item | Status |
|---|---|
| Script and controlled references prepared | `COMPLETE_LOCAL_SCRIPTED` |
| Prohibited-material boundary encoded | `READY_FOR_INTERNAL_REVIEW` |
| Status-only rehearsal package smoke | `PASS_LOCAL_ENGINEERING_ONLY` after test execution |
| Named independent internal reviewer | `TODO` |
| Live internal rehearsal | `NOT_RUN` |
| Approved external recipient/channel | `TODO` |
| External pre-consultation | `NOT_CONDUCTED` |
| Institution answers | `NOT_RECEIVED` |
