# AnoCrypto-C CI Matrix Skeleton

Status: P1-23 engineering draft.

| Check | Linux | macOS | Windows | Notes |
|---|---:|---:|---:|---|
| C99 shared-library build | Required | Required | Required | Extensions off and strict warnings. |
| Unit/KAT/negative tests | Required | Required | Required | SHA-2/HMAC output is status-only. |
| SHA-512-family implementation policy | Required | Required | Required | Source, KAT, negative, evidence, and PoC wiring. |
| SHA-512 portability review | Required | Required | Required | Endian metadata, variable chunks, and unaligned input; no values. |
| OpenSSL 3.x differential lane | Required dedicated job | Not selected | Not selected | Test-only provider; exact version/status artifact. |
| Linux GCC `-m32` portability lane | Required workflow | N/A | N/A | Provider and OS entropy disabled; actual result needs review. |
| Installed-package smoke | Required | Required | Required | All SHA-2/HMAC variants through installed API. |
| Customer PoC package smoke | Required | Optional | Optional | No OpenSSL runtime dependency or result values. |
| Public surface and 61-symbol baseline | Required | Required | Required | Tests/examples/hooks OFF; internal symbols hidden. |
| Thread/concurrency policy | Required | Required | Required | Win32/pthread native regression where supported. |
| GCC ASan/UBSan | Required | Not selected | Not selected | Full Linux CTest with `address-undefined`. |
| Random/entropy hardening lanes | Required | Optional | Optional | Dedicated hooks only in test builds. |
| Evidence draft | Required | Optional | Optional | Engineering artifact; not release approval. |

## P1-23 additions

- Discover OpenSSL 3.x only when test-only differential tests are enabled.
- Require the provider in a dedicated Linux job and fail configure if absent.
- Compare SHA-256/384/512 and HMAC variants over deterministic one-shot,
  streaming, boundary, long-key, larger-message, and unaligned cases.
- Run `ano_crypto_sha512_portability_review` on standard hosted runners.
- Add a Linux GCC `-m32` workflow with a reviewed multilib package; disable the
  external provider in that lane unless a reviewed 32-bit package is added.
- Run `ano_crypto_differential_portability_policy_check` in applicable jobs.
- Keep the product target, installed SDK, PoC runtime, and public headers free
  of OpenSSL dependencies introduced by the differential lane.
- Upload status/provider/version/architecture metadata only. Never upload
  digest, MAC, key, message, entropy, seed, DRBG-state, or random values.

## Hosted-result boundary

Workflow definitions and artifact upload steps do not prove successful hosted
execution. Actual Linux 32-bit, Windows, macOS, and any future big-endian
results must be retained and reviewed before portability support wording is
changed. Clang sanitizer runtime and ThreadSanitizer remain follow-up items.
