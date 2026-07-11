# AnoCrypto-C customer PoC package v0 staging script.
# This creates a local engineering package draft from a configured build tree.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_POC_PACKAGE_DIR)
    set(ANO_CRYPTO_POC_PACKAGE_DIR "${ANO_CRYPTO_BUILD_DIR}/poc-package/AnoCrypto-C-PoC-v0")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_POC_PACKAGE_DIR "${ANO_CRYPTO_POC_PACKAGE_DIR}" ABSOLUTE)

if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    message(FATAL_ERROR "Build directory does not exist: ${ANO_CRYPTO_BUILD_DIR}")
endif()

if(NOT DEFINED ANO_CRYPTO_POC_BUILD_CONFIG)
    set(ANO_CRYPTO_POC_BUILD_CONFIG "Release")
endif()

set(_clean_build_dir "${ANO_CRYPTO_BUILD_DIR}/poc-clean-release-build")
set(_sdk_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/sdk")
set(_docs_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/docs")
set(_examples_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/examples")
set(_evidence_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/evidence-template")

file(REMOVE_RECURSE "${ANO_CRYPTO_POC_PACKAGE_DIR}" "${_clean_build_dir}")
file(MAKE_DIRECTORY "${_sdk_dir}" "${_docs_dir}" "${_examples_dir}" "${_evidence_dir}")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -S "${ANO_CRYPTO_SOURCE_DIR}"
        -B "${_clean_build_dir}"
        -DANO_CRYPTO_BUILD_TESTS=OFF
        -DANO_CRYPTO_BUILD_EXAMPLES=OFF
        -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF
        -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF
        -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
        -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON
        -DBUILD_SHARED_LIBS=ON
        -DCMAKE_BUILD_TYPE=${ANO_CRYPTO_POC_BUILD_CONFIG}
    RESULT_VARIABLE _configure_result
    OUTPUT_VARIABLE _configure_stdout
    ERROR_VARIABLE _configure_stderr)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD_CONFIGURE.log"
    "# Configure clean AnoCrypto-C PoC SDK build\n"
    "# Entropy test hooks: OFF\n"
    "# Random lifecycle test hooks: OFF\n"
    "# Internal symbol hiding: ON\n"
    "# Result: ${_configure_result}\n\n"
    "${_configure_stdout}${_configure_stderr}")
if(NOT _configure_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK configure failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD_CONFIGURE.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --parallel
    RESULT_VARIABLE _build_result
    OUTPUT_VARIABLE _build_stdout
    ERROR_VARIABLE _build_stderr)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD.log"
    "# Build clean AnoCrypto-C PoC SDK\n"
    "# Result: ${_build_result}\n\n"
    "${_build_stdout}${_build_stderr}")
if(NOT _build_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK build failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --target ano_crypto_exported_symbol_baseline_check
    RESULT_VARIABLE _symbol_result
    OUTPUT_VARIABLE _symbol_stdout
    ERROR_VARIABLE _symbol_stderr)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_RELEASE_SYMBOL_BASELINE.log"
    "# Compare clean PoC SDK exports with ABI-0 baseline\n"
    "# Expected public symbols: 53\n"
    "# Result: ${_symbol_result}\n\n"
    "${_symbol_stdout}${_symbol_stderr}")
if(NOT _symbol_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK exported-symbol baseline check failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_RELEASE_SYMBOL_BASELINE.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --prefix "${_sdk_dir}"
    RESULT_VARIABLE _install_result
    OUTPUT_VARIABLE _install_stdout
    ERROR_VARIABLE _install_stderr)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log"
    "# Install clean AnoCrypto-C SDK into PoC package\n"
    "# Result: ${_install_result}\n\n"
    "${_install_stdout}${_install_stderr}")
if(NOT _install_result EQUAL 0)
    message(FATAL_ERROR "SDK install into PoC package failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log")
endif()

