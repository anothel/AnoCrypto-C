# Block Cipher KAT and Evidence Plan

Status: P1-24 pre-implementation plan.

## Selected next candidate slice

AES-128/192/256 primitive and AES-CBC without padding. This is an engineering
scope decision only.

## Public sources

- FIPS 197 for AES algorithm examples and known-answer material.
- NIST SP 800-38A for CBC mode examples.

Exact vector identifiers and transcription checks must be entered in
`cleanroom/SOURCE_MANIFEST.md` when implementation begins. No private submission
material or third-party proprietary fixture may be used.

## Required evidence before the slice is called implemented

- startup KAT result status;
- extended AES and CBC KAT status;
- encrypt/decrypt and multi-block status;
- negative-test status;
- CSP lifecycle status;
- test-only differential-provider identity and comparison count;
- install and PoC smoke status without key/plaintext/ciphertext output;
- clean Release exported-symbol comparison;
- compiler/platform and side-channel review TODOs;
- known limitations and deferred modes.

Logs must never contain AES keys, expanded schedules, IVs, plaintext, ciphertext,
or intermediate state.
