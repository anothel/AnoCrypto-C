cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_HRH_CHECK_OUTPUT OR ANO_CRYPTO_HRH_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_HRH_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/HUMAN_REHEARSAL_HANDOFF_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_HRH_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
set(_violations "")

function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}" OR IS_DIRECTORY "${ANO_CRYPTO_SOURCE_DIR}/${rel}" OR IS_SYMLINK "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing/unsafe ${rel}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()

function(require_text rel needle)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" _text)
    string(FIND "${_text}" "${needle}" _pos)
    if(_pos EQUAL -1)
      list(APPEND _violations "${rel} missing marker: ${needle}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_LIVE_REHEARSAL_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_POLICY.cmake")

if(NOT ANO_CRYPTO_HRH_POLICY_ID STREQUAL "HRH-P2-11-R1" OR
   NOT ANO_CRYPTO_HRH_STATUS STREQUAL "READY_FOR_NAMED_HUMAN_EXECUTION" OR
   NOT ANO_CRYPTO_HRH_PACKAGE_CLASS STREQUAL "STATUS_ONLY_HUMAN_REHEARSAL_HANDOFF" OR
   NOT ANO_CRYPTO_HRH_DOCUMENT_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_HRH_AGENDA_COUNT EQUAL 10 OR
   NOT ANO_CRYPTO_HRH_REQUIRED_OUTPUT_COUNT EQUAL 4 OR
   NOT ANO_CRYPTO_HRH_CONTACT_PRECONDITION_COUNT EQUAL 6 OR
   NOT ANO_CRYPTO_HRH_REVIEWER_IDENTITY STREQUAL "UNASSIGNED" OR
   NOT ANO_CRYPTO_HRH_REVIEWER_INDEPENDENCE STREQUAL "NOT_RECORDED" OR
   NOT ANO_CRYPTO_HRH_INDEPENDENT_HUMAN_REHEARSAL STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_HRH_PROJECT_OWNER_AUTHORIZATION STREQUAL "NOT_RECORDED" OR
   NOT ANO_CRYPTO_HRH_CONTACT_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_HRH_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_HRH_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_HRH_FORMAL_VALIDATION STREQUAL "NOT_STARTED" OR
   NOT ANO_CRYPTO_HRH_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_HRH_EXACT_OE01 STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_HRH_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_HRH_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_HRH_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_HRH_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_HRH_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_HRH_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_HRH_NEXT_IMPLEMENTATION_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P2-11 human-rehearsal handoff values drifted")
endif()

if(NOT ANO_CRYPTO_LIVE_REHEARSAL_STATUS STREQUAL "COMPLETE_LOCAL_ASSISTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_INDEPENDENT_HUMAN STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED")
  list(APPEND _violations "P2-10 assisted rehearsal was incorrectly promoted")
endif()

set(_required
  docs/kcmvp/KCMVP_HUMAN_REHEARSAL_HANDOFF_PACK_INDEX.md
  docs/kcmvp/KCMVP_INDEPENDENT_HUMAN_REHEARSAL_HANDOFF.md
  docs/kcmvp/KCMVP_NAMED_REVIEWER_EXECUTION_GUIDE.md
  docs/kcmvp/KCMVP_LIVE_REHEARSAL_MINUTES_TEMPLATE.md
  docs/kcmvp/KCMVP_REHEARSAL_OBJECTION_AND_DEFECT_REGISTER.md
  docs/kcmvp/KCMVP_CONSULTATION_AUTHORIZATION_RECORD.md
  docs/kcmvp/P2_11_IMPLEMENTATION_TEST_DELTA.md
  release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_POLICY.cmake
  release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_MANIFEST.txt
  tools/create_human_rehearsal_handoff_package.cmake
  tools/check_human_rehearsal_handoff_package.cmake
  tools/run_human_rehearsal_handoff_package_smoke.cmake)
foreach(_item IN LISTS _required)
  require_file("${_item}")
endforeach()

require_text("docs/kcmvp/KCMVP_HUMAN_REHEARSAL_HANDOFF_PACK_INDEX.md" "Independent human live rehearsal | `NOT_RUN`")
require_text("docs/kcmvp/KCMVP_INDEPENDENT_HUMAN_REHEARSAL_HANDOFF.md" "An automated assistant, document author, generated transcript, or policy check cannot be represented as the independent reviewer")
require_text("docs/kcmvp/KCMVP_INDEPENDENT_HUMAN_REHEARSAL_HANDOFF.md" "READY_FOR_NAMED_HUMAN_EXECUTION")
require_text("docs/kcmvp/KCMVP_NAMED_REVIEWER_EXECUTION_GUIDE.md" "Required ten-agenda execution table")
require_text("docs/kcmvp/KCMVP_LIVE_REHEARSAL_MINUTES_TEMPLATE.md" "Reviewer name | `UNASSIGNED`")
require_text("docs/kcmvp/KCMVP_REHEARSAL_OBJECTION_AND_DEFECT_REGISTER.md" "No human-review finding exists yet")
require_text("docs/kcmvp/KCMVP_CONSULTATION_AUTHORIZATION_RECORD.md" "Institution contact authorization | `NO`")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "| Actual hosted lanes | `NOT_RUN` |")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-02-hkdf-sha256")
require_text("README.md" "0.1.0-p3-02-hkdf-sha256")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_NAMED_REVIEWER_EXECUTION_GUIDE.md" _guide)
string(REGEX MATCHALL "[|] `A-[0-9][0-9]` [|]" _agenda_rows "${_guide}")
list(LENGTH _agenda_rows _agenda_count)
if(NOT _agenda_count EQUAL 10)
  list(APPEND _violations "human rehearsal agenda count drifted: ${_agenda_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_CONSULTATION_AUTHORIZATION_RECORD.md" _auth)
