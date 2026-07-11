# CMAC KAT and Evidence Plan

Status: P1-27 planning gate. No CMAC KAT is executed by this step.

## Public vector source

The future implementation should use public NIST AES-CMAC examples from
SP 800-38B, including AES-128 examples for zero-length, partial-block,
one-block, and multi-block messages. AES-192 and AES-256 coverage must use a
public source or a clearly recorded project-owned derivation checked against an
independent test-only provider.

Vector identifiers and provenance must be recorded in
`cleanroom/SOURCE_MANIFEST.md`. Third-party product code, private submission
material, and unexplained copied fixtures are excluded.

## Startup self-test candidate

The initial startup KAT candidate is one AES-128-CMAC full-tag computation with
fixed public key and message material. A failure must follow the existing
self-test failure path and prevent cryptographic services from entering the
operational state.

The startup KAT must not log key, message, subkey, intermediate chaining value,
or tag bytes.

## Extended tests required before implementation completion

- AES-128 zero-length, partial-block, exact-block, and multi-block examples;
- AES-192 and AES-256 full-tag computation;
- one-shot and streaming equivalence;
- byte-by-byte and irregular chunking;
- exact 16-byte output and short-buffer rejection;
- verify success and authentication-failure status;
- wrong-purpose, stale, destroyed, and malformed CSP handles;
- empty input, NULL/zero-length conventions, context lifecycle, and repeated
  finalization failures;
- subkey-generation and final-block branch coverage;
- install consumer and customer PoC status-only smoke;
- clean Release exported-symbol review;
- test-only independent differential comparison.

## Secret and evidence boundary

Evidence records may contain algorithm, key-size class, message-length class,
provider identity, comparison count, status, compiler, and target metadata.
They must not contain key bytes, CMAC subkeys, messages, tags, intermediate
state, customer data, entropy, seed material, DRBG state, or generated random
bytes.

## Remaining review

Target-specific compiler output and timing/cache behavior, fault behavior,
final validation-target scope, and any use of CMAC inside a protocol or KDF
remain separate review items. A passing KAT or differential test is not an
official validation result.
