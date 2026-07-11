# AES/CBC Implementation Policy

Status: P1-25 engineering implementation policy.

## Implemented slice

- AES-128, AES-192, and AES-256 single-block encrypt/decrypt.
- CBC without padding for complete, non-empty 16-byte blocks.
- Caller-provided 16-byte IV; no implicit IV generation or persistence.
- CSP `BLOCK_CIPHER` key handles with exact 16/24/32-byte key checks.
- Caller-owned opaque context and one-shot convenience processing.
- Exact in-place operation is supported; partial overlap is rejected.

## Key lifecycle

Cipher initialization copies a validated non-exportable in-memory CSP key into a
bounded local buffer, expands it into the caller-owned context, and clears the raw
copy before returning. Destroying the CSP handle after successful initialization
does not invalidate that already initialized context. `ano_crypto_cipher_uninit`
clears the caller-owned expanded schedule and chaining state.

## Portable implementation strategy

The portable AES core uses a table-free arithmetic S-box and inverse S-box based
on fixed-operation GF(2^8) multiplication/inversion. It does not use secret-indexed
lookup tables. Key size and round-count control flow is public parameter dependent.

This is a constant-time-oriented engineering strategy, not a certification or a
claim that every compiler/target emits constant-time machine code. Compiler output,
microarchitecture, cache behavior, fault behavior, and target-specific timing
review remain TODO before stronger statements.

## Deferred

- ECB as a general public data mode;
- CTR and counter-exhaustion policy;
- GCM, tags, AAD, nonce policy, and GMAC;
- automatic padding;
- hardware acceleration and dispatch;
- domestic algorithms pending explicit requirements;
- persistent or wrapped keys.
