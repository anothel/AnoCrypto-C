cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_P2_CLOSEOUT_CHECK_OUTPUT OR ANO_CRYPTO_P2_CLOSEOUT_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_P2_CLOSEOUT_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/P2_CLOSEOUT_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_P2_CLOSEOUT_CHECK_OUTPUT}" ABSOLUTE)
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_PRECONSULTATION_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_P2_CLOSEOUT_POLICY.cmake")

if(NOT ANO_CRYPTO_P2_CLOSEOUT_POLICY_ID STREQUAL "P2C-P2-09-R1" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_STATUS STREQUAL "COMPLETE_LOCAL_CLOSEOUT" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_SEQUENCE_START STREQUAL "P2-01" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_SEQUENCE_END STREQUAL "P2-09" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_PACKAGE_CLASS STREQUAL "STATUS_ONLY_PRECONSULTATION_CLOSEOUT" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_FORMAL_VALIDATION STREQUAL "NOT_STARTED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_EXACT_OE01 STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_P2_CLOSEOUT_NEXT_IMPLEMENTATION_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P2-09 closeout values drifted")
endif()

if(NOT ANO_CRYPTO_PRECONSULTATION_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_INSTITUTION_RESPONSES STREQUAL "NOT_RECEIVED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_PRECONSULTATION_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_PRECONSULTATION_SUBMISSION_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_PRECONSULTATION_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "P2-08 pre-consultation blockers were incorrectly advanced")
endif()

set(_required_docs
  docs/kcmvp/P2_VALIDATION_READINESS_CLOSEOUT_REVIEW.md
  docs/kcmvp/KCMVP_PRECONSULTATION_PACK_INDEX.md
  docs/kcmvp/KCMVP_OPEN_ITEMS_AND_EXIT_CRITERIA.md
  docs/kcmvp/KCMVP_CLAIMS_AND_STATUS_SUMMARY.md
  docs/kcmvp/P2_09_IMPLEMENTATION_TEST_DELTA.md
  docs/roadmap/P2_CLOSEOUT_AND_NEXT_WORK_GATE.md
  release/ANOCRYPTO_C_P2_CLOSEOUT_POLICY.cmake
  release/ANOCRYPTO_C_PRECONSULTATION_CLOSEOUT_MANIFEST.txt
  tools/create_preconsultation_closeout_package.cmake
  tools/check_preconsultation_closeout_package.cmake
  tools/run_preconsultation_closeout_package_smoke.cmake)
foreach(_doc IN LISTS _required_docs)
  require_file("${_doc}")
endforeach()

require_text("docs/kcmvp/P2_VALIDATION_READINESS_CLOSEOUT_REVIEW.md" "Controlled internal pre-consultation pack assembly | `GO`")
require_text("docs/kcmvp/P2_VALIDATION_READINESS_CLOSEOUT_REVIEW.md" "Formal test/validation claim | `NO_GO`")
require_text("docs/kcmvp/P2_VALIDATION_READINESS_CLOSEOUT_REVIEW.md" "P1-34 actual hosted lanes: `NOT_RUN` and `UNREVIEWED`")
require_text("docs/kcmvp/KCMVP_CLAIMS_AND_STATUS_SUMMARY.md" "External consultation | `NOT_CONDUCTED`")
require_text("docs/kcmvp/KCMVP_CLAIMS_AND_STATUS_SUMMARY.md" "External release authorization | `NO`")
require_text("docs/kcmvp/KCMVP_OPEN_ITEMS_AND_EXIT_CRITERIA.md" "Gate A — internal pre-consultation pack ready")
require_text("docs/kcmvp/KCMVP_OPEN_ITEMS_AND_EXIT_CRITERIA.md" "Gate E — external release ready")
require_text("docs/roadmap/P2_CLOSEOUT_AND_NEXT_WORK_GATE.md" "no automatic P3 feature expansion")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "| Actual hosted lanes | `NOT_RUN` |")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-01-downstream-requirement-consolidation")
require_text("README.md" "0.1.0-p3-01-downstream-requirement-consolidation")

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_PRECONSULTATION_CLOSEOUT_MANIFEST.txt" _manifest_lines)
set(_pack_docs "")
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _line)
  if(_line STREQUAL "" OR _line MATCHES "^#")
    continue()
  endif()
  list(APPEND _pack_docs "${_line}")
