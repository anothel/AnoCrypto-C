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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")

if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    message(FATAL_ERROR "Build directory does not exist: ${ANO_CRYPTO_BUILD_DIR}")
endif()

if(NOT DEFINED ANO_CRYPTO_POC_BUILD_CONFIG)
    set(ANO_CRYPTO_POC_BUILD_CONFIG "Release")
endif()

set(_clean_build_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}-clean-release-build")
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
        -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF
        -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=OFF
        -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
        -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON
        -DANO_CRYPTO_SANITIZER_MODE=none
        -DBUILD_SHARED_LIBS=ON
        -DCMAKE_BUILD_TYPE=${ANO_CRYPTO_POC_BUILD_CONFIG}
    RESULT_VARIABLE _configure_result
    OUTPUT_VARIABLE _configure_stdout
    ERROR_VARIABLE _configure_stderr
    TIMEOUT 180)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD_CONFIGURE.log"
    "# Configure clean AnoCrypto-C PoC SDK build\n"
    "# Entropy test hooks: OFF\n"
    "# Random lifecycle test hooks: OFF\n"
    "# OpenSSL differential tests: OFF\n"
    "# OpenSSL differential provider required: OFF\n"
    "# Internal symbol hiding: ON\n"
    "# Sanitizer mode: none\n"
    "# Result: ${_configure_result}\n\n"
    "${_configure_stdout}${_configure_stderr}")
if(NOT _configure_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK configure failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD_CONFIGURE.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
        "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --parallel
    RESULT_VARIABLE _build_result
    OUTPUT_VARIABLE _build_stdout
    ERROR_VARIABLE _build_stderr
    TIMEOUT 600)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD.log"
    "# Build clean AnoCrypto-C PoC SDK\n"
    "# Result: ${_build_result}\n\n"
    "${_build_stdout}${_build_stderr}")
if(NOT _build_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK build failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_BUILD.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
        "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --target ano_crypto_exported_symbol_baseline_check
    RESULT_VARIABLE _symbol_result
    OUTPUT_VARIABLE _symbol_stdout
    ERROR_VARIABLE _symbol_stderr
    TIMEOUT 600)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_RELEASE_SYMBOL_BASELINE.log"
    "# Compare clean PoC SDK exports with current additive baseline\n"
    "# Expected public symbols: ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "# Result: ${_symbol_result}\n\n"
    "${_symbol_stdout}${_symbol_stderr}")
if(NOT _symbol_result EQUAL 0)
    message(FATAL_ERROR "Clean PoC SDK exported-symbol baseline check failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/CLEAN_RELEASE_SYMBOL_BASELINE.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
        "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --target ano_crypto_release_compatibility_policy_check
    RESULT_VARIABLE _release_compat_result
    OUTPUT_VARIABLE _release_compat_stdout
    ERROR_VARIABLE _release_compat_stderr
    TIMEOUT 300)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_COMPATIBILITY_CHECK.log"
    "# Check current release identity and retained P1-29 public compatibility baseline\n"
    "# Expected version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "# Expected public symbols: ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "# Result: ${_release_compat_result}\n\n"
    "${_release_compat_stdout}${_release_compat_stderr}")
if(NOT _release_compat_result EQUAL 0)
    message(FATAL_ERROR "PoC release compatibility policy check failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_COMPATIBILITY_CHECK.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
        "${CMAKE_COMMAND}" --build "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --target ano_crypto_release_provenance_policy_check
    RESULT_VARIABLE _release_provenance_result
    OUTPUT_VARIABLE _release_provenance_stdout
    ERROR_VARIABLE _release_provenance_stderr
    TIMEOUT 300)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_PROVENANCE_POLICY_CHECK.log"
    "# Check P1-33 unsigned release provenance and source-SBOM policy boundary\n"
    "# Expected version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "# Signing mode: ${ANO_CRYPTO_RELEASE_SIGNING_MODE}\n"
    "# Result: ${_release_provenance_result}\n\n"
    "${_release_provenance_stdout}${_release_provenance_stderr}")
if(NOT _release_provenance_result EQUAL 0)
    message(FATAL_ERROR "PoC release provenance policy check failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_PROVENANCE_POLICY_CHECK.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${_clean_build_dir}" --config "${ANO_CRYPTO_POC_BUILD_CONFIG}" --prefix "${_sdk_dir}"
    RESULT_VARIABLE _install_result
    OUTPUT_VARIABLE _install_stdout
    ERROR_VARIABLE _install_stderr
    TIMEOUT 300)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log"
    "# Install clean AnoCrypto-C SDK into PoC package\n"
    "# Result: ${_install_result}\n\n"
    "${_install_stdout}${_install_stderr}")
if(NOT _install_result EQUAL 0)
    message(FATAL_ERROR "SDK install into PoC package failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log")
endif()

if(NOT EXISTS "${_sdk_dir}/include/anocrypto/ac_cipher.h")
    message(FATAL_ERROR "PoC SDK is missing the reviewed public AES/CBC header: ${_sdk_dir}/include/anocrypto/ac_cipher.h")
endif()

