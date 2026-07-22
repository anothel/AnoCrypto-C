cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_CSP_ZEROIZATION_POLICY_CHECK_OUTPUT OR
   ANO_CRYPTO_CSP_ZEROIZATION_POLICY_CHECK_OUTPUT STREQUAL "")
    set(ANO_CRYPTO_CSP_ZEROIZATION_POLICY_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/CSP_ZEROIZATION_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_CSP_ZEROIZATION_POLICY_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_out_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
set(_violations "")

function(require_file _relative)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
        list(APPEND _violations "missing ${_relative}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(require_text _relative _needle)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
        list(APPEND _violations "missing ${_relative}")
    else()
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _content)
        string(FIND "${_content}" "${_needle}" _position)
        if(_position EQUAL -1)
            list(APPEND _violations "${_relative} missing marker: ${_needle}")
        endif()
    endif()
    set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_CSP_ZEROIZATION_POLICY.cmake")

if(NOT ANO_CRYPTO_CSP_ZEROIZATION_POLICY_ID STREQUAL "CZP-P2-05-R1" OR
   NOT ANO_CRYPTO_CSP_ZEROIZATION_POLICY_STATUS STREQUAL "FROZEN_INTERNAL_POLICY" OR
   NOT ANO_CRYPTO_CSP_ZEROIZATION_FORMAL_ACCEPTANCE STREQUAL "EXTERNAL_CONFIRMATION_REQUIRED" OR
   NOT ANO_CRYPTO_CSP_ZEROIZATION_CANDIDATE_ID STREQUAL ANO_CRYPTO_VALIDATION_CANDIDATE_ID OR
   NOT ANO_CRYPTO_CSP_TABLE_CAPACITY EQUAL 16 OR
   NOT ANO_CRYPTO_CSP_MAX_SECRET_BYTES EQUAL 128 OR
   NOT ANO_CRYPTO_CSP_EXPORT_SUPPORTED STREQUAL "NO" OR
   NOT ANO_CRYPTO_CSP_PERSISTENCE_SUPPORTED STREQUAL "NO" OR
   NOT ANO_CRYPTO_CSP_WRAPPING_SUPPORTED STREQUAL "NO" OR
   NOT ANO_CRYPTO_CSP_MODULE_SHUTDOWN_WIPE STREQUAL "IMPLEMENTED_P2_05" OR
   NOT ANO_CRYPTO_CSP_FATAL_TRANSITION_WIPE STREQUAL "IMPLEMENTED_P2_05" OR
   NOT ANO_CRYPTO_CALLER_CONTEXT_TRACKING STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_CALLER_CONTEXT_CLEANUP STREQUAL "EXPLICIT_UNINIT_REQUIRED" OR
   NOT ANO_CRYPTO_ZEROIZATION_PRIMITIVE STREQUAL "VOLATILE_BYTE_STORE_LOOP" OR
   NOT ANO_CRYPTO_ZEROIZATION_SELECTED_OE_COMPILER_REVIEW STREQUAL "NOT_RUN")
    list(APPEND _violations "frozen P2-05 CSP/zeroization policy values drifted")
endif()

foreach(_p IN ITEMS
    docs/kcmvp/KCMVP_CSP_INVENTORY.md
    docs/kcmvp/KCMVP_ZEROIZATION_EVIDENCE.md
    docs/kcmvp/KCMVP_SERVICE_CSP_OPERATION_MATRIX.md
    docs/kcmvp/P2_05_IMPLEMENTATION_TEST_DELTA.md
    release/ANOCRYPTO_C_CSP_ZEROIZATION_POLICY.cmake
    tools/capture_zeroization_compiler_review.cmake
    tests/operational/csp_zeroization_policy_review.c
    tests/unit/csp_shutdown_zeroization_tests.c
    tests/unit/csp_fatal_zeroization_tests.c
    src/csp/zeroize.c
    src/csp/csp_handle.c
    src/core/module_state.c
    src/internal/ac_csp_internal.h
    abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
    require_file("${_p}")
endforeach()

require_text("src/csp/zeroize.c" "volatile unsigned char *p")
require_text("src/csp/csp_handle.c" "ano_crypto_internal_csp_zeroize_all_unlocked")
require_text("src/csp/csp_handle.c" "ANO_CRYPTO_CSP_STATE_ZEROIZED")
require_text("src/core/module_state.c" "ano_crypto_internal_csp_zeroize_all_unlocked")
require_text("docs/kcmvp/KCMVP_CSP_INVENTORY.md" "CSP-001")
require_text("docs/kcmvp/KCMVP_CSP_INVENTORY.md" "CSP-104")
require_text("docs/kcmvp/KCMVP_ZEROIZATION_EVIDENCE.md" "Selected OE review status")
require_text("tests/operational/csp_zeroization_policy_review.c" "sensitive_values_printed=0")
require_text("tools/capture_zeroization_compiler_review.cmake" "Automated proof of effective zeroization: no")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-10-restricted-sha1-ocsp-certid")

set(_public_headers "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
set(_public_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbol_lines)
set(_symbol_count 0)
foreach(_line IN LISTS _symbol_lines)
    string(STRIP "${_line}" _trimmed)
    if(NOT _trimmed STREQUAL "" AND NOT _trimmed MATCHES "^#")
        math(EXPR _symbol_count "${_symbol_count} + 1")
    endif()
endforeach()
if(NOT _public_header_count EQUAL 14 OR NOT _symbol_count EQUAL 69 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_7")
    list(APPEND _violations
        "public ABI changed: headers=${_public_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_text)
    if(_header_text MATCHES "CZP-P2-05|CSP-10[1-4]|IMPLEMENTED_P2_05|ZEROIZATION_SELECTED_OE")
        list(APPEND _violations "internal P2-05 policy token leaked into public header: ${_header}")
    endif()
endforeach()

file(WRITE "${_output}"
    "AnoCrypto-C P2-05 CSP and Zeroization Policy Check\n"
    "Policy ID: ${ANO_CRYPTO_CSP_ZEROIZATION_POLICY_ID}\n"
    "Policy status: ${ANO_CRYPTO_CSP_ZEROIZATION_POLICY_STATUS}\n"
    "Formal acceptance: ${ANO_CRYPTO_CSP_ZEROIZATION_FORMAL_ACCEPTANCE}\n"
    "Candidate ID: ${ANO_CRYPTO_CSP_ZEROIZATION_CANDIDATE_ID}\n")

if(_violations)
    file(APPEND "${_output}" "Result: FAIL\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${_output}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "P2-05 CSP/zeroization policy check failed")
endif()

file(APPEND "${_output}"
    "Result: PASS\n"
    "Implemented module key purposes: HMAC, BLOCK_CIPHER, CMAC\n"
    "CSP table capacity: ${ANO_CRYPTO_CSP_TABLE_CAPACITY}\n"
    "Maximum slot bytes: ${ANO_CRYPTO_CSP_MAX_SECRET_BYTES}\n"
    "Shutdown wipe: ${ANO_CRYPTO_CSP_MODULE_SHUTDOWN_WIPE}\n"
    "Fatal-transition wipe: ${ANO_CRYPTO_CSP_FATAL_TRANSITION_WIPE}\n"
    "Caller context tracking: ${ANO_CRYPTO_CALLER_CONTEXT_TRACKING}\n"
    "Selected-OE compiler review: ${ANO_CRYPTO_ZEROIZATION_SELECTED_OE_COMPILER_REVIEW}\n"
    "Public headers: ${_public_header_count}\n"
    "Exported symbols: ${_symbol_count}\n"
    "ABI line: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "Hosted lanes: ${ANO_CRYPTO_CSP_ZEROIZATION_HOSTED_STATUS}\n"
    "Hosted review: ${ANO_CRYPTO_CSP_ZEROIZATION_HOSTED_REVIEW}\n"
    "Repository signing: ${ANO_CRYPTO_CSP_ZEROIZATION_REPOSITORY_SIGNING}\n"
    "Signing handoff: ${ANO_CRYPTO_CSP_ZEROIZATION_HANDOFF}\n"
    "External release authorization: ${ANO_CRYPTO_CSP_ZEROIZATION_RELEASE_AUTHORIZATION}\n"
    "Sensitive values recorded: none\n")
message(STATUS "P2-05 CSP/zeroization policy check passed: ${_output}")
