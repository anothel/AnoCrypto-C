cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_LIVE_REHEARSAL_CHECK_OUTPUT OR ANO_CRYPTO_LIVE_REHEARSAL_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_LIVE_REHEARSAL_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/LIVE_REHEARSAL_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_LIVE_REHEARSAL_CHECK_OUTPUT}" ABSOLUTE)
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_P2_CLOSEOUT_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_LIVE_REHEARSAL_POLICY.cmake")

if(NOT ANO_CRYPTO_LIVE_REHEARSAL_POLICY_ID STREQUAL "LAR-P2-10-R1" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_STATUS STREQUAL "COMPLETE_LOCAL_ASSISTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_PACKAGE_CLASS STREQUAL "STATUS_ONLY_ASSISTED_REHEARSAL" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_AGENDA_COUNT EQUAL 10 OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_RESPONSE_COUNT EQUAL 10 OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_GAP_COUNT EQUAL 9 OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_CONTACT_PRECONDITION_COUNT EQUAL 6 OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_INDEPENDENT_HUMAN STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_CONTACT_DECISION STREQUAL "CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_FORMAL_VALIDATION STREQUAL "NOT_STARTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_EXACT_OE01 STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_LIVE_REHEARSAL_NEXT_IMPLEMENTATION_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P2-10 assisted rehearsal values drifted")
endif()

if(NOT ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "P2-09 closeout blockers were incorrectly advanced")
endif()

set(_required
  docs/kcmvp/KCMVP_INTERNAL_ASSISTED_REHEARSAL_REPORT.md
  docs/kcmvp/KCMVP_ASSISTED_REHEARSAL_TRANSCRIPT.md
  docs/kcmvp/KCMVP_REHEARSAL_RESPONSE_SCORECARD.md
  docs/kcmvp/KCMVP_REHEARSAL_GAPS_AND_REMEDIATION.md
  docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_DECISION_GATE.md
  docs/kcmvp/P2_10_IMPLEMENTATION_TEST_DELTA.md
  release/ANOCRYPTO_C_LIVE_REHEARSAL_POLICY.cmake
  release/ANOCRYPTO_C_LIVE_REHEARSAL_MANIFEST.txt
  tools/create_live_rehearsal_package.cmake
  tools/check_live_rehearsal_package.cmake
  tools/run_live_rehearsal_package_smoke.cmake)
foreach(_item IN LISTS _required)
  require_file("${_item}")
endforeach()

require_text("docs/kcmvp/KCMVP_INTERNAL_ASSISTED_REHEARSAL_REPORT.md" "Independent human live rehearsal | `NOT_RUN`")
require_text("docs/kcmvp/KCMVP_INTERNAL_ASSISTED_REHEARSAL_REPORT.md" "READY_FOR_NAMED_HUMAN_INTERNAL_REHEARSAL")
require_text("docs/kcmvp/KCMVP_ASSISTED_REHEARSAL_TRANSCRIPT.md" "It is not checked by a runtime verifier today")
require_text("docs/kcmvp/KCMVP_REHEARSAL_RESPONSE_SCORECARD.md" "No agenda was rated `NOT_READY`")
require_text("docs/kcmvp/KCMVP_REHEARSAL_GAPS_AND_REMEDIATION.md" "| RGH-008 | P1-34 actual hosted evidence remains unavailable")
require_text("docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_DECISION_GATE.md" "Current decision | `CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION`")
require_text("docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_DECISION_GATE.md" "All six must be retained")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "| Actual hosted lanes | `NOT_RUN` |")
require_text("include/anocrypto/ac_version.h" "0.1.0-p2-11-human-rehearsal-handoff")
require_text("README.md" "0.1.0-p2-11-human-rehearsal-handoff")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_INTERNAL_ASSISTED_REHEARSAL_REPORT.md" _report)
string(REGEX MATCHALL "[|] `A-[0-9][0-9]` " _agenda_rows "${_report}")
list(LENGTH _agenda_rows _agenda_count)
if(NOT _agenda_count EQUAL 10)
  list(APPEND _violations "assisted rehearsal agenda count drifted: ${_agenda_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_REHEARSAL_GAPS_AND_REMEDIATION.md" _gaps)
string(REGEX MATCHALL "[|] RGH-[0-9][0-9][0-9] [|]" _gap_rows "${_gaps}")
list(LENGTH _gap_rows _gap_count)
if(NOT _gap_count EQUAL 9)
  list(APPEND _violations "rehearsal gap count drifted: ${_gap_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_DECISION_GATE.md" _gate)
string(REGEX MATCHALL "[0-9]+[.] `AUTH-[0-9][0-9]`" _auth_rows "${_gate}")
list(LENGTH _auth_rows _auth_count)
if(NOT _auth_count EQUAL 6)
  list(APPEND _violations "contact precondition count drifted: ${_auth_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_LIVE_REHEARSAL_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _line)
  if(_line STREQUAL "" OR _line MATCHES "^#")
    continue()
  endif()
  require_file("${_line}")
  math(EXPR _manifest_count "${_manifest_count}+1")
endforeach()
if(NOT _manifest_count EQUAL 13)
  list(APPEND _violations "assisted rehearsal package allowlist count drifted: ${_manifest_count}")
endif()

file(GLOB _public_headers "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(LENGTH _public_headers _header_count)
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _line)
  if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count}+1")
  endif()
endforeach()
if(NOT _header_count EQUAL ANO_CRYPTO_LIVE_REHEARSAL_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_LIVE_REHEARSAL_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL ANO_CRYPTO_LIVE_REHEARSAL_ABI_LINE)
  list(APPEND _violations "public ABI changed: headers=${_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

file(WRITE "${_output}"
  "AnoCrypto-C P2-10 Assisted Rehearsal Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_LIVE_REHEARSAL_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_LIVE_REHEARSAL_STATUS}\n"
  "Engineering version: ${ANO_CRYPTO_RELEASE_VERSION}\n")
if(_violations)
  file(APPEND "${_output}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${_output}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "P2-10 assisted rehearsal policy check failed")
endif()
file(APPEND "${_output}"
  "Result: PASS\n"
  "Agendas/responses/gaps/preconditions: ${_agenda_count}/${ANO_CRYPTO_LIVE_REHEARSAL_RESPONSE_COUNT}/${_gap_count}/${_auth_count}\n"
  "Independent human rehearsal: ${ANO_CRYPTO_LIVE_REHEARSAL_INDEPENDENT_HUMAN}\n"
  "External consultation/responses: ${ANO_CRYPTO_LIVE_REHEARSAL_EXTERNAL_CONSULTATION}/${ANO_CRYPTO_LIVE_REHEARSAL_INSTITUTION_RESPONSES}\n"
  "Contact decision: ${ANO_CRYPTO_LIVE_REHEARSAL_CONTACT_DECISION}\n"
  "Hosted status/review: ${ANO_CRYPTO_LIVE_REHEARSAL_HOSTED_STATUS}/${ANO_CRYPTO_LIVE_REHEARSAL_HOSTED_REVIEW}\n"
  "Signing/handoff: ${ANO_CRYPTO_LIVE_REHEARSAL_REPOSITORY_SIGNING}/${ANO_CRYPTO_LIVE_REHEARSAL_SIGNING_HANDOFF}\n"
  "Public ABI: headers=${_header_count}, symbols=${_symbol_count}, line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
message(STATUS "P2-10 assisted rehearsal policy check passed")
