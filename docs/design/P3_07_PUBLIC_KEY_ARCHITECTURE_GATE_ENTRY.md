# P3-07 Public-Key Key-Handle Architecture Gate Entry

Status: `READY_TO_START_DESIGN_ONLY`

## Baseline

- core version: `0.1.0-p3-05-scrypt-fixed-profile`;
- reviewed core commit: `89e16a261ab7696d9489167d7403d9f2c704629e`;
- public headers/symbols: 16/80;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_4`;
- retained compatibility baseline: P1-29;
- AnoSecureKit P3-06 parity/no-fallback: complete engineering checkpoint;
- selected subsequent public-key algorithm implementation: `NOT_SELECTED`.

## Gate objective

Freeze the minimal generic public-key key/CSP architecture needed by exact
AnoPKI Community/Enterprise operations before any restricted SHA-1, RSA, or
ECDSA implementation.

## Required decisions

1. public/private handle separation versus common tagged handle;
2. handle ownership, copy prohibition, destruction, zeroization, and stale use;
3. module shutdown/fatal-state invalidation;
4. thread-safety and concurrent sign/verify rules;
5. RSA/EC algorithm metadata and purpose binding;
6. algorithm-native key-component import representation;
7. rejection of PEM/DER/ASN.1 parsing at the core boundary;
8. message-based versus digest-based sign/verify API;
9. RSA DigestInfo ownership and digest selection;
10. ECDSA primitive signature representation;
11. size-query, null, overflow, alias, overlap, and output-clearing rules;
12. future ABI and smallest symbol budget;
13. KAT, negative, boundary, differential, and AnoPKI compatibility plan;
14. selection of at most one subsequent implementation slice.

## Non-goals

P3-07 must not:

- add or modify a public header, function, type, enum value, or symbol;
- introduce or reserve `ANOCRYPTO_C_5`;
- implement SHA-1, RSA, ECDSA, RSA-PSS, Ed25519, ECDH, or PQC;
- implement PEM, DER, ASN.1, PKCS#8, SPKI, CSR, X.509, CRL, or OCSP;
- implement key persistence/export/wrapping;
- implement HSM, KMS, PKCS#11, provider routing, retry, authorization, or audit;
- change KCMVP, hosted, signing, or release-authorization status.

## Required outputs

- exact AnoPKI operation/fixture inventory;
- ownership review and boundary decision;
- key-handle/CSP architecture decision record;
- key-ingress alternatives and selected boundary;
- sign/verify API alternatives and selected candidate contract;
- lifecycle, concurrency, buffer, and failure rules;
- proposed future symbol budget without ABI reservation;
- verification plan;
- `APPROVED`, `DEFERRED`, or `EXTERNAL_CONFIRMATION_REQUIRED` disposition for
  each unresolved item;
- at most one selected subsequent implementation slice.

## Exit criterion

The gate closes only when the design is self-consistent, exact downstream
requirements are cited, repository ownership is preserved, the current public
surface is unchanged, and one or zero future implementation slices are selected.
