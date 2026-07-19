# P3-07 Public-Key Requirement Review

Status: `COMPLETE_ARCHITECTURE_APPROVED_RSA_NEXT`

## Intake identity

| Source | Exact identity | P3-07 result |
|---|---|---|
| AnoPKI Community | commit `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5`; source SHA-256 `2f5b5750631a4af39293e10a420a6667ff430e682a3ab24bf3f3e54dbc298ac2` | exact commit and relevant source/test paths re-read; `VERIFIED_EXACT_COMMIT` |
| AnoPKI Enterprise | commit `68b379e7684c9d8b44c53728949774088352a9d9`; source SHA-256 `af5acc349deee69650a4767a51fdc0214cd529561a9ece906a18ab0eebe7033d`; collection SHA-256 `ed55d2aa2ebdd561dc2549d01d1bd8f3fd0bd2d2144c3ce3b74083d86fb47704` | exact identity and collection records cross-checked; raw source ZIP byte recomputation `EXTERNAL_CONFIRMATION_REQUIRED` |
| P3-07 CURRENT ONLY pack | 12 Markdown entries | `PACK_MANIFEST.sha256` verified 12/12 |
| AnoCrypto-C working source | SHA-256 `1aac40228d16978b44ec4b95fc9779f98ba1605248c40efd6afc7e35132563bc` | verified before modification |

The Enterprise raw-archive limitation does not authorize inference about
private adapter implementation. The approved core architecture relies on exact
Community operation/tests plus Enterprise ownership, profile, no-fallback, and
provider-boundary records. Enterprise implementation-specific details remain
`EXTERNAL_CONFIRMATION_REQUIRED` until the exact archive is locally rehashed and
reviewed.

## Exact Community paths reviewed

- `include/anopki/crypto/backend.hpp`;
- `include/anopki/core/csr.hpp`, `issue.hpp`, `crl.hpp`, and `ocsp.hpp`;
- `src/backends/openssl/csr.cpp`, `issue.cpp`, `crl.cpp`, and `ocsp.cpp`;
- `src/backends/openssl/key_providers/file_key_provider.hpp` and `.cpp`;
- `src/backends/openssl/key_providers/provider_resolver.hpp` and `.cpp`;
- `tests/core_issue_profile_test.cpp`;
- `tests/file_key_provider_test.cpp`;
- `tests/crl_file_key_provider_test.cpp`;
- `tests/ocsp_file_key_provider_test.cpp`.

These paths were used to extract behavior and fixtures only. No OpenSSL or PKI
implementation was copied into AnoCrypto-C.

## Operation-level extraction

| Operation | Exact downstream behavior | Core capability consequence | Disposition |
|---|---|---|---|
| CSR inspect | parses PEM/DER, reports RSA/ECDSA key and signature metadata; issue path verifies CSR proof-of-possession | public-key verify over exact CSR TBS bytes | `APPROVED_REQUIREMENT` |
| certificate issue | AnoPKI builds TBSCertificate/profile/extensions/serial and resolves issuer key; current source supports RSA/ECDSA SHA-2 signing | message-based RSA/ECDSA sign; key binding | `APPROVED_REQUIREMENT` |
| certificate inspect/validation | AnoPKI owns parse, chain, profile, and normalized results | primitive verify only where adapter requires it | `APPROVED_SPLIT` |
| CRL generate | AnoPKI builds TBSCertList; provider signs with SHA-256; RSA and P-256 ECDSA success fixtures exist | SHA-256 RSA/ECDSA sign | `APPROVED_REQUIREMENT` |
| CRL inspect | AnoPKI parses issuer, number, entries, times, and DER/PEM | no new core codec | `PRODUCT_OWNED` |
| OCSP request inspect | AnoPKI parses DER, nonce, CertID and digest identifier | SHA-1/SHA-2 digest interoperability only | `APPROVED_SPLIT` |
| OCSP issuer inspect | empty digest selection normalizes to SHA-1; SHA-1/256/384/512 are recognized | restricted SHA-1 need reconfirmed | `RECONFIRMED_RESTRICTED_DEFERRED` |
| OCSP response | AnoPKI builds BasicResponse and status entries; response signing uses SHA-256; RSA and P-256 ECDSA fixtures exist | SHA-256 RSA/ECDSA sign | `APPROVED_REQUIREMENT` |
| responder validation | AnoPKI owns issuer/EKU/time policy and verifies responder certificate signature | primitive public-key verify under product codec/policy | `APPROVED_SPLIT` |
| provider use | move-only provider handle, exact key binding, stable failures, no fallback, evidence without raw key reference | local core handle must remain non-exportable and purpose-bound; provider routing stays outside core | `BOUNDARY_APPROVED` |

## Positive evidence inventory

- RSA-2048 issuer keys are used in certificate, CRL, and OCSP provider tests.
- P-256 issuer keys are used in certificate, CRL, and OCSP provider tests.
- RSA certificate provider output is compared with direct deterministic DER and
  verified by the issuer public key.
