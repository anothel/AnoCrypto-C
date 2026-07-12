# Block Cipher KAT and Evidence Plan

Status: P1-26 implemented and hardened engineering evidence slice.

## Public sources

- FIPS 197 AES-128/192/256 cipher examples.
- NIST SP 800-38A CBC-AES128/192/256 examples.

The vector identifiers are recorded in `cleanroom/SOURCE_MANIFEST.md`. No private
submission material or third-party product implementation was used.

## Implemented evidence

- startup AES-128/192/256 encrypt/decrypt KAT;
- startup AES-128-CBC encrypt/decrypt path KAT;
- extended four-block CBC-AES128/192/256 KAT;
- streaming and exact in-place regression;
- invalid key/IV/input/output/mode/direction/overlap/context tests;
- stale and wrong-type CSP handle tests;
- OpenSSL 3.x test-only AES-CBC differential comparison with 96 deterministic cases per key size and one-shot, streaming, exact in-place, encrypt, and decrypt paths;
- 128-iteration lifecycle stress, unaligned/variable-chunk portability, failed-reinitialization, internal-context guard, and cross-platform CSP HMAC/AES concurrency regression;
- compiler-output capture artifacts for manual review without automated leakage-resistance conclusions;
- install and PoC smoke without key/IV/plaintext/ciphertext output;
- clean Release 69-symbol baseline check.

Evidence logs retain status, provider identity, comparison counts, and policy
metadata only. Keys, expanded schedules, IVs, plaintext, ciphertext, and
intermediate states are excluded.

## Remaining review

Manual disposition of compiler-output artifacts and supported-target side-channel review, actual 32-bit/big-endian hosted
results, hardware provider decisions, and final validation-target scope remain TODO.

## P1-28 AES-CMAC evidence

AES-CMAC now has separate startup and extended public-source KATs, negative
API/CSP tests, OpenSSL 3.x test-only differential comparison, install and PoC
smoke, value-free evidence notes, and a 69-symbol clean Release baseline. CMAC
keys, messages, tags, subkeys, and intermediate states are excluded from logs.
