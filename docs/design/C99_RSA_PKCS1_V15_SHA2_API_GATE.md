# C99 RSA PKCS#1 v1.5 SHA-2 API Gate

Status: `P3_08_API_ABI_APPROVED_IMPLEMENTED`

## Frozen public surface

P3-08 adds one public header, `ac_signature.h`, and exactly three functions:

- `ano_crypto_rsa_key_import`;
- `ano_crypto_signature_sign`;
- `ano_crypto_signature_verify`.

The resulting surface is 17 headers and 83 symbols. Existing ABI lines remain
unchanged; the three names above are the only names on `ANOCRYPTO_C_5`.

## Algorithm profile

- RSA modulus: exactly 2048 bits, unsigned big-endian, top bit set, odd;
- public exponent: exactly 65537;
- private ingress: full CRT components `n,e,d,p,q,dp,dq,qInv`;
- import representation: explicit pointer/length pairs; no public aggregate or encoded-container layout;
- signature schemes: RSASSA-PKCS1-v1_5 with SHA-256, SHA-384, or SHA-512;
- signature length: exactly 256 bytes;
- input model: caller message bytes; hashing and canonical DigestInfo belong to core.

PEM, DER, ASN.1, PKCS#1/PKCS#8/SPKI containers, CSR, X.509, CRL, and OCSP
remain downstream-owned.

## Handle and lifecycle contract

The existing opaque CSP handle is reused. Public imports create verify-only
public-key handles. Complete private imports create sign-capable private-key
handles; private handles may also verify. Handles are non-copyable values,
non-exportable through this API, stale after destroy/shutdown, and zeroized by
the existing CSP lifecycle. The bounded CSP table performs no production heap
allocation.

The P3-08 implementation serializes RSA access with the existing CSP lock. This
is a conservative operation lease: destroy and shutdown cannot erase a key
while sign/verify is active. It is correct but intentionally not a throughput
claim.

## Failure and buffer rules

- Size query is `out_signature == NULL && out_signature_len == 0`; it returns 256.
- Sign output must not overlap the message or `out_written`.
- Every non-query sign failure sets `out_written` to zero and clears the
  caller-provided output range.
- Verify accepts exactly 256 bytes and reports mathematical mismatch as
  `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED`.
- Invalid handles, role mismatch, malformed components, unsupported algorithms,
  overflow, and illegal overlap fail closed.

## Private-operation controls

CRT exponentiation uses independently blinded CRT exponents derived from the
platform entropy seam. The result is checked by reapplying the public exponent
before release. Fault-check failure returns `ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED`
and enters the sticky module error state.

This is engineering hardening evidence, not a universal constant-time or formal
side-channel certification claim. Further variance analysis remains appropriate
before any external validation or release authorization.
