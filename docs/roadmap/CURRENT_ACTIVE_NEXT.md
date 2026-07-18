# Current / Active / Next

Status: `P3_03_COMPLETE_P3_04_NEXT`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 minimal HKDF-SHA-256 API and additive `ANOCRYPTO_C_1` evidence.
- P3-03 minimal AES-256-GCM one-shot API, authenticated-output-only decrypt,
  additive `ANOCRYPTO_C_2`, NIST/differential tests and exact downstream v1
  primitive fixture evidence.

## ACTIVE

P3-03 local closeout and artifact handoff. No push, tag, signing, timestamping,
publication, or external distribution is authorized.

## NEXT

`P3-04: AES-256-GCM streaming API and pre-final plaintext policy`

P3-04 requires its own state-machine and API/ABI gate. It must not expand the
one-shot API or copy AnoSecureKit packet workflows into AnoCrypto-C.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- External release authorization: `NO`.
- AnoSecureKit Community identity conflict: `OPEN_DOWNSTREAM_SYNC_DECISION`.
