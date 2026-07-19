# Current / Active / Next

Status: `P3_05_COMPLETE_P3_06_NEXT`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 minimal HKDF-SHA-256 API and additive `ANOCRYPTO_C_1` evidence.
- P3-03 minimal AES-256-GCM one-shot API and additive `ANOCRYPTO_C_2` evidence.
- P3-04 AES-256-GCM streaming API and additive `ANOCRYPTO_C_3` evidence.
- P3-05 fixed-profile scrypt API, caller-owned resource gate, additive `ANOCRYPTO_C_4`, OpenSSL test-only differential, and exact `SKP1` v1 primitive compatibility evidence.

## ACTIVE

P3-05 local closeout and artifact handoff. No push, tag, signing, timestamping, publication, or external distribution is authorized.

## NEXT

`P3-06: AnoSecureKit Enterprise parity and no-fallback checkpoint`

P3-06 is downstream integration work. It must consume the exact reviewed AnoCrypto-C artifact, complete the Enterprise adapter capability path, prove `SKT1`/`SKF1`/`SKP1` parity and no OpenSSL fallback, and keep the Community identity conflict open until AnoSecureKit Enterprise resolves it. No new AnoCrypto-C primitive or ABI is pre-authorized.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- External release authorization: `NO`.
- AnoSecureKit Community identity conflict: `OPEN_DOWNSTREAM_SYNC_DECISION`.
