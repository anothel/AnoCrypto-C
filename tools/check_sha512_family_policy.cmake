# AnoCrypto-C SHA-384/SHA-512 family implementation policy check.
# Engineering guardrail for P1-22 implementation, KAT, package, and evidence wiring.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT)
    set(ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/SHA512_FAMILY_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_out_dir "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_violations "")
set(_warnings "")

function(ano_crypto_require_file rel_path description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing ${description}: ${rel_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text rel_path needle description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing file for ${description}: ${rel_path}")
    else()
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}" _content)
        string(FIND "${_content}" "${needle}" _pos)
        if(_pos EQUAL -1)
            list(APPEND _violations "${description} missing expected marker in ${rel_path}")
        endif()
    endif()
    set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

function(ano_crypto_forbid_text rel_path needle description)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}" _content)
        string(FIND "${_content}" "${needle}" _pos)
        if(NOT _pos EQUAL -1)
            list(APPEND _violations "${description}: ${rel_path}")
        endif()
    endif()
    set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

ano_crypto_require_file("src/hash/sha512.c" "SHA-512-family implementation")
ano_crypto_require_file("src/internal/ac_sha512_internal.h" "SHA-512-family internal header")
ano_crypto_require_file("tests/kat/sha512_family_kat_tests.c" "SHA-512-family KAT test")
ano_crypto_require_file("tests/negative/sha512_family_api_negative_tests.c" "SHA-512-family negative test")
ano_crypto_require_file("docs/design/SHA2_384_512_SCOPE_DECISION.md" "SHA-512-family implementation decision")
ano_crypto_require_file("docs/kcmvp/SHA2_384_512_KAT_PLAN.md" "SHA-512-family KAT record")

if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tests/negative/sha2_reserved_api_tests.c")
    list(APPEND _violations "obsolete reserved SHA-384/SHA-512 API test is still present")
endif()
if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tools/check_sha2_scope_policy.cmake")
    list(APPEND _violations "obsolete reserved SHA-2 scope policy script is still present")
endif()

ano_crypto_require_text("include/anocrypto/ac_hash.h" "SHA-256, SHA-384, and SHA-512" "public hash implementation statement")
ano_crypto_require_text("include/anocrypto/ac_hmac.h" "HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512" "public HMAC implementation statement")
ano_crypto_require_text("src/hash/sha512.c" "ano_crypto_internal_sha384_digest" "SHA-384 one-shot implementation")
ano_crypto_require_text("src/hash/sha512.c" "ano_crypto_internal_sha512_digest" "SHA-512 one-shot implementation")
ano_crypto_require_text("src/mac/hmac.c" "ANO_CRYPTO_HASH_ALG_SHA384" "HMAC-SHA-384 dispatch")
ano_crypto_require_text("src/mac/hmac.c" "ANO_CRYPTO_HASH_ALG_SHA512" "HMAC-SHA-512 dispatch")
ano_crypto_require_text("src/selftest/selftest.c" "ano_crypto_selftest_sha512_family" "startup hash KAT")
ano_crypto_require_text("src/selftest/selftest.c" "ano_crypto_selftest_hmac_sha512_family" "startup HMAC KAT")
ano_crypto_require_text("src/selftest/kat_manifest.c" "p1-22-implemented-startup" "implemented KAT manifest entries")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha512_family_kat_tests" "SHA-512-family KAT target")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha512_family_api_negative_tests" "SHA-512-family negative target")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha512_family_policy_check" "SHA-512-family policy target")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "SHA512_FAMILY_CHECK.txt" "evidence policy output")
ano_crypto_require_text("tools/create_poc_package.cmake" "SHA512_FAMILY_PACKAGE_NOTES.txt" "PoC package family note")
ano_crypto_require_text("tools/run_poc_package_smoke.cmake" "SHA512_FAMILY_PACKAGE_NOTES.txt" "PoC smoke family note")

ano_crypto_forbid_text("include/anocrypto/ac_hash.h" "ABI-reserved" "hash header still claims reserved SHA-512-family processing")
ano_crypto_forbid_text("include/anocrypto/ac_hmac.h" "ABI-reserved" "HMAC header still claims reserved SHA-512-family processing")
ano_crypto_forbid_text("src/selftest/kat_manifest.c" "p1-16-reserved-kat-plan" "KAT manifest still contains reserved SHA-512-family entries")

list(APPEND _warnings "Independent external-provider differential automation remains a P1-23 candidate.")
list(APPEND _warnings "Official validation or KCMVP completion is not claimed by this engineering policy check.")

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}"
    "AnoCrypto-C SHA-384/SHA-512 family implementation check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- SHA-384/SHA-512 implementation and internal context files present\n"
    "- HMAC-SHA-384/HMAC-SHA-512 dispatch present\n"
    "- Startup and extended public-source KAT wiring present\n"
    "- Negative API regression, evidence, CI, and PoC wiring present\n"
    "- Obsolete reserved-processing guards removed\n\n")

if(_warnings)
    file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "Warnings / tracked TODOs:\n")
    foreach(_warning IN LISTS _warnings)
        file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "- ${_warning}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "\n")
endif()

if(_violations)
    file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C SHA-512-family policy check failed; see ${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C SHA-512-family policy check passed: ${ANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT}")
