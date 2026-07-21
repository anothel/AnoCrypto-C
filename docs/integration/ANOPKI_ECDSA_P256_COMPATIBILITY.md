# AnoPKI ECDSA P-256 Compatibility

Status: `P3_09_PRIMITIVE_COMPATIBILITY_COMPLETE_ADAPTER_PENDING`

P3-09 supplies only the generic primitive beneath AnoPKI. The differential
fixture covers message classes corresponding to CSR proof-of-possession,
certificate TBS signing, CRL TBS signing, and OCSP response TBS signing.

Compatibility evidence includes:

- AnoCrypto-C deterministic raw `r || s` signatures verified by OpenSSL;
- OpenSSL DER ECDSA signatures converted test-only to raw `r || s` and verified
  by AnoCrypto-C;
- invalid, truncated, zero, and out-of-range `r`/`s` rejection;
- public/private key role enforcement;
- key-point and private/public binding validation;
- concurrent signing and destroy-race behavior.

AnoPKI remains responsible for CSR/X.509/CRL/OCSP parsing and construction,
ECDSA-Sig-Value DER encoding/decoding, certificate/key binding at the product
boundary, and normalized product errors. Enterprise adapter wiring and
no-fallback parity are separate work. No OpenSSL production fallback is added.
