cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_SUBMISSION_SKELETON_OUTPUT_DIR)
  set(ANO_CRYPTO_SUBMISSION_SKELETON_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/submission-skeleton-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_SUBMISSION_SKELETON_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SUBMISSION_DOCUMENT_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt" _lines)
set(_files "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _line)
  if(_line STREQUAL "" OR _line MATCHES "^#")
    continue()
  endif()
  if(_line MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_line}")
    message(FATAL_ERROR "unsafe staging path: ${_line}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_line}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid staging source: ${_line}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "BEGIN ([A-Z0-9 ]+ )?PRIVATE KEY|PRIVATE_KEY=|ACCESS_TOKEN=|SECRET_KEY=")
    message(FATAL_ERROR "sensitive marker in staging source: ${_line}")
  endif()
  get_filename_component(_dir "${_out}/${_line}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_line}" ONLY_IF_DIFFERENT)
  list(APPEND _files "${_line}")
endforeach()
list(LENGTH _files _count)
if(NOT _count EQUAL ANO_CRYPTO_SUBMISSION_CONTROLLED_DOCUMENT_COUNT)
  message(FATAL_ERROR "staged document count drifted: ${_count}")
endif()
list(SORT _files)
file(WRITE "${_out}/SUBMISSION_SKELETON_MANIFEST.sha256" "")
foreach(_rel IN LISTS _files)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/SUBMISSION_SKELETON_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P2-07 internal submission skeleton\n"
  "policy_id=${ANO_CRYPTO_SUBMISSION_POLICY_ID}\n"
  "policy_status=${ANO_CRYPTO_SUBMISSION_POLICY_STATUS}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "staging_level=${ANO_CRYPTO_SUBMISSION_STAGING_LEVEL}\n"
  "controlled_documents=${_count}\n"
  "formal_template_conformity=${ANO_CRYPTO_SUBMISSION_FORMAL_TEMPLATE_CONFORMITY}\n"
  "institution_results=${ANO_CRYPTO_SUBMISSION_INSTITUTION_RESULTS}\n"
  "external_authorization=${ANO_CRYPTO_SUBMISSION_EXTERNAL_AUTHORIZATION}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n")
message(STATUS "Created P2-07 internal submission skeleton package at ${_out}")