if(NOT EXISTS "${_sdk_dir}/include/anocrypto/ac_cmac.h")
    message(FATAL_ERROR "PoC SDK is missing the reviewed public AES-CMAC header: ${_sdk_dir}/include/anocrypto/ac_cmac.h")
endif()

file(GLOB_RECURSE _installed_headers LIST_DIRECTORIES false "${_sdk_dir}/include/*.h")
foreach(_header IN LISTS _installed_headers)
    file(READ "${_header}" _header_content)
    if(_header_content MATCHES "ano_crypto_internal_|ENTROPY_TEST_HOOK|RANDOM_TEST_HOOK|ENABLE_TEST_HOOK|RAW_NOISE_TEST_CANDIDATE|PROCESS_GUARD|test_set_reseed_counter|HMAC_DRBG_RESEED_INTERVAL|openssl/|OPENSSL_DIFFERENTIAL")
        message(FATAL_ERROR "PoC public header exposes internal/test-hook token: ${_header}")
    endif()
endforeach()

find_program(_nm_exe nm)
file(GLOB_RECURSE _installed_shared_libraries LIST_DIRECTORIES false
    "${_sdk_dir}/*anocrypto_c*.so*"
    "${_sdk_dir}/*anocrypto_c*.dylib")
if(_installed_shared_libraries AND NOT _nm_exe)
    message(FATAL_ERROR "PoC shared-library inspection requires nm")
endif()
if(_nm_exe)
    foreach(_library IN LISTS _installed_shared_libraries)
        execute_process(
            COMMAND "${_nm_exe}" -D --defined-only "${_library}"
            RESULT_VARIABLE _nm_result
            OUTPUT_VARIABLE _nm_stdout
            ERROR_VARIABLE _nm_stderr
            TIMEOUT 60)
        if(NOT _nm_result EQUAL 0)
            message(FATAL_ERROR
                "PoC shared-library exported-symbol inspection failed: ${_library}: ${_nm_stderr}")
        endif()
        if(_nm_stdout MATCHES "ano_crypto_internal_|entropy_test_hook|random_test_hook|test_set_reseed_counter|process_guard")
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
    docs/kcmvp/OPENSSL_DIFFERENTIAL_PROVIDER_DECISION.md
    docs/kcmvp/RELEASE_EVIDENCE_WORKFLOW.md
    docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md
    docs/kcmvp/SBOM_SIGNING_PLAN.md
    docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md
    docs/design/API_ABI_REVIEW_CHECKLIST.md
    docs/design/C99_HKDF_SHA256_API_GATE.md
    docs/design/C99_AES256_GCM_API_GATE.md
    docs/design/P3_ABI_EVOLUTION_GATE.md
    docs/integration/ANOSECUREKIT_SKF1_HKDF_COMPATIBILITY.md
    docs/integration/ANOSECUREKIT_AES256_GCM_COMPATIBILITY.md
    docs/product/P3_02_IMPLEMENTATION_TEST_DELTA.md
    docs/product/P3_03_IMPLEMENTATION_TEST_DELTA.md
    docs/design/BUILD_IDENTITY_ABI_0_DECISION.md
    docs/design/SHA2_384_512_SCOPE_DECISION.md
    docs/design/SHA512_FAMILY_PORTABILITY_POLICY.md
    docs/design/PUBLIC_RANDOM_API_DECISION.md
    docs/design/C99_RANDOM_SERVICE_API_DRAFT.md
    docs/design/C99_RANDOM_OPERATIONAL_PORTABILITY_POLICY.md
    docs/design/C99_DRBG_DESIGN.md
    docs/design/C99_ENTROPY_ADAPTER_DESIGN.md
    docs/design/C99_ENTROPY_FAILURE_INJECTION_AND_HEALTH_TEST.md
    docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md
    docs/design/BLOCK_CIPHER_MODE_SELECTION.md
    docs/design/C99_BLOCK_CIPHER_API_GATE.md
    docs/design/AES_CBC_IMPLEMENTATION_POLICY.md
    docs/design/AES_CBC_HARDENING_POLICY.md
    docs/design/CMAC_SELECTION_DECISION.md
    docs/design/C99_CMAC_API_GATE.md
    docs/design/AES_CMAC_HARDENING_POLICY.md
    docs/kcmvp/BLOCK_CIPHER_KAT_AND_EVIDENCE_PLAN.md
    docs/kcmvp/AES_COMPILER_OUTPUT_REVIEW_PLAN.md
    docs/kcmvp/CMAC_KAT_AND_EVIDENCE_PLAN.md
    docs/kcmvp/CMAC_COMPILER_OUTPUT_REVIEW_PLAN.md
    docs/design/C99_THREAD_SAFETY_POLICY.md
    docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md
    docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md
    docs/ci/CI_MATRIX.md
    docs/ci/THREAD_SAFETY_CI_CHECKLIST.md
    docs/ci/CONCURRENCY_SANITIZER_CI_CHECKLIST.md
    docs/ci/HOSTED_PLATFORM_ARTIFACT_REVIEW.md
    docs/ci/ENTROPY_FAILURE_CI_CHECKLIST.md
    docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md
    docs/ci/RANDOM_SERVICE_CI_CHECKLIST.md
    docs/ci/RANDOM_LIFECYCLE_HARDENING_CI_CHECKLIST.md
    docs/ci/DIFFERENTIAL_PORTABILITY_CI_CHECKLIST.md
    docs/ci/AES_HARDENING_CI_CHECKLIST.md
    docs/ci/AES_CMAC_CI_CHECKLIST.md
    docs/ci/CMAC_HARDENING_CI_CHECKLIST.md
    docs/ci/RELEASE_COMPATIBILITY_CI_CHECKLIST.md
    docs/ci/RELEASE_PROVENANCE_CI_CHECKLIST.md
    docs/release/TAGGED_RELEASE_COMPATIBILITY_POLICY.md
    docs/release/RELEASE_PROVENANCE_AND_SIGNING_POLICY.md
    docs/kcmvp/TAGGED_RELEASE_COMPATIBILITY_EVIDENCE.md
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
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/RELEASE_COMPATIBILITY_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/RANDOM_LIFECYCLE_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/EXPORTED_SYMBOL_BASELINE_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/CONCURRENCY_SANITIZER_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/SHA512_FAMILY_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/DIFFERENTIAL_PORTABILITY_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/AES_CBC_IMPLEMENTATION_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/AES_HARDENING_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/AES_CMAC_IMPLEMENTATION_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/AES_CMAC_HARDENING_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/RELEASE_PROVENANCE_REVIEW.md"
    "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/AES256_GCM_IMPLEMENTATION_REVIEW.md"
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
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/anocrypto_c.map"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/hkdf_sha256_kat_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/hkdf_sha256_api_negative_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/hkdf_sha256_openssl_differential_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes256_gcm_kat_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes256_gcm_api_negative_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes256_gcm_openssl_differential_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/sha2_hmac_openssl_differential_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cbc_openssl_differential_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cbc_hardening_review.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cbc_lifecycle_stress_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES_COMPILER_OUTPUT_REVIEW.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_aes_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_cipher_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/ac_cmac_internal.h"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/abi/baselines"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/tests/compat"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/release"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/cmac.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/cmac_kat_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/cmac_api_negative_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cmac_openssl_differential_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cmac_hardening_review.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/aes_cmac_lifecycle_stress_tests.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/CMAC_COMPILER_OUTPUT_REVIEW.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/capture_cmac_compiler_review.cmake"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/check_cmac_hardening_policy.cmake"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/src/cipher"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/sha512_portability_review.c"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/tests/differential"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/openssl"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/libcrypto"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/libssl"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/anocrypto-c-source.tar.gz"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SOURCE_FILES.sha256"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SOURCE_SBOM.spdx.json"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SOURCE_SBOM_SUMMARY.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_PROVENANCE.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_ARTIFACTS.sha256"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_REQUEST.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_POLICY.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_HANDOFF_FILES.sha256"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_HANDOFF_STATUS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/APPROVED_RELEASE_SIGNERS.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/APPROVED_TIMESTAMP_POLICY.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_SIGNING_HANDOFF_CHECK.txt"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/release-signing-handoff"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_AUTHORIZATION.md")
foreach(_forbidden_path IN LISTS _forbidden_internal_package_paths)
    if(EXISTS "${_forbidden_path}")
        message(FATAL_ERROR "PoC package contains an internal entropy/test artifact or release artifact: ${_forbidden_path}")
    endif()
