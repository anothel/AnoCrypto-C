# Cross-Repository Requirement Ownership

Status: `PHASE_1_BOUNDARIES_VERIFIED_AND_FROZEN`

## Retained P3-01 policy compatibility

AnoPKI retains all codec, object-construction, and PKI-workflow ownership.

## Fixed ownership

| Area | Owner |
|---|---|
| generic hash, MAC, KDF, AEAD, RSA/P-256 primitives, restricted OCSP CertID digest, random, CSP/key lifecycle, module state, stable C ABI | AnoCrypto-C |
| `SKT1`, `SKF1`, `SKP1`, framing, file/packet workflow and output-commit policy | AnoSecureKit |
| ASN.1, PEM/DER, PKCS#10, X.509, CRL, OCSP, TBS construction, signature DER, chain/EKU/time policy | AnoPKI |
| provider selection, `key_ref`, authorization, evidence, audit, packaging and release policy | Enterprise product |
| production HSM/KMS/PKCS#11, retries and remote-operation semantics | future Enterprise decision |

## Verified Phase 1 boundaries

- AnoSecureKit consumed public symmetric/KDF/random AnoCrypto-C services without
  moving product formats into the module.
- AnoPKI consumed public RSA, P-256 and restricted OCSP digest services without
  moving PKI codecs/workflows into the module.
- ECDSA raw `r || s` to DER conversion stayed in AnoPKI.
- AnoPKI provider routing, issuer binding, evidence and no-fallback policy stayed
  in Enterprise.
- No selected AnoCrypto-C profile silently switched to OpenSSL.

## Frozen limits

- a local AnoCrypto-C key handle is not an HSM/KMS identity;
- no key persistence/export/wrapping was approved;
- no new algorithm or public ABI is selected;
- commercialization and external release remain undecided/blocked.
