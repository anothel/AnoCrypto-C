# Configuration Management Plan Skeleton

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-CM-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Configuration-management lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Configuration identification

Current controlled identifiers:

- repository: `anocrypto-c`;
- engineering version: `0.1.0-p3-01-downstream-requirement-consolidation`;
- public ABI: `ANOCRYPTO_C_0`;
- public headers: 14;
- exported symbols: 69;
- candidate ID: `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`;
- submission-document policy: `SDP-P2-07-R1`.

A submission baseline requires a named commit, source archive hash, document manifest, binary hash, exact build-environment record and approval record. P2-07 does not create that approved baseline.

## 2. Source and change control

- One numbered work item normally maps to one commit.
- Already-pushed P1 history is not rewritten for cosmetic reasons.
- Public API/ABI changes require a separate gate and baseline updates.
- Security-relevant changes require impact analysis across requirement, design, tests, evidence, OE, operator guidance and validation status.
- Push, tag, production signing, timestamp and external release require explicit authorization.

## 3. Change classes

| Class | Examples | Required disposition |
|---|---|---|
| Documentation-only | wording, cross-reference, controlled metadata | review for claim/status drift and package manifest update |
| Internal implementation | bug fix without public ABI change | regression, static/security review and validation impact analysis |
| Public ABI | header, symbol, constant, public layout | explicit approval, new compatibility baseline and likely retest |
| Algorithm/service scope | new algorithm/mode/API | new scope and validation decision gate |
| Operating environment | OS/compiler/CPU/linkage change | environment-specific evidence and consultation/retest decision |
| Security mechanism | integrity, entropy, CSP, self-test changes | detailed design/test/evidence update and external disposition |

## 4. Build and reproducibility control

The exact candidate build procedure must record OS image/package provenance, CPU/kernel, GCC/CMake versions, generator, options, environment, source commit/hash, command log, library hash, SONAME/imports and named reviewer. Current local evidence outside OE-01 is not a substitute.

## 5. Document control

`AC-KCMVP-DC-001` controls document IDs, revisions, owners, reviewers, approvals and change records. Every approved package must bind the exact document bytes through a SHA-256 manifest.

## 6. Problem reporting and corrective action

Required workflow:

1. receive and classify report without exposing sensitive material;
2. assess affected versions, candidate environments, requirements and evidence;
3. assign severity, owner and disclosure controls;
4. implement on a controlled branch;
5. run regression and targeted security tests;
6. determine validation/retest and customer notification impact;
7. update findings, documents, SBOM/provenance and release authorization;
8. retain closure and independent review.

Specific contact channels, SLA and legal terms are `TODO`.

## 7. Evidence retention and access

Evidence is hash-bound, status-oriented and access-controlled. Keys, entropy, seeds, DRBG state, random output, plaintext/ciphertext, IVs, tags, private signing material and customer data are prohibited. Retention duration, approved storage, backup, access roles and destruction schedule remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## 8. Release/signing and integrity material

Repository signing remains `UNSIGNED`; signer/TSA are `UNPROVISIONED`; handoff is `BLOCKED`; external authorization is `NO`. The future runtime-integrity key is distinct from release signing and must use a separately approved custody, injection, rotation, backup and destruction procedure.

## 9. Maintenance and retest

Every post-baseline change requires documented impact to algorithm scope, boundary, OE, code, binary, CSP, self-tests, entropy, side-channel properties, operator assumptions and public claims. Institution-approved retest categories remain `EXTERNAL_CONFIRMATION_REQUIRED`.
