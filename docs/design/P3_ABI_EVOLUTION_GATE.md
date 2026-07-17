# P3 API and ABI Evolution Gate

Status: `P3_02_ADDITIVE_ABI_APPROVED`

## Retained baseline

- public headers: 14;
- exported symbols: 69;
- existing ABI line: `ANOCRYPTO_C_0`.

P3-01 changes no public declaration or exported symbol.

## Approved P3-02 strategy

HKDF-SHA-256 may add a new public header `include/anocrypto/ac_kdf.h` and
additive symbols under a new symbol-version line `ANOCRYPTO_C_1`.
`ANOCRYPTO_C_0` symbols and retained P1-29 consumers must remain unchanged.

The precise function names and count are not approved by this document. P3-02
must complete API review before implementation and update all baselines only
after tests pass.

## P3-02 required evidence

- RFC 5869 KATs and output-length limits 0, 1, 32, 8160 and 8161;
- empty IKM/salt/info semantics;
- no partial output and output clearing on failure;
- integer overflow and alias rules;
- OpenSSL test-only differential validation;
- exact `SKF1` v1 info-byte compatibility case;
- old/current header consumers and symbol-version inspection;
- install/export/PoC and AnoSecureKit Enterprise adapter integration plan.

All later AEAD, scrypt and public-key APIs require separate numbered gates.
