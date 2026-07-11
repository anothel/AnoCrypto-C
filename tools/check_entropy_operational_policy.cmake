# AnoCrypto-C P1-18 entropy source and operational process-boundary policy check.
# Engineering guardrail only; not official entropy-source or validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT)
    set(ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/ENTROPY_OPERATIONAL_POLICY_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(_output_dir "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}" DIRECTORY)
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

ano_crypto_require_file("src/drbg/process_guard.c" "internal process-boundary guard")
ano_crypto_require_file("src/internal/ac_process_guard_internal.h" "internal process-boundary header")
ano_crypto_require_file("tests/unit/entropy_operational_policy_tests.c" "entropy operational policy test")
ano_crypto_require_file("tests/unit/process_guard_tests.c" "process-boundary regression test")
ano_crypto_require_file("docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md" "entropy source operational policy")
ano_crypto_require_file("docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md" "entropy operational CI checklist")

ano_crypto_require_text("src/internal/ac_entropy_internal.h"
    "ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG"
    "OS provider classified as external conditioned RBG")
ano_crypto_require_text("src/internal/ac_entropy_internal.h"
    "ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER"
    "OS provider health-test ownership metadata")
ano_crypto_require_text("src/drbg/entropy_adapter.c"
    "source->kind != ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS"
    "operational probe restricted to OS provider")
ano_crypto_require_text("src/drbg/entropy_adapter.c"
    "source->kind != ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK"
    "module health skeleton excluded from OS provider output")
ano_crypto_require_text("src/drbg/entropy_adapter.c"
    "ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_RAW_NOISE_TEST_CANDIDATE"
    "health skeleton requires explicit raw-noise candidate classification")
ano_crypto_require_text("src/drbg/drbg.c"
    "ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED"
    "source-backed DRBG process-boundary reseed result")
ano_crypto_require_text("src/drbg/drbg.c"
    "ano_crypto_internal_process_guard_check"
    "source-backed DRBG checks process boundary before generation")
ano_crypto_require_text("tests/unit/process_guard_tests.c"
    "test_explicit_entropy_drbg_remains_caller_managed"
    "explicit-entropy DRBG process inheritance remains caller-managed")
ano_crypto_require_text("tests/negative/platform_entropy_failure_injection_tests.c"
    "partial operational probe failure"
    "operational probe failure-injection coverage")
ano_crypto_require_text("include/anocrypto/ac_random.h"
    "ANO_CRYPTO_RANDOM_SERVICE_RESERVED 0u"
    "public random service remains reserved")
ano_crypto_require_text("tools/collect_release_evidence.cmake"
    "ENTROPY_OPERATIONAL_POLICY_CHECK.txt"
    "evidence operational policy output")
ano_crypto_require_text("tools/collect_release_evidence.cmake"
    "ENTROPY_OPERATIONAL_REVIEW_NOTES.txt"
    "evidence operational review notes")

file(GLOB _public_headers LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_content)
    if(_header_content MATCHES "process_guard|PROCESS_GUARD|RAW_NOISE_TEST_CANDIDATE|ENTROPY_HEALTH_OWNER")
        file(RELATIVE_PATH _header_rel "${ANO_CRYPTO_SOURCE_DIR}" "${_header}")
        list(APPEND _violations "public header exposes internal entropy/process policy token: ${_header_rel}")
    endif()
endforeach()

set(_sensitive_review_files
    "tests/unit/entropy_operational_policy_tests.c"
    "tests/unit/process_guard_tests.c"
    "tests/negative/platform_entropy_failure_injection_tests.c"
    "docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md"
    "docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md")
set(_sensitive_literals
    "entropy bytes:"
    "seed bytes:"
    "DRBG state:"
    "generated secret bytes:")
foreach(_rel IN LISTS _sensitive_review_files)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _content)
        foreach(_literal IN LISTS _sensitive_literals)
            string(FIND "${_content}" "${_literal}" _sensitive_position)
            if(NOT _sensitive_position EQUAL -1)
                list(APPEND _violations "possible sensitive logging label in ${_rel}: ${_literal}")
            endif()
        endforeach()
    endif()
endforeach()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}"
    "AnoCrypto-C entropy source and operational process-boundary policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n\n"
    "Checks completed:\n")
foreach(_check IN LISTS _checks)
    file(APPEND "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}" "- ${_check}\n")
endforeach()

if(_violations)
    file(APPEND "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}" "\nResult: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C entropy operational policy check failed; see ${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}"
    "\nResult: PASS\n"
    "Boundary: platform OS APIs are treated as external conditioned random providers, not direct raw-noise interfaces.\n"
    "Health policy: P1-17 duplicate-block candidates can attach only to explicitly classified internal raw-noise test callbacks.\n"
    "Process policy: source-backed DRBG state requires reseed after a detected process identity change.\n"
    "Public API: random generation remains reserved and no internal policy type is installed.\n")
message(STATUS "AnoCrypto-C entropy operational policy check passed: ${ANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT}")
