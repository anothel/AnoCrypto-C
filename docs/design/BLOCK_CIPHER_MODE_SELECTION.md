# Block Cipher and Mode Selection Decision Record

Status: P1-24 entry gate approved for the next implementation slice.

## Decision

P1-24 does not add block-cipher processing code or public symbols. It approves a
narrow P1-25 implementation entry slice:

- algorithm: AES only;
- key sizes: 128, 192, and 256 bits;
- first primitive: internal single-block encrypt/decrypt for KAT and mode reuse;
- first public mode candidate: CBC without padding;
- block size: 16 bytes;
- IV size: exactly 16 bytes for CBC;
- caller supplies complete blocks; padding belongs outside AnoCrypto-C;
- ECB is not proposed as a general customer-facing data mode;
- AEAD, CTR, CMAC, GMAC, domestic algorithms, and automatic padding remain deferred.

The choice is an engineering entry decision, not a validation or customer-scope
claim. A customer or validation-target requirement may reopen the decision.

## Public API gate

Before P1-25 merges, an API review must approve:

- a new `ac_cipher.h` public header;
- opaque caller-owned context sizing rather than a public struct layout;
- `ano_crypto_status_t` return values and output parameters;
- explicit algorithm, direction, mode, key-size, and IV-size validation;
- exact output-length rules and `written` behavior;
- documented in-place aliasing policy;
- no automatic padding and no hidden IV generation;
- module-state/self-test gating identical to existing cryptographic services.

P1-24 intentionally does not add the header because names and signatures are not
ABI-approved yet. The recorded 53-symbol ABI-0 baseline must remain unchanged.

## CSP gate

Raw AES keys must not be retained by a public context after initialization unless
the reviewed API explicitly requires it. The preferred path is an internal key
schedule bound to a CSP key handle with:

- exact key-type and key-size checks;
- non-exportable-by-default lifecycle;
- destroy/stale-handle rejection;
- key schedule and temporary block zeroization;
- no key, schedule, IV, plaintext, or ciphertext logging.

## KAT and negative-test gate

Public-source vectors must be recorded before implementation. The planned minimum
is:

- FIPS 197 AES-128/192/256 encrypt and decrypt known-answer tests;
- NIST SP 800-38A CBC encrypt/decrypt examples;
- startup KAT for each supported AES key size and the selected CBC direction path;
- extended multi-block and in-place tests;
- invalid key, IV, input length, output length, state, direction, mode, and stale
  handle tests;
- differential comparison through a test-only provider that does not enter the
  product dependency graph.

## Side-channel and implementation gate

The implementation review must cover key-dependent table lookups, branches,
memory access, compiler output, zeroization, and supported CPU/toolchain scope.
A portable constant-time-oriented implementation strategy must be selected before
coding. Hardware acceleration, if later introduced, requires a separate provider
and dispatch decision.

## Deferred items

- AES-GCM and all nonce/tag/AAD policy;
- AES-CTR counter construction and exhaustion policy;
- PKCS#7 or other padding;
- CMAC/GMAC;
- ARIA or another domestic algorithm pending an explicit requirement and public
  vector/reference review;
- higher-level nonce, envelope, and storage policy, which belongs to AnoSecureKit.

## Gate outcome

**GO for P1-25 design and implementation of the narrow AES primitive plus CBC
no-padding slice, subject to the API, CSP, KAT, and side-channel conditions above.**
