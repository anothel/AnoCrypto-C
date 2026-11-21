# Current / Active / Next

## Strategy A checkpoint — 2026-11-22 KST

Strategy A covers internal installed-SDK stabilization for AnoSecureKit
Enterprise and AnoPKI Enterprise. Core `main@76d4af2e32b8bf9d7a624205f93607a24814843d`
includes the Policy v3 documentation. The Master authorizes bounded ordinary
packets by recording `READY_FOR_IMPLEMENTATION — OWNER_STANDING_POLICY_V3`
on an existing issue. An explicit owner decision can also authorize a packet.
Review and merge remain separate decisions. [Owner-adopted Policy v3 #13](https://github.com/anothel/AnoCrypto-C/issues/13#issuecomment-7069225969) ·
[Current queue #15](https://github.com/anothel/AnoCrypto-C/issues/15)

The installed SDK used in the Linux x86_64 GCC Release preflights came from
`56fb11bc9049780d948308a5225cbbbf6c467a6f`. Its local core suite passed 102/102.
Official SDK artifact selection and downstream acceptance remain pending.
[Artifact tracking #30](https://github.com/anothel/AnoCrypto-C/issues/30) ·
[Exact build and SDK digests](https://github.com/anothel/AnoCrypto-C/issues/15#issuecomment-7062859177)

- AnoSecureKit `954c267e99a4e6c4d1d1db021f0c86e063bcc9fc` passed 9/9 local
  tests with the exploratory SDK identity. [Preflight #5](https://github.com/anothel/AnoSecureKit-Enterprise/issues/5#issuecomment-7064471731)
- AnoPKI's initial `0f3c1e82869a8ee2083559f9c6f7ae322f869a46` run passed
  13/15. The R1+R2 candidate `aebc5ad59250840d74d93ea5ae7195801188f04f`
  passed 15/15 local tests and 27/27 hosted CI jobs. It merged as
  `d2d488696f898ff092ab23fd264941ec8ca6158a`, whose hosted CI passed 27/27.
  Selected-profile local Release evidence is from the candidate. R3/R4 and
  protected workflow qualification remain pending. [Initial preflight #8](https://github.com/anothel/AnoPKI-Enterprise/issues/8#issuecomment-7064479283) ·
  [Candidate verification](https://github.com/anothel/AnoPKI-Enterprise/issues/8#issuecomment-7069333325) ·
  [Merge CI](https://github.com/anothel/AnoPKI-Enterprise/actions/runs/44950830031)

The premerge review and separate approval records for PKI PR #9 and Core PR #31
still require clarification. [PKI postmerge record](https://github.com/anothel/AnoPKI-Enterprise/issues/8#issuecomment-7069407145) ·
[Core postmerge record](https://github.com/anothel/AnoCrypto-C/issues/13#issuecomment-7069407526)

`EXTERNAL_RELEASE_NOT_AUTHORIZED`. The preceding checkpoints are current as of
the date shown. The original Phase 1 record and 2026-11-21 overlay follow.

---

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
