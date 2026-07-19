# Next AnoCrypto-C Work Prompt — P3-08

```text
Continue AnoCrypto-C from the completed P3-07 design-only public-key
architecture gate.

Current runtime baseline:
- version: 0.1.0-p3-05-scrypt-fixed-profile
- reviewed implementation commit: 89e16a261ab7696d9489167d7403d9f2c704629e
- public headers: 16
- exported symbols: 80
- ABI lines: ANOCRYPTO_C_0 through ANOCRYPTO_C_4
- retained compatibility baseline: P1-29

Completed P3-07 decisions:
- reuse ano_crypto_csp_handle_t as the common opaque handle
- public handles are verify-only; private handles are sign-only
- import normalized algorithm-native components, not encoded key containers
- use one role-selected RSA import candidate
- use generic message-based signature sign/verify candidates
- AnoCrypto-C owns SHA-256/384/512, canonical RSA DigestInfo, and
  EMSA-PKCS1-v1_5 processing
- use operation leases for sign/verify versus destroy/shutdown
- reuse existing destroy/state/is-valid lifecycle functions
- candidate public-function budget ceiling: 3
- public API/ABI/runtime change in P3-07: none
- ANOCRYPTO_C_5: not reserved

Task:
P3-08: RSA PKCS#1 v1.5 SHA-256/384/512 and RSA key-handle import

Required sequence:
1. Reconfirm exact AnoPKI RSA requirements and fixtures. RSA-2048 is positively
   evidenced. Review 3072/4096 support and mark unconfirmed sizes TODO or
   EXTERNAL_CONFIRMATION_REQUIRED. Treat RSA-1024 inspection as policy metadata,
   not import authorization.
2. Freeze bounded asymmetric storage capacity, object size, operation memory,
   exhaustion behavior, handle-index partitioning, active-lease ceiling, and
   destroy/shutdown waiting behavior.
3. Freeze the smallest C99 API. P3-07 candidate ceiling:
   - one RSA public/private component import selected by role
   - one generic message-based signature sign function
   - one generic message-based signature verify function
   Exact names, prototypes, component descriptors, scheme enum, flags, header
   placement, and final count are not pre-approved.
4. Reuse ano_crypto_csp_handle_t and existing key destroy/state and CSP
   validity/state functions.
5. Accept canonical unsigned big-endian RSA components. Public role requires
   n/e and zero private lengths. Private role requires n/e/d/p/q. Validate
   canonical form, two-prime consistency, exponent relationships, and derive
   and validate CRT values internally before success.
6. Keep PEM, DER, ASN.1, PKCS#8, SPKI, PKCS#1 containers, CSR, X.509, CRL, and
   OCSP codecs in AnoPKI.
7. Implement message-based RSA PKCS#1 v1.5 with SHA-256/384/512. Empty message is
   valid. AnoCrypto-C hashes the exact message/TBS bytes, constructs canonical
   DigestInfo, and strictly encodes/verifies EMSA-PKCS1-v1_5. Do not expose raw
   RSA, caller-provided DigestInfo, or a public prehashed API in this slice.
8. Define exact null, overflow, alias, overlap, size-query, short-buffer,
   no-partial-output, output-clearing, invalid-signature, role/scheme mismatch,
   stale-handle, shutdown, fatal-state, and concurrent-destroy rules.
9. Add public-source KATs, component negatives, blinding/fault/zeroization,
   lifecycle/exhaustion/concurrency-lease tests, OpenSSL test-only differential
   validation, and exact AnoPKI CSR proof-of-possession, certificate, CRL, OCSP
   response, and responder-verification TBS-byte compatibility evidence.
10. Preserve every existing public declaration and ABI assignment. Introduce one
    additive ABI line only after the API review passes.
11. Update current and retained consumers, install/export, PoC, policy, evidence,
    symbol-version, public-surface, clean-room, and roadmap manifests.
12. Keep KCMVP, hosted, signing, provenance, and external release states
    unchanged.

Required failure rules:
- out_handle is ANO_CRYPTO_CSP_INVALID_HANDLE on every import failure
- no partially live object and no retained caller pointer on import failure
- no partial signature on failure
- size query performs no signing
- short sign buffer reports required size, writes no signature, and clears the
  caller-authorized output span
- any non-empty, safely addressed signature that is wrong, modified, truncated,
  overlong, or non-canonical returns ANO_CRYPTO_ERR_AUTHENTICATION_FAILED after
  all non-cryptographic inputs validate
- malformed key-component lengths, unsupported schemes, wrong role, null misuse,
  and wrapping/unsafe ranges return the corresponding argument/parameter status
  before verification
- no fallback to OpenSSL or another profile
- no private key, message, digest, signature, TBS, certificate, credential, or
  customer value in logs/evidence

Prohibited:
- no ECDSA, SHA-1, RSA-PSS, Ed25519, ECDH, or PQC implementation
- no public digest/prehashed or streaming signature API in this slice
- no encoded PKI/key-container parsing
- no HSM/KMS/PKCS#11 or key_ref routing
- no key generation, export, persistence, wrapping, or high-level PKI API
- no push, tag, production signing, timestamp, or external distribution

Unresolved items must be marked APPROVED, DEFERRED, TODO, or
EXTERNAL_CONFIRMATION_REQUIRED.

Recommended artifact label:
- p3-08-rsa-pkcs1-v15-sha2

Recommended commit title:
- feat(pkey): add RSA PKCS1 v1.5 SHA-2 with additive ABI
```