endforeach()


file(GLOB_RECURSE _package_boundary_files LIST_DIRECTORIES false "${ANO_CRYPTO_POC_PACKAGE_DIR}/*")
foreach(_package_file IN LISTS _package_boundary_files)
    string(TOLOWER "${_package_file}" _package_file_lower)
    if(_package_file_lower MATCHES "(^|[/\\])(libcrypto|libssl|openssl)([./\\]|$)" OR
       _package_file_lower MATCHES "(sha2_hmac|hkdf_sha256|aes_cbc|aes_cmac|aes256_gcm)_openssl_differential_tests\.c$" OR
       _package_file_lower MATCHES "sha512_portability_review\.c$" OR
       _package_file_lower MATCHES "aes_(cbc|cmac)_(hardening_review|lifecycle_stress_tests)\.c$" OR
       _package_file_lower MATCHES "(aes|cmac)_compiler_output_(review|disassembly)\.txt$" OR
       _package_file_lower MATCHES "(capture_cmac_compiler_review|check_cmac_hardening_policy)\.cmake$" OR
       _package_file_lower MATCHES "ac_(aes|cipher|cmac)_internal\.h$" OR
       _package_file_lower MATCHES "cmac(_kat_tests|_api_negative_tests|_openssl_differential_tests)?\.c$" OR
       _package_file_lower MATCHES "p1_29_compatibility_probe\.c$" OR
       _package_file_lower MATCHES "hosted_artifact_manifest" OR
       _package_file_lower MATCHES "(create_release_signing_handoff|check_release_signing_handoff|verify_release_detached_signature)\.cmake$" OR
       _package_file_lower MATCHES "(^|[/\\])(source_sbom\.spdx\.json|source_files\.sha256|release_provenance\.txt|release_artifacts\.sha256|signing_request\.txt|signing_policy\.txt|signing_handoff_files\.sha256|signing_handoff_status\.txt|approved_release_signers\.txt|approved_timestamp_policy\.txt|release_signing_handoff_check\.txt|release_authorization\.md|anocrypto-c-source\.tar\.gz)$" OR
       _package_file_lower MATCHES "(^|[/\\])release-signing-handoff([/\\]|$)" OR
       _package_file_lower MATCHES "\.(ed25519|tsq|tsr|pem|key|p12|pfx|jks|keystore|sig|asc)$" OR
       _package_file_lower MATCHES "(^|[/\\])abi[/\\]baselines([/\\]|$)")
        message(FATAL_ERROR "PoC package contains a test-only differential/provider artifact: ${_package_file}")
    endif()
