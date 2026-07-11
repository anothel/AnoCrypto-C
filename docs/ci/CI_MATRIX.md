# AnoCrypto-C CI Matrix Skeleton

Status: P1-22 engineering draft.

| Check | Linux | macOS | Windows | Notes |
|---|---:|---:|---:|---|
| C99 shared-library build | Required | Required | Required | Extensions off and strict warnings. |
| Unit/KAT/negative tests | Required | Required | Required | Includes SHA-384/SHA-512 family tests; output is status-only. |
| SHA-512-family implementation policy | Required | Required | Required | Checks source, startup/extended KAT, negative, evidence, and PoC wiring. |
| Installed-package smoke | Required | Required | Required | Exercises SHA-256/384/512 and HMAC variants through installed API. |
| Customer PoC package smoke | Required | Optional | Optional | Digest/MAC/random values are not printed. |
| Public-surface and 53-symbol clean Release baseline | Required | Required | Required | Tests/examples/hooks OFF and internal symbols hidden. |
| Thread/concurrency policy | Required | Required | Required | Win32/pthread native regression where supported. |
| GCC ASan/UBSan | Required | Not selected | Not selected | Full Linux CTest with `address-undefined`. |
| Provider-disabled and dedicated random hardening lanes | Required | Optional | Optional | Test hooks only in dedicated test builds. |
| Random lifecycle, entropy/health, and operational policies | Required | Required | Required | Status and policy only; no secret material. |
| Evidence draft | Required | Optional | Optional | Engineering artifact; not release approval. |

## P1-22 additions

- Add `ano_crypto_sha512_family_kat_tests` and `ano_crypto_sha512_family_api_negative_tests` to normal hosted lanes.
- Run `ano_crypto_sha512_family_policy_check` on Linux, macOS, and Windows.
- Exercise SHA-384/SHA-512 and HMAC variants in install and PoC consumers.
- Keep clean Release exports equal to the existing 53-symbol ABI-0 baseline.
- Do not upload digest, MAC, key, entropy, seed, DRBG-state, or generated-random values.

## Hosted-result boundary

Workflow definitions do not prove hosted execution. Actual Linux/macOS/Windows results must be retained and reviewed separately. Clang sanitizer runtime and ThreadSanitizer remain follow-up items.
