cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_PRECONSULTATION_PACKAGE_OUTPUT_DIR)
  set(ANO_CRYPTO_PRECONSULTATION_PACKAGE_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/preconsultation-rehearsal-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_PRECONSULTATION_PACKAGE_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_PRECONSULTATION_POLICY.cmake")
set(_staged "")
foreach(_rel IN LISTS ANO_CRYPTO_PRECONSULTATION_PACKAGE_FILES)
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx)$")
    message(FATAL_ERROR "unsafe pre-consultation package path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid pre-consultation package source: ${_rel}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\\r\\n])ACCESS_TOKEN[ ]*=|(^|[\\r\\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in pre-consultation source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL 9)
  message(FATAL_ERROR "pre-consultation package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/PRECONSULTATION_REHEARSAL_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/PRECONSULTATION_REHEARSAL_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P2-08 internal pre-consultation rehearsal package\n"
  "policy_id=${ANO_CRYPTO_PRECONSULTATION_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_PRECONSULTATION_PACKAGE_CLASS}\n"
  "scripted_rehearsal=${ANO_CRYPTO_PRECONSULTATION_REHEARSAL_STATUS}\n"
  "live_internal_rehearsal=${ANO_CRYPTO_PRECONSULTATION_LIVE_INTERNAL_REHEARSAL}\n"
  "external_consultation=${ANO_CRYPTO_PRECONSULTATION_EXTERNAL_CONSULTATION}\n"
  "institution_responses=${ANO_CRYPTO_PRECONSULTATION_INSTITUTION_RESPONSES}\n"
  "documents=${_count}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "submission_authorization=${ANO_CRYPTO_PRECONSULTATION_SUBMISSION_AUTHORIZATION}\n"
  "external_authorization=${ANO_CRYPTO_PRECONSULTATION_EXTERNAL_AUTHORIZATION}\n")
message(STATUS "Created P2-08 internal pre-consultation rehearsal package at ${_out}")
