# SHA-512 Family Portability Policy

Status: P1-23 engineering policy

## Supported C representation assumptions

AnoCrypto-C configuration fails unless the C implementation provides:

- `CHAR_BIT == 8`;
- a 32-bit `uint32_t`;
- a 64-bit `uint64_t`;
- `size_t` no wider than `uint64_t`.

These assumptions are checked with a C99 compile test in the top-level
`CMakeLists.txt`. They are implementation prerequisites, not public ABI
fields.

## Byte order and alignment

The SHA-512-family compression path loads and stores words byte-by-byte in
big-endian order. It must not dereference an input buffer through a
`uint64_t *` or assume host alignment.

`ano_crypto_sha512_portability_review` checks:

- runtime byte order against CMake's compile-time byte-order result;
- aligned and intentionally unaligned SHA-384/SHA-512 inputs;
- one-shot and variable-chunk streaming equality;
- HMAC-SHA-384/HMAC-SHA-512 with unaligned messages and long unaligned keys;
- status-only output with no digest, MAC, or key bytes.

The OpenSSL differential test also uses intentionally unaligned inputs.

## 32-bit and 64-bit targets

The primary local and hosted matrix remains 64-bit Linux, Windows, and macOS.
P1-23 adds a Linux GCC `-m32` CI lane with OS entropy disabled and the external
differential provider disabled. The lane builds and runs the public/internal
test suite and emits the status-only portability review.

A workflow definition is not proof that a 32-bit run passed. Actual hosted
artifacts must be reviewed before 32-bit support wording is adopted.

## Large length handling

SHA-384/SHA-512 maintain a two-word byte counter and encode the final 128-bit
bit length. The public API accepts `size_t`, and configuration rejects targets
where `size_t` is wider than the internal 64-bit addend.

The P1-23 differential corpus includes one-megabyte input and block/padding
boundaries. It does not allocate or process inputs near the theoretical
SHA-512 length limit. Counter-overflow simulation remains a future internal
negative-test candidate.

## Compiler and optimization review

Required engineering checks include:

- GCC and Clang normal builds;
- GCC ASan/UBSan;
- Release clean-symbol comparison;
- hosted Linux/macOS/Windows status artifacts;
- Linux 32-bit hosted build when the multilib toolchain is available.

No throughput, constant-time, or compiler-code-generation assurance claim is
made. Performance benchmarks, disassembly review, big-endian hardware/emulator
execution, and additional compiler families remain TODO / confirmation
required.
