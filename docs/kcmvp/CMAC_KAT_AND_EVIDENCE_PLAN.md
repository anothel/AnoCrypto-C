# AES-CMAC KAT and Evidence Plan

Status: P1-28 engineering implementation and evidence plan.

## Public vector source

The implementation uses public NIST AES-CMAC examples associated with
SP 800-38B. Extended tests cover AES-128, AES-192, and AES-256 for zero-length,
partial-block, exact-block, and multi-block messages. Vector provenance is
recorded in `cleanroom/SOURCE_MANIFEST.md`.

## Startup self-test

Startup self-test runs one AES-128-CMAC full-tag known-answer test using public
fixed material. Failure follows the existing self-test failure path and
prevents entry into OPERATIONAL. The key, message, subkeys, chaining state, and
tag are not logged.

## Extended tests

- all 12 public AES-CMAC examples for AES-128/192/256;
- one-shot and irregular streaming equivalence;
- empty, partial, complete, and multi-block messages;
- exact 16-byte output and short-buffer rejection;
- verify success and authentication-failure status;
- wrong-purpose, stale, destroyed, and malformed CSP handles;
- context lifecycle and repeated-finalization rejection;
- install consumer and customer PoC status-only smoke;
- clean Release exported-symbol review;
- OpenSSL 3.x test-only differential comparison.

## Evidence boundary

Evidence may retain algorithm, key-size class, message-length class, provider
identity, comparison count, status, compiler, and target metadata. It must not
retain key bytes, CMAC subkeys, messages, tags, intermediate state, customer
data, entropy, seed material, DRBG state, or generated random bytes.

## Remaining review

Target-specific compiler output, timing/cache behavior, fault behavior, final
validation-target scope, and protocol/KDF use remain separate review items.
Passing KAT and differential tests is engineering evidence, not an official
validation result.
