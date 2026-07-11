# C99 Block Cipher API Decision

Status: P1-26 reviewed API retained; hardening adds no public cipher functions or enum values.

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

P1-25 deliberately added `ac_cipher.h` and eight exported functions. P1-26 retains those same eight functions and the 61-entry ABI-0 symbol baseline while hardening internal context, lifecycle, testing, and review workflows. Existing symbol names,
status values, enum values, and public structure layouts are unchanged.
