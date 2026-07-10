# AnoCrypto-C random-service and DRBG/entropy boundary policy check.
# This is an engineering guardrail, not official validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT)
    set(ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/RANDOM_BOUNDARY_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_out_dir "${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_violations "")

function(ano_crypto_require_file rel_path)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing required random-boundary file: ${rel_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text rel_path needle description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing file for text check: ${rel_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
        return()
    endif()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}" _content)
    string(FIND "${_content}" "${needle}" _pos)
    if(_pos EQUAL -1)
        list(APPEND _violations "${description}: ${rel_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

ano_crypto_require_file("include/anocrypto/ac_random.h")
ano_crypto_require_file("src/drbg/random.c")
ano_crypto_require_file("tests/negative/random_reserved_api_tests.c")
ano_crypto_require_file("docs/design/PUBLIC_RANDOM_API_DECISION.md")
ano_crypto_require_file("docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md")

ano_crypto_require_text("include/anocrypto/anocrypto.h" "#include <anocrypto/ac_random.h>" "umbrella header does not include ac_random.h")
ano_crypto_require_text("CMakeLists.txt" "include/anocrypto/ac_random.h" "CMake public headers do not install ac_random.h")
ano_crypto_require_text("CMakeLists.txt" "src/drbg/random.c" "CMake library does not build random.c")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_random_reserved_api_tests" "CMake does not register random reserved API tests")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_random_boundary_policy_check" "CMake does not expose random boundary policy check target")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "RANDOM_BOUNDARY_CHECK.txt" "evidence script does not emit random boundary check")
ano_crypto_require_text("tools/create_poc_package.cmake" "RANDOM_BOUNDARY_PACKAGE_NOTES.txt" "PoC package does not include random boundary notes")
ano_crypto_require_text("tools/run_poc_package_smoke.cmake" "RANDOM_BOUNDARY_PACKAGE_NOTES.txt" "PoC smoke does not require random boundary notes")

ano_crypto_require_text("include/anocrypto/ac_random.h" "ANO_CRYPTO_ERR_NOT_IMPLEMENTED" "random header does not document reserved NOT_IMPLEMENTED behavior")
ano_crypto_require_text("src/drbg/random.c" "return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;" "random implementation is not fail-closed as reserved")
ano_crypto_require_text("src/drbg/random.c" "ano_crypto_internal_zeroize(out, out_len);" "random generate does not zeroize output on closed-fail path")
ano_crypto_require_text("tests/negative/random_reserved_api_tests.c" "ANO_CRYPTO_ERR_NOT_IMPLEMENTED" "random reserved API test does not check NOT_IMPLEMENTED behavior")
ano_crypto_require_text("docs/design/PUBLIC_RANDOM_API_DECISION.md" "reserved public boundary" "random decision doc does not record reserved boundary")
ano_crypto_require_text("docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md" "raw entropy bytes" "DRBG/entropy evidence doc does not prohibit raw entropy bytes")

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}"
    "AnoCrypto-C random-service boundary policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- Reserved ac_random.h public boundary\n"
    "- Fail-closed ano_crypto_random_* placeholder implementation\n"
    "- Random reserved API negative regression test\n"
    "- DRBG/entropy evidence boundary documentation\n"
    "- Evidence workflow and PoC package notes\n\n")

if(_violations)
    file(APPEND "${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C random boundary policy check failed; see ${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C random boundary policy check passed: ${ANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT}")
