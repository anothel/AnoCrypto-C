# C99 Priority TODO

Status: `PHASE_1_CLOSED_NO_ACTIVE_TODO`

## Frozen baseline

- version: `0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- source commit: `98f544a574f70b387c300f171243b4e87174937a`;
- public headers: 18;
- exported symbols: 85;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_7`;
- P1-29 compatibility retained.

## Active TODO

`NONE`

Phase 1 met its engineering objective: AnoSecureKit and AnoPKI Enterprise both
consumed public AnoCrypto-C APIs and locally verified their selected product
paths without silent OpenSSL fallback.

## Reopen gate

Do not add an algorithm, API, provider, packaging promise, or release claim
without a new product decision. A future phase must explicitly define:

1. product or research objective;
2. repository and exact source baseline;
3. allowed API/ABI change;
4. validation and evidence requirements;
5. commercialization/release authority;
6. new numbered phase and owner.
