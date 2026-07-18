# Validate the P2-03 self-test, integrity-candidate, state, and recovery policy.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR OR ANO_CRYPTO_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "ANO_CRYPTO_SOURCE_DIR is required")
endif()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)

if(NOT DEFINED ANO_CRYPTO_SELF_TEST_POLICY_CHECK_OUTPUT OR
   ANO_CRYPTO_SELF_TEST_POLICY_CHECK_OUTPUT STREQUAL "")
    set(ANO_CRYPTO_SELF_TEST_POLICY_CHECK_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/SELF_TEST_INTEGRITY_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_SELF_TEST_POLICY_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")

function(require_file _relative)
    set(_path "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}" OR IS_SYMLINK "${_path}")
        message(FATAL_ERROR "Required P2-03 file is missing or unsafe: ${_relative}")
    endif()
endfunction()

function(require_text _relative _needle)
    require_file("${_relative}")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _text)
    string(FIND "${_text}" "${_needle}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "${_relative} is missing required P2-03 text: ${_needle}")
    endif()
endfunction()

function(forbid_text _relative _needle)
    require_file("${_relative}")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _text)
    string(FIND "${_text}" "${_needle}" _position)
    if(NOT _position EQUAL -1)
        message(FATAL_ERROR "${_relative} contains prohibited P2-03 text: ${_needle}")
    endif()
endfunction()

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SELF_TEST_POLICY.cmake")

if(NOT ANO_CRYPTO_SELF_TEST_POLICY_ID STREQUAL "STP-P2-03-R1")
    message(FATAL_ERROR "Unexpected self-test policy ID")
endif()
if(NOT ANO_CRYPTO_FINITE_STATE_POLICY_ID STREQUAL "FSM-P2-03-R1")
    message(FATAL_ERROR "Unexpected finite-state policy ID")
endif()
if(NOT ANO_CRYPTO_INTEGRITY_CANDIDATE_ID STREQUAL "INT-HMAC-SHA256-SINGLE-SO-R1")
    message(FATAL_ERROR "Unexpected integrity candidate ID")
endif()
if(NOT ANO_CRYPTO_INTEGRITY_CANDIDATE_STATUS STREQUAL
   "SELECTED_CANDIDATE_NOT_IMPLEMENTED")
    message(FATAL_ERROR "Integrity candidate must remain explicitly not implemented")
endif()
if(NOT ANO_CRYPTO_INTEGRITY_EXTERNAL_CONFIRMATION_REQUIRED)
    message(FATAL_ERROR "Integrity candidate must require external confirmation")
endif()
if(ANO_CRYPTO_INTEGRITY_RUNTIME_IMPLEMENTED)
    message(FATAL_ERROR "P2-03 must not claim a runtime integrity implementation")
endif()
if(ANO_CRYPTO_ON_DEMAND_STATE_TRANSITION_IMPLEMENTED)
    message(FATAL_ERROR "P2-03 must not claim the on-demand state transition is implemented")
endif()
if(NOT ANO_CRYPTO_INTEGRITY_ALGORITHM STREQUAL "HMAC-SHA-256" OR
   NOT ANO_CRYPTO_INTEGRITY_TAG_BYTES EQUAL 32 OR
   NOT ANO_CRYPTO_INTEGRITY_SINGLE_FILE_BOUNDARY)
    message(FATAL_ERROR "Integrity candidate algorithm/tag/boundary fields are inconsistent")
endif()
if(NOT ANO_CRYPTO_FATAL_RECOVERY_MODEL STREQUAL
   "PROCESS_TERMINATION_AND_FRESH_LOAD" OR
   NOT ANO_CRYPTO_ORDERLY_REINITIALIZATION_ALLOWED)
    message(FATAL_ERROR "Recovery policy fields are inconsistent")
endif()
if(NOT ANO_CRYPTO_SELF_TEST_ABI_LINE STREQUAL "ANOCRYPTO_C_0" OR
   NOT ANO_CRYPTO_SELF_TEST_PUBLIC_HEADER_COUNT EQUAL 14 OR
   NOT ANO_CRYPTO_SELF_TEST_EXPORTED_SYMBOL_COUNT EQUAL 69)
    message(FATAL_ERROR "P2-03 public ABI guard fields changed")
endif()
if(NOT ANO_CRYPTO_SELF_TEST_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SELF_TEST_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_SELF_TEST_RELEASE_AUTHORIZATION STREQUAL "NO")
    message(FATAL_ERROR "P2-03 incorrectly advances hosted/release status")
endif()

