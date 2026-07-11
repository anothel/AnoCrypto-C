# Differential Validation and SHA-512 Portability Review

Status: template for engineering evidence only

## Build identity

- Source revision:
- Compiler and version:
- Target OS/architecture:
- Pointer and `size_t` widths:
- Runtime/CMake byte order:
- Build type:

## Differential provider

- OpenSSL package/version:
- Provider required at configure time: yes / no
- Differential target built: yes / no
- SHA-2/HMAC comparison result:
- Comparison count:
- Values logged: must be no

## Portability

- C99 representation guard:
- Unaligned-input test:
- Variable-chunk streaming test:
- GCC result:
- Clang result:
- ASan/UBSan result:
- Linux 32-bit result or not-run reason:
- Hosted Windows result or not-reviewed:
- Hosted macOS result or not-reviewed:
- Big-endian execution result or TODO:

## Product boundary

- `anocrypto_c` links to OpenSSL: must be no
- installed header contains OpenSSL include: must be no
- PoC SDK links to OpenSSL due to differential lane: must be no
- public symbol count changed: must be no unless separately reviewed

## Review notes

Do not include digest bytes, MAC bytes, generated test keys, entropy, seed
material, DRBG state, random output, private keys, tokens, customer data, or
internal deployment details. Differential agreement and portability tests are
engineering checks, not official validation evidence.
