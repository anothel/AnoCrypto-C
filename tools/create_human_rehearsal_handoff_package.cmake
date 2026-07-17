cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_HRH_OUTPUT_DIR)
  set(ANO_CRYPTO_HRH_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/human-rehearsal-handoff-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_HRH_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_MANIFEST.txt" _lines)
set(_staged "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe human rehearsal handoff path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid human rehearsal handoff source: ${_rel}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\r\n])ACCESS_TOKEN[ ]*=|(^|[\r\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in human rehearsal handoff source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL ANO_CRYPTO_HRH_DOCUMENT_COUNT)
  message(FATAL_ERROR "human rehearsal handoff package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/HUMAN_REHEARSAL_HANDOFF_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/HUMAN_REHEARSAL_HANDOFF_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P2-11 independent human rehearsal handoff package\n"
  "policy_id=${ANO_CRYPTO_HRH_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_HRH_PACKAGE_CLASS}\n"
  "handoff_status=${ANO_CRYPTO_HRH_STATUS}\n"
  "reviewer_identity=${ANO_CRYPTO_HRH_REVIEWER_IDENTITY}\n"
  "reviewer_independence=${ANO_CRYPTO_HRH_REVIEWER_INDEPENDENCE}\n"
  "independent_human_rehearsal=${ANO_CRYPTO_HRH_INDEPENDENT_HUMAN_REHEARSAL}\n"
  "project_owner_authorization=${ANO_CRYPTO_HRH_PROJECT_OWNER_AUTHORIZATION}\n"
  "contact_authorization=${ANO_CRYPTO_HRH_CONTACT_AUTHORIZATION}\n"
  "external_consultation=${ANO_CRYPTO_HRH_EXTERNAL_CONSULTATION}\n"
  "institution_responses=${ANO_CRYPTO_HRH_INSTITUTION_RESPONSES}\n"
  "formal_validation=${ANO_CRYPTO_HRH_FORMAL_VALIDATION}\n"
  "hosted_status=${ANO_CRYPTO_HRH_HOSTED_STATUS}\n"
  "hosted_review=${ANO_CRYPTO_HRH_HOSTED_REVIEW}\n"
  "repository_signing=${ANO_CRYPTO_HRH_REPOSITORY_SIGNING}\n"
  "signing_handoff=${ANO_CRYPTO_HRH_SIGNING_HANDOFF}\n"
  "documents=${_count}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "submission_authorization=${ANO_CRYPTO_HRH_SUBMISSION_AUTHORIZATION}\n"
  "external_authorization=${ANO_CRYPTO_HRH_EXTERNAL_AUTHORIZATION}\n"
  "next_implementation_authorization=${ANO_CRYPTO_HRH_NEXT_IMPLEMENTATION_AUTHORIZATION}\n")
message(STATUS "Created P2-11 human rehearsal handoff package at ${_out}")
