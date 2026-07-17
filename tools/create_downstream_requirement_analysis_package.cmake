cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_DRA_OUTPUT_DIR)
  set(ANO_CRYPTO_DRA_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/downstream-requirement-analysis-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_DRA_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_ANALYSIS_MANIFEST.txt" _lines)
set(_staged "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe downstream requirement analysis path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid downstream requirement analysis source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL ANO_CRYPTO_DRA_DOCUMENT_COUNT)
  message(FATAL_ERROR "downstream requirement analysis package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/DOWNSTREAM_REQUIREMENT_ANALYSIS_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/DOWNSTREAM_REQUIREMENT_ANALYSIS_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P3-01 downstream requirement analysis package\n"
  "policy_id=${ANO_CRYPTO_DRA_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_DRA_PACKAGE_CLASS}\n"
  "readiness_status=${ANO_CRYPTO_DRA_STATUS}\n"
  "imported_source_sets=${ANO_CRYPTO_DRA_IMPORTED_SOURCE_SET_COUNT}\n"
  "source_identity_conflicts=${ANO_CRYPTO_DRA_SOURCE_IDENTITY_CONFLICT_COUNT}\n"
  "accepted_requirements=${ANO_CRYPTO_DRA_ACCEPTED_REQUIREMENT_COUNT}\n"
  "approved_gaps=${ANO_CRYPTO_DRA_APPROVED_GAP_COUNT}\n"
  "selected_implementation=${ANO_CRYPTO_DRA_SELECTED_IMPLEMENTATION}\n"
  "public_api_change_authorization=${ANO_CRYPTO_DRA_PUBLIC_API_CHANGE_AUTHORIZATION}\n"
  "new_abi_line_authorization=${ANO_CRYPTO_DRA_NEW_ABI_LINE_AUTHORIZATION}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "documents=${_count}\n"
  "external_authorization=${ANO_CRYPTO_DRA_EXTERNAL_AUTHORIZATION}\n")
message(STATUS "Created P3-01 downstream requirement analysis package at ${_out}")
