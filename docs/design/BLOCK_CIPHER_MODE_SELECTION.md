# Block Cipher and MAC Selection

Status: P1-29 AES/CBC retained implementation and AES-CMAC hardened engineering scope.

## Implemented confidentiality scope

- AES-128, AES-192, and AES-256 primitive;
- CBC without padding;
- caller-provided 16-byte IV;
- complete-block input only;
- exact in-place support and partial-overlap rejection;
- CSP-backed block-cipher key purpose.

## Implemented authentication scope

- AES-CMAC with AES-128/192/256;
- dedicated CMAC-purpose CSP handles;
- full 16-byte tags only;
- empty and arbitrary-length messages;
- one-shot compute/verify and streaming compute.

## Deferred scope

AES-CTR remains deferred because counter construction, uniqueness, exhaustion,
persistence, process/VM duplication, and recovery policy need a separate gate.
GCM, GMAC, padding helpers, tag truncation, protocol formatting, hardware
dispatch, and domestic algorithms also remain outside P1-29.

The current implementation and tests are engineering evidence and do not imply
that target-specific side-channel or official validation review is complete.
