# AnoCrypto-C entropy failure-injection and health-test policy check.
# Engineering guardrail only; not official validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT)
    set(ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/ENTROPY_HEALTH_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(_output_dir "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")

set(_violations "")
set(_checks "")

function(ano_crypto_require_file relative_path description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}")
        list(APPEND _violations "missing ${description}: ${relative_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    else()
        list(APPEND _checks "${description}: ${relative_path}")
        set(_checks "${_checks}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text relative_path required_text description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}")
        list(APPEND _violations "missing file for ${description}: ${relative_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
        return()
    endif()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}" _content)
    string(FIND "${_content}" "${required_text}" _position)
    if(_position EQUAL -1)
        list(APPEND _violations "missing ${description} in ${relative_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    else()
        list(APPEND _checks "${description}: ${relative_path}")
        set(_checks "${_checks}" PARENT_SCOPE)
    endif()
endfunction()

ano_crypto_require_file("src/drbg/entropy_health.c" "internal entropy health skeleton")
ano_crypto_require_file("src/internal/ac_entropy_health_internal.h" "internal entropy health header")
ano_crypto_require_file("tests/negative/entropy_source_negative_tests.c" "entropy negative test")
ano_crypto_require_file("tests/negative/entropy_health_error_state_tests.c" "health error-state test")
ano_crypto_require_file("tests/negative/platform_entropy_failure_injection_tests.c" "platform failure-injection test")
ano_crypto_require_file("docs/design/C99_ENTROPY_FAILURE_INJECTION_AND_HEALTH_TEST.md" "entropy failure/health design")
ano_crypto_require_file("docs/ci/ENTROPY_FAILURE_CI_CHECKLIST.md" "entropy CI checklist")

ano_crypto_require_text("CMakeLists.txt"
    "option(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS \"Enable internal entropy failure-injection hooks for dedicated test builds\" OFF)"
    "test-hook option defaults OFF")
ano_crypto_require_text("CMakeLists.txt"
    "ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS AND NOT ANO_CRYPTO_BUILD_TESTS"
    "test-hook release-build guard")
ano_crypto_require_text("tools/create_poc_package.cmake"
    "-DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF"
    "PoC clean build disables entropy hooks")
ano_crypto_require_text("tools/create_poc_package.cmake"
    "-DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON"
    "PoC clean build hides internal symbols")
ano_crypto_require_text("tools/create_poc_package.cmake"
    "PoC package contains an internal entropy/test artifact"
    "PoC package rejects internal entropy/test implementation artifacts")
ano_crypto_require_text("src/drbg/entropy_adapter.c"
    "ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN"
    "oversized request guard")
ano_crypto_require_text("src/drbg/entropy_adapter.c"
    "ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED)"
    "fatal health failure transition")
ano_crypto_require_text("src/drbg/drbg.c"
    "A fatal entropy health-test failure invalidates the DRBG context."
    "DRBG invalidation on fatal health failure")
ano_crypto_require_text(".github/workflows/ci.yml"
    "entropy-failure-injection"
    "dedicated entropy failure CI lane")
ano_crypto_require_text("tools/collect_release_evidence.cmake"
    "ENTROPY_HEALTH_CHECK.txt"
    "evidence policy-check output")
ano_crypto_require_text("tools/collect_release_evidence.cmake"
    "ENTROPY_HEALTH_REVIEW_NOTES.txt"
    "evidence review notes")

file(GLOB _public_headers LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_content)
    if(_header_content MATCHES "ENTROPY_TEST_HOOK|entropy_test_hook|ano_crypto_internal_entropy")
        file(RELATIVE_PATH _header_rel "${ANO_CRYPTO_SOURCE_DIR}" "${_header}")
        list(APPEND _violations "public header exposes entropy test/internal token: ${_header_rel}")
    endif()
endforeach()

string(CONCAT _sensitive_entropy_label "entropy" " bytes:")
string(CONCAT _sensitive_seed_label "seed" " bytes:")
string(CONCAT _sensitive_drbg_label "DRBG" " state:")
string(CONCAT _sensitive_generated_label "generated secret" " bytes:")
set(_sensitive_log_literals
    "${_sensitive_entropy_label}"
    "${_sensitive_seed_label}"
    "${_sensitive_drbg_label}"
    "${_sensitive_generated_label}")
set(_review_files
    "tests/negative/entropy_source_negative_tests.c"
    "tests/negative/entropy_health_error_state_tests.c"
    "tests/negative/platform_entropy_failure_injection_tests.c"
    "tests/unit/entropy_health_tests.c")
foreach(_rel IN LISTS _review_files)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _content)
        foreach(_literal IN LISTS _sensitive_log_literals)
            string(FIND "${_content}" "${_literal}" _sensitive_position)
            if(NOT _sensitive_position EQUAL -1)
                list(APPEND _violations "possible sensitive entropy/seed logging label in ${_rel}: ${_literal}")
            endif()
        endforeach()
    endif()
endforeach()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}"
    "AnoCrypto-C entropy failure-injection and health-test policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n\n"
    "Checks completed:\n")
foreach(_check IN LISTS _checks)
    file(APPEND "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}" "- ${_check}\n")
endforeach()

if(_violations)
    file(APPEND "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}" "\nResult: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C entropy health policy check failed; see ${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}"
    "\nResult: PASS\n"
    "Boundary: health-test candidates are internal and disabled by default; test hooks are dedicated-build only.\n"
    "Sensitive-data rule: no entropy material, seed, DRBG state, or generated secret bytes are recorded.\n")
message(STATUS "AnoCrypto-C entropy health policy check passed: ${ANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT}")
