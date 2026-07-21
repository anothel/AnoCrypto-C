cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_DRA_CHECK_OUTPUT OR ANO_CRYPTO_DRA_CHECK_OUTPUT STREQUAL "")
  set(ANO_CRYPTO_DRA_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_DRA_CHECK_OUTPUT}" ABSOLUTE)
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
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY.cmake")
if(NOT ANO_CRYPTO_DRA_POLICY_ID STREQUAL "DRA-P3-01-R1" OR
   NOT ANO_CRYPTO_DRA_STATUS STREQUAL "DOWNSTREAM_REQUIREMENTS_CONSOLIDATED" OR
   NOT ANO_CRYPTO_DRA_PACKAGE_CLASS STREQUAL "STATUS_ONLY_DOWNSTREAM_REQUIREMENT_ANALYSIS" OR
   NOT ANO_CRYPTO_DRA_DOCUMENT_COUNT EQUAL 14 OR
   NOT ANO_CRYPTO_DRA_SOURCE_SET_COUNT EQUAL 4 OR
   NOT ANO_CRYPTO_DRA_IMPORTED_SOURCE_SET_COUNT EQUAL 4 OR
   NOT ANO_CRYPTO_DRA_SOURCE_IDENTITY_CONFLICT_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_DRA_ACCEPTED_REQUIREMENT_COUNT EQUAL 20 OR
   NOT ANO_CRYPTO_DRA_APPROVED_GAP_COUNT EQUAL 8 OR
   NOT ANO_CRYPTO_DRA_SELECTED_IMPLEMENTATION STREQUAL "P3-02_HKDF_SHA256" OR
   NOT ANO_CRYPTO_DRA_PUBLIC_API_CHANGE_AUTHORIZATION STREQUAL "P3_02_ONLY" OR
   NOT ANO_CRYPTO_DRA_NEW_ABI_LINE_AUTHORIZATION STREQUAL "ANOCRYPTO_C_1_ADDITIVE_FOR_P3_02" OR
   NOT ANO_CRYPTO_DRA_KCMVP_ACTIVE_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_DRA_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_DRA_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_DRA_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_DRA_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "frozen P3-01 analysis values drifted")
endif()
foreach(_rel IN ITEMS
  docs/product/DOWNSTREAM_SOURCE_BASELINES.md
  docs/product/DOWNSTREAM_SOURCE_BASELINES.json
  docs/product/DOWNSTREAM_SOURCE_CONFLICTS.md
  docs/product/DOWNSTREAM_REQUIREMENT_ANALYSIS_P3_01.md
  docs/product/PRODUCT_OPERATION_CAPABILITY_MATRIX.md
  docs/product/CROSS_REPOSITORY_REQUIREMENT_OWNERSHIP.md
  docs/product/ANOCRYPTO_C_CAPABILITY_GAP_REGISTER.md
  docs/product/ANOSECUREKIT_REQUIREMENT_TRACE.md
  docs/product/ANOPKI_REQUIREMENT_TRACE.md
  docs/product/P3_01_IMPLEMENTATION_TEST_DELTA.md
  docs/roadmap/CURRENT_ACTIVE_NEXT.md
  docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md
  docs/design/P3_ABI_EVOLUTION_GATE.md
  release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY.cmake
  release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_MANIFEST.txt)
  require_file("${_rel}")
endforeach()
require_text("docs/product/DOWNSTREAM_SOURCE_BASELINES.md" "4_OF_4_IMPORTED_WITH_ONE_IDENTITY_CONFLICT")
require_text("docs/product/DOWNSTREAM_SOURCE_CONFLICTS.md" "SRC-CONFLICT-ASK-001")
require_text("docs/product/PRODUCT_OPERATION_CAPABILITY_MATRIX.md" "accepted product/contract rows: 20")
require_text("docs/product/ANOCRYPTO_C_CAPABILITY_GAP_REGISTER.md" "AC-GAP-001")
require_text("docs/product/ANOCRYPTO_C_CAPABILITY_GAP_REGISTER.md" "8_GAPS_APPROVED_HKDF_SATISFIED")
require_text("docs/product/CROSS_REPOSITORY_REQUIREMENT_OWNERSHIP.md" "AnoPKI retains all codec")
require_text("docs/roadmap/CURRENT_ACTIVE_NEXT.md" "P3-02 minimal HKDF-SHA-256 API")
require_text("docs/design/P3_ABI_EVOLUTION_GATE.md" "ANOCRYPTO_C_1")
require_text("docs/kcmvp/KCMVP_DEFERRED_PRODUCT_POSITIONING.md" "DEFERRED_GOVERNANCE_TRACK_PRODUCT_ROADMAP_ACTIVE")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "BLOCKED_EXTERNAL")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-08-rsa-pkcs1-v15-sha2")
require_text("README.md" "0.1.0-p3-08-rsa-pkcs1-v15-sha2")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_MANIFEST.txt" _manifest_lines)
set(_manifest_count 0)
foreach(_line IN LISTS _manifest_lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  math(EXPR _manifest_count "${_manifest_count}+1")
  require_file("${_rel}")
endforeach()
if(NOT _manifest_count EQUAL ANO_CRYPTO_DRA_DOCUMENT_COUNT)
  list(APPEND _violations "P3-01 analysis manifest count drifted: ${_manifest_count}")
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
if(NOT _header_count EQUAL ANO_CRYPTO_DRA_PUBLIC_HEADER_COUNT OR
   NOT _symbol_count EQUAL ANO_CRYPTO_DRA_EXPORTED_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_DRA_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_0")
  list(APPEND _violations "public ABI baseline drifted: headers=${_header_count}, symbols=${_symbol_count}, abi=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()
string(JOIN "\n- " _violations_text ${_violations})
file(WRITE "${_output}"
  "AnoCrypto-C P3-01 Downstream Requirement Analysis Policy Check\n"
  "Policy ID: ${ANO_CRYPTO_DRA_POLICY_ID}\n"
  "Status: ${ANO_CRYPTO_DRA_STATUS}\n"
  "Source sets/conflicts: ${ANO_CRYPTO_DRA_IMPORTED_SOURCE_SET_COUNT}/${ANO_CRYPTO_DRA_SOURCE_IDENTITY_CONFLICT_COUNT}\n"
  "Requirements/gaps: ${ANO_CRYPTO_DRA_ACCEPTED_REQUIREMENT_COUNT}/${ANO_CRYPTO_DRA_APPROVED_GAP_COUNT}\n"
  "Selected/API/ABI: ${ANO_CRYPTO_DRA_SELECTED_IMPLEMENTATION}/${ANO_CRYPTO_DRA_PUBLIC_API_CHANGE_AUTHORIZATION}/${ANO_CRYPTO_DRA_NEW_ABI_LINE_AUTHORIZATION}\n"
  "Hosted/signing/release: ${ANO_CRYPTO_DRA_HOSTED_STATUS}/${ANO_CRYPTO_DRA_REPOSITORY_SIGNING}/${ANO_CRYPTO_DRA_EXTERNAL_AUTHORIZATION}\n"
  "Public ABI: ${_header_count} headers, ${_symbol_count} symbols, ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
  "Violations: ${_violations_text}\n")
if(_violations)
  message(FATAL_ERROR "P3-01 downstream requirement analysis policy check failed")
endif()
message(STATUS "P3-01 downstream requirement analysis policy check passed")
