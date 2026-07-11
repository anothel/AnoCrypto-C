# C99 Block Cipher API Decision

Status: P1-25 reviewed API implemented.

## Public API

`include/anocrypto/ac_cipher.h` provides:

- algorithm/mode/direction enums with explicit values;
- caller-owned opaque context sizing;
- init/update/uninit lifecycle;
- CSP-key-handle one-shot processing;
- status returns and output-length reporting;
- no public context layout.

CBC without padding accepts only non-empty complete 16-byte blocks. Output length
is exactly input length. No allocation, padding, IV generation, key export, or
persistence is implicit.

## Aliasing and failure behavior

Exact in-place operation is supported. Any partial overlap is rejected before
processing. Parameter and short-buffer failures do not begin output processing.
An unexpected internal block failure clears the requested output region and the
cipher context before returning.

## ABI decision

P1-25 deliberately adds `ac_cipher.h` and eight exported functions. The stored
ABI-0 symbol baseline changes from 53 to 61 entries. Existing symbol names,
status values, enum values, and public structure layouts are unchanged.
