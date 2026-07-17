cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_DRI_CHECK_OUTPUT OR ANO_CRYPTO_DRI_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_DRI_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/DOWNSTREAM_REQUIREMENT_INTAKE_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_DRI_CHECK_OUTPUT}" ABSOLUTE)
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_POLICY.cmake")
if(NOT ANO_CRYPTO_DRI_POLICY_ID STREQUAL "DRI-P3-00-R1" OR
   NOT ANO_CRYPTO_DRI_STATUS STREQUAL "READY_TO_COLLECT_MATCHED_DOWNSTREAM_SOURCES" OR
   NOT ANO_CRYPTO_DRI_PACKAGE_CLASS STREQUAL "STATUS_ONLY_DOWNSTREAM_REQUIREMENT_INTAKE" OR
   NOT ANO_CRYPTO_DRI_DOCUMENT_COUNT EQUAL 15 OR
   NOT ANO_CRYPTO_DRI_SOURCE_SET_COUNT EQUAL 4 OR
   NOT ANO_CRYPTO_DRI_IMPORTED_SOURCE_SET_COUNT EQUAL 0 OR
   NOT ANO_CRYPTO_DRI_ACCEPTED_REQUIREMENT_COUNT EQUAL 0 OR
   NOT ANO_CRYPTO_DRI_APPROVED_GAP_COUNT EQUAL 0 OR
   NOT ANO_CRYPTO_DRI_SELECTED_IMPLEMENTATION STREQUAL "NONE" OR
   NOT ANO_CRYPTO_DRI_PUBLIC_API_CHANGE_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_DRI_NEW_ABI_LINE_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_DRI_KCMVP_ACTIVE_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_DRI_EXTERNAL_CONSULTATION STREQUAL "NOT_CONDUCTED" OR
   NOT ANO_CRYPTO_DRI_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_DRI_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_DRI_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_DRI_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P3-00 downstream intake values drifted")
endif()
foreach(_rel IN ITEMS
  docs/product/DOWNSTREAM_REQUIREMENT_INTAKE.md
  docs/product/CROSS_REPOSITORY_REQUIREMENT_OWNERSHIP.md
  docs/product/PRODUCT_OPERATION_CAPABILITY_MATRIX.md
  docs/product/ANOCRYPTO_C_CAPABILITY_GAP_REGISTER.md
  docs/product/DOWNSTREAM_SOURCE_REQUESTS.md
  docs/product/PROJECT_SOURCE_REPLACEMENT_PLAN.md
  docs/product/P3_00_IMPLEMENTATION_TEST_DELTA.md
  docs/roadmap/CURRENT_ACTIVE_NEXT.md
  docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md
  docs/design/P3_ABI_EVOLUTION_GATE.md
  docs/kcmvp/KCMVP_DEFERRED_PRODUCT_POSITIONING.md
  release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_POLICY.cmake
  release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_MANIFEST.txt)
  require_file("${_rel}")
endforeach()
require_text("docs/product/DOWNSTREAM_REQUIREMENT_INTAKE.md" "No algorithm or public API is approved")
require_text("docs/product/PRODUCT_OPERATION_CAPABILITY_MATRIX.md" "WAITING_FOR_SOURCE")
require_text("docs/product/ANOCRYPTO_C_CAPABILITY_GAP_REGISTER.md" "NO_PRODUCT_GAP_APPROVED")
require_text("docs/roadmap/CURRENT_ACTIVE_NEXT.md" "No implementation item is selected yet")
require_text("docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md" "P3_00_INTAKE_ONLY")
require_text("docs/design/P3_ABI_EVOLUTION_GATE.md" "NO_CHANGE_APPROVED")
require_text("docs/kcmvp/KCMVP_DEFERRED_PRODUCT_POSITIONING.md" "DEFERRED_GOVERNANCE_TRACK")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "BLOCKED_EXTERNAL")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "NOT_RUN")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "UNREVIEWED")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-00-downstream-requirement-intake")
require_text("README.md" "0.1.0-p3-00-downstream-requirement-intake")
require_text("release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake" "ANOCRYPTO_C_0")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  math(EXPR _manifest_count "${_manifest_count}+1")
  require_file("${_rel}")
endforeach()
if(NOT _manifest_count EQUAL ANO_CRYPTO_DRI_DOCUMENT_COUNT)
  list(APPEND _violations "P3-00 downstream intake manifest count drifted: ${_manifest_count}")
endif()
file(GLOB _headers RELATIVE "${ANO_CRYPTO_SOURCE_DIR}" "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(LENGTH _headers _header_count)
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _s)
  if(NOT _s STREQUAL "" AND NOT _s MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count}+1")
  endif()
endforeach()
if(NOT _header_count EQUAL ANO_CRYPTO_DRI_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_DRI_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL ANO_CRYPTO_DRI_ABI_LINE)
  list(APPEND _violations "public ABI baseline drifted: headers=${_header_count}, symbols=${_symbol_count}, abi=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()
string(JOIN "\n- " _violations_text ${_violations})
file(WRITE "${_output}"
  "AnoCrypto-C P3-00 Downstream Requirement Intake Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_DRI_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_DRI_STATUS}\n"
  "Source sets imported/total: ${ANO_CRYPTO_DRI_IMPORTED_SOURCE_SET_COUNT}/${ANO_CRYPTO_DRI_SOURCE_SET_COUNT}\n"
  "Accepted requirements/approved gaps: ${ANO_CRYPTO_DRI_ACCEPTED_REQUIREMENT_COUNT}/${ANO_CRYPTO_DRI_APPROVED_GAP_COUNT}\n"
  "Selected implementation/API/ABI authorization: ${ANO_CRYPTO_DRI_SELECTED_IMPLEMENTATION}/${ANO_CRYPTO_DRI_PUBLIC_API_CHANGE_AUTHORIZATION}/${ANO_CRYPTO_DRI_NEW_ABI_LINE_AUTHORIZATION}\n"
  "KCMVP/consultation: ${ANO_CRYPTO_DRI_KCMVP_ACTIVE_LANE}/${ANO_CRYPTO_DRI_EXTERNAL_CONSULTATION}\n"
  "Hosted/signing/release: ${ANO_CRYPTO_DRI_HOSTED_STATUS}/${ANO_CRYPTO_DRI_REPOSITORY_SIGNING}/${ANO_CRYPTO_DRI_EXTERNAL_AUTHORIZATION}\n"
  "Public ABI: ${_header_count} headers, ${_symbol_count} symbols, ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
  "Violations: ${_violations_text}\n")
if(_violations)
  message(FATAL_ERROR "P3-00 downstream requirement intake policy check failed")
endif()
message(STATUS "P3-00 downstream requirement intake policy check passed")
