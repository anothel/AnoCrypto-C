cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_OUTPUT_DIR)
  set(ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/preconsultation-closeout-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_P2_CLOSEOUT_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_PRECONSULTATION_CLOSEOUT_MANIFEST.txt" _lines)
set(_staged "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe closeout package path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid closeout package source: ${_rel}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\\r\\n])ACCESS_TOKEN[ ]*=|(^|[\\r\\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in closeout source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL 17)
  message(FATAL_ERROR "closeout package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/PRECONSULTATION_CLOSEOUT_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/PRECONSULTATION_CLOSEOUT_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P2-09 internal pre-consultation closeout package\n"
  "policy_id=${ANO_CRYPTO_P2_CLOSEOUT_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_P2_CLOSEOUT_PACKAGE_CLASS}\n"
  "closeout_status=${ANO_CRYPTO_P2_CLOSEOUT_STATUS}\n"
  "external_consultation=${ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_CONSULTATION}\n"
  "institution_responses=${ANO_CRYPTO_P2_CLOSEOUT_INSTITUTION_RESPONSES}\n"
  "formal_validation=${ANO_CRYPTO_P2_CLOSEOUT_FORMAL_VALIDATION}\n"
  "hosted_status=${ANO_CRYPTO_P2_CLOSEOUT_HOSTED_STATUS}\n"
  "hosted_review=${ANO_CRYPTO_P2_CLOSEOUT_HOSTED_REVIEW}\n"
  "repository_signing=${ANO_CRYPTO_P2_CLOSEOUT_REPOSITORY_SIGNING}\n"
  "signing_handoff=${ANO_CRYPTO_P2_CLOSEOUT_SIGNING_HANDOFF}\n"
  "documents=${_count}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "submission_authorization=${ANO_CRYPTO_P2_CLOSEOUT_SUBMISSION_AUTHORIZATION}\n"
  "external_authorization=${ANO_CRYPTO_P2_CLOSEOUT_EXTERNAL_AUTHORIZATION}\n"
  "next_implementation_authorization=${ANO_CRYPTO_P2_CLOSEOUT_NEXT_IMPLEMENTATION_AUTHORIZATION}\n")
message(STATUS "Created P2-09 internal pre-consultation closeout package at ${_out}")
