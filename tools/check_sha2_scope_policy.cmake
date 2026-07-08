# AnoCrypto-C SHA-384/SHA-512 reserved scope policy check.
# This is an engineering guardrail for P1-15 scope/evidence wiring.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT)
    set(ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/SHA2_SCOPE_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_out_dir "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_violations "")
set(_warnings "")

function(ano_crypto_require_file path description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${path}")
        list(APPEND _violations "missing ${description}: ${path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text path needle description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${path}")
        list(APPEND _violations "missing ${description}: ${path}")
    else()
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${path}" _content)
        string(FIND "${_content}" "${needle}" _pos)
        if(_pos EQUAL -1)
            list(APPEND _violations "${description} missing expected marker in ${path}")
        endif()
    endif()
    set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

ano_crypto_require_file("docs/design/SHA2_384_512_SCOPE_DECISION.md" "SHA-384/SHA-512 scope decision document")
ano_crypto_require_file("docs/kcmvp/SHA2_384_512_KAT_PLAN.md" "SHA-384/SHA-512 KAT plan document")
ano_crypto_require_file("tests/negative/sha2_reserved_api_tests.c" "reserved SHA-2 API regression test")

ano_crypto_require_text("include/anocrypto/ac_hash.h" "ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE" "public SHA-384 digest-size macro")
ano_crypto_require_text("include/anocrypto/ac_hash.h" "ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE" "public SHA-512 digest-size macro")
ano_crypto_require_text("src/selftest/kat_manifest.c" "p1-15-reserved-kat-plan" "KAT manifest reserved-plan entries")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha2_reserved_api_tests" "CTest target for reserved SHA-2 API behavior")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha2_scope_policy_check" "SHA-2 scope policy target")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "SHA2_SCOPE_CHECK.txt" "evidence SHA-2 scope check output")
ano_crypto_require_text("tools/create_poc_package.cmake" "SHA2_SCOPE_PACKAGE_NOTES.txt" "PoC package SHA-2 scope note")
ano_crypto_require_text("tools/run_poc_package_smoke.cmake" "SHA2_SCOPE_PACKAGE_NOTES.txt" "PoC smoke SHA-2 package note check")

if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/src/hash/hash.c")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/hash/hash.c" _hash_c)
    string(FIND "${_hash_c}" "ANO_CRYPTO_ERR_NOT_IMPLEMENTED" _not_impl_pos)
    if(_not_impl_pos EQUAL -1)
        list(APPEND _violations "hash.c does not record not-implemented behavior for reserved algorithms")
    endif()
endif()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}"
    "AnoCrypto-C SHA-384/SHA-512 reserved scope check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- Scope decision document present\n"
    "- KAT plan document present\n"
    "- Reserved-API negative test present\n"
    "- Public digest-size macros present\n"
    "- KAT manifest reserved-plan entries present\n"
    "- CMake test/policy/evidence/PoC wiring present\n\n")

if(_warnings)
    file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "Warnings / tracked TODOs:\n")
    foreach(_warning IN LISTS _warnings)
        file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "- ${_warning}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "\n")
else()
    file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "Warnings / tracked TODOs: none\n\n")
endif()

if(_violations)
    file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C SHA-2 scope check failed; see ${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C SHA-2 scope check passed: ${ANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT}")
