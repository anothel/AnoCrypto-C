# AnoPKI Requirement Trace

## Authority

- Community: `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5`.
- Enterprise: `68b379e7684c9d8b44c53728949774088352a9d9`.

## Operation ownership

| Operation | AnoPKI-owned behavior | Candidate AnoCrypto-C portion |
|---|---|---|
| CSR inspect | PEM/DER parsing, subject/SAN/extensions, normalized result | SHA-2 and RSA/ECDSA signature verification |
| certificate issue | TBSCertificate/extensions/serial/profile/PEM | random if selected, RSA/ECDSA signing, key binding |
| CRL generate/inspect | TBSCertList encoding/parsing and revocation semantics | SHA-2 and RSA/ECDSA signing |
| OCSP request/issuer inspect | DER/CertID/nonce parsing and issuer semantics | SHA-1/SHA-256 digest interoperability |
| OCSP response/validation | BasicResponse encoding, status/time/EKU/chain policy | SHA-2, RSA/ECDSA sign/verify |

## Decision

AnoPKI retains all codec and workflow responsibility. AnoCrypto-C will expose
only generic primitives and key lifecycle services. Current Enterprise adapter
operations remain `unsupported_pending` until real SDK-backed parity tests run.
