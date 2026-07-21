# AnoPKI Requirement Trace

Status: `P3_08_RSA_CORE_COMPLETE_ADAPTER_PENDING`

## Authority and verification

- Community exact commit:
  `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5` — directly re-read at the exact
  revision during P3-07.
- Community source SHA-256:
  `2f5b5750631a4af39293e10a420a6667ff430e682a3ab24bf3f3e54dbc298ac2`.
- Enterprise exact commit:
  `68b379e7684c9d8b44c53728949774088352a9d9`.
- Enterprise source SHA-256:
  `af5acc349deee69650a4767a51fdc0214cd529561a9ece906a18ab0eebe7033d`.
- Enterprise collection SHA-256:
  `ed55d2aa2ebdd561dc2549d01d1bd8f3fd0bd2d2144c3ce3b74083d86fb47704`.
- Enterprise raw ZIP byte recomputation in the P3-07 runtime:
  `EXTERNAL_CONFIRMATION_REQUIRED`.

Exact source and tests override stale embedded labels and generic OpenSSL call
summaries.

## Operation trace

| ID | AnoPKI operation/evidence | AnoPKI-owned behavior | AnoCrypto-C primitive consequence | P3-07 disposition |
|---|---|---|---|---|
| `APKI-PK-01` | CSR inspect and issue-time proof-of-possession | PEM/DER parse, subject/SAN/extensions, exact CertificationRequestInfo bytes | RSA/ECDSA verify | `APPROVED_SPLIT` |
| `APKI-PK-02` | certificate issue | profile/extensions/serial/TBSCertificate/signatureAlgorithm/PEM | RSA/ECDSA sign and key binding | `APPROVED_SPLIT` |
| `APKI-PK-03` | certificate inspect/issuer validation | X.509 parse, CA/key-usage/name/time/chain policy | signature verify only below policy | `APPROVED_SPLIT` |
| `APKI-PK-04` | CRL generate/inspect | TBSCertList, entries, reasons, times, PEM/DER | SHA-256 RSA/ECDSA sign | `APPROVED_SPLIT` |
| `APKI-PK-05` | OCSP request/issuer inspect | DER, nonce, CertID composition and normalized fields | SHA-1/SHA-2 digest only | `APPROVED_SPLIT` |
| `APKI-PK-06` | OCSP response and responder validation | BasicResponse/status/time/EKU/issuer policy and DER | SHA-256 RSA/ECDSA sign/verify | `APPROVED_SPLIT` |
| `APKI-PK-07` | RSA provider fixtures | RSA-2048 positive cert/CRL/OCSP, deterministic certificate DER, SHA-2 scheme selection | RSA PKCS#1 v1.5 SHA-256/384/512 | `SATISFIED_P3_08_CORE_ADAPTER_PENDING` |
| `APKI-PK-08` | ECDSA provider fixtures | P-256 positive cert/CRL/OCSP, TBS equivalence, DER signature object | P-256 SHA-256 raw primitive signature | `REQUIRED_NOT_SELECTED` |
| `APKI-PK-09` | key-provider failures | reference parsing, exportability, provider routing, evidence, no fallback | role/algorithm/binding checks only; routing stays downstream | `BOUNDARY_APPROVED` |
| `APKI-PK-10` | OCSP CertID default | empty digest selection and exact fixture use SHA-1 | restricted SHA-1 digest | `RECONFIRMED_DEFERRED` |

## Positive fixture requirements

- RSA-2048 and P-256 issuer keys must sign certificate, CRL, and OCSP paths.
- CSR proof-of-possession must be verified before issue.
- RSA direct/provider signatures must preserve exact deterministic certificate
  DER where inputs are identical.
- ECDSA provider paths must preserve TBS bytes, use `ecdsa-with-SHA256`, and
  verify successfully.
- OCSP CertID SHA-1 interoperability must remain distinguishable from signature
  algorithm policy.

## Negative fixture requirements

- malformed, encrypted, missing, or unavailable key sources fail closed;
- algorithm/key mismatch and certificate/private-key mismatch remain distinct;
- unsupported provider references do not retry through another provider;
- production exportable file keys are rejected;
- invalid CSR proof-of-possession and invalid signatures fail without output or
  fallback;
- raw key references, paths, passwords, key bytes, and signatures are not logged.

## Frozen P3-07 contract direction

- common opaque CSP token with internal public/private role and algorithm tags;
- normalized algorithm-native component import only;
- message-based sign/verify;
- core-owned SHA-2 and RSA DigestInfo;
- raw fixed-width P-256 `r || s`, with DER in AnoPKI;
- existing key/CSP lifecycle functions reused;
- implemented RSA slice: three new symbols on ANOCRYPTO_C_5;
- no ABI line reserved.

Detailed decisions are in
`docs/design/C99_PUBLIC_KEY_KEY_HANDLE_ARCHITECTURE_GATE.md` and exact product
extraction is in `docs/product/P3_07_PUBLIC_KEY_REQUIREMENT_REVIEW.md`.

## Current parity state

AnoPKI Enterprise public-key operations remain `unsupported_pending` until a
real later SDK and adapter path execute. P3-07 changes no public API/ABI and does
not claim public-key parity, hosted completion, signing, KCMVP validation, or
release authorization.
