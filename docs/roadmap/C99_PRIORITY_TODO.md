# C99 Priority TODO

## Where to find current work — 2026-11-22 KST

The status, baseline and TODO below record the completed Phase 1 scope. Active
Strategy A work for internal SDK stabilization is tracked in the Master queue.
Policy v3 governs authorization of bounded implementation packets. Official SDK
artifact selection has its own record. [Current queue #15](https://github.com/anothel/AnoCrypto-C/issues/15) ·
[Policy v3 #13](https://github.com/anothel/AnoCrypto-C/issues/13#issuecomment-7069225969) ·
[SDK artifact tracking #30](https://github.com/anothel/AnoCrypto-C/issues/30)

[Strategy A checkpoint](CURRENT_ACTIVE_NEXT.md#strategy-a-checkpoint--2026-11-22-kst)
links the exact consumer evidence and postmerge follow-ups.
`EXTERNAL_RELEASE_NOT_AUTHORIZED`.

---

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
