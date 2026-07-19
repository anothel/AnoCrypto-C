cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_SECURE_CODING_CHECK_OUTPUT OR
   ANO_CRYPTO_SECURE_CODING_CHECK_OUTPUT STREQUAL "")
    set(ANO_CRYPTO_SECURE_CODING_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_SECURE_CODING_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SECURE_CODING_POLICY.cmake")

if(NOT ANO_CRYPTO_SECURE_CODING_POLICY_ID STREQUAL "SCP-P2-06-R1" OR
   NOT ANO_CRYPTO_SECURE_CODING_POLICY_STATUS STREQUAL "COMPLETE_LOCAL_REVIEW_WITH_OPEN_FINDINGS" OR
   NOT ANO_CRYPTO_SECURE_CODING_FORMAL_ACCEPTANCE STREQUAL "EXTERNAL_CONFIRMATION_REQUIRED" OR
   NOT ANO_CRYPTO_SECURE_CODING_CANDIDATE_ID STREQUAL ANO_CRYPTO_VALIDATION_CANDIDATE_ID OR
   NOT ANO_CRYPTO_SECURE_CODING_BUILD_PROFILE_ID STREQUAL "SCB-P2-06-LINUX-ELF-R1" OR
   NOT ANO_CRYPTO_SECURE_CODING_SELECTED_OE_REVIEW STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SIDE_CHANNEL_SELECTED_OE_DISASSEMBLY_REVIEW STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SIDE_CHANNEL_EMPIRICAL_TIMING STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SIDE_CHANNEL_UNIVERSAL_CLAIM STREQUAL "PROHIBITED" OR
   NOT ANO_CRYPTO_RELEASE_HARDENING_FORTIFY_SOURCE STREQUAL "3" OR
   NOT ANO_CRYPTO_RELEASE_HARDENING_STACK_PROTECTOR STREQUAL "strong" OR
   NOT ANO_CRYPTO_RELEASE_HARDENING_RELRO STREQUAL "FULL" OR
   NOT ANO_CRYPTO_RELEASE_HARDENING_BIND_NOW STREQUAL "ON" OR
   NOT ANO_CRYPTO_RELEASE_HARDENING_EXECUTABLE_STACK STREQUAL "OFF")
    list(APPEND _violations "frozen P2-06 secure-coding/side-channel policy values drifted")
endif()

foreach(_path IN ITEMS
    docs/kcmvp/KCMVP_SECURE_CODING_REVIEW.md
    docs/kcmvp/KCMVP_SIDE_CHANNEL_REVIEW.md
    docs/kcmvp/KCMVP_SECURITY_FINDINGS_REGISTER.md
    docs/kcmvp/KCMVP_COMPILER_CPU_REVIEW_MATRIX.md
    docs/kcmvp/P2_06_IMPLEMENTATION_TEST_DELTA.md
    release/ANOCRYPTO_C_SECURE_CODING_POLICY.cmake
    tools/capture_secure_coding_review.cmake
    tests/operational/secure_coding_policy_review.c
    src/cipher/aes.c
    src/mac/cmac.c
    src/csp/zeroize.c
    src/csp/csp_handle.c
    abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
    require_file("${_path}")
endforeach()

require_text("src/cipher/aes.c" "ano_crypto_aes_gf_inverse")
require_text("src/cipher/aes.c" "for (i = 0u; i < 8u; ++i)")
require_text("src/mac/cmac.c" "volatile uint8_t difference")
require_text("src/csp/zeroize.c" "volatile unsigned char *p")
require_text("src/csp/csp_handle.c" "case ANO_CRYPTO_CSP_STATE_INVALID:")
require_text("CMakeLists.txt" "ANO_CRYPTO_ENABLE_RELEASE_HARDENING")
require_text("CMakeLists.txt" "-fstack-protector-strong")
require_text("CMakeLists.txt" "-fstack-clash-protection")
require_text("CMakeLists.txt" "_FORTIFY_SOURCE=3")
require_text("CMakeLists.txt" "-z,relro,-z,now,-z,noexecstack")
require_text("docs/kcmvp/KCMVP_SECURITY_FINDINGS_REGISTER.md" "P2-06-F-012")
require_text("tests/operational/secure_coding_policy_review.c" "sensitive_values_printed=0")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-04-aes256-gcm-streaming")

# Product source must avoid common high-risk APIs and dynamic allocation.
file(GLOB_RECURSE _product_sources LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/src/*.c"
    "${ANO_CRYPTO_SOURCE_DIR}/src/*.h")
foreach(_source IN LISTS _product_sources)
    file(READ "${_source}" _text)
    if(_text MATCHES "(^|[^A-Za-z0-9_])(gets|strcpy|strcat|sprintf|vsprintf|system|popen|rand|srand|malloc|calloc|realloc|free|alloca)[ \\t\\r\\n]*\\(")
        file(RELATIVE_PATH _relative "${ANO_CRYPTO_SOURCE_DIR}" "${_source}")
        list(APPEND _violations "high-risk or dynamic-allocation API found in product source: ${_relative}")
    endif()
endforeach()

# The portable AES implementation must not silently regress to a secret-indexed
# S-box/T-table source representation.
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/cipher/aes.c" _aes_source)
if(_aes_source MATCHES "static[ \\t]+const[ \\t]+uint(8|32)_t[^;]*(sbox|SBOX|t_table|T_TABLE)[ \\t]*\\[")
    list(APPEND _violations "AES lookup table detected in portable source")
endif()

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
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_3")
    list(APPEND _violations
        "public ABI changed: headers=${_public_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_text)
    if(_header_text MATCHES "SCP-P2-06|P2-06-F-|SCB-P2-06|SIDE_CHANNEL_SELECTED_OE")
        list(APPEND _violations "internal P2-06 token leaked into public header: ${_header}")
    endif()
endforeach()

file(WRITE "${_output}"
    "AnoCrypto-C P2-06 Secure-Coding and Side-Channel Policy Check\n"
    "Policy ID: ${ANO_CRYPTO_SECURE_CODING_POLICY_ID}\n"
    "Policy status: ${ANO_CRYPTO_SECURE_CODING_POLICY_STATUS}\n"
    "Formal acceptance: ${ANO_CRYPTO_SECURE_CODING_FORMAL_ACCEPTANCE}\n"
    "Candidate ID: ${ANO_CRYPTO_SECURE_CODING_CANDIDATE_ID}\n"
    "Build profile: ${ANO_CRYPTO_SECURE_CODING_BUILD_PROFILE_ID}\n")

if(_violations)
    file(APPEND "${_output}" "Result: FAIL\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${_output}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "P2-06 secure-coding/side-channel policy check failed")
endif()

file(APPEND "${_output}"
    "Result: PASS\n"
    "Source review: ${ANO_CRYPTO_SECURE_CODING_SOURCE_REVIEW}\n"
    "GCC analyzer: ${ANO_CRYPTO_SECURE_CODING_GCC_ANALYZER}\n"
    "Clang strict: ${ANO_CRYPTO_SECURE_CODING_CLANG_STRICT}\n"
    "Clang analyzer: ${ANO_CRYPTO_SECURE_CODING_CLANG_ANALYZER}\n"
    "Selected-OE review: ${ANO_CRYPTO_SECURE_CODING_SELECTED_OE_REVIEW}\n"
    "AES source profile: ${ANO_CRYPTO_SIDE_CHANNEL_AES_SOURCE_PROFILE}\n"
    "CMAC compare profile: ${ANO_CRYPTO_SIDE_CHANNEL_CMAC_COMPARE_PROFILE}\n"
    "Empirical timing: ${ANO_CRYPTO_SIDE_CHANNEL_EMPIRICAL_TIMING}\n"
    "Power/EM review: ${ANO_CRYPTO_SIDE_CHANNEL_POWER_EM_REVIEW}\n"
    "Fault review: ${ANO_CRYPTO_SIDE_CHANNEL_FAULT_REVIEW}\n"
    "Release hardening: ${ANO_CRYPTO_RELEASE_HARDENING_IMPLEMENTATION}\n"
    "Public headers: ${_public_header_count}\n"
    "Exported symbols: ${_symbol_count}\n"
    "ABI line: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "Hosted lanes: ${ANO_CRYPTO_SECURE_CODING_HOSTED_STATUS}\n"
    "Hosted review: ${ANO_CRYPTO_SECURE_CODING_HOSTED_REVIEW}\n"
    "Repository signing: ${ANO_CRYPTO_SECURE_CODING_REPOSITORY_SIGNING}\n"
    "Signing handoff: ${ANO_CRYPTO_SECURE_CODING_HANDOFF}\n"
    "External release authorization: ${ANO_CRYPTO_SECURE_CODING_RELEASE_AUTHORIZATION}\n"
    "Sensitive values recorded: none\n")

message(STATUS "P2-06 secure-coding/side-channel policy check passed")
