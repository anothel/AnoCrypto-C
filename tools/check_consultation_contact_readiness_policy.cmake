cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_CCR_CHECK_OUTPUT OR ANO_CRYPTO_CCR_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_CCR_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/CONSULTATION_CONTACT_READINESS_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_CCR_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_out_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
set(_violations "")
function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}" OR IS_DIRECTORY "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing required file: ${rel}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()
function(require_text rel needle)
  require_file("${rel}")
  if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" _text)
    string(FIND "${_text}" "${needle}" _pos)
    if(_pos EQUAL -1)
      list(APPEND _violations "${rel} missing required text: ${needle}")
      set(_violations "${_violations}" PARENT_SCOPE)
    endif()
  endif()
endfunction()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_POLICY.cmake")
if(NOT ANO_CRYPTO_CCR_POLICY_ID STREQUAL "CCR-P2-12-R1" OR
   NOT ANO_CRYPTO_CCR_STATUS STREQUAL "READY_FOR_OWNER_SELECTION_AFTER_HUMAN_REHEARSAL" OR
   NOT ANO_CRYPTO_CCR_PACKAGE_CLASS STREQUAL "STATUS_ONLY_CONSULTATION_CONTACT_READINESS" OR
   NOT ANO_CRYPTO_CCR_DOCUMENT_COUNT EQUAL 15 OR
   NOT ANO_CRYPTO_CCR_PUBLIC_SOURCE_COUNT EQUAL 5 OR
   NOT ANO_CRYPTO_CCR_CONTACT_CANDIDATE_COUNT EQUAL 5 OR
   NOT ANO_CRYPTO_CCR_PRIORITY_AGENDA_COUNT EQUAL 10 OR
   NOT ANO_CRYPTO_CCR_INDEPENDENT_HUMAN_REHEARSAL STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_CCR_PROJECT_OWNER_AUTHORIZATION STREQUAL "NOT_RECORDED" OR
   NOT ANO_CRYPTO_CCR_SELECTED_INSTITUTION STREQUAL "UNSELECTED" OR
   NOT ANO_CRYPTO_CCR_AUTHORIZED_SENDER STREQUAL "UNASSIGNED" OR
   NOT ANO_CRYPTO_CCR_CONTACT_CHANNEL STREQUAL "UNSELECTED" OR
   NOT ANO_CRYPTO_CCR_FIRST_CONTACT STREQUAL "NOT_SENT" OR
   NOT ANO_CRYPTO_CCR_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_CCR_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_CCR_FORMAL_VALIDATION STREQUAL "NOT_STARTED" OR
   NOT ANO_CRYPTO_CCR_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_CCR_EXACT_OE01 STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_CCR_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_CCR_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_CCR_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_CCR_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_CCR_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_CCR_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_CCR_NEXT_IMPLEMENTATION_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P2-12 consultation contact-readiness values drifted")
endif()
foreach(_rel IN ITEMS
  docs/kcmvp/KCMVP_PUBLIC_PROCEDURE_SOURCE_REGISTER.md
  docs/kcmvp/KCMVP_CONTACT_CANDIDATE_MATRIX.md
  docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_CONTACT_PLAN.md
  docs/kcmvp/KCMVP_CONSULTATION_CONTACT_REQUEST_TEMPLATE.md
  docs/kcmvp/KCMVP_CONSULTATION_CHANNEL_AND_DATA_HANDLING.md
  docs/kcmvp/KCMVP_CONTACT_READINESS_CHECKLIST.md
  docs/kcmvp/P2_12_IMPLEMENTATION_TEST_DELTA.md
  release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_POLICY.cmake
  release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_MANIFEST.txt)
  require_file("${_rel}")
endforeach()
require_text("docs/kcmvp/KCMVP_PUBLIC_PROCEDURE_SOURCE_REGISTER.md" "Checked date | `2026-07-17`")
require_text("docs/kcmvp/KCMVP_PUBLIC_PROCEDURE_SOURCE_REGISTER.md" "PUBLIC_SOURCE_REFRESH_COMPLETE")
require_text("docs/kcmvp/KCMVP_CONTACT_CANDIDATE_MATRIX.md" "Candidate selection | `UNSELECTED`")
require_text("docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_CONTACT_PLAN.md" "First contact | `NOT_SENT`")
require_text("docs/kcmvp/KCMVP_CONSULTATION_CONTACT_REQUEST_TEMPLATE.md" "Status: `DRAFT_NOT_SENT`")
require_text("docs/kcmvp/KCMVP_CONSULTATION_CHANNEL_AND_DATA_HANDLING.md" "Non-public transfer authorization | `NO`")
require_text("docs/kcmvp/KCMVP_CONTACT_READINESS_CHECKLIST.md" "NO_GO_PENDING_HUMAN_REHEARSAL_AND_OWNER_SELECTION")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "BLOCKED_EXTERNAL")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "NOT_RUN")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "UNREVIEWED")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-04-aes256-gcm-streaming")
require_text("README.md" "0.1.0-p3-04-aes256-gcm-streaming")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_0 {")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  math(EXPR _manifest_count "${_manifest_count}+1")
  require_file("${_rel}")
endforeach()
if(NOT _manifest_count EQUAL ANO_CRYPTO_CCR_DOCUMENT_COUNT)
  list(APPEND _violations "P2-12 contact-readiness manifest count drifted: ${_manifest_count}")
endif()
set(_headers "${ANO_CRYPTO_RETAINED_PUBLIC_HEADERS}")
set(_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _s)
  if(NOT _s STREQUAL "" AND NOT _s MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count}+1")
  endif()
endforeach()
if(NOT _header_count EQUAL ANO_CRYPTO_CCR_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_CCR_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_CCR_ABI_LINE STREQUAL "ANOCRYPTO_C_0")
  list(APPEND _violations "public ABI baseline drifted: headers=${_header_count}, symbols=${_symbol_count}, abi=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()
string(JOIN "\n- " _violations_text ${_violations})
file(WRITE "${_output}"
  "AnoCrypto-C P2-12 Consultation Contact Readiness Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_CCR_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_CCR_STATUS}\n"
  "Public source/candidate/agenda counts: ${ANO_CRYPTO_CCR_PUBLIC_SOURCE_COUNT}/${ANO_CRYPTO_CCR_CONTACT_CANDIDATE_COUNT}/${ANO_CRYPTO_CCR_PRIORITY_AGENDA_COUNT}\n"
  "Human rehearsal/owner authorization: ${ANO_CRYPTO_CCR_INDEPENDENT_HUMAN_REHEARSAL}/${ANO_CRYPTO_CCR_PROJECT_OWNER_AUTHORIZATION}\n"
  "Institution/sender/channel: ${ANO_CRYPTO_CCR_SELECTED_INSTITUTION}/${ANO_CRYPTO_CCR_AUTHORIZED_SENDER}/${ANO_CRYPTO_CCR_CONTACT_CHANNEL}\n"
  "First contact/consultation/responses: ${ANO_CRYPTO_CCR_FIRST_CONTACT}/${ANO_CRYPTO_CCR_EXTERNAL_CONSULTATION}/${ANO_CRYPTO_CCR_INSTITUTION_RESPONSES}\n"
  "Hosted status/review: ${ANO_CRYPTO_CCR_HOSTED_STATUS}/${ANO_CRYPTO_CCR_HOSTED_REVIEW}\n"
  "Signing/handoff/authorization: ${ANO_CRYPTO_CCR_REPOSITORY_SIGNING}/${ANO_CRYPTO_CCR_SIGNING_HANDOFF}/${ANO_CRYPTO_CCR_EXTERNAL_AUTHORIZATION}\n"
  "Public ABI: ${_header_count} headers, ${_symbol_count} symbols, ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
  "Violations: ${_violations_text}\n")
if(_violations)
  message(FATAL_ERROR "P2-12 consultation contact-readiness policy check failed")
endif()
message(STATUS "P2-12 consultation contact-readiness policy check passed")
