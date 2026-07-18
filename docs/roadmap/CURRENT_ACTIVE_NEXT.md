# Current / Active / Next

Status: `P3_02_COMPLETE_P3_03_NEXT`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 minimal HKDF-SHA-256 API, additive ABI, RFC 5869 tests, OpenSSL
  differential evidence, exact SKF1 info-byte evidence, install/PoC regression.

## ACTIVE

P3-02 local closeout and artifact handoff. No push, tag, signing, timestamping,
publication, or external distribution is authorized.

## NEXT

`P3-03: AES-256-GCM one-shot API and authenticated-output-only evidence`

P3-03 requires its own API/ABI gate. It must not be inferred from the HKDF API.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- External release authorization: `NO`.
- AnoSecureKit Community identity conflict: open downstream decision.
