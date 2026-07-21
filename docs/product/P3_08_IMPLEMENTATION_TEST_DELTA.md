# P3-08 Implementation and Test Delta

Version: `0.1.0-p3-08-rsa-pkcs1-v15-sha2`

## Public delta

- new `include/anocrypto/ac_signature.h`;
- `ac_key.h` gains algorithm-native RSA component import through explicit pointer/length pairs with no public struct layout;
- three new symbols on additive `ANOCRYPTO_C_5`;
- total: 17 public headers and 83 exported symbols;
- ABI-0 through ABI-4 and P1-29 consumers remain unchanged.

## Implementation delta

- bounded public/private RSA CSP storage;
- RSA-2048/e=65537 component validation;
- SHA-256/384/512 DigestInfo and EMSA-PKCS1-v1_5;
- public modular exponentiation and CRT private operation;
- CRT exponent blinding through the platform entropy seam;
- post-private public fault check and sticky error transition;
- failure-output clearing and operation/destroy/shutdown serialization.

## Evidence delta

- deterministic SHA-256/384/512 KATs and empty-message case;
- negative, malformed-component, role, stale-handle, capacity, overlap,
  short-buffer, and failure-clearing coverage;
- OpenSSL 3 test-only differential coverage;
- current and retained header consumers;
- installed-package, PoC, clean export, ELF symbol-version, sanitizer, and
  local CTest regression.

## Explicit non-delta

No ECDSA, SHA-1, RSA-PSS, Ed25519, ECDH, PQC, PKI codec, external-provider
routing, key persistence/export/wrapping, hosted success, signing, release, or
KCMVP claim is added.
