# AES/CBC Hardening Review

- Version slice: P1-26
- Status: engineering review template; not official validation evidence

## Public surface

- [ ] `ac_cipher.h` still contains the reviewed eight public functions.
- [ ] Clean Release exports match the stored 61-symbol ABI-0 baseline.
- [ ] No new mode, padding, AEAD, hardware-dispatch, or domestic-algorithm API
      was introduced.

## Context and lifecycle

- [ ] Internal AES round count and schedule-size guards pass.
- [ ] Failed reinitialization invalidates the earlier schedule.
- [ ] CSP handle destruction after successful init does not invalidate the
      copied caller-owned context.
- [ ] Successful uninit clears the entire reported context size.
- [ ] 128-iteration lifecycle stress passes.
- [ ] Shared/per-thread CSP HMAC and AES concurrency regression passes on the
      reviewed thread backend.

## Differential and portability

- [ ] OpenSSL 3.x test-only differential reports 2,016 AES-CBC comparisons.
- [ ] One-shot, streaming, exact in-place, encrypt, and decrypt paths pass.
- [ ] Unaligned key/IV/input/output byte-buffer regression passes.
- [ ] Hosted 32-bit artifact run and identifier recorded, or marked TODO.
- [ ] Hosted big-endian artifact run and identifier recorded, or marked TODO.
- [ ] Windows and macOS artifact runs and identifiers recorded, or marked TODO.

## Compiler-output review

- [ ] `AES_COMPILER_OUTPUT_REVIEW.txt` identifies compiler, target, and library
      hash.
- [ ] Full disassembly artifact is retained internally for the reviewed target.
- [ ] Reviewer checked selected AES routines for secret-dependent control flow
      and memory addressing.
- [ ] Compiler/toolchain-specific conclusion is recorded without generalizing
      to unreviewed targets.
- [ ] No stronger side-channel or validation claim is made.

## Sensitive-output boundary

- [ ] Logs contain no key, expanded schedule, IV, plaintext, ciphertext,
      differential result bytes, entropy, seed, DRBG state, or random output.
- [ ] Compiler artifacts and OpenSSL test sources are absent from the installed
      SDK and customer PoC package.
