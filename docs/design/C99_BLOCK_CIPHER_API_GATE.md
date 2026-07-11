# C99 Block Cipher API Gate

Status: P1-24 design gate; no public API is added in this phase.

## Candidate service model

The next phase may add opaque caller-owned cipher contexts with separate size,
init, update/final or one-shot operations. Final signatures remain TODO until ABI
review. Required semantics are fixed here:

- C99 only;
- status return plus output parameters;
- exact key/IV/input/output validation;
- CBC without padding accepts only complete 16-byte blocks;
- zero-length processing is rejected for the first slice unless the final API
  review records a different rule;
- no implicit allocation, IV generation, padding, or key export;
- output is cleared on failures after output ownership begins;
- cryptographic service calls require the module operational state;
- contexts from another service or algorithm must be rejected.

## Aliasing candidate

Exact in-place operation (`input == output`) is a desired requirement for CBC,
but partial overlap is prohibited. This remains subject to implementation review
and must be regression-tested before being documented as supported.

## ABI decision

P1-24 adds no header, enum value, exported function, public struct, or symbol.
P1-25 must either preserve ABI-0 by using already reserved identifiers or submit a
reviewed ABI change with baseline update. No silent change is allowed.
