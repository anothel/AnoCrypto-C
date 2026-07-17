# KCMVP Open Items and Exit Criteria

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-EXIT-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_CLOSEOUT` |
| Policy ID | `P2C-P2-09-R1` |

## 1. Open-item classes

### Institution-dependent decisions

- candidate physical/logical boundary and shared/static interpretation;
- exact operating-environment granularity and retest triggers;
- target security level and responsibility split;
- submitted algorithms/services and unsupported identifiers;
- startup/on-demand/conditional self-test classification;
- runtime integrity mechanism, build injection, and key custody;
- OS entropy-provider acceptance, entropy credit, health-test responsibility, and DRBG profile;
- CSP classification and zeroization evidence expectations;
- side-channel, timing/cache, power/EM/fault applicability and methodology;
- official forms, artifact list, evidence retention, protected transfer, and allowed claims.

### Internal work that remains unexecuted

- named independent human live rehearsal;
- project-owner authorization for a named institution/contact purpose;
- exact OE-01 build/test/compiler-output review;
- named security reviewer disposition for retained findings;
- legal/evaluation terms and support scope;
- signer/TSA/approver provisioning design;
- secure submission and consultation channel approval.

These items are not authorized merely because they appear here.

### External blockers

- GitHub Actions billing prevents P1-34 actual hosted execution and artifact review;
- institution consultation has not occurred;
- institution test and validation have not started;
- formal submission, signing, timestamp, and release authorization are absent.

## 2. Exit criteria

### Gate A — internal pre-consultation pack ready

Status: `MET_LOCAL`

Required:

- P2-01 through P2-08 local work products present;
- P2-09 cumulative closeout and claims summary present;
- P2-10 assisted rehearsal, scorecard, gap register, and contact decision gate present;
- 14 headers, 69 symbols, and `ANOCRYPTO_C_0` verified;
- status-only package allowlist and hash validation pass;
- no source, binary, credential, cryptographic result value, or false claim included.

Gate A does not authorize external contact. P2-10 assisted rehearsal is complete, but independent human rehearsal remains `NOT_RUN`.

### Gate B — external pre-consultation authorized and conducted

Status: `NOT_MET`

Required:

- named internal owner and independent reviewer;
- approved institution/recipient and protected channel;
- retained session date, participant roles, scope assumptions, and materials sent;
- written or otherwise retainable answers entered into response records;
- contradictory or conditional answers explicitly identified.

### Gate C — post-consultation implementation ready

Status: `NOT_MET`

Required:

- relevant decision record moved to `ACCEPTED_CONTROLLED_DECISION` or approved conditional state;
- impact classification completed;
- numbered work item approved;
- public API/ABI and algorithm scope gates completed when applicable;
- tests, evidence, rollback, and documentation plan approved.

### Gate D — formal submission ready

Status: `NOT_MET`

Required:

- official current forms and target security level confirmed;
- candidate boundary/OE and runtime integrity mechanism implemented and reviewed;
- exact OE evidence, CSP/zeroization, entropy/DRBG, and required specialist evidence complete;
- submission document set converted from skeleton to reviewed controlled documents;
- secure delivery, named approvers, legal controls, and institution-required artifacts ready;
- P1-34 or accepted replacement hosted evidence disposition complete when required.

### Gate E — external release ready

Status: `NOT_MET`

Required in addition to Gate D or a separately approved release basis:

- approved tag and reviewed commit;
- production signer and TSA provisioned under approved custody;
- detached signature/timestamp verification completed;
- actual hosted artifact review retained;
- legal/customer distribution terms approved;
- explicit external release authorization `YES`.

## 3. Stop conditions

Work must stop and return to the responsible gate when:

- an answer would expand algorithms, services, public API, ABI, or operating environments;
- the requested mechanism conflicts with the frozen candidate or clean-room boundary;
- a record would disclose prohibited material;
- a local/synthetic result is being presented as hosted, institution, formal, or production evidence;
- a tag, signature, timestamp, submission, customer distribution, or release lacks explicit authorization.
