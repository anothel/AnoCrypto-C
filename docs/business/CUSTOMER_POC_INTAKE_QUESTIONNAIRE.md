# Customer PoC Intake Questionnaire

Status: P1-15 draft.

## Purpose

Use this questionnaire before expanding the AnoCrypto-C customer PoC scope. The goal is to record platform, toolchain, integration, and algorithm requirements without collecting secrets or customer-sensitive deployment data.

## Do not collect

Do not request or store:

- Secret keys.
- Entropy captures.
- Generated secret bytes.
- Private keys.
- Tokens.
- Customer certificate private material.
- Live customer data.
- Internal deployment secrets.

## Customer/platform inputs

| Topic | Questions | Notes |
|---|---|---|
| Operating system | Which OS family and version range should be supported? | Example: Linux distro family, Windows Server/Desktop, macOS, embedded OS. |
| CPU/architecture | Which CPU architectures are required? | Example: x86_64, arm64, embedded target. |
| Compiler/toolchain | Which compiler, version range, and generator are required? | Example: GCC/Clang/MSVC, Ninja/Make/MSBuild. |
| Linkage | Shared library, static library, or both? | Initial candidate boundary is shared-library centered. |
| Packaging | CMake package, raw headers/libs, or vendor build integration? | Current package target is `AnoCryptoC`. |
| Threading | Single-threaded, multi-threaded, or mixed? | Confirm per-context and global-state expectations. |
| FFI/language binding | C only, C++, Rust, Java/JNI, Python, or other? | Bindings should not change the C core boundary by default. |
| Deployment model | Server, desktop, mobile, embedded, or appliance? | Avoid collecting internal deployment secrets. |
| Algorithm scope | Which primitives are required in the PoC? | Record hash, MAC, DRBG, cipher/mode, KDF, AEAD, public-key needs separately. |
| Key lifecycle | Import, in-memory use, persistence, wrapping, export, or destruction requirements? | Current P1 slice is in-memory HMAC key handle only. |
| Entropy/random | Is OS entropy enough for PoC, or is a platform provider required? | Do not collect entropy samples. |
| Evidence needs | What logs or reports does the customer need for internal review? | Keep evidence wording bounded to engineering readiness. |
| Support boundary | Evaluation only, integration support, or long-term maintenance discussion? | Legal/evaluation notice remains TODO. |

## PoC scope output

After intake, write a short PoC scope note with:

- Customer-visible package name.
- Target OS/architecture/toolchain.
- Linkage model.
- Required algorithms and modes.
- Out-of-scope algorithms and product layers.
- Evidence/logging expectations.
- Known limitations.
- Open TODOs.

## Boundary reminder

AnoCrypto-C is the C99 core. App-level envelope formats, nonce persistence workflows, platform keychain adapters, CSR workflows, certificate chain validation, and trust policy belong to separate layers unless a later architecture decision says otherwise.
