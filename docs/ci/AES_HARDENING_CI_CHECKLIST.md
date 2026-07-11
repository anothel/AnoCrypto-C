# AES/CBC Hardening CI Checklist

Status: P1-26 engineering checklist. Workflow presence is not evidence that a hosted run succeeded.

## Required local and hosted checks

| Check | Required result | Logging boundary |
|---|---|---|
| Default Linux/macOS/Windows suite | AES/CBC KAT, negative, lifecycle, portability, and concurrency tests pass where supported | Status, counts, platform metadata only |
| OpenSSL 3.x differential lane | AES-128/192/256 CBC one-shot, streaming, exact in-place, encrypt, and decrypt comparisons pass | Provider/version and comparison count only |
| Linux GCC ASan+UBSan | Full supported test set passes without sanitizer diagnostics | No key, IV, plaintext, ciphertext, or intermediate values |
| Linux GCC 32-bit lane | AES hardening and SHA-512 portability reviews execute on an actual 32-bit process | Pointer/size/endian metadata only |
| Linux s390x big-endian lane | Selected AES/SHA-512 tests execute through the reviewed cross-runner setup | Architecture/endian/status only |
| GCC/Clang compiler-output capture | Review summary and full disassembly artifacts are generated | Binary hash, compiler, target, symbol markers, instruction text; no runtime values |
| Clean Release symbol baseline | Exactly 61 reviewed public symbols; no internal AES/test symbols | Symbol names only |
| Install and PoC smoke | Installed API performs AES-CBC round trips without leaking values | Status and byte counts only |

## Compiler-output boundary

`ano_crypto_aes_compiler_review` captures artifacts for a human review. The target does not decide that machine code is constant-time, side-channel resistant, suitable for every microarchitecture, or externally reviewed. Each supported compiler, optimization level, architecture, and relevant feature set requires a separately retained artifact and manual disposition.

## Hosted-result boundary

The source archive does not contain completed Windows, macOS, 32-bit, or big-endian hosted review results unless a reviewer has recorded the workflow run, commit, toolchain, artifact hash, result, limitations, and date. A workflow definition or artifact-upload step alone must not widen support wording.

## Prohibited artifacts

Do not upload or retain:

- operational AES keys or customer data;
- generated IV, plaintext, ciphertext, expanded schedule, or intermediate AES state;
- entropy, seed, DRBG state, generated random output, private keys, signing keys, or tokens;
- unreviewed external implementation source or unclear-provenance vectors.
