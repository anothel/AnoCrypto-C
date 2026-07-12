# Hosted Platform Artifact Review

Status: P1-29 template; actual hosted Windows, macOS, Linux 32-bit, s390x big-endian, and compiler-output review results are not recorded in this source archive.

Use this table only after the corresponding workflow run has completed. Do not mark a platform reviewed based solely on workflow YAML.

| Platform | Workflow run/commit | Thread backend | Concurrent CSP HMAC/AES/CMAC | AES/CMAC hardening and endian | Random provider/process marker | Sanitizer/compiler review | Reviewer/date | Limitations |
|---|---|---|---|---|---|---|---|
| Linux 64-bit | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Linux 32-bit | TODO | TODO | TODO | TODO | provider disabled in portability lane | TODO | TODO | TODO |
| Linux s390x big-endian | TODO | TODO | TODO | TODO | provider disabled in portability lane | TODO | TODO | TODO |
| macOS | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Windows | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |

Artifacts must remain status-only. Do not attach entropy captures, generated random bytes, key material, MAC output, seed material, DRBG state, private keys, signing material, tokens, or customer data.


P1-29 review must include the CMAC hardening status artifact and, where the
toolchain supports disassembly, the CMAC compiler-output artifacts. Presence of
an artifact is not a completed side-channel review; record reviewer, target,
compiler, date, and unresolved limitations explicitly.
