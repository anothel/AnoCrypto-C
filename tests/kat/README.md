# AnoCrypto-C KAT Tests

Status: P1-2 draft.

This directory contains public-source known-answer tests for the currently implemented algorithm slice:

- SHA-256: empty message, `abc`, the standard multi-block message, and the million-`a` vector.
- HMAC-SHA-256: RFC 4231 test cases 1, 2, 3, 4, 6, and 7.
- API consistency checks: deterministic chunk-boundary tests compare one-shot and streaming APIs.

Important boundaries:

- These tests are implementation correctness checks only.
- They are not official validation evidence.
- Public HMAC test-vector keys are synthetic KAT inputs, not operational secrets.
- Deterministic chunk-boundary fixtures are local API consistency tests, not public KAT sources.
- Additional vectors should be added only after `cleanroom/SOURCE_MANIFEST.md` records the public source.
