cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_DRI_OUTPUT_DIR)
  set(ANO_CRYPTO_DRI_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/downstream-requirement-intake-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_DRI_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_DOWNSTREAM_REQUIREMENT_INTAKE_MANIFEST.txt" _lines)
set(_staged "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe downstream requirement intake path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid downstream requirement intake source: ${_rel}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\r\n])ACCESS_TOKEN[ ]*=|(^|[\r\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in downstream requirement intake source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL ANO_CRYPTO_DRI_DOCUMENT_COUNT)
  message(FATAL_ERROR "downstream requirement intake package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/DOWNSTREAM_REQUIREMENT_INTAKE_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/DOWNSTREAM_REQUIREMENT_INTAKE_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P3-00 downstream requirement intake package\n"
  "policy_id=${ANO_CRYPTO_DRI_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_DRI_PACKAGE_CLASS}\n"
  "readiness_status=${ANO_CRYPTO_DRI_STATUS}\n"
  "source_sets=${ANO_CRYPTO_DRI_SOURCE_SET_COUNT}\n"
  "imported_source_sets=${ANO_CRYPTO_DRI_IMPORTED_SOURCE_SET_COUNT}\n"
  "accepted_requirements=${ANO_CRYPTO_DRI_ACCEPTED_REQUIREMENT_COUNT}\n"
  "approved_gaps=${ANO_CRYPTO_DRI_APPROVED_GAP_COUNT}\n"
  "selected_implementation=${ANO_CRYPTO_DRI_SELECTED_IMPLEMENTATION}\n"
  "public_api_change_authorization=${ANO_CRYPTO_DRI_PUBLIC_API_CHANGE_AUTHORIZATION}\n"
  "new_abi_line_authorization=${ANO_CRYPTO_DRI_NEW_ABI_LINE_AUTHORIZATION}\n"
  "kcmvp_active_lane=${ANO_CRYPTO_DRI_KCMVP_ACTIVE_LANE}\n"
  "external_consultation=${ANO_CRYPTO_DRI_EXTERNAL_CONSULTATION}\n"
  "hosted_status=${ANO_CRYPTO_DRI_HOSTED_STATUS}\n"
  "hosted_review=${ANO_CRYPTO_DRI_HOSTED_REVIEW}\n"
  "repository_signing=${ANO_CRYPTO_DRI_REPOSITORY_SIGNING}\n"
  "documents=${_count}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "external_authorization=${ANO_CRYPTO_DRI_EXTERNAL_AUTHORIZATION}\n")
message(STATUS "Created P3-00 downstream requirement intake package at ${_out}")