file(GLOB_RECURSE _installed_headers LIST_DIRECTORIES false "${_sdk_dir}/include/*.h")
foreach(_header IN LISTS _installed_headers)
    file(READ "${_header}" _header_content)
    if(_header_content MATCHES "ano_crypto_internal_|ENTROPY_TEST_HOOK|RANDOM_TEST_HOOK|ENABLE_TEST_HOOK|RAW_NOISE_TEST_CANDIDATE|PROCESS_GUARD|test_set_reseed_counter|HMAC_DRBG_RESEED_INTERVAL")
        message(FATAL_ERROR "PoC public header exposes internal/test-hook token: ${_header}")
    endif()
endforeach()

find_program(_nm_exe nm)
if(_nm_exe)
    file(GLOB_RECURSE _installed_shared_libraries LIST_DIRECTORIES false
        "${_sdk_dir}/*anocrypto_c*.so*"
        "${_sdk_dir}/*anocrypto_c*.dylib")
    foreach(_library IN LISTS _installed_shared_libraries)
        execute_process(
            COMMAND "${_nm_exe}" -D --defined-only "${_library}"
            RESULT_VARIABLE _nm_result
            OUTPUT_VARIABLE _nm_stdout
            ERROR_VARIABLE _nm_stderr)
        if(_nm_result EQUAL 0 AND _nm_stdout MATCHES "ano_crypto_internal_|entropy_test_hook|random_test_hook|test_set_reseed_counter|process_guard")
            message(FATAL_ERROR "PoC shared library exports internal entropy/test-hook symbols: ${_library}")
        endif()
    endforeach()
endif()

set(_doc_files
    README.md
    LICENSE
    docs/business/POC_CUSTOMER_PACKAGE_V0.md
    docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md
    docs/business/POC_PACKAGE_REVIEW_MANIFEST.md
    docs/integration/SDK_INTEGRATION_GUIDE.md
    docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md
    docs/kcmvp/SERVICE_SCOPE_MATRIX.md
    docs/kcmvp/SHA2_384_512_KAT_PLAN.md
    docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md
    docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md
    docs/kcmvp/RELEASE_EVIDENCE_WORKFLOW.md
    docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md
    docs/kcmvp/SBOM_SIGNING_PLAN.md
    docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md
    docs/design/API_ABI_REVIEW_CHECKLIST.md
    docs/design/SHA2_384_512_SCOPE_DECISION.md
    docs/design/PUBLIC_RANDOM_API_DECISION.md
    docs/design/C99_RANDOM_SERVICE_API_DRAFT.md
    docs/design/C99_RANDOM_OPERATIONAL_PORTABILITY_POLICY.md
    docs/design/C99_DRBG_DESIGN.md
    docs/design/C99_ENTROPY_ADAPTER_DESIGN.md
    docs/design/C99_ENTROPY_FAILURE_INJECTION_AND_HEALTH_TEST.md
    docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md
    docs/design/BLOCK_CIPHER_MODE_SELECTION.md
    docs/design/C99_THREAD_SAFETY_POLICY.md
    docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md
    docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md
    docs/ci/CI_MATRIX.md
    docs/ci/THREAD_SAFETY_CI_CHECKLIST.md
    docs/ci/ENTROPY_FAILURE_CI_CHECKLIST.md
    docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md
    docs/ci/RANDOM_SERVICE_CI_CHECKLIST.md
    docs/ci/RANDOM_LIFECYCLE_HARDENING_CI_CHECKLIST.md
    docs/roadmap/DEVELOPMENT_WORKFLOW.md
    docs/roadmap/C99_PRIORITY_TODO.md)

foreach(_doc IN LISTS _doc_files)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_doc}")
        set(_dest_rel "${_doc}")
        if(_dest_rel MATCHES "^docs/(.*)$")
            set(_dest_rel "${CMAKE_MATCH_1}")
        endif()
        get_filename_component(_doc_dir "${_dest_rel}" DIRECTORY)
        if(_doc_dir STREQUAL "" OR _doc_dir STREQUAL ".")
            set(_dest_dir "${_docs_dir}")
        else()
            set(_dest_dir "${_docs_dir}/${_doc_dir}")
        endif()
        file(MAKE_DIRECTORY "${_dest_dir}")
        file(COPY "${ANO_CRYPTO_SOURCE_DIR}/${_doc}" DESTINATION "${_dest_dir}")
    endif()
