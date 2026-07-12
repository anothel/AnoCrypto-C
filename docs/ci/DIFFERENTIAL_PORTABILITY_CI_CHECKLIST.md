# Differential Validation and Portability CI Checklist

Status: P1-29 engineering checklist

## Required Linux differential lane

Configure with:

```text
ANO_CRYPTO_BUILD_TESTS=ON
ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON
ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
```

Required checks:

- OpenSSL 3.x `Crypto` is found;
- `ano_crypto_sha2_hmac_openssl_differential_tests`, `ano_crypto_aes_cbc_openssl_differential_tests`, and `ano_crypto_aes_cmac_openssl_differential_tests` build;
- CTest label `differential` passes;
- the log records provider version, comparison count, and status only;
- the policy check passes;
- OpenSSL is absent from `anocrypto_c` runtime linkage and installed headers.

## Required portability checks

- `ano_crypto_sha512_portability_review`, `ano_crypto_aes_cbc_hardening_review`, and `ano_crypto_aes_cmac_hardening_review` pass on each standard hosted matrix runner where tests execute;
- CMake representation guards pass;
- Linux GCC and Clang normal tests pass;
- Linux GCC ASan/UBSan passes;
- clean Release public exports match the 69-symbol ABI-0 baseline;
- customer PoC and install consumers remain independent of OpenSSL.

## Linux 32-bit lane

- install a reviewed multilib compiler/runtime package on the hosted runner;
- compile and link with `-m32`;
- disable the OpenSSL differential provider in this lane unless a reviewed
  32-bit OpenSSL development package is explicitly installed;
- disable the OS entropy provider to keep the lane focused on C representation,
  SHA-2/HMAC, AES/CBC, and ABI-size portability;
- run the full applicable CTest suite and verbose SHA-512/AES portability reviews;
- upload only status/compiler/architecture output.

## Failure review

Do not print digest, MAC, key, entropy, seed, DRBG state, or generated random
values. Record algorithm, path, lengths, provider version, target architecture,
and status. A workflow file or artifact upload is not itself evidence that the
runner passed; review the actual job and artifact before changing support
claims.

## Big-endian lane

- use the reviewed s390x cross compiler and QEMU user emulator setup;
- disable OS entropy and external differential providers;
- run selected AES/CBC, cipher-negative, and SHA-512 tests;
- capture endian/status metadata only;
- do not mark the target reviewed until the actual workflow artifact is inspected.

## Compiler-output lane

- capture separate GCC and Clang Release artifacts for both AES-CBC and AES-CMAC review scopes;
- retain binary hash, compiler/target metadata, summary, and full disassembly;
- require a human disposition for the intended compiler, flags, architecture, and CPU policy;
- do not convert branch/call counts or symbol presence into an automated leakage-resistance conclusion.
