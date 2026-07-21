cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_SUBMISSION_DOCUMENT_CHECK_OUTPUT OR ANO_CRYPTO_SUBMISSION_DOCUMENT_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_SUBMISSION_DOCUMENT_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/SUBMISSION_DOCUMENT_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_SUBMISSION_DOCUMENT_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
set(_violations "")

function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()
function(require_text rel needle)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" txt)
    string(FIND "${txt}" "${needle}" pos)
    if(pos EQUAL -1)
      list(APPEND _violations "${rel} missing marker: ${needle}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SUBMISSION_DOCUMENT_POLICY.cmake")

if(NOT ANO_CRYPTO_SUBMISSION_POLICY_ID STREQUAL "SDP-P2-07-R1" OR
   NOT ANO_CRYPTO_SUBMISSION_POLICY_STATUS STREQUAL "CONTROLLED_INTERNAL_SKELETON_COMPLETE" OR
   NOT ANO_CRYPTO_SUBMISSION_FORMAL_TEMPLATE_CONFORMITY STREQUAL "EXTERNAL_CONFIRMATION_REQUIRED" OR
   NOT ANO_CRYPTO_SUBMISSION_SECURITY_LEVEL STREQUAL "UNSELECTED" OR
   NOT ANO_CRYPTO_SUBMISSION_INSTITUTION_RESULTS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SUBMISSION_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_SUBMISSION_RUNTIME_INTEGRITY STREQUAL "NOT_IMPLEMENTED" OR
   NOT ANO_CRYPTO_SUBMISSION_SELECTED_OE_EXECUTION STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SUBMISSION_CANDIDATE_ID STREQUAL ANO_CRYPTO_VALIDATION_CANDIDATE_ID)
  list(APPEND _violations "frozen P2-07 submission-document policy values drifted")
endif()

set(_docs
  docs/kcmvp/submission/KCMVP_SUBMISSION_PACKAGE_INDEX.md
  docs/kcmvp/submission/KCMVP_DOCUMENT_CONTROL_REGISTER.md
  docs/kcmvp/submission/KCMVP_MODULE_BASIC_DESIGN_SKELETON.md
  docs/kcmvp/submission/KCMVP_MODULE_DETAILED_DESIGN_SKELETON.md
  docs/kcmvp/submission/KCMVP_CONFIGURATION_MANAGEMENT_PLAN_SKELETON.md
  docs/kcmvp/submission/KCMVP_TEST_DOCUMENT_SET_SKELETON.md
  docs/kcmvp/submission/KCMVP_INSTALLATION_OPERATOR_GUIDE_SKELETON.md
  docs/kcmvp/submission/KCMVP_REQUIREMENT_TRACEABILITY_APPENDIX.md
  docs/kcmvp/submission/KCMVP_CONTROLLED_REFERENCE_REGISTER.md
  docs/kcmvp/submission/KCMVP_SUBMISSION_STAGING_AND_HANDLING.md
  docs/kcmvp/submission/KCMVP_SUBMISSION_REVIEW_CHECKLIST.md)
list(LENGTH _docs _doc_count)
if(NOT _doc_count EQUAL ANO_CRYPTO_SUBMISSION_CONTROLLED_DOCUMENT_COUNT)
  list(APPEND _violations "controlled document count drifted: ${_doc_count}")
endif()
foreach(doc IN LISTS _docs)
  require_file("${doc}")
  require_text("${doc}" "CONTROLLED_INTERNAL_SKELETON")
  require_text("${doc}" "Approval")
endforeach()

