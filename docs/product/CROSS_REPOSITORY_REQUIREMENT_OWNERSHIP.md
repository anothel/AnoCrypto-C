# Cross-Repository Requirement Ownership

Status: `P3_09_PUBLIC_KEY_SOFTWARE_CORE_IMPLEMENTED`

## Retained P3-01 policy compatibility

The historical P3-01 policy statement remains true: **AnoPKI retains all codec**
work, object construction, and PKI workflow ownership. P3-07 narrows only the
generic primitive/key-handle boundary and does not transfer those responsibilities.

## Fixed ownership

| Area | Owner |
|---|---|
| Community public API, CLI, format, and backend-neutral semantics | corresponding Community repository |
| Enterprise profile, diagnostics, packaging, capability mask, and no-fallback adapter | corresponding Enterprise repository |
| Generic hash, MAC, KDF, AEAD, public-key primitive, random, CSP/key lifecycle, module state, and stable C ABI | AnoCrypto-C after an explicit gate |
| `SKT1`, `SKF1`, `SKP1`, framing, nonce composition, and application output policy | AnoSecureKit |
| ASN.1, PEM/DER, PKCS#10, X.509, CRL, OCSP, chain/EKU/time policy, and normalized results | AnoPKI |
| `key_ref`, provider selection, HSM/KMS/PKCS#11, retries, authorization, signing evidence, and audit | Enterprise product |

## Approved public-key boundary

| Concern | AnoCrypto-C | AnoPKI / Enterprise |
|---|---|---|
| handle identity | common opaque `ano_crypto_csp_handle_t` with internal role/algorithm/purpose tags | adapters retain only opaque tokens for the local module path |
| lifecycle | non-exportable in-memory import, stale rejection, destroy, zeroization, module-state, concurrency lease | provider custody, authorization, persistence, recovery, and external provider lifetime |
| key ingress | normalized RSA and P-256 components | PEM/DER/ASN.1/PKCS#8/SPKI parsing and product policy |
| binding | mathematical component consistency and private/public material match supplied at import | extract issuer/CSR public material and select the expected binding |
| sign/verify | generic message-based RSA/ECDSA primitive | exact TBS construction, signatureAlgorithm identifiers, object embedding, result policy |
| RSA encoding | SHA-2, DigestInfo, EMSA-PKCS1-v1_5 | PKI object identifiers and TBS bytes |
| ECDSA encoding | raw fixed-width `r || s` | canonical ASN.1 DER conversion |
| external key | no `key_ref`, HSM, KMS, PKCS#11, retry, evidence, or audit | Enterprise-owned, possibly signing without private-key export |

## Rejected or deferred ownership transfers

- AnoCrypto-C does not parse or emit PKI objects or key containers.
- AnoCrypto-C does not implement a product key-provider router.
- AnoPKI does not reimplement RSA/ECDSA/SHA algorithms inside an adapter.
- Enterprise does not treat a local AnoCrypto-C handle as an HSM/KMS identity.
- OpenSSL call sequences are implementation evidence, not a core API proposal.
- No AnoCrypto-C profile silently falls back to OpenSSL.

## Resolved P3-07 questions

- common internally tagged handle: `APPROVED`;
- algorithm-native components: `APPROVED`;
- message-based sign/verify: `APPROVED`;
- RSA DigestInfo in core: `APPROVED`;
- P-256 raw `r || s`: `APPROVED`;
- RSA budget of three symbols: `IMPLEMENTED_P3_08_ANOCRYPTO_C_5`;
- RSA slice: `COMPLETE_P3_08_CORE_ADAPTER_PENDING`;
- ECDSA P-256 slice: `COMPLETE_P3_09_CORE_ADAPTER_PENDING`;
- P-256 budget: one import symbol on `ANOCRYPTO_C_6`; generic sign/verify reused.

## Remaining decisions

- RSA-2048/e=65537 and final prototypes: `APPROVED_IMPLEMENTED_P3_08`;
- bounded software asymmetric storage and operation lease: `APPROVED_IMPLEMENTED`;
- ECDSA P-256/SHA-256, RFC 6979, raw `r || s`: `APPROVED_IMPLEMENTED_P3_09`;
- low-`s` normalization: `DEFERRED_NOT_REQUIRED_FOR_CURRENT_ANOPKI_COMPATIBILITY`;
- restricted SHA-1 public API containment: `TODO_P3_10_GATE`;
- exact Enterprise raw archive byte recomputation:
  `EXTERNAL_CONFIRMATION_REQUIRED`;
- HSM/KMS/PKCS#11 integration: `DEFERRED_ENTERPRISE_OWNED`.
