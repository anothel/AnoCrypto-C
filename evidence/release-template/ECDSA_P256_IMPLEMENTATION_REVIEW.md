# ECDSA P-256 Implementation Review

- profile: P-256 with SHA-256;
- key import: fixed-width affine coordinates plus optional private scalar;
- private/public binding: import verifies `d*G == Q`;
- nonce: RFC 6979 HMAC-SHA-256;
- signature representation: 64-byte raw `r || s`;
- private state: CSP-owned, non-exported, zeroized on destroy/shutdown/fatal path;
- concurrency: operation lease protects started sign/verify operations;
- production OpenSSL dependency: none;
- side-channel posture: fixed-width loops, fixed-length carry propagation,
  fixed-step scalar ladder, branchless secret-sensitive modular selection, and
  RFC 6979; no complete target-specific constant-time verdict claimed;
- PKI/container logic: excluded;
- ABI: one symbol on `ANOCRYPTO_C_6`, total 84;
- hosted/signing/release/KCMVP states: unchanged and incomplete.
