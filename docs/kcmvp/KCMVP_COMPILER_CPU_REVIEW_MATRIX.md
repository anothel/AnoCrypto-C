# KCMVP Compiler and CPU Review Matrix — P2-06

| Review profile | Environment | Work performed | Result | Validation meaning |
|---|---|---|---|---|
| `OE-01` selected candidate | Ubuntu Server 24.04.4, x86-64 generic, GCC 13.3.0, CMake 3.28.3, Release shared library | Strict compile, static analysis decision, AES/CMAC/zeroization disassembly, ELF hardening, timing/cache methodology, named human review | `NOT_RUN` | Required before selected-OE evidence closure. |
| `LOCAL-GCC14` | Debian Linux x86-64, GCC 14.2.0, CMake 3.31.6 | Strict diagnostics, `-fanalyzer`, Release disassembly, ELF hardening capture, full regression | `LOCAL_ENGINEERING_ONLY` | Useful defect discovery; not OE-01 evidence. |
| `LOCAL-CLANG17` | Linux x86-64, Clang 17 | `-Weverything`-style strict compile and Clang static analyzer | `PASS_AFTER_INTERNAL_FIX` | Useful independent compiler review; not a supported candidate. |
| Hosted Linux GCC/Clang | Workflow definitions exist | Actual artifact execution/review | `BLOCKED_EXTERNAL` / `NOT_RUN` | P1-34 billing blocker retained. |
| Windows/MSVC x64 | Deferred candidate | No actual P2-06 compiler/CPU review | `NOT_RUN` | Not initial validation environment. |
| macOS/Apple Clang | Deferred candidate | No actual P2-06 compiler/CPU review | `NOT_RUN` | Not initial validation environment. |
| 32-bit / big-endian / ARM64 / other CPUs | Deferred or unsupported initial candidate | No P2-06 review | `NOT_RUN` | No portability or side-channel claim. |

## Local GCC 14 observations

- Table-free AES source compiled to arithmetic/vector operations with no
  compiler-generated S-box/T-table symbol observed.
- CMAC tag comparison retained a full length-controlled loop without early exit
  on mismatch position.
- The volatile zeroization loop remained present in optimized output.
- P2-06 Release hardening produced non-executable stack, GNU RELRO, BIND_NOW,
  and stack-protector references.
- These observations are binary-hash-specific and do not transfer to GCC 13.3.0.

## Required OE-01 retained record

- immutable OS image/package snapshot and CPU/microcode identity;
- exact configure command and complete compile/link flags;
- source commit/archive hash and produced library hash;
- warning/analyzer logs;
- AES, CMAC compare and zeroization function-level disassembly;
- ELF program headers/dynamic tags/symbol imports;
- human reviewer, review date, findings and disposition;
- timing/cache test record if required by consultation;
- no raw key, plaintext, ciphertext, tag, entropy, seed or generated random data.
