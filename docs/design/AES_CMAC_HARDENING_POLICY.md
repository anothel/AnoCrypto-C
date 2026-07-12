# AES-CMAC Hardening Policy

Status: P1-29 engineering policy.

## Purpose

P1-29 hardens the P1-28 AES-CMAC implementation without widening the public
algorithm or ABI surface. This document records the failure, cleanup, context,
aliasing, concurrency, and compiler-review boundaries that must stay aligned
with the code and tests.

## No public API widening

P1-29 retains:

- `ac_cmac.h` with exactly eight public CMAC functions;
- `ANO_CRYPTO_KEY_TYPE_CMAC = 7`;
- AES-128/192/256 dedicated CMAC-purpose CSP handles;
- full 16-byte tags only;
- the 69-symbol ABI-0 exported-symbol baseline.

No public context layout, raw-key CMAC entry point, truncated-tag option,
protocol format, key export, persistence, or implicit allocation is added.
The private opaque-context extent changes in P1-29; callers must obtain the
required size from `ano_crypto_cmac_context_size()` for the loaded library and
must not hardcode, serialize, or persist a size obtained from an older build.

## Context integrity and failure policy

The private CMAC context records an internal marker, state version, key size,
pending length, and an AES schedule whose rounds and expanded-key extent must
match the selected AES key size.

- A marked context with inconsistent private state is rejected and cleared.
- A failed reinitialization after context-size validation clears any earlier
  schedule and leaves the context unusable until a successful new init.
- A rejected preflight call caused only by disallowed aliasing or a short final
  output does not consume an otherwise valid streaming context.
- Successful finalization clears the complete private CMAC context.
- One-shot compute clears its local context on every exit after creation.
- One-shot operational failures clear the full caller tag buffer extent used by
  the API and set `out_written` to zero.

## Aliasing policy

CMAC context storage, message input, output tag storage, and `out_written`
storage must be disjoint for the applicable call. Verification additionally
requires the message input and supplied tag storage to be disjoint. Exact or
partial overlap is rejected with `ANO_CRYPTO_ERR_INVALID_ARGUMENT` before the
context state is advanced.

The policy intentionally does not infer object bounds from arbitrary pointers.
Callers remain responsible for providing valid objects with the stated sizes.

## Cleanup policy

`ano_crypto_cmac_uninit()` is callable outside the operational module state.

- `ctx == NULL, ctx_len == 0` is an idempotent no-op.
- `ctx == NULL, ctx_len != 0` is invalid.
- A non-NULL context with `ctx_len == 0` cannot authorize a write: the call
  returns `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL` and leaves storage unchanged.
- A short nonzero length clears only the caller-authorized prefix, returns
  `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL`, and invalidates the context. The caller
  must retry with the queried context size to clear the complete extent.
- A sufficient or oversized length clears only the reviewed opaque-context
  extent; trailing caller storage is not modified.

## Verification policy

Tag comparison processes all 16 bytes through one source-level accumulation
path and returns only success or `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED` for a
well-formed mismatching tag. This is a constant-work source-level candidate,
not a target-independent timing conclusion. Compiler output and target behavior
require separate review.

## Concurrency and CSP ownership

A CMAC streaming context owns its copied AES schedule after successful init, so
destroying the source key handle does not invalidate that already initialized
context. CSP handle lookup and copying remain protected by the configured
internal lock.

Regression coverage includes:

- independent CMAC-purpose handles per worker;
- a shared read-only CMAC-purpose handle across workers;
- concurrent one-shot compute and verify;
- immediate clearing of temporary tags;
- no key, message, tag, subkey, or intermediate values in test output.

## Remaining review

TODO / 확인 필요:

- actual hosted Windows and macOS concurrency artifacts;
- 32-bit and big-endian execution artifacts;
- target/compiler-specific review of CMAC subkey generation, block selection,
  AES calls, cleanup paths, and tag comparison;
- ThreadSanitizer policy and runtime suitability;
- any later tag-truncation policy or protocol-level use guidance.