endforeach()

file(COPY "${ANO_CRYPTO_SOURCE_DIR}/examples/core_poc" DESTINATION "${_examples_dir}")
file(COPY
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/README.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/RANDOM_LIFECYCLE_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/EXPORTED_SYMBOL_BASELINE_REVIEW.md"
    DESTINATION "${_evidence_dir}")

# The customer package may describe the internal boundary, but it must not
# carry internal entropy-provider headers, implementation sources, or the
# dedicated failure-injection test program.
set(_forbidden_internal_package_paths
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/src/internal"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/tests"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_entropy_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_entropy_health_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/entropy_adapter.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/entropy_health.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/process_guard.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_process_guard_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/entropy_operational_policy_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/process_guard_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/platform_entropy_failure_injection_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/random_service_failure_injection_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/random_reseed_limit_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/random_lifecycle_stress_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/random_platform_review.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_drbg_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt")
foreach(_forbidden_path IN LISTS _forbidden_internal_package_paths)
    if(EXISTS "${_forbidden_path}")
        message(FATAL_ERROR "PoC package contains an internal entropy/test artifact: ${_forbidden_path}")
    endif()
endforeach()

string(TIMESTAMP _created_utc "%Y-%m-%dT%H:%M:%SZ" UTC)

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SBOM_STATUS.txt"
    "AnoCrypto-C PoC package SBOM status\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: placeholder only; no complete SBOM is generated for this P1-20 PoC package.\n"
    "Package hash manifest: PACKAGE_MANIFEST.md\n"
    "SBOM plan document: docs/kcmvp/SBOM_SIGNING_PLAN.md\n"
    "Do not add secrets, private keys, tokens, entropy captures, generated secret bytes, customer data, or internal deployment details to SBOM inputs.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_STATUS.txt"
    "AnoCrypto-C PoC package signing status\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: signing not performed for this P1-20 PoC package.\n"
    "No private signing key, signing token, or detached signature file is included.\n"
    "Future detached-signature workflow remains TODO until signing tool, signer identity, and verification policy are selected.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/THREAD_SAFETY_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package thread-safety notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 support-matrix draft for customer PoC review.\n"
    "Support matrix: docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md\n"
    "CI checklist: docs/ci/THREAD_SAFETY_CI_CHECKLIST.md\n"
    "Build option: ANO_CRYPTO_ENABLE_THREAD_SAFETY controls internal global-state locking where supported.\n"
    "Caller-owned hash, HMAC, DRBG, and random-service contexts must not be concurrently mutated by multiple threads.\n"
    "PoC examples do not print keys, entropy input, nonce input, DRBG state, MAC output, generated DRBG bytes, private keys, tokens, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SHA2_SCOPE_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package SHA-384/SHA-512 scope notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 reserved-scope draft for customer PoC review.\n"
    "Scope decision: docs/design/SHA2_384_512_SCOPE_DECISION.md\n"
    "KAT plan: docs/kcmvp/SHA2_384_512_KAT_PLAN.md\n"
    "Current behavior: SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 size/name metadata is queryable, but processing APIs return ANO_CRYPTO_ERR_NOT_IMPLEMENTED.\n"
    "PoC examples do not exercise SHA-384/SHA-512 processing until implementation, KAT, differential-validation, and evidence updates are added.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RANDOM_SERVICE_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package public random-service notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 engineering customer PoC boundary; not official validation evidence.\n"
    "Public random API decision: docs/design/PUBLIC_RANDOM_API_DECISION.md\n"
    "Random service design: docs/design/C99_RANDOM_SERVICE_API_DRAFT.md\n"
    "Random service CI checklist: docs/ci/RANDOM_SERVICE_CI_CHECKLIST.md\n"
    "DRBG/entropy evidence notes: docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md\n"
    "Current behavior: ano_crypto_random_init/reseed draw from the reviewed platform provider and drive HMAC_DRBG-SHA-256; ano_crypto_random_generate accepts only source-backed random contexts.\n"
    "A detected process change returns ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED until ano_crypto_random_reseed succeeds; missing process-marker support makes the public service unavailable.\n"
    "Prediction resistance and automatic VM snapshot/restore or clone detection are not implemented. Known duplication events require discarding inherited state and independently reinitializing each instance.\n"
    "Suspend/resume handling remains a customer product policy; caller-owned contexts require external synchronization.\n"
    "PoC examples report only status and byte counts and do not print entropy, seed material, DRBG state, or generated random bytes.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RANDOM_LIFECYCLE_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package random lifecycle and portability notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 customer PoC boundary; not official validation evidence.\n"
    "Operational policy: docs/design/C99_RANDOM_OPERATIONAL_PORTABILITY_POLICY.md\n"
    "CI checklist: docs/ci/RANDOM_LIFECYCLE_HARDENING_CI_CHECKLIST.md\n"
    "Service availability requires a reviewed platform provider and a supported process marker; unsupported process-marker platforms fail closed.\n"
    "VM snapshot/restore and clone events are not detected automatically. Known duplication events require discarding inherited random contexts and independently initializing each resulting instance.\n"
    "The clean package build disables both entropy and random lifecycle test hooks and hides internal symbols.\n"
    "The clean shared library was checked against the stored ABI-0 baseline of 53 public symbols; see CLEAN_RELEASE_SYMBOL_BASELINE.log.\n"
    "Package records contain status and policy only; generated bytes, entropy, seed material, and DRBG state are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/ENTROPY_HEALTH_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package entropy/health-test boundary notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 engineering boundary summary.\n"
    "The SDK in this package is rebuilt with ANO_CRYPTO_BUILD_TESTS=OFF, ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF, ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF, and ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON.\n"
    "No internal entropy provider header or deterministic failure-injection API is installed.\n"
    "Health-test candidates remain internal, disabled by default, and require noise-source-specific review before operational policy selection.\n"
    "Package logs and evidence must not contain entropy bytes, seed material, DRBG state, generated secret bytes, private keys, tokens, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package entropy operational policy notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-20 engineering boundary summary.\n"
    "OS APIs are classified internally as external conditioned random providers; the module raw-noise health skeleton is not attached to their output.\n"
    "The source-backed DRBG process-change guard is private implementation state and no process-guard type or function is installed.\n"
    "Explicit-entropy DRBG contexts remain caller-managed across process inheritance; this package does not claim automatic fork safety for that API.\n"
    "The public random-generation service is implemented on reviewed provider branches using source-backed HMAC_DRBG-SHA-256 contexts.\n"
    "Package evidence records status and policy only; provider output, seed material, nonce material, DRBG state, and generated random output are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_REVIEW_MANIFEST.md"
    "# AnoCrypto-C PoC Package Review Manifest\n\n"
    "Generated UTC: ${_created_utc}\n\n"
    "Status: engineering package review aid.\n\n"
    "## Required review points\n\n"
    "- Confirm `PACKAGE_MANIFEST.md` lists only package-relative paths.\n"
    "- Confirm `SBOM_STATUS.txt` is still accurate for this package.\n"
    "- Confirm `SIGNING_STATUS.txt` is still accurate for this package.\n"
    "- Confirm `THREAD_SAFETY_PACKAGE_NOTES.txt` matches the current support matrix.\n"
    "- Confirm `SHA2_SCOPE_PACKAGE_NOTES.txt` matches the current SHA-384/SHA-512 reserved scope decision.\n"
    "- Confirm `RANDOM_SERVICE_PACKAGE_NOTES.txt` matches the current public random-service implementation and stated limitations.\n"
    "- Confirm `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt` records fail-closed process-marker, VM clone/snapshot, suspend/resume, and hook boundaries.\n"
    "- Confirm `CLEAN_RELEASE_SYMBOL_BASELINE.log` reports the stored 53-symbol ABI-0 baseline match.\n"
    "- Confirm `ENTROPY_HEALTH_PACKAGE_NOTES.txt` records test hooks OFF and internal symbol hiding ON.\n"
    "- Confirm `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt` records external OS-provider classification, private process guard, and implemented public random service.\n"
    "- Confirm installed headers and shared-library exports do not expose internal entropy, process-guard, reseed-limit, or failure-injection test-hook symbols.\n"
    "- Confirm examples do not print key bytes, entropy bytes, seed material, nonce material, DRBG state, generated DRBG output, private keys, or tokens.\n"
    "- Confirm install logs do not disclose internal deployment details before sharing.\n"
    "- Confirm package contents match the intended customer PoC scope.\n"
    "- Confirm customer secrets, customer data, private keys, signing keys, signing tokens, and entropy captures are not present.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/README.md"
"# AnoCrypto-C PoC Package v0\n\n"
"Generated UTC: ${_created_utc}\n\n"
"This directory is a customer PoC package draft for AnoCrypto-C. It is intended to check C ABI integration, installed CMake package lookup, module lifecycle, self-test startup behavior, and a limited SHA-256/HMAC-SHA-256/HMAC_DRBG-SHA-256 and public random-service smoke flow.\n\n"
"## Contents\n\n"
"- `sdk/`: installed headers, library, and `AnoCryptoC` CMake package files.\n"
"- `examples/core_poc/`: standalone public-API integration example.\n"
"- `docs/`: selected design, integration, evidence, and roadmap notes.\n"
"- `evidence-template/`: release evidence template notes for customer review workflows.\n"
"- `PACKAGE_MANIFEST.md`: package-relative SHA-256 file manifest.\n"
"- `PACKAGE_REVIEW_MANIFEST.md`: review notes before sharing.\n"
"- `SBOM_STATUS.txt` and `SIGNING_STATUS.txt`: placeholder status records.\n"
"- `THREAD_SAFETY_PACKAGE_NOTES.txt`: P1-20 thread-safety support-matrix summary.\n"
"- `SHA2_SCOPE_PACKAGE_NOTES.txt`: P1-20 SHA-384/SHA-512 reserved-scope summary.\n"
"- `RANDOM_SERVICE_PACKAGE_NOTES.txt`: P1-20 public random-service implementation summary.\n"
"- `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt`: P1-20 lifecycle, process-marker, VM duplication, and clean export-baseline summary.\n"
"- `CLEAN_RELEASE_SYMBOL_BASELINE.log`: clean shared-library comparison with the stored 53-symbol ABI-0 baseline.\n"
"- `ENTROPY_HEALTH_PACKAGE_NOTES.txt`: clean-build and internal entropy/test-hook exclusion summary.\n"
"- `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt`: OS-provider classification, private process-boundary guard, and implemented random-service summary.\n\n"
"## Build the PoC example\n\n"
"```sh\n"
"cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=$PWD/sdk\n"
"cmake --build build-core-poc\n"
"./build-core-poc/anocrypto_c_core_poc\n"
"```\n\n"
"## Boundary\n\n"
"This package is not official validation evidence. Review local paths, compiler identifiers, binary provenance, source provenance, manifest files, SBOM/signing status placeholders, thread-safety notes, random-service notes, and logs before sharing externally. Do not add secrets, entropy captures, generated secret bytes, private keys, tokens, signing keys, signing tokens, customer data, or customer deployment details to this package.\n")

file(GLOB_RECURSE _package_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_POC_PACKAGE_DIR}"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/*")
list(SORT _package_files)

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md"
"# AnoCrypto-C PoC Package v0 Manifest\n\n"
"Generated UTC: ${_created_utc}\n\n"
"Status: engineering package draft.\n\n"
"The manifest excludes itself and uses package-relative paths. Review `PACKAGE_REVIEW_MANIFEST.md`, `SBOM_STATUS.txt`, and `SIGNING_STATUS.txt` before external sharing.\n\n"
"## Files\n\n")
foreach(_rel IN LISTS _package_files)
    if(NOT _rel STREQUAL "PACKAGE_MANIFEST.md")
        file(SHA256 "${ANO_CRYPTO_POC_PACKAGE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md" "- `${_rel}` — SHA-256 `${_hash}`\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C PoC package v0 written to ${ANO_CRYPTO_POC_PACKAGE_DIR}")