endforeach()
list(LENGTH _pack_docs _package_doc_count)
if(NOT _package_doc_count EQUAL 17)
  list(APPEND _violations "closeout package allowlist count drifted: ${_package_doc_count}")
endif()
foreach(_rel IN LISTS _pack_docs)
  require_file("${_rel}")
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    list(APPEND _violations "unsafe closeout package path: ${_rel}")
  endif()
endforeach()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_PRECONSULTATION_REHEARSAL.md" _rehearsal)
string(REGEX MATCHALL "\\| A-[0-9][0-9] \\|" _agendas "${_rehearsal}")
list(LENGTH _agendas _agenda_count)
if(NOT _agenda_count EQUAL 10)
  list(APPEND _violations "Priority-1 agenda count drifted: ${_agenda_count}")
endif()
file(READ "${ANO_CRYPTO_SOURCE_DIR}/docs/kcmvp/KCMVP_UNRESOLVED_DECISION_REGISTER.md" _decisions)
string(REGEX MATCHALL "\\| DEC-[0-9][0-9][0-9] \\|" _decision_rows "${_decisions}")
list(LENGTH _decision_rows _decision_count)
if(NOT _decision_count EQUAL 20)
  list(APPEND _violations "decision-register count drifted: ${_decision_count}")
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
if(NOT _header_count EQUAL ANO_CRYPTO_P2_CLOSEOUT_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_P2_CLOSEOUT_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL ANO_CRYPTO_P2_CLOSEOUT_ABI_LINE)
  list(APPEND _violations "public ABI changed: headers=${_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

file(WRITE "${_output}"
  "AnoCrypto-C P2-09 Validation-readiness Closeout Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_P2_CLOSEOUT_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_P2_CLOSEOUT_STATUS}\n"
  "Engineering version: ${ANO_CRYPTO_RELEASE_VERSION}\n")
if(_violations)
  file(APPEND "${_output}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${_output}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "P2-09 closeout policy check failed")
endif()
file(APPEND "${_output}"
  "Result: PASS\n"
  "Sequence: ${ANO_CRYPTO_P2_CLOSEOUT_SEQUENCE_START} through ${ANO_CRYPTO_P2_CLOSEOUT_SEQUENCE_END}\n"
  "Priority-1 agendas: ${_agenda_count}\n"
  "Unresolved decisions: ${_decision_count}\n"
  "Package allowlist documents: ${_package_doc_count}\n"
  "External consultation/responses: ${ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_CONSULTATION}/${ANO_CRYPTO_P2_CLOSEOUT_INSTITUTION_RESPONSES}\n"
  "Hosted status/review: ${ANO_CRYPTO_P2_CLOSEOUT_HOSTED_STATUS}/${ANO_CRYPTO_P2_CLOSEOUT_HOSTED_REVIEW}\n"
  "Signing/handoff: ${ANO_CRYPTO_P2_CLOSEOUT_REPOSITORY_SIGNING}/${ANO_CRYPTO_P2_CLOSEOUT_SIGNING_HANDOFF}\n"
  "Submission/release/next implementation authorization: ${ANO_CRYPTO_P2_CLOSEOUT_SUBMISSION_AUTHORIZATION}/${ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_AUTHORIZATION}/${ANO_CRYPTO_P2_CLOSEOUT_NEXT_IMPLEMENTATION_AUTHORIZATION}\n"
  "Public ABI: headers=${_header_count}, symbols=${_symbol_count}, line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
message(STATUS "P2-09 closeout policy check passed")