string(REGEX MATCHALL "[|] `AUTH-[0-9][0-9]` [|]" _auth_rows "${_auth}")
list(LENGTH _auth_rows _auth_count)
if(NOT _auth_count EQUAL 6)
  list(APPEND _violations "contact precondition count drifted: ${_auth_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _line)
  if(_line STREQUAL "" OR _line MATCHES "^#")
    continue()
  endif()
  require_file("${_line}")
  math(EXPR _manifest_count "${_manifest_count}+1")
endforeach()
if(NOT _manifest_count EQUAL ANO_CRYPTO_HRH_DOCUMENT_COUNT)
  list(APPEND _violations "human rehearsal handoff allowlist count drifted: ${_manifest_count}")
endif()

set(_public_headers "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
set(_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _line)
  if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count}+1")
  endif()
endforeach()
if(NOT _header_count EQUAL ANO_CRYPTO_HRH_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_HRH_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_HRH_ABI_LINE STREQUAL "ANOCRYPTO_C_0")
  list(APPEND _violations "public ABI changed: headers=${_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

file(WRITE "${_output}"
  "AnoCrypto-C P2-11 Human Rehearsal Handoff Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_HRH_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_HRH_STATUS}\n"
  "Engineering version: ${ANO_CRYPTO_RELEASE_VERSION}\n")
if(_violations)
  file(APPEND "${_output}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${_output}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "P2-11 human rehearsal handoff policy check failed")
endif()
file(APPEND "${_output}"
  "Result: PASS\n"
  "Documents/agendas/outputs/preconditions: ${_manifest_count}/${_agenda_count}/${ANO_CRYPTO_HRH_REQUIRED_OUTPUT_COUNT}/${_auth_count}\n"
  "Reviewer/independence: ${ANO_CRYPTO_HRH_REVIEWER_IDENTITY}/${ANO_CRYPTO_HRH_REVIEWER_INDEPENDENCE}\n"
  "Independent human rehearsal: ${ANO_CRYPTO_HRH_INDEPENDENT_HUMAN_REHEARSAL}\n"
  "Project-owner/contact authorization: ${ANO_CRYPTO_HRH_PROJECT_OWNER_AUTHORIZATION}/${ANO_CRYPTO_HRH_CONTACT_AUTHORIZATION}\n"
  "External consultation/responses: ${ANO_CRYPTO_HRH_EXTERNAL_CONSULTATION}/${ANO_CRYPTO_HRH_INSTITUTION_RESPONSES}\n"
  "Hosted status/review: ${ANO_CRYPTO_HRH_HOSTED_STATUS}/${ANO_CRYPTO_HRH_HOSTED_REVIEW}\n"
  "Signing/handoff: ${ANO_CRYPTO_HRH_REPOSITORY_SIGNING}/${ANO_CRYPTO_HRH_SIGNING_HANDOFF}\n"
  "Public ABI: headers=${_header_count}, symbols=${_symbol_count}, line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
message(STATUS "P2-11 human rehearsal handoff policy check passed")
