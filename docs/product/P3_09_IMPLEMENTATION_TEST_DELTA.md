# P3-09 Implementation and Test Delta

Version: `0.1.0-p3-09-ecdsa-p256-sha256`

## Implementation

- added one public P-256 component-import function;
- reused generic sign/verify with an ECDSA P-256 SHA-256 algorithm selector;
- added deterministic RFC 6979 HMAC-SHA-256 nonce generation;
- added fixed raw 64-byte `r || s` signatures;
- added P-256 field/order arithmetic and point validation without OpenSSL;
- used fixed-width loops, fixed-length carry propagation, fixed-step scalar
  multiplication, and branchless secret-sensitive modular selection;
- added CSP role, stale-handle, zeroization, and operation-lease integration;
- added one symbol on `ANOCRYPTO_C_6`;
- retained all 83 symbols on ABI lines 0 through 5.

## Tests

- RFC 6979 known-answer vectors for `sample` and `test`;
- empty and ordinary message sign/verify;
- invalid point, scalar, key binding, role, alias, buffer, and signature cases;
- OpenSSL test-only bidirectional differential validation for AnoPKI operation
  message classes;
- concurrent deterministic signing and destroy race;
- current and retained P1-29 header consumers;
- release export and symbol-version checks;
- install/export, PoC, provenance, and evidence regression.

## Assurance boundary

P3-09 validates functional correctness, API/ABI behavior, memory-safety builds,
and the documented fixed-width side-channel controls. It does not claim a
complete target-specific constant-time verdict for every compiler, CPU, OS, or
physical attack model.

## Non-goals

No PEM, DER, ASN.1, SEC1, SPKI, PKCS#8, CSR, certificate, CRL, OCSP, provider
routing, HSM/KMS/PKCS#11, RSA-PSS, Ed25519, ECDH, PQC, production signing, or
external release work is included.