endforeach()

string(TIMESTAMP _created_utc "%Y-%m-%dT%H:%M:%SZ" UTC)

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SBOM_STATUS.txt"
    "AnoCrypto-C PoC package SBOM status\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: PACKAGE_SBOM_NOT_GENERATED\n"
    "The P1-33 internal source-inventory SBOM describes the reviewed source archive, not this customer PoC package.\n"
    "The internal source archive and source SBOM are not included in this package.\n"
    "Package hash manifest: PACKAGE_MANIFEST.md\n"
    "SBOM plan document: docs/kcmvp/SBOM_SIGNING_PLAN.md\n"
    "Dependency discovery, vulnerability analysis, and license conclusions for this PoC package remain TODO.\n"
    "Do not add secrets, private keys, tokens, entropy captures, generated secret bytes, customer data, or internal deployment details to SBOM inputs.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SIGNING_STATUS.txt"
    "AnoCrypto-C PoC package signing status\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: UNSIGNED\n"
    "Detached signature present: no\n"
    "Cryptographic verification performed: no\n"
    "External release authorized: NO\n"
    "Signing handoff readiness: BLOCKED\n"
    "P1-33 policy subject: RELEASE_ARTIFACTS.sha256\n"
    "P1-33 policy: Ed25519 raw 64-byte signature, SHA-256 SPKI DER signer fingerprint, and required RFC3161 timestamp over the signature.\n"
    "Security Reviewer and Product Release Owner identities must be distinct; GitHub artifact attestation is optional secondary evidence only.\n"
    "No signing key, token, signer public-key record, detached signature, timestamp response, certificate, or trust anchor is included.\n"
    "The production signer and timestamp authority remain unprovisioned.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_PROVENANCE_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package release provenance boundary\n"
    "Generated UTC: ${_created_utc}\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Signing mode: UNSIGNED\n"
    "External release authorized: NO\n"
    "Signing handoff readiness: BLOCKED\n"
    "The internal source archive and source SBOM are not included in this customer package.\n"
    "The package does not include RELEASE_PROVENANCE.txt, RELEASE_ARTIFACTS.sha256, the internal signing handoff, signer/TSA approval records, release authorization records, keys, tokens, signatures, timestamp responses, certificates, or trust anchors.\n"
    "PACKAGE_MANIFEST.md is a package-local hash inventory and is not a cryptographic attestation or release authorization.\n"
    "Actual tag: NOT_CREATED; tag/commit verification: NOT_RUN. Hosted execution: NOT_RUN; hosted review: UNREVIEWED.\n"
    "Package-specific SBOM and distinct role approvals remain TODO; production signer/TSA status is UNPROVISIONED, signing/timestamp verification is NOT_RUN, handoff is BLOCKED, and external sharing authorization is NO.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/THREAD_SAFETY_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package thread-safety notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 retained support-matrix draft for customer PoC review.\n"
    "Support matrix: docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md\n"
    "CI checklist: docs/ci/THREAD_SAFETY_CI_CHECKLIST.md\n"
    "Build option: ANO_CRYPTO_ENABLE_THREAD_SAFETY controls internal global-state locking where supported.\n"
    "Caller-owned hash, HMAC, DRBG, and random-service contexts must not be concurrently mutated by multiple threads.\n"
    "PoC examples do not print keys, entropy input, nonce input, DRBG state, MAC output, generated DRBG bytes, private keys, tokens, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package concurrency and sanitizer notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering boundary; not official validation evidence.\n"
    "Cross-platform concurrency test: Win32 native threads or POSIX pthreads in internal test builds.\n"
    "Selected required diagnostic lane: Linux GCC address+undefined sanitizer. Clang runtime review and ThreadSanitizer remain TODO.\n"
    "The clean PoC SDK is rebuilt with ANO_CRYPTO_SANITIZER_MODE=none, tests/hooks OFF, and internal symbols hidden.\n"
    "Build-identity ABI-0 layout is retained at version 1 without adding public symbols; see docs/design/BUILD_IDENTITY_ABI_0_DECISION.md.\n"
    "Actual hosted platform artifacts must be reviewed separately before widening support wording.\n"
    "Package records must not contain key bytes, MAC output, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/SHA512_FAMILY_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package SHA-384/SHA-512 family notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 retained engineering implementation summary for customer PoC review; not official validation evidence.\n"
    "Implementation decision: docs/design/SHA2_384_512_SCOPE_DECISION.md\n"
    "KAT record: docs/kcmvp/SHA2_384_512_KAT_PLAN.md\n"
    "Current behavior: SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 one-shot, streaming, and HMAC key-handle processing are implemented.\n"
    "Startup KATs cover empty/abc and RFC 4231 cases 1/2; extended CTest coverage includes long messages, million-a, RFC 4231 cases 3/4/6/7, and negative API boundaries.\n"
    "PoC examples exercise all three SHA-2 digest and HMAC variants but report only status and byte counts; digest and MAC values are not printed.\n"
    "Current internal engineering tests retain deterministic OpenSSL 3.x test-only SHA-2/HMAC, HKDF-SHA-256, AES-256-GCM, AES-CBC, and AES-CMAC differential and portability coverage; provider binaries, headers, source, and result values are excluded from this package.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package differential-validation and portability boundary notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 retained engineering package boundary; not official validation evidence.\n"
    "OpenSSL 3.x is used only by internal test executables for deterministic SHA-2/HMAC, HKDF-SHA-256, AES-CBC, AES-CMAC, and AES-256-GCM comparison.\n"
    "The clean PoC SDK is configured with ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF and ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=OFF.\n"
    "The installed SDK and public example do not include or require OpenSSL headers, libcrypto, differential-test source, reference outputs, or test corpus files.\n"
    "C99 build guards require 8-bit bytes, 32-bit uint32_t, 64-bit uint64_t, and size_t no wider than uint64_t. Unaligned-input and endian metadata are exercised by internal tests only.\n"
    "Actual hosted 32-bit, Windows, macOS, and big-endian artifacts require separate review before portability support wording changes.\n"
    "Package records contain status and policy only; digest, MAC, AES key, IV, plaintext, ciphertext, entropy, seed, DRBG state, and generated random values are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES_CBC_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package AES/CBC notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 retained engineering implementation summary; not official validation evidence.\n"
    "Public scope: AES-128/192/256 and CBC without padding through CSP-backed in-memory keys.\n"
    "API rules: caller-provided 16-byte IV, complete non-empty blocks, exact in-place allowed, partial overlap rejected.\n"
    "The package includes public ac_cipher.h and the product library, but excludes internal AES context headers, implementation source, KAT/differential source, and reference outputs.\n"
    "OpenSSL is used only by internal test executables and is not an SDK or PoC runtime dependency.\n"
    "PoC output records status and byte counts only; AES keys, IVs, plaintext, ciphertext, expanded schedules, and comparison values are not printed.\n"
    "Compiler-output and target-specific side-channel review remain TODO before stronger implementation claims.\n")


