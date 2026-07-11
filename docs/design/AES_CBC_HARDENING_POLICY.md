# AES/CBC Hardening Policy

Status: P1-26 engineering hardening slice.

## No public API widening

P1-26 does not add a cipher algorithm, mode, enum value, flag, public function,
public structure, or exported symbol. The reviewed P1-25 `ac_cipher.h` surface
remains eight functions and the ABI-0 exported-symbol baseline remains 61 names.
The caller-owned context remains opaque and its required size must continue to
be obtained with `ano_crypto_cipher_context_size`.

## Internal context integrity

The private AES context now records an internal magic value and the exact
expanded-round-key byte count expected for AES-128, AES-192, or AES-256.
Single-block operations reject a context whose round count and schedule size do
not agree. These fields are an engineering corruption guard, not a general
fault-attack countermeasure.

A failed internal AES reinitialization clears the earlier key schedule before
returning. A failed public cipher reinitialization also clears an earlier cipher
context once the supplied storage has been validated as large enough for the
selected algorithm and mode. Preflight failures during `cipher_update`, such as
a short output buffer or partial overlap, do not consume or invalidate an
otherwise valid context.

## Lifecycle and portability regression

The P1-26 regression suite adds:

- 128 repeated AES key-import/init/destroy/update/uninit lifecycles;
- continued use of an initialized context after its source CSP handle is
  destroyed, matching the documented copied-schedule ownership model;
- context-zeroization checks after every successful uninit;
- intentionally unaligned key, IV, input, and output byte buffers;
- variable complete-block streaming partitions;
- exact in-place comparison against out-of-place processing;
- shared and per-thread CSP HMAC/AES handle use on supported thread backends.

Opaque context storage itself must retain the natural alignment supplied by a
normal C allocator. P1-26 does not claim support for deliberately misaligned
context storage.

## Differential expansion

The OpenSSL 3.x test-only AES-CBC provider lane now uses 96 deterministic cases
for each AES key size. It covers 1 through 64 blocks, changing keys and IVs,
one-shot processing, variable streaming partitions, exact in-place processing,
and encrypt/decrypt in both implementations. Only case metadata and comparison
counts may be logged.

OpenSSL remains outside the product target, installed SDK, customer PoC runtime,
and public headers.

## Compiler-output review

`ano_crypto_aes_compiler_review` captures a Release-library disassembly and a
status-only summary when a supported disassembler is available. The automation
does not classify the output as constant-time. Human review must inspect the
selected AES routines for compiler-introduced tables, secret-dependent branches,
secret-indexed memory access, spills, and target-specific instructions.

Compiler output is toolchain, optimization, architecture, and configuration
specific. A result from one target cannot be generalized to another target.

## Claims boundary

The portable implementation continues to avoid a source-level lookup-table
S-box and uses fixed-iteration arithmetic. P1-26 does not claim universal
constant-time behavior, side-channel resistance, fault resistance, formal
verification, audit completion, or official validation. Target-specific review
and external assessment remain TODO.
