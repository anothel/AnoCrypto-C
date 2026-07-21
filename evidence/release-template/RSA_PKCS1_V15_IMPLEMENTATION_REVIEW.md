# RSA PKCS#1 v1.5 Implementation Review

Status: `LOCAL_ENGINEERING_REVIEW_COMPLETE_EXTERNAL_RELEASE_BLOCKED`

- Profile: RSA-2048, public exponent 65537.
- Schemes: PKCS#1 v1.5 with SHA-256/384/512.
- Public delta: 1 header, 3 symbols, `ANOCRYPTO_C_5`.
- Key ingress: unsigned big-endian algorithm-native components through explicit pointer/length pairs; no public struct layout.
- Private controls: CRT exponent blinding and post-operation public fault check.
- Output policy: no partial signature; caller output cleared on failure.
- Production OpenSSL dependency: none.
- PKI/container ownership: AnoPKI.
- External provider/routing/evidence ownership: Enterprise.
- KCMVP: `DEFERRED`.
- Hosted lanes/review: `NOT_RUN` / `UNREVIEWED`.
- Repository signing/handoff: `UNSIGNED` / `BLOCKED`.
- External release authorization: `NO`.
- Provenance: `CONTENT_RECONCILED_PROVENANCE_OPEN`.

This review records local engineering evidence only. It is not formal validation,
a side-channel certification, a production-signing record, or release approval.