require_file("docs/kcmvp/submission/README.md")
require_file("release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt")
require_file("release/ANOCRYPTO_C_SUBMISSION_DOCUMENT_POLICY.cmake")
require_file("docs/kcmvp/P2_07_IMPLEMENTATION_TEST_DELTA.md")
require_file("tools/create_submission_skeleton_package.cmake")
require_file("tools/check_submission_skeleton_package.cmake")
require_file("tools/run_submission_skeleton_package_smoke.cmake")
require_text("docs/kcmvp/submission/README.md" "SDP-P2-07-R1")
require_text("docs/kcmvp/submission/KCMVP_SUBMISSION_PACKAGE_INDEX.md" "P1-34 actual hosted evidence: `BLOCKED_EXTERNAL`")
require_text("docs/kcmvp/submission/KCMVP_MODULE_BASIC_DESIGN_SKELETON.md" "Runtime integrity verification remains `NOT_IMPLEMENTED`")
require_text("docs/kcmvp/submission/KCMVP_TEST_DOCUMENT_SET_SKELETON.md" "Institution test result tables remain blank")
require_text("docs/kcmvp/submission/KCMVP_SUBMISSION_STAGING_AND_HANDLING.md" "submission/external release authorization: `NO`")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-09-ecdsa-p256-sha256")

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(line IN LISTS _manifest_lines)
  string(STRIP "${line}" line)
  if(line STREQUAL "" OR line MATCHES "^#")
    continue()
  endif()
  math(EXPR _manifest_count "${_manifest_count}+1")
  list(FIND _docs "${line}" idx)
  if(idx EQUAL -1)
    list(APPEND _violations "unexpected staging-manifest path: ${line}")
  endif()
  if(line MATCHES "(^|/)(src|tests|evidence|build)/" OR line MATCHES "[.](c|h|o|a|so|dll|dylib|exe)$")
    list(APPEND _violations "non-document artifact in S0 staging manifest: ${line}")
  endif()
endforeach()
if(NOT _manifest_count EQUAL _doc_count)
  list(APPEND _violations "staging manifest count ${_manifest_count} does not equal document count ${_doc_count}")
endif()

set(_public_headers "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
set(_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(line IN LISTS _symbols)
  string(STRIP "${line}" line)
  if(NOT line STREQUAL "" AND NOT line MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count}+1")
  endif()
endforeach()
if(NOT _header_count EQUAL 14 OR NOT _symbol_count EQUAL 69 OR NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_6")
  list(APPEND _violations "public ABI changed: headers=${_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

foreach(doc IN LISTS _docs)
  file(READ "${ANO_CRYPTO_SOURCE_DIR}/${doc}" text)
  if(text MATCHES "formal validation completed|institution test passed|production release authorized|universal constant-time")
    # Allowed only when negated/prohibited; reject unsafe affirmative phrases outside checklist prose.
    if(NOT text MATCHES "No formal validation|Do not|prohibited|must never|remain blank")
      list(APPEND _violations "potential prohibited claim in ${doc}")
    endif()
  endif()
endforeach()

file(WRITE "${_output}"
  "AnoCrypto-C P2-07 Submission Document Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_SUBMISSION_POLICY_ID}\n"
  "Policy status: ${ANO_CRYPTO_SUBMISSION_POLICY_STATUS}\n"
  "Formal template conformity: ${ANO_CRYPTO_SUBMISSION_FORMAL_TEMPLATE_CONFORMITY}\n"
  "Controlled documents: ${_doc_count}\n")
if(_violations)
  file(APPEND "${_output}" "Result: FAIL\n")
  foreach(v IN LISTS _violations)
    file(APPEND "${_output}" "- ${v}\n")
  endforeach()
  message(FATAL_ERROR "P2-07 submission-document policy check failed")
endif()
file(APPEND "${_output}"
  "Result: PASS\n"
  "Candidate ID: ${ANO_CRYPTO_SUBMISSION_CANDIDATE_ID}\n"
  "Security level: ${ANO_CRYPTO_SUBMISSION_SECURITY_LEVEL}\n"
  "Institution results: ${ANO_CRYPTO_SUBMISSION_INSTITUTION_RESULTS}\n"
  "Runtime integrity: ${ANO_CRYPTO_SUBMISSION_RUNTIME_INTEGRITY}\n"
  "Hosted status: ${ANO_CRYPTO_SUBMISSION_HOSTED_STATUS}\n"
  "Hosted review: ${ANO_CRYPTO_SUBMISSION_HOSTED_REVIEW}\n"
  "Repository signing: ${ANO_CRYPTO_SUBMISSION_REPOSITORY_SIGNING}\n"
  "Signing handoff: ${ANO_CRYPTO_SUBMISSION_SIGNING_HANDOFF}\n"
  "External authorization: ${ANO_CRYPTO_SUBMISSION_EXTERNAL_AUTHORIZATION}\n"
  "Public ABI: headers=${_header_count}, symbols=${_symbol_count}, line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
message(STATUS "P2-07 submission-document policy check passed")