- ECDSA certificate/CRL tests compare unchanged TBS bytes, assert the
  `ecdsa-with-SHA256` identifier, and verify the result.
- OCSP requests in the exact provider fixture use a SHA-1 CertID.
- RSA and ECDSA OCSP responses are cryptographically verified and retain the
  requested certificate status.
- Certificate issuance accepts SHA-256/384/512 digest families through the
  provider compatibility path; CRL and OCSP response signing are SHA-256 in the
  exact source.
- CSR proof-of-possession is verified before issuance.
- A dedicated exact mutated-but-parseable CSR signature test was not located in
  the reviewed exact test set; future compatibility coverage is
  `EXTERNAL_CONFIRMATION_REQUIRED`.

## Negative evidence inventory

| Negative | Required future behavior |
|---|---|
| empty or malformed key reference | fail before signing |
| missing key file/directory/malformed PEM | stable fail-closed provider error |
| encrypted PEM without a password channel | reject without prompting or leaking password/key data |
| requested RSA scheme with EC key or ECDSA scheme with RSA key | algorithm mismatch |
| private key not bound to issuer certificate | binding mismatch |
| file provider in production | exportability violation |
| unsupported `kms:` reference | provider unavailable; no retry as file path |
| selected malformed provider input with another valid key present | fail selected provider; no fallback search |
| modified/invalid CSR signature | issue path must fail proof-of-possession; a dedicated exact mutated-signature fixture was not located and is `EXTERNAL_CONFIRMATION_REQUIRED` |
| invalid issuer CA/time/name constraints | reject before issuance |
| unsupported Ed25519 OCSP signer | algorithm mismatch; no implicit support |

## Ownership review

### AnoCrypto-C

- generic normalized RSA/P-256 key objects;
- role/algorithm/purpose binding and consistency validation;
- message hashing and RSA/ECDSA sign/verify primitives;
- CSP lifecycle, zeroization, stale-handle, module-state, concurrency, and C ABI.

### AnoPKI

- every PEM/DER/ASN.1 and PKI object codec;
- exact TBS construction and signature identifier encoding;
- ECDSA DER conversion;
- CSR/certificate/CRL/OCSP policy and normalized results.

### AnoPKI Enterprise

- AnoCrypto-C adapter and explicit profile selection;
- `key_ref`, HSM/KMS/PKCS#11 provider routing;
- capability masks, stable adapter errors, no fallback, evidence, audit,
  packaging, and release gates.

## Architecture decisions

- `APPROVED`: reuse common opaque `ano_crypto_csp_handle_t` with internal role
  and algorithm tags.
- `APPROVED`: reuse existing key destroy/state and CSP validity/state functions.
- `APPROVED`: one algorithm-specific import function per algorithm family,
  selected by public/private role.
- `APPROVED`: RSA inputs use big-endian `n,e` for public and
  `n,e,d,p,q` for two-prime private import; core validates consistency and
  derives CRT values.
- `APPROVED`: P-256 public point is uncompressed 65-byte SEC1; private import
  also requires the 32-byte scalar and expected public point.
- `APPROVED`: message-based generic sign and verify functions.
- `APPROVED`: AnoCrypto-C owns SHA-2 and canonical RSA DigestInfo construction.
- `APPROVED`: P-256 signatures cross the primitive boundary as 64-byte raw
  `r || s`; AnoPKI owns DER.
- `APPROVED_CANDIDATE_BUDGET`: three new symbols for the RSA slice and one later
  P-256 import symbol while reusing generic sign/verify; no public metadata
  query is justified by current product evidence.
- `DEFERRED`: digest-based public API, key export, key generation, encoded
  container import, HSM/KMS routing, ECDH, RSA-PSS, Ed25519, and PQC.
- `TODO_P3_08`: exact RSA modulus/exponent allow-list, bounded storage capacity,
  exact KAT set, header placement, and final public names/prototypes.
- `TODO_P3_09`: ECDSA nonce/determinism and low-`s` policy.
- `EXTERNAL_CONFIRMATION_REQUIRED`: byte-for-byte Enterprise source archive
  recomputation in an environment containing the exact ZIP.

## Selection of the next slice

P3-07 selects exactly one subsequent implementation slice:

`P3-08: RSA PKCS#1 v1.5 SHA-256/384/512 and RSA key-handle import`

Reasoning:

- RSA-2048 is the dominant exact certificate/CRL/OCSP fixture path.
- RSA signatures are deterministic, allowing exact cross-provider comparison in
  addition to verification.
- The slice exercises the approved handle, import, binding, lifecycle, and
  generic sign/verify architecture.
- P-256 ECDSA remains positively required but follows after the shared API is
  proven.
- Restricted SHA-1 is reconfirmed only for OCSP CertID and needs a separate
  misuse-resistant API policy; it is not an issuance-signature prerequisite.

P3-08 is not implemented by this review. Its API/ABI gate must run before any
public change and must not assume that the candidate names or count are already
approved.
