# Current / Active / Next

Status: `P3_07_COMPLETE_P3_08_READY`

## Retained P3-01 policy compatibility

`P3-02 minimal HKDF-SHA-256 API` is the historical selected-implementation
token retained for the frozen P3-01 policy checker. P3-02 is complete and is
not the current task.

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 HKDF-SHA-256.
- P3-03 AES-256-GCM one-shot.
- P3-04 AES-256-GCM streaming.
- P3-05 fixed-profile scrypt.
- P3-06 AnoSecureKit Enterprise seven-capability parity/no-fallback checkpoint.
- P3-07 public-key key-handle architecture and AnoPKI contract gate.

P3-07 approved a common internally tagged CSP handle, normalized RSA/P-256
component import, message-based generic sign/verify, core-owned RSA DigestInfo,
and raw P-256 `r || s`. It changed no public code or ABI.

## ACTIVE

No implementation is active in this artifact.

## NEXT

`P3-08: RSA PKCS#1 v1.5 SHA-256/384/512 and RSA key-handle import`

P3-08 must begin with its own API/ABI review. The current candidate target is:

- one RSA public/private component import function;
- one generic message-based sign function;
- one generic message-based verify function;
- reuse existing key destroy/state and CSP validity/state functions;
- at most three new public symbols unless the review proves another symbol is
  necessary;
- one additive ABI line only after exact approval.

P3-08 must freeze RSA key-size/exponent policy, bounded storage, component
validation, concurrency leases, buffers/failure clearing, KATs, differential
validation, AnoPKI compatibility, and symbol-version evidence before merge.

## LATER

- P-256 ECDSA implementation using the shared sign/verify contract;
- restricted SHA-1 only through a separate misuse-resistant OCSP CertID gate;
- AnoPKI Enterprise public-key parity/no-fallback checkpoint after relevant
  primitives exist.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- Signing handoff: `BLOCKED`.
- External release authorization: `NO`.
- Original AnoSecureKit upstream Git-tree reproduction:
  `EXTERNAL_CONFIRMATION_REQUIRED`.
- Exact AnoPKI Enterprise source ZIP byte recomputation for P3-07:
  `EXTERNAL_CONFIRMATION_REQUIRED`.
- Trusted release provenance acceptance: `OPEN`.
