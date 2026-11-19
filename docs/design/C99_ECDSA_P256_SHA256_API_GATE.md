# C99 ECDSA P-256 SHA-256 API Gate

Status: `APPROVED_AND_IMPLEMENTED_P3_09`

## Public delta

P3-09 adds exactly one public function, `ano_crypto_ec_p256_key_import`, to
`ac_key.h`. The existing generic `ano_crypto_signature_sign` and
`ano_crypto_signature_verify` functions are reused with
`ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256`.

The resulting surface is 17 public headers and 84 exported symbols. The single
new name is assigned to additive `ANOCRYPTO_C_6`; ABI lines 0 through 5 remain
unchanged.

## Fixed profile

- curve: NIST P-256 / secp256r1;
- digest: SHA-256;
- key ingress: 32-byte unsigned big-endian affine `x`, `y`, and optional private
  scalar `d`;
- public import: `x`, `y`, no scalar;
- private import: `x`, `y`, `d`, with import-time proof that `d*G == Q`;
- signature: fixed 64-byte raw `r || s`;
- nonce: deterministic RFC 6979 using HMAC-SHA-256;
- signature DER encoding: downstream-owned;
- SEC1, SPKI, PKCS#8, PEM, DER, ASN.1: downstream-owned.

## Failure and buffer contract

The existing generic signature contract applies. Size query returns 64 bytes.
Signing failures set `out_written` to zero and clear every supplied output byte.
Exact or partial message/output overlap is rejected. Verification accepts
exactly 64 bytes and returns `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED` for malformed
or mathematically invalid signatures. Public handles verify only; private
handles sign and may verify.

## Lifecycle and concurrency

P-256 keys use the shared CSP table. The implementation holds the global CSP
lock during key-slot access and the P-256 computation that uses it. Destroy and
lock-protected shutdown or fatal cleanup acquire the same lock before clearing
the stored key. Signing clears operation-local private intermediates before
returning. Stale handles fail closed. See the
[CSP lifecycle](C99_CSP_LIFECYCLE.md#p3-08p3-09-implemented-public-key-lifecycle)
for the caller's responsibility to coordinate shutdown with complete calls.

## Side-channel posture

The implementation uses fixed-width 256-bit arithmetic, fixed-length carry
propagation, a fixed 256-step scalar ladder, branchless result selection for
secret-sensitive modular reductions, deterministic RFC 6979 nonces, and private
state zeroization. These controls reduce obvious timing and state-leakage risks,
but P3-09 does not claim a complete target-specific constant-time verdict across
all compilers, microarchitectures, operating systems, or physical attack models.
Such a claim requires separate toolchain- and platform-specific review.

## Ownership

AnoCrypto-C owns the generic P-256 primitive, component validation, SHA-256,
RFC 6979, raw signature representation, CSP lifetime, and stable C ABI. AnoPKI
owns every PKI codec and DER signature conversion. Enterprise owns provider
selection, `key_ref`, HSM/KMS/PKCS#11 routing, authorization, retries, evidence,
and audit.
