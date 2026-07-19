# Current / Active / Next

Status: `P3_04_COMPLETE_P3_05_NEXT`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 minimal HKDF-SHA-256 API and additive `ANOCRYPTO_C_1` evidence.
- P3-03 minimal AES-256-GCM one-shot API and additive `ANOCRYPTO_C_2` evidence.
- P3-04 AES-256-GCM streaming API, explicit state machine, unverified pre-final
  decrypt-output policy, additive `ANOCRYPTO_C_3`, differential tests, and exact
  `SKT1` v1 primitive compatibility evidence.

## ACTIVE

P3-04 local closeout and artifact handoff. No push, tag, signing, timestamping,
publication, or external distribution is authorized.

## NEXT

`P3-05: scrypt fixed-profile/API/resource gate`

P3-05 must begin with a separate API/resource gate. It must expose only a
generic primitive contract and must not implement `SKP1` framing, password
workflow, storage policy, or product orchestration.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- External release authorization: `NO`.
- AnoSecureKit Community identity conflict: `OPEN_DOWNSTREAM_SYNC_DECISION`.