foreach(_required IN ITEMS
        docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md
        docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md
        docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md
        docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md
        release/ANOCRYPTO_C_SELF_TEST_POLICY.cmake
        src/core/module_state.c
        src/selftest/selftest.c
        include/anocrypto/ac_module.h
        include/anocrypto/ac_selftest.h
        abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt
        docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md)
    require_file("${_required}")
endforeach()

require_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "SELECTED_CANDIDATE_NOT_IMPLEMENTED")
require_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "EXTERNAL_CONFIRMATION_REQUIRED")
require_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "INT-HMAC-SHA256-SINGLE-SO-R1")
require_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "The current runtime has startup algorithm KATs")
require_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "does not yet authenticate the installed shared-object bytes")
require_text("docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md"
    "There is no transition from `ERROR_STATE` to `OPERATIONAL`")
require_text("docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md"
    "SHUTDOWN -> SELF_TEST_RUNNING -> OPERATIONAL")
require_text("docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md"
    "Status-to-fatality rule")
require_text("docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md"
    "P2-03-IMP-001")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md"
    "BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "NOT_RUN")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "UNREVIEWED")

require_text("src/core/module_state.c" "ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING")
require_text("src/core/module_state.c" "g_fatal_latched")
require_text("src/core/module_state.c" "ANO_CRYPTO_MODULE_STATE_ERROR_STATE")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_check_build_identity")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_check_kat_manifest")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_sha256_kat")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_hmac_sha256_kat")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_sha512_family_kat")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_hmac_sha512_family_kat")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_aes_cbc_kat")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_aes_cmac_kat")
require_text("src/selftest/selftest.c" "ano_crypto_internal_hmac_drbg_sha256_selftest")

# P2-03 is a design/specification step. A runtime integrity implementation or
# completed-acceptance claim must not be silently introduced.
forbid_text("src/selftest/selftest.c" "ano_crypto_internal_module_integrity_verify")
forbid_text("src/core/module_state.c" "ano_crypto_internal_module_integrity_verify")
forbid_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "FORMALLY_VALIDATED")
forbid_text("docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md"
    "INTEGRITY_IMPLEMENTED_AND_ACCEPTED")

set(_public_headers "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
set(_public_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
if(NOT _public_header_count EQUAL 14)
    message(FATAL_ERROR "Expected 14 public headers, found ${_public_header_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt"
    _symbol_lines)
set(_symbol_count 0)
foreach(_line IN LISTS _symbol_lines)
    string(STRIP "${_line}" _line)
    if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
        math(EXPR _symbol_count "${_symbol_count} + 1")
    endif()
endforeach()
if(NOT _symbol_count EQUAL 69)
    message(FATAL_ERROR "Expected 69 exported-symbol baseline entries, found ${_symbol_count}")
endif()

list(LENGTH ANO_CRYPTO_STARTUP_TEST_CLASSES _startup_class_count)
if(NOT _startup_class_count EQUAL 8)
    message(FATAL_ERROR "Expected 8 startup check/test classes, found ${_startup_class_count}")
endif()

file(WRITE "${_output}"
    "AnoCrypto-C P2-03 self-test/integrity policy check\n"
    "Result: PASS\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Validation candidate: ${ANO_CRYPTO_VALIDATION_CANDIDATE_ID}\n"
    "Self-test policy: ${ANO_CRYPTO_SELF_TEST_POLICY_ID}\n"
    "Finite-state policy: ${ANO_CRYPTO_FINITE_STATE_POLICY_ID}\n"
    "Integrity candidate: ${ANO_CRYPTO_INTEGRITY_CANDIDATE_ID}\n"
    "Integrity status: ${ANO_CRYPTO_INTEGRITY_CANDIDATE_STATUS}\n"
    "Runtime integrity implementation: NOT_IMPLEMENTED\n"
    "External confirmation: REQUIRED\n"
    "On-demand transition implementation: NOT_IMPLEMENTED\n"
    "Fatal recovery: ${ANO_CRYPTO_FATAL_RECOVERY_MODEL}\n"
    "Orderly reinitialization: ALLOWED\n"
    "Startup check/test classes: ${_startup_class_count}\n"
    "Public headers: ${_public_header_count}\n"
    "Exported symbol baseline: ${_symbol_count}\n"
    "ABI line: ${ANO_CRYPTO_SELF_TEST_ABI_LINE}\n"
    "Actual hosted lanes: ${ANO_CRYPTO_SELF_TEST_HOSTED_STATUS}\n"
    "Actual hosted review: ${ANO_CRYPTO_SELF_TEST_HOSTED_REVIEW}\n"
    "External release authorization: ${ANO_CRYPTO_SELF_TEST_RELEASE_AUTHORIZATION}\n")

message(STATUS "P2-03 self-test/integrity policy check passed: ${_output}")
