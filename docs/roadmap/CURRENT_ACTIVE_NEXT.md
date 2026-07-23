# Current / Active / Next

Status: `ENGINEERING_PHASE_1_COMPLETE`

## Retained historical policy token

`P3-02 minimal HKDF-SHA-256 API` is a completed historical token, not current
work.

## CURRENT

- AnoCrypto-C runtime baseline: `0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- canonical source commit: `98f544a574f70b387c300f171243b4e87174937a`;
- public headers: 18;
- exported symbols: 85;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_7`;
- P1-29 retained compatibility baseline: preserved;
- AnoSecureKit Enterprise parity/no-fallback checkpoint: complete;
- AnoPKI Enterprise P3-11 parity/no-fallback checkpoint: complete;
- AnoPKI Enterprise P3-12 Phase 1 closeout: complete.

## ACTIVE

`NONE`

No implementation, release, hosted, signing, KCMVP, or commercialization task
is authorized by the Phase 1 closeout.

## NEXT

`FUTURE_WORK_NOT_SELECTED`

A future phase may start only after an explicit product decision identifies a
new objective, exact source baselines, scope, owner, and authorization.

## CLOSED DECISION

```text
P3-11: COMPLETE
P3-12: COMPLETE
PHASE 1: CLOSED
CUSTOMER RELEASE: BLOCKED
NEXT PRODUCT DECISION: UNDECIDED
```

## DEFERRED / BLOCKED

- commercialization: `UNDECIDED`;
- customer/external release: `BLOCKED` / `NOT_AUTHORIZED`;
- hosted qualification/review: `NOT_RUN` / `UNREVIEWED`;
- repository and production signing: `UNSIGNED` / `BLOCKED`;
- KCMVP: `DEFERRED` / `NOT_VALIDATED`;
- production HSM/KMS/PKCS#11: not implemented or qualified;
- race-clean claim: prohibited because the final local authorization run did
  not execute the Go `-race` phase.
