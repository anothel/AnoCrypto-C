# Current / Active / Next

Status: `ENGINEERING_PHASE_1_COMPLETE`

## Post-Phase-1 portfolio overlay — 2026-11-21 (information only)

**Current strategic direction:** The product owner selected **A — internal
installed-SDK consumer stabilization for AnoSecureKit Enterprise and AnoPKI
Enterprise**. The limited Linux x86_64 / GCC / Release / shared-SDK core
validation profile was separately approved and locally executed. The strategy
decision does **not** select a complete downstream integration baseline, close
G0/A5, or authorize broad implementation, pinned-SDK migration, qualified
human-review bypass, KCMVP, customer distribution, or external release.

**Latest observed engineering repository SHA for this note:**
`main@99e3ff22a0d35201e96bf4efdd3a1eab91d2000f`.
The bounded DRBG installed-consumer fix [PR #27](https://github.com/anothel/AnoCrypto-C/pull/27)
was merged as `ccfc0383d993dba8a9bdb676d66a2f662315879a`;
the historical prompt cleanup [PR #28](https://github.com/anothel/AnoCrypto-C/pull/28)
was merged as `99e3ff22a0d35201e96bf4efdd3a1eab91d2000f`.
Both have successful exact-SHA CI/installed smoke evidence, but **their
respective pre-merge independent-review / owner-review records require
clarification**. A merged PR or green CI is not a security approval.

**Planning and review authority:** [GitHub Master work index #15](https://github.com/anothel/AnoCrypto-C/issues/15),
[Jira ACC-4](https://anothel.atlassian.net/browse/ACC-4) (portfolio scope),
[Jira ACC-9](https://anothel.atlassian.net/browse/ACC-9) (downstream evidence),
[Jira ACC-5](https://anothel.atlassian.net/browse/ACC-5) (human-review gates).
For exact issues, see [#11](https://github.com/anothel/AnoCrypto-C/issues/11)
(lifecycle), [#13](https://github.com/anothel/AnoCrypto-C/issues/13)
(reviewer policy), [#16](https://github.com/anothel/AnoCrypto-C/issues/16) /
[#18](https://github.com/anothel/AnoCrypto-C/issues/18) (alias verification),
and [#24](https://github.com/anothel/AnoCrypto-C/issues/24) (DRBG fix / post-merge
review follow-up). Individual changes and downstream pins still need their own
recorded authorization and exact-SHA review.

**Reading rule:** The `CURRENT`, `ACTIVE`, `NEXT`, and `CLOSED DECISION`
sections **below** are the preserved **Phase 1 closeout snapshot**. In
particular, the original `NEXT PRODUCT DECISION: UNDECIDED` and
`FUTURE_WORK_NOT_SELECTED` tokens describe the historical closeout point,
**not today's strategic portfolio choice**. `ACTIVE NONE` does not retroactively
erase the two merged PRs or authorize any new implementation work. Historic
P3-10 source/archive identity, candidate validation OE, hosted review, signing,
and KCMVP statuses must not be silently replaced by today's internal core pilot.

**External release:** `EXTERNAL_RELEASE_NOT_AUTHORIZED`.


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
