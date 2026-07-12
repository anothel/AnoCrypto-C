# CMAC Selection Decision

Status: P1-28 engineering implementation decision.

## Decision

AnoCrypto-C implements a narrow AES-CMAC service before considering AES-CTR.
The implemented slice is limited to:

- AES-CMAC with AES-128, AES-192, and AES-256 keys;
- complete 16-byte tags only;
- arbitrary message length, including the empty message;
- one-shot computation and verification;
- streaming computation through caller-owned opaque context storage;
- in-memory CSP handles with a dedicated CMAC key purpose;
- startup and extended public-source KAT coverage;
- test-only independent differential comparison;
- no key export, persistence, wrapping, truncation, or protocol formatting.

## Rationale

AES-CMAC adds a bounded message-authentication primitive while avoiding the
counter construction, uniqueness, exhaustion, clone, and recovery policy that
would accompany AES-CTR. CMAC is not treated as AEAD and does not approve any
protocol construction.

## Key-purpose decision

`ANO_CRYPTO_KEY_TYPE_CMAC` is distinct from
`ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER`. CBC rejects CMAC-purpose handles, and CMAC
rejects encryption-purpose handles. Callers may import identical byte values
as separate keys, but AnoCrypto-C enforces handle purpose rather than external
key-separation policy.

## Deferred scope

- tag truncation;
- AES-CTR, GCM, GMAC, and padding helpers;
- protocol or KDF formatting;
- persistent or exportable CMAC keys;
- a claim that target-specific side-channel review is complete.

The clean Release ABI-0 baseline is 69 exported names after the reviewed eight
CMAC functions are added.