file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES_HARDENING_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package AES/CBC hardening notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering boundary; not official validation evidence.\n"
    "Public API boundary: the same eight cipher functions remain on the retained 69-symbol ANOCRYPTO_C_0 line; the current additive total is 72.\n"
    "Internal hardening: bounded context integrity metadata, fail-closed reinitialization, lifecycle/unaligned/streaming/concurrency regressions, and expanded test-only differential comparison.\n"
    "Compiler-output capture and full disassembly are internal review evidence and are not included in this customer package. Manual compiler/target review remains required.\n"
    "The package excludes internal AES source/context headers, hardening/lifecycle test source, OpenSSL differential source/provider artifacts, and compiler disassembly.\n"
    "Package records contain status, counts, and policy only; keys, IVs, plaintext, ciphertext, expanded schedules, intermediate state, entropy, seed, DRBG state, and generated random values are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES_CMAC_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package AES-CMAC notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering implementation and hardening boundary; not official validation evidence.\n"
    "Public scope: AES-CMAC with AES-128/192/256 dedicated CSP handles, full 16-byte tags, eight public functions on the retained 69-symbol ANOCRYPTO_C_0 line; the current additive total is 72.\n"
    "Lifecycle policy: successful init owns a copied schedule; failed valid-size reinit clears prior state; final consumes the context.\n"
    "Buffer policy: reviewed context/input/output/output-length overlaps are rejected; short uninit clears the supplied extent and requires a correct-size cleanup retry; oversized uninit does not clear trailing storage.\n"
    "Concurrency policy: separate contexts and shared read-only CMAC handles are covered by engineering regression; one caller context is not concurrently mutable.\n"
    "Cross-purpose policy: AES encryption handles and CMAC handles are rejected by the other service.\n"
    "Package boundary: public ac_cmac.h and policy notes are included; internal CMAC context/source/tests, compiler disassembly, and OpenSSL provider artifacts are excluded.\n"
    "Compiler/timing boundary: target-specific human review remains required and no completed side-channel conclusion is claimed.\n"
    "Key, message, tag, subkey, chain, AES schedule, entropy, seed, DRBG state, random output, private key, token, and customer data are not included.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RANDOM_SERVICE_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package public random-service notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering customer PoC boundary; not official validation evidence.\n"
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
    "Status: P1-29 customer PoC boundary; not official validation evidence.\n"
    "Operational policy: docs/design/C99_RANDOM_OPERATIONAL_PORTABILITY_POLICY.md\n"
    "CI checklist: docs/ci/RANDOM_LIFECYCLE_HARDENING_CI_CHECKLIST.md\n"
    "Service availability requires a reviewed platform provider and a supported process marker; unsupported process-marker platforms fail closed.\n"
    "VM snapshot/restore and clone events are not detected automatically. Known duplication events require discarding inherited random contexts and independently initializing each resulting instance.\n"
    "The clean package build disables both entropy and random lifecycle test hooks and hides internal symbols.\n"
    "The clean shared library was checked against the 72-symbol current additive baseline while retaining all 69 ANOCRYPTO_C_0 symbols; see CLEAN_RELEASE_SYMBOL_BASELINE.log.\n"
    "Package records contain status and policy only; generated bytes, entropy, seed material, and DRBG state are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/ENTROPY_HEALTH_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package entropy/health-test boundary notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering boundary summary.\n"
    "The SDK in this package is rebuilt with ANO_CRYPTO_BUILD_TESTS=OFF, ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF, ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF, and ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON.\n"
    "No internal entropy provider header or deterministic failure-injection API is installed.\n"
    "Health-test candidates remain internal, disabled by default, and require noise-source-specific review before operational policy selection.\n"
    "Package logs and evidence must not contain entropy bytes, seed material, DRBG state, generated secret bytes, private keys, tokens, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package entropy operational policy notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P1-29 engineering boundary summary.\n"
    "OS APIs are classified internally as external conditioned random providers; the module raw-noise health skeleton is not attached to their output.\n"
    "The source-backed DRBG process-change guard is private implementation state and no process-guard type or function is installed.\n"
    "Explicit-entropy DRBG contexts remain caller-managed across process inheritance; this package does not claim automatic fork safety for that API.\n"
    "The public random-generation service is implemented on reviewed provider branches using source-backed HMAC_DRBG-SHA-256 contexts.\n"
    "Package evidence records status and policy only; provider output, seed material, nonce material, DRBG state, and generated random output are excluded.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/HKDF_SHA256_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package HKDF-SHA-256 notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P3-02 local engineering evidence; not official validation or external release evidence.\n"
    "Public primitive: one generic RFC 5869 extract-and-expand function in ac_kdf.h.\n"
    "Output contract: 0..8160 bytes accepted; 8161 rejected; structurally valid output is fully cleared on every failure.\n"
    "Empty IKM, salt, info, and zero-length output semantics are defined by the public header and API gate.\n"
    "AnoSecureKit SKF1 framing, workflow, and product policy are not implemented by AnoCrypto-C.\n"
    "Compatibility evidence covers only the exact 28-byte SKF1 v1 info label and resulting 32-byte file-key vector.\n"
    "OpenSSL is used only by internal test executables for differential validation and is not a package runtime dependency.\n"
    "Current ABI: 69 retained ANOCRYPTO_C_0 symbols plus ano_crypto_hkdf_sha256 on ANOCRYPTO_C_1.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/AES256_GCM_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package AES-256-GCM one-shot notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Status: P3-03 local engineering evidence; not official validation or external release evidence.\n"
    "Public primitive: generic AES-256-GCM one-shot encrypt and decrypt functions in ac_aead.h.\n"
    "Key contract: existing in-memory block-cipher CSP handle with exactly 32 key bytes; 12-byte nonce and 16-byte tag only.\n"
    "Buffer contract: empty AAD and payload are valid; exact payload in-place is supported; partial overlap is rejected.\n"
    "Decryption authenticates the complete input before writing any plaintext. Authentication or validation failure returns no partial plaintext and clears the accepted output range.\n"
    "AnoSecureKit SKT1, SKF1, and SKP1 framing, nonce construction, password derivation, workflow, and product policy are not implemented by AnoCrypto-C.\n"
    "Compatibility evidence covers exact AnoSecureKit v1 GCM inputs and outputs only.\n"
    "OpenSSL is used only by internal test executables for differential validation and is not a package runtime dependency.\n"
    "Current ABI: 69 retained ANOCRYPTO_C_0 symbols, one HKDF symbol on ANOCRYPTO_C_1, and two AES-256-GCM symbols on ANOCRYPTO_C_2; current total 72.\n"
    "KCMVP remains deferred; hosted lanes remain NOT_RUN/UNREVIEWED; repository signing remains UNSIGNED; external release authorization remains NO.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt"
    "AnoCrypto-C PoC package release compatibility notes\n"
    "Generated UTC: ${_created_utc}\n"
    "Current version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected release tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Compatibility baseline: ${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\n"
    "Public headers: ${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}\n"
    "Retained ANOCRYPTO_C_0 exports: 69\n"
    "Current additive exports: ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "Current ABI line: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "Clean compatibility policy result: RELEASE_COMPATIBILITY_CHECK.log\n"
    "Clean provenance policy result: RELEASE_PROVENANCE_POLICY_CHECK.log\n"
    "The retained P1-29 header snapshot, compatibility probe source, release tooling, and hosted CI manifests are not included in the customer package.\n"
    "Expected tag metadata does not establish that a tag exists or identifies this binary; release provenance requires separate review.\n"
    "Compatibility results are toolchain-specific engineering guardrails and do not establish universal ABI or semantic compatibility.\n"
    "Package records contain no keys, entropy, seed material, DRBG state, generated random bytes, CMAC tags, or customer data.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_REVIEW_MANIFEST.md"
    "# AnoCrypto-C PoC Package Review Manifest\n\n"
    "Generated UTC: ${_created_utc}\n\n"
    "Status: engineering package review aid.\n\n"
    "## Required review points\n\n"
    "- Confirm `PACKAGE_MANIFEST.md` lists only package-relative paths.\n"
    "- Confirm `SBOM_STATUS.txt` is still accurate for this package.\n"
    "- Confirm `SIGNING_STATUS.txt` is still accurate for this package.\n"
    "- Confirm `RELEASE_PROVENANCE_PACKAGE_NOTES.txt` states that the internal source archive/SBOM/handoff and all credential/signature/timestamp artifacts are excluded and external release remains unauthorized.\n"
    "- Confirm `THREAD_SAFETY_PACKAGE_NOTES.txt` matches the current support matrix.\n"
    "- Confirm `CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt` records clean sanitizer mode, hosted-review limits, and ABI-0 decision.\n"
    "- Confirm `SHA512_FAMILY_PACKAGE_NOTES.txt` matches the current SHA-384/SHA-512 implementation and KAT boundary.\n"
    "- Confirm `DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt` records the test-only OpenSSL boundary and unreviewed hosted portability artifacts.\n"
    "- Confirm `HKDF_SHA256_PACKAGE_NOTES.txt` records the generic primitive boundary, exact SKF1 info-byte evidence, 8160-byte limit, clearing rule, and additive ABI line.\n"
    "- Confirm `AES256_GCM_PACKAGE_NOTES.txt` records the generic one-shot AEAD boundary, fixed key/nonce/tag sizes, authenticate-before-release rule, exact AnoSecureKit compatibility evidence, and additive ABI line.\n"
    "- Confirm `AES_CBC_PACKAGE_NOTES.txt` records the public AES/CBC scope, internal-source exclusion, and remaining side-channel review.\n"
    "- Confirm `AES_HARDENING_PACKAGE_NOTES.txt` records retained AES/CBC public API, internal hardening, compiler-review boundary, and excluded artifacts.\n"
    "- Confirm `AES_CMAC_PACKAGE_NOTES.txt` records the full-tag API, dedicated key purpose, hardening lifecycle/alias/cleanup rules, retained 69-symbol ANOCRYPTO_C_0 baseline, current 72-symbol total, and internal/provider exclusions.\n"
    "- Confirm `RANDOM_SERVICE_PACKAGE_NOTES.txt` matches the current public random-service implementation and stated limitations.\n"
    "- Confirm `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt` records fail-closed process-marker, VM clone/snapshot, suspend/resume, and hook boundaries.\n"
    "- Confirm `CLEAN_RELEASE_SYMBOL_BASELINE.log` reports the 72-symbol current additive baseline and retained 69-symbol ANOCRYPTO_C_0 match.\n"
    "- Confirm `RELEASE_COMPATIBILITY_CHECK.log` and `RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt` match the P3-03/P1-29 compatibility policy.\n"
    "- Confirm `RELEASE_PROVENANCE_POLICY_CHECK.log` records a passing unsigned/source-SBOM boundary check without asserting a signature or external approval.\n"
    "- Confirm no retained baseline header tree, compatibility test source, hosted artifact manifest, source archive, source SBOM, signing handoff/policy/request, signer/TSA record, release authorization record, key, token, signature, timestamp response, certificate, trust anchor, or release-internal metadata directory is packaged.\n"
    "- Confirm `ENTROPY_HEALTH_PACKAGE_NOTES.txt` records test hooks OFF and internal symbol hiding ON.\n"
    "- Confirm `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt` records external OS-provider classification, private process guard, and implemented public random service.\n"
    "- Confirm installed headers and shared-library exports do not expose internal entropy, process-guard, reseed-limit, failure-injection test-hook, or internal AES context symbols.\n"
    "- Confirm compiler-output review/disassembly and AES/CMAC hardening/lifecycle test sources are not in the customer package.\n"
    "- Confirm the SDK and example do not contain or depend on OpenSSL headers, libcrypto/libssl, differential source, or reference outputs.\n"
    "- Confirm examples do not print key bytes, entropy bytes, seed material, nonce material, DRBG state, generated DRBG output, private keys, or tokens.\n"
    "- Confirm install logs do not disclose internal deployment details before sharing.\n"
    "- Confirm package contents match the intended customer PoC scope.\n"
    "- Confirm customer secrets, customer data, private keys, signing keys, signing tokens, and entropy captures are not present.\n")

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/README.md"
"# AnoCrypto-C PoC Package v0\n\n"
"Generated UTC: ${_created_utc}\n\n"
"This directory is a customer PoC package draft for AnoCrypto-C. It checks C ABI integration, installed CMake package lookup, module lifecycle, self-test startup behavior, SHA-2/HMAC, HKDF-SHA-256, HMAC_DRBG-SHA-256, AES-CBC, AES-CMAC, AES-256-GCM one-shot, and public random-service smoke flows.\n\n"
"## Contents\n\n"
"- `sdk/`: installed headers, library, and `AnoCryptoC` CMake package files.\n"
"- `examples/core_poc/`: standalone public-API integration example.\n"
"- `docs/`: selected design, integration, evidence, and roadmap notes.\n"
"- `evidence-template/`: release evidence template notes for customer review workflows.\n"
"- `PACKAGE_MANIFEST.md`: package-relative SHA-256 file manifest.\n"
"- `PACKAGE_REVIEW_MANIFEST.md`: review notes before sharing.\n"
"- `SBOM_STATUS.txt` and `SIGNING_STATUS.txt`: explicit package-SBOM-not-generated and unsigned status records.\n"
"- `RELEASE_PROVENANCE_PACKAGE_NOTES.txt`: customer-package exclusion and external-release authorization boundary.\n"
"- `THREAD_SAFETY_PACKAGE_NOTES.txt`: P1-29 retained thread-safety support-matrix summary.\n"
"- `CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt`: P1-29 retained cross-platform concurrency, diagnostic-lane, and build-identity ABI summary.\n"
"- `SHA512_FAMILY_PACKAGE_NOTES.txt`: P1-29 retained SHA-384/SHA-512 family implementation and KAT summary.\n"
"- `DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt`: test-only OpenSSL provider and portability-review boundary.\n"
"- `HKDF_SHA256_PACKAGE_NOTES.txt`: P3-02 generic HKDF API, boundary, exact SKF1 compatibility evidence, and additive ABI summary.\n"
"- `AES256_GCM_PACKAGE_NOTES.txt`: P3-03 generic AES-256-GCM one-shot API, authenticate-before-release boundary, exact AnoSecureKit compatibility evidence, and additive ABI summary.\n"
"- `AES_CBC_PACKAGE_NOTES.txt`: public AES/CBC scope, package boundary, and remaining review notes.\n"
"- `AES_HARDENING_PACKAGE_NOTES.txt`: unchanged public API, lifecycle hardening, and compiler-review artifact boundary.\n"
"- `AES_CMAC_PACKAGE_NOTES.txt`: P1-29 AES-CMAC lifecycle, alias, bounded-cleanup, concurrency, compiler-review, dedicated key-purpose, and retained 69-symbol ANOCRYPTO_C_0 boundary and current 72-symbol total.\n"
"- `RANDOM_SERVICE_PACKAGE_NOTES.txt`: P1-29 retained public random-service implementation summary.\n"
"- `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt`: P1-29 retained lifecycle, process-marker, VM duplication, and clean export-baseline summary.\n"
"- `CLEAN_RELEASE_SYMBOL_BASELINE.log`: clean shared-library comparison with the current 72-symbol additive baseline and retained 69-symbol ANOCRYPTO_C_0 line.\n"
"- `RELEASE_COMPATIBILITY_CHECK.log`: current release identity and retained P1-29 public source/symbol compatibility policy result.\n"
"- `RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt`: customer-facing compatibility scope, exclusions, and provenance limitations.\n"
"- `RELEASE_PROVENANCE_POLICY_CHECK.log`: status-only P3-03 unsigned/source-SBOM policy result.\n"
"- `ENTROPY_HEALTH_PACKAGE_NOTES.txt`: clean-build and internal entropy/test-hook exclusion summary.\n"
"- `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt`: OS-provider classification, private process-boundary guard, and implemented random-service summary.\n\n"
"## Build the PoC example\n\n"
"```sh\n"
"cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=$PWD/sdk\n"
"cmake --build build-core-poc\n"
"./build-core-poc/anocrypto_c_core_poc\n"
"```\n\n"
"## Boundary\n\n"
"This package is not official validation evidence. Review local paths, compiler identifiers, binary provenance, source provenance, manifest files, package SBOM/signing status boundaries, thread-safety notes, random-service notes, and logs before sharing externally. Do not add secrets, entropy captures, generated secret bytes, private keys, tokens, signing keys, signing tokens, signer/TSA records, signatures, timestamp responses, certificates, trust anchors, customer data, or customer deployment details to this package.\n")

file(GLOB_RECURSE _package_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_POC_PACKAGE_DIR}"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/*")
list(SORT _package_files)

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md"
"# AnoCrypto-C PoC Package v0 Manifest\n\n"
"Generated UTC: ${_created_utc}\n\n"
"Status: engineering package draft.\n\n"
"The manifest excludes itself and uses package-relative paths. Review `PACKAGE_REVIEW_MANIFEST.md`, `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, and `RELEASE_PROVENANCE_PACKAGE_NOTES.txt` before external sharing.\n\n"
"## Files\n\n")
foreach(_rel IN LISTS _package_files)
    if(NOT _rel STREQUAL "PACKAGE_MANIFEST.md")
        file(SHA256 "${ANO_CRYPTO_POC_PACKAGE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md" "- `${_rel}` — SHA-256 `${_hash}`\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C PoC package v0 written to ${ANO_CRYPTO_POC_PACKAGE_DIR}")
