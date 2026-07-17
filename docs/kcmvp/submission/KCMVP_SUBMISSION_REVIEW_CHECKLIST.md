# Submission Document Review Checklist

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-RC-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Independent internal reviewer |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Document control

- [ ] Every file has ID, revision, status, owner, reviewer and approval fields.
- [ ] Cross-references resolve to current controlled files.
- [ ] Change history and source commit are recorded.
- [ ] Superseded documents are excluded from current staging.

## 2. Technical consistency

- [ ] Boundary and OE match `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`.
- [ ] Public surface is 14 headers, 69 symbols and `ANOCRYPTO_C_0`.
- [ ] Service/algorithm lists do not add unapproved scope.
- [ ] State, self-test, entropy, CSP and zeroization wording matches P2 policies.
- [ ] Runtime integrity is marked `NOT_IMPLEMENTED`.
- [ ] Exact OE-01 and empirical side-channel work remain `NOT_RUN` where applicable.

## 3. Evidence integrity

- [ ] Local, synthetic, actual hosted and institution result classes are distinct.
- [ ] P1-34 remains `BLOCKED_EXTERNAL`, `NOT_RUN`, `UNREVIEWED`, artifacts 0.
- [ ] Every evidence claim has an exact artifact/hash/reviewer or remains open.
- [ ] Sensitive values are absent.

## 4. Claims and external dependencies

- [ ] No formal validation, audit, production-readiness or universal side-channel claim appears.
- [ ] Security level, official template conformity and institution acceptance remain externally confirmable.
- [ ] Repository signing is `UNSIGNED`, handoff `BLOCKED`, authorization `NO`.
- [ ] Current KISA guide/form references have been rechecked before external use.

## 5. Staging

- [ ] Only staging-manifest files are in S0.
- [ ] No source, binary, disassembly, hooks, credentials, customer data or raw sensitive evidence is present.
- [ ] Package SHA-256 manifest verifies.
- [ ] Recipient, channel, reviewer and approval are recorded before S1/S2 transfer.

## 6. P2-08 readiness

- [ ] High-priority consultation questions map to affected documents and decisions.
- [ ] Review walkthrough can be completed without showing prohibited material.
- [ ] Every unresolved answer has an owner and impact field.
- [ ] No implementation or submission is authorized solely by completing this checklist.


## 7. P2-08 internal rehearsal status

- [x] Scripted walkthrough and prohibited-material boundary prepared.
- [x] Ten Priority-1 agendas have owner roles and intended recipients.
- [x] Twenty unresolved decisions map to response/action controls.
- [x] Status-only rehearsal package policy and smoke test are defined.
- [ ] Named independent live internal rehearsal completed.
- [ ] Approved external recipient/channel recorded.
- [ ] External consultation conducted and responses retained.

Checked items above represent internal document preparation only and do not authorize submission.
