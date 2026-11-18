# AnoPKI Requirement Trace

Status: `P3_11_LOCAL_PARITY_COMPLETE_PHASE_1_CLOSED`

## Final authority

| Item | Frozen identity |
|---|---|
| AnoPKI Enterprise | `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec` |
| Enterprise parent | `c3f7b679782f52c90311b7b7670b577ee972c1f6` |
| AnoPKI Community baseline | `ab9d76597df93ac1ac8b7938f4d25ba64f59f8dc` |
| Community source ZIP SHA-256 | `3a2a47573c3c9c8f5dac4e6c6eaa932bd7ab8580f64268d888a30ced5262e9c4` |
| AnoCrypto-C source | `98f544a574f70b387c300f171243b4e87174937a` / `0.1.0-p3-10-restricted-sha1-ocsp-certid` |
| AnoCrypto-C source ZIP SHA-256 | `913979ceea9110de5dbfe9e6702c634071102b00901840763367ca83e15d59c4` |
| Pinned AnoCrypto-C DLL SHA-256 | `7dd56b00f38699509e90a62968ec0ff2c37a2735e4479c0df4a2c60f8b2e26cd` |

The exact Git commit tree is the Enterprise source authority. The convenience
P3-11 Source ZIP is a handoff artifact and has a known CRLF conversion issue in
`tests/fixtures/backend-parity/manifest.txt`; it is not the canonical byte-level
source baseline.

## Operation trace

| Operation | AnoPKI-owned behavior | AnoCrypto-C primitive use | Phase 1 result |
|---|---|---|---|
| CSR inspect/PoP | PEM/DER parsing, CertificationRequestInfo extraction and policy | RSA/P-256 import and verify | PASS |
| certificate issue | profile, TBS, AlgorithmIdentifier, PEM/DER | RSA SHA-2 or P-256 sign | PASS |
| CRL generate/inspect | CRL construction, DER/PEM and policy | RSA/P-256 sign/verify | PASS |
| OCSP request inspect | request/nonce/CertID parsing | restricted CertID digest when required | PASS |
| OCSP issuer inspect | issuer Name/SPKI extraction and result formatting | dedicated restricted SHA-1 | PASS |
| OCSP response generate | response structure, status, nonce and DER | RSA/P-256 sign | PASS |
| OCSP responder validate | X.509/EKU/constraint policy | RSA/P-256 verify | PASS |

Eight Enterprise/AnoCrypto-C capabilities were reported truthfully and tested
with no OpenSSL fallback. RSA-PSS, generic SHA-1, P-384, unsupported providers,
malformed keys, mismatched bindings, and failed evidence paths remain
fail-closed.

## Limits retained

- local file keys are development/test only;
- no production HSM/KMS/PKCS#11 qualification;
- no hosted-success claim;
- no race-clean claim;
- no customer release, production readiness, KCMVP certification, signing, or timestamp claim.
