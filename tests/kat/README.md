# AnoCrypto-C KAT Tests

Status: P1-1 draft.

This directory contains public-source known-answer tests for the currently implemented algorithm slice:

- SHA-256: empty message and `abc` message digests.
- HMAC-SHA-256: RFC 4231 test cases 1 and 2.

Important boundaries:

- These tests are implementation correctness checks only.
- They are not official validation evidence.
- Public HMAC test-vector keys are synthetic KAT inputs, not operational secrets.
- Additional vectors should be added only after `cleanroom/SOURCE_MANIFEST.md` records the public source.
