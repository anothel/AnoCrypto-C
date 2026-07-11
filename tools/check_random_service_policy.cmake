# AnoCrypto-C public random-service implementation policy check.
# This is an engineering guardrail, not official validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT)
    set(ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/RANDOM_SERVICE_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_out_dir "${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_violations "")

function(ano_crypto_require_file rel_path)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing required random-service file: ${rel_path}")
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
ano_crypto_require_file("tests/unit/random_service_tests.c")
ano_crypto_require_file("tests/negative/random_service_failure_injection_tests.c")
ano_crypto_require_file("docs/design/PUBLIC_RANDOM_API_DECISION.md")
ano_crypto_require_file("docs/ci/RANDOM_SERVICE_CI_CHECKLIST.md")
ano_crypto_require_file("docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md")

if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tests/negative/random_reserved_api_tests.c")
    list(APPEND _violations "obsolete reserved random API test is still present")
endif()

ano_crypto_require_text("include/anocrypto/anocrypto.h" "#include <anocrypto/ac_random.h>" "umbrella header does not include ac_random.h")
ano_crypto_require_text("include/anocrypto/ac_random.h" "P1-20 public random-service lifecycle and portability policy" "random header does not document implemented service")
ano_crypto_require_text("include/anocrypto/ac_random.h" "ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE" "random header lacks request ceiling")
ano_crypto_require_text("include/anocrypto/ac_random.h" "VM snapshot/restore" "random header lacks snapshot/restore limitation")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_random_service_tests" "CMake does not register random service tests")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_random_service_policy_check" "CMake does not expose random service policy target")
ano_crypto_require_text("src/drbg/random.c" "ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source" "random init is not wired through the entropy-source DRBG helper")
ano_crypto_require_text("src/drbg/random.c" "ano_crypto_internal_hmac_drbg_sha256_generate_source_backed" "random generate does not enforce source-backed context ownership")
ano_crypto_require_text("src/drbg/random.c" "ano_crypto_internal_zeroize(out, out_len);" "random failures do not clear caller output")
ano_crypto_require_text("src/drbg/drbg.c" "internal_ctx->source_backed == 0u" "DRBG internal helper does not reject cross-API contexts")
ano_crypto_require_text("src/drbg/drbg.c" "internal_ctx->source_backed != 0u" "explicit DRBG API does not reject random-service contexts")
ano_crypto_require_text("tests/unit/random_service_tests.c" "ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED" "random tests do not cover process-change reseed behavior")
ano_crypto_require_text("tests/unit/random_service_tests.c" "random rejects explicit DRBG context" "random tests do not cover cross-API context misuse")
ano_crypto_require_text("tests/unit/random_service_tests.c" "explicit DRBG generate rejects random context" "random tests do not cover reverse cross-API context misuse")
ano_crypto_require_text("tests/negative/random_service_failure_injection_tests.c" "ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED" "random failure-injection test does not cover provider failure")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "RANDOM_SERVICE_CHECK.txt" "evidence script does not emit random service policy result")
ano_crypto_require_text("tools/create_poc_package.cmake" "RANDOM_SERVICE_PACKAGE_NOTES.txt" "PoC package does not include random service notes")
ano_crypto_require_text("tools/run_poc_package_smoke.cmake" "RANDOM_SERVICE_PACKAGE_NOTES.txt" "PoC smoke does not require random service notes")
ano_crypto_require_text("docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md" "generated random bytes" "DRBG/entropy evidence doc does not prohibit generated random-byte capture")

file(GLOB _public_headers "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_content)
    if(_header_content MATCHES "ac_entropy_internal|ac_drbg_internal|entropy_test_hook|random_test_hook|test_set_reseed_counter|process_guard_internal")
        list(APPEND _violations "public header exposes an internal random/entropy token: ${_header}")
    endif()
endforeach()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}"
    "AnoCrypto-C public random-service implementation policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- OS-provider-backed HMAC_DRBG-SHA-256 public service wiring\n"
    "- Caller-owned opaque context and cross-API context separation\n"
    "- Process-change reseed-required behavior and output clearing\n"
    "- Provider-disabled and failure-injection coverage\n"
    "- Evidence and PoC package non-disclosure boundary\n\n")

if(_violations)
    file(APPEND "${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C random service policy check failed; see ${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C random service policy check passed: ${ANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT}")
