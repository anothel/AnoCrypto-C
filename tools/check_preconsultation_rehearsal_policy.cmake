cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_PRECONSULTATION_CHECK_OUTPUT OR ANO_CRYPTO_PRECONSULTATION_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_PRECONSULTATION_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/PRECONSULTATION_REHEARSAL_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_PRECONSULTATION_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
set(_violations "")

function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}" OR IS_DIRECTORY "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_PRECONSULTATION_POLICY.cmake")

if(NOT ANO_CRYPTO_PRECONSULTATION_POLICY_ID STREQUAL "PCR-P2-08-R1" OR
   NOT ANO_CRYPTO_PRECONSULTATION_CANDIDATE_ID STREQUAL ANO_CRYPTO_VALIDATION_CANDIDATE_ID OR
   NOT ANO_CRYPTO_PRECONSULTATION_REHEARSAL_STATUS STREQUAL "COMPLETE_LOCAL_SCRIPTED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_LIVE_INTERNAL_REHEARSAL STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_PRECONSULTATION_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_EXACT_OE01 STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_PRECONSULTATION_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_PRECONSULTATION_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_PRECONSULTATION_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P2-08 pre-consultation values drifted")
endif()

set(_required_docs
  docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md
  docs/kcmvp/KCMVP_EVIDENCE_WALKTHROUGH_SCRIPT.md
  docs/kcmvp/KCMVP_UNRESOLVED_DECISION_REGISTER.md
  docs/kcmvp/KCMVP_CONSULTATION_RESPONSE_CAPTURE_TEMPLATE.md
  docs/kcmvp/KCMVP_POST_CONSULTATION_ACTION_IMPACT_MATRIX.md
  docs/kcmvp/P2_08_IMPLEMENTATION_TEST_DELTA.md
  docs/kcmvp/KCMVP_PRECONSULTATION_QUESTIONS.md
  docs/kcmvp/KCMVP_READINESS_GAP_P2.md
  docs/kcmvp/submission/KCMVP_SUBMISSION_REVIEW_CHECKLIST.md)
foreach(_doc IN LISTS _required_docs)
  require_file("${_doc}")
endforeach()
require_file("release/ANOCRYPTO_C_PRECONSULTATION_POLICY.cmake")
require_file("tools/create_preconsultation_rehearsal_package.cmake")
require_file("tools/check_preconsultation_rehearsal_package.cmake")
require_file("tools/run_preconsultation_rehearsal_package_smoke.cmake")

require_text("docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md" "External consultation | `NOT_CONDUCTED`")
require_text("docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md" "Institution response | `NOT_RECEIVED`")
require_text("docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md" "P1-34 hosted lanes remain `NOT_RUN`")
require_text("docs/kcmvp/KCMVP_EVIDENCE_WALKTHROUGH_SCRIPT.md" "runtime integrity status `NOT_IMPLEMENTED`")
require_text("docs/kcmvp/KCMVP_EVIDENCE_WALKTHROUGH_SCRIPT.md" "Institution answers | `NOT_RECEIVED`")
require_text("docs/kcmvp/KCMVP_UNRESOLVED_DECISION_REGISTER.md" "DEC-020")
require_text("docs/kcmvp/KCMVP_CONSULTATION_RESPONSE_CAPTURE_TEMPLATE.md" "Authorized public API/ABI change: NO")
require_text("docs/kcmvp/KCMVP_POST_CONSULTATION_ACTION_IMPACT_MATRIX.md" "ACT-4_PUBLIC_API_ABI_GATE")
require_text("docs/kcmvp/P2_08_IMPLEMENTATION_TEST_DELTA.md" "Institution consultation: NOT_CONDUCTED")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "| Actual hosted lanes | `NOT_RUN` |")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-00-downstream-requirement-intake")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md" _rehearsal)
string(REGEX MATCHALL "\\| A-[0-9][0-9] \\|" _agendas "${_rehearsal}")
list(LENGTH _agendas _agenda_count)
if(NOT _agenda_count EQUAL ANO_CRYPTO_PRECONSULTATION_HIGH_PRIORITY_AGENDAS)
  list(APPEND _violations "Priority-1 agenda count drifted: ${_agenda_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_UNRESOLVED_DECISION_REGISTER.md" _decisions)
string(REGEX MATCHALL "\\| DEC-[0-9][0-9][0-9] \\|" _decision_rows "${_decisions}")
list(LENGTH _decision_rows _decision_count)
if(NOT _decision_count EQUAL ANO_CRYPTO_PRECONSULTATION_DECISION_COUNT)
  list(APPEND _violations "decision-register count drifted: ${_decision_count}")
endif()

list(LENGTH ANO_CRYPTO_PRECONSULTATION_PACKAGE_FILES _package_doc_count)
if(NOT _package_doc_count EQUAL 9)
  list(APPEND _violations "rehearsal package allowlist count drifted: ${_package_doc_count}")
endif()
foreach(_rel IN LISTS ANO_CRYPTO_PRECONSULTATION_PACKAGE_FILES)
  require_file("${_rel}")
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx)$")
    list(APPEND _violations "unsafe rehearsal package path: ${_rel}")
  endif()
endforeach()

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
if(NOT _header_count EQUAL ANO_CRYPTO_PRECONSULTATION_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_PRECONSULTATION_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL ANO_CRYPTO_PRECONSULTATION_ABI_LINE)
  list(APPEND _violations "public ABI changed: headers=${_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

file(WRITE "${_output}"
  "AnoCrypto-C P2-08 Pre-consultation Rehearsal Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_PRECONSULTATION_POLICY_ID}\n"
  "Candidate ID: ${ANO_CRYPTO_PRECONSULTATION_CANDIDATE_ID}\n"
  "Scripted rehearsal: ${ANO_CRYPTO_PRECONSULTATION_REHEARSAL_STATUS}\n"
  "External consultation: ${ANO_CRYPTO_PRECONSULTATION_EXTERNAL_CONSULTATION}\n"
  "Institution responses: ${ANO_CRYPTO_PRECONSULTATION_INSTITUTION_RESPONSES}\n")
if(_violations)
  file(APPEND "${_output}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${_output}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "P2-08 pre-consultation rehearsal policy check failed")
endif()
file(APPEND "${_output}"
  "Result: PASS\n"
  "Priority-1 agendas: ${_agenda_count}\n"
  "Unresolved decisions: ${_decision_count}\n"
  "Package allowlist documents: ${_package_doc_count}\n"
  "Runtime integrity: ${ANO_CRYPTO_PRECONSULTATION_RUNTIME_INTEGRITY}\n"
  "Exact OE-01: ${ANO_CRYPTO_PRECONSULTATION_EXACT_OE01}\n"
  "Hosted status/review: ${ANO_CRYPTO_PRECONSULTATION_HOSTED_STATUS}/${ANO_CRYPTO_PRECONSULTATION_HOSTED_REVIEW}\n"
  "Repository signing/handoff: ${ANO_CRYPTO_PRECONSULTATION_REPOSITORY_SIGNING}/${ANO_CRYPTO_PRECONSULTATION_SIGNING_HANDOFF}\n"
  "Submission/external authorization: ${ANO_CRYPTO_PRECONSULTATION_SUBMISSION_AUTHORIZATION}/${ANO_CRYPTO_PRECONSULTATION_EXTERNAL_AUTHORIZATION}\n"
  "Public ABI: headers=${_header_count}, symbols=${_symbol_count}, line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
message(STATUS "P2-08 pre-consultation rehearsal policy check passed")
