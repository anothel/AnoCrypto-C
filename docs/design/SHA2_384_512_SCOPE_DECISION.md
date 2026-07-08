# SHA-384 / SHA-512 Scope Decision

Status: P1-15 draft.

## Decision

P1-15 keeps SHA-384 and SHA-512 as ABI-reserved hash algorithm IDs and does not add their compression-function implementations yet.

The current public behavior is intentionally limited:

- `ano_crypto_hash_algorithm_name()` returns stable names for SHA-384 and SHA-512.
- `ano_crypto_hash_digest_size()` returns 48 bytes for SHA-384 and 64 bytes for SHA-512.
- `ano_crypto_hash_context_size()` returns an opaque buffer size so callers can compile against the reserved IDs.
- `ano_crypto_hmac_mac_size()` returns the corresponding MAC size metadata.
- Hash and HMAC processing calls return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` for SHA-384 and SHA-512 based variants.

This preserves API shape without implying that SHA-384/SHA-512 processing, startup KATs, differential tests, or implementation review are complete.

## Why not implement in P1-15

The P1 baseline already exercises the full implementation/evidence loop with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, CSP key-handle lifecycle, install smoke, PoC package smoke, public-surface checks, and thread-safety policy checks.

Before adding a second SHA-2 word-size family, the project still needs:

1. A reviewed implementation plan for the 64-bit SHA-2 core.
2. Public KAT vector entries for SHA-384, SHA-512, HMAC-SHA-384, and HMAC-SHA-512.
3. Startup self-test policy updates.
4. Differential validation tooling using an independently selected public/reference source or tool.
5. Public API/ABI review for context-size growth and symbol surface impact.
6. Side-channel and portability review for 64-bit operations on supported targets.

## P1-15 guardrail

P1-15 adds an explicit reserved-API regression test and a scope policy check so the reserved algorithms cannot silently drift into partial or misleading behavior.

Required behavior:

| API family | SHA-384/SHA-512 P1-15 behavior |
|---|---|
| Algorithm name | Queryable |
| Digest/MAC size | Queryable |
| Context size | Queryable opaque size |
| One-shot hash | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` |
| Streaming hash init | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` |
| One-shot HMAC | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` |
| Streaming HMAC init | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` |
| Startup self-test | No SHA-384/SHA-512 startup KAT yet |
| Evidence | Scope/KAT plan only |

## Entry criteria for implementation

SHA-384/SHA-512 should enter the implemented candidate slice only after these are ready:

- `cleanroom/SOURCE_MANIFEST.md` records every public standard/vector source used.
- `src/selftest/kat_manifest.c` changes reserved entries to implemented or extended-test entries with matching tests.
- Startup self-test or startup-scope rationale is updated.
- `tests/kat` includes public vectors and streaming boundary coverage.
- Negative tests cover unsupported parameters, invalid state, output length, and finalized context behavior.
- `docs/kcmvp/SHA2_384_512_KAT_PLAN.md` is updated from plan to completed evidence mapping.
- `tools/check_sha2_scope_policy.cmake` is updated so partial implementation cannot bypass evidence expectations.

## Out of scope

- No SHA-384/SHA-512 compression implementation is added in P1-15.
- No HMAC-SHA-384/HMAC-SHA-512 processing implementation is added in P1-15.
- No new key material, entropy input, generated secret bytes, private keys, tokens, customer data, or internal deployment details are added.
- No broader marketing or validation claim is made by this decision.
