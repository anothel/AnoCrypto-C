# CMAC Selection Decision

Status: P1-27 engineering entry gate. No CMAC code or public API is implemented by this step.

## Decision

The next algorithm-expansion candidate after the implemented AES/CBC slice is a
narrow AES-CMAC service rather than AES-CTR.

The proposed future slice is limited to:

- AES-CMAC with AES-128, AES-192, and AES-256 keys;
- complete 16-byte tags only;
- arbitrary message length, including the empty message;
- one-shot computation and verification;
- streaming computation through caller-owned opaque context storage;
- in-memory CSP key handles with a dedicated CMAC key purpose;
- startup and extended public-source KAT coverage;
- test-only independent differential comparison;
- no key export, persistence, wrapping, truncation, or protocol formatting.

## Why CMAC is selected before CTR

AnoCrypto-C already has a narrow confidentiality primitive through AES-CBC
without padding. AES-CTR would add another unauthenticated confidentiality mode
with counter construction, uniqueness, wrap, persistence, clone, and recovery
responsibilities that cannot be safely hidden by a low-level API.

CMAC instead adds a bounded message-authentication primitive that can later
support reviewed integrity services or KDF planning. This does not make CMAC a
replacement for AEAD and does not approve a protocol construction.

## Key-purpose decision

A future CMAC implementation must not silently reuse the generic
`ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER` service authorization. The implementation
step must add an explicit CMAC key purpose, currently proposed as a new enum
value such as `ANO_CRYPTO_KEY_TYPE_CMAC`, or an equally explicit reviewed usage
mechanism before any public CMAC operation is accepted.

Callers may choose identical byte values for separate keys, but AnoCrypto-C
must require separately imported handles for encryption and CMAC use. The
module cannot prove external key separation; it can enforce handle purpose.

## Explicitly deferred

P1-27 does not add:

- `include/anocrypto/ac_cmac.h`;
- a CMAC enum value or public function;
- `src/mac/cmac.c` or internal CMAC state;
- AES-CTR, GCM, GMAC, padding, protocol formatting, or tag truncation;
- a claim that hosted or side-channel review is complete.

The clean Release public-symbol baseline remains 61 names.
