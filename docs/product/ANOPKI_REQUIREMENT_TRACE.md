# AnoPKI Requirement Trace

Status: `P3_07_PUBLIC_KEY_ARCHITECTURE_GATE_READY`

## Authority

- Community: `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5`.
- Enterprise: `68b379e7684c9d8b44c53728949774088352a9d9`.
- Exact source ZIPs and collection manifests override stale embedded revision
  labels.

## Operation ownership

| Operation | AnoPKI-owned behavior | Candidate AnoCrypto-C portion |
|---|---|---|
| CSR inspect | PEM/DER parsing, subject/SAN/extensions, normalized result | SHA-2 and RSA/ECDSA signature verification |
| certificate issue | TBSCertificate/extensions/serial/profile/PEM | random if selected, RSA/ECDSA signing, key binding |
| CRL generate/inspect | TBSCertList encoding/parsing and revocation semantics | SHA-2 and RSA/ECDSA signing |
| OCSP request/issuer inspect | DER/CertID/nonce parsing and issuer semantics | restricted SHA-1/SHA-256 digest interoperability |
| OCSP response/validation | BasicResponse encoding, status/time/EKU/chain policy | SHA-2, RSA/ECDSA sign/verify |

## P3-07 gate questions

P3-07 must review and decide, without implementation:

- public/private key handle separation and algorithm metadata binding;
- algorithm-native key import versus encoded-container input;
- message-based versus digest-based sign/verify services;
- RSA PKCS#1 v1.5 digest selection and DigestInfo ownership;
- ECDSA signature representation at the core boundary;
- key ownership, destruction, zeroization, module-state, and concurrency rules;
- size query, null, overflow, alias, overlap, and failure-output contracts;
- the smallest future public symbol budget;
- at most one subsequent implementation slice.

## Decision boundary

AnoPKI retains all codec and workflow responsibility. AnoCrypto-C may expose
only generic primitives and key lifecycle services after a separate API/ABI
review. Current AnoPKI Enterprise public-key operations remain
`unsupported_pending`; P3-07 does not claim SDK-backed parity and does not add a
public API or ABI line.
