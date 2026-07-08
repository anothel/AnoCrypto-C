# AnoCrypto-C Customer PoC Package v0

## Purpose

The v0 customer PoC package is an integration-oriented SDK draft. Its goal is to let a customer or internal evaluator confirm that AnoCrypto-C can be found through CMake, linked through the stable C target name, initialized through the public lifecycle API, and exercised through a small public-API smoke flow.

This package is not official validation evidence and does not promise certification outcome, schedule, or suitability for every customer operating environment.

## Intended contents

```text
AnoCrypto-C-PoC-v0/
  README.md
  PACKAGE_MANIFEST.md
  PACKAGE_REVIEW_MANIFEST.md
  SBOM_STATUS.txt
  SIGNING_STATUS.txt
  INSTALL_PACKAGE.log
  sdk/
    include/anocrypto/*.h
    lib*/libanocrypto_c.*
    lib*/cmake/AnoCryptoC/*.cmake
  examples/core_poc/
    CMakeLists.txt
    README.md
    main.c
  docs/
    README.md
    LICENSE
    business/POC_CUSTOMER_PACKAGE_V0.md
    business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md
    business/POC_PACKAGE_REVIEW_MANIFEST.md
    integration/SDK_INTEGRATION_GUIDE.md
    kcmvp/*.md
    design/API_ABI_REVIEW_CHECKLIST.md
    design/BLOCK_CIPHER_MODE_SELECTION.md
    security/SIDE_CHANNEL_REVIEW_CHECKLIST.md
    ci/CI_MATRIX.md
    roadmap/C99_PRIORITY_TODO.md
  evidence-template/README.md
```

## Public integration checks

The packaged `examples/core_poc` project checks:

- `find_package(AnoCryptoC CONFIG REQUIRED)`
- target link through `AnoCryptoC::AnoCryptoC`
- `include/anocrypto/anocrypto.h` availability
- version and build identity query flow
- module init/shutdown lifecycle
- SHA-256 digest flow
- HMAC-SHA-256 using an in-memory HMAC key handle
- HMAC_DRBG-SHA-256 instantiate/generate/uninstantiate flow with deterministic public demo input

The example does not print raw key bytes, entropy input, nonce input, DRBG state, or generated DRBG bytes.

## Customer evaluation boundaries

Allowed v0 evaluation questions:

- Can the customer build system find the installed package?
- Does the public header set compile as C99 in the customer toolchain?
- Does the shared or static library link in the customer environment?
- Does the module lifecycle and startup self-test path run successfully?
- Are status codes and output-parameter conventions usable for the customer integration layer?

Out of scope for v0:

- production deployment decision
- official validation evidence
- customer-specific entropy policy approval
- persistent keystore integration
- formal side-channel review
- performance commitments
- public random-service API commitment
- binary reproducibility commitment across hosts

## Packaging commands

From a configured build tree:

```sh
cmake --build build --target ano_crypto_create_poc_package
```

Run the package smoke flow:

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The smoke target stages the package, configures `examples/core_poc` as a standalone installed-package consumer, builds it, and runs it.

## Evidence handling notes

Before sharing a package externally, review:

- `PACKAGE_MANIFEST.md`
- `PACKAGE_REVIEW_MANIFEST.md`
- `SBOM_STATUS.txt`
- `SIGNING_STATUS.txt`
- `INSTALL_PACKAGE.log`
- generated example smoke logs
- local absolute paths in logs
- compiler, generator, and OS identifiers
- source provenance notes
- test-vector provenance notes
- whether any customer or internal deployment details were accidentally included

Do not add or retain secrets, entropy captures, generated secret bytes, private keys, signing keys, signing tokens, tokens, customer certificate bundles, customer data, or internal deployment details in the package.

## TODO before broader PoC use

- Decide binary SDK versus source-available evaluation terms.
- Add license/evaluation notice text after legal review.
- Replace SBOM/signing placeholders with reviewed generated outputs after tool and key-management policies are selected.
- Customer questionnaire for OS, CPU, compiler, linker, deployment model, and required algorithm scope is available as `CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md`.
- Add a support boundary document for error reports and evidence requests.
