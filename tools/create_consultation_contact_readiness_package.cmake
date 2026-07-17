cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_CCR_OUTPUT_DIR)
  set(ANO_CRYPTO_CCR_OUTPUT_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/consultation-contact-readiness-package")
endif()
get_filename_component(_out "${ANO_CRYPTO_CCR_OUTPUT_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${_out}")
file(MAKE_DIRECTORY "${_out}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_POLICY.cmake")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_CONSULTATION_CONTACT_READINESS_MANIFEST.txt" _lines)
set(_staged "")
foreach(_line IN LISTS _lines)
  string(STRIP "${_line}" _rel)
  if(_rel STREQUAL "" OR _rel MATCHES "^#")
    continue()
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR IS_ABSOLUTE "${_rel}" OR
     _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe consultation contact-readiness path: ${_rel}")
  endif()
  set(_src "${ANO_CRYPTO_SOURCE_DIR}/${_rel}")
  if(NOT EXISTS "${_src}" OR IS_DIRECTORY "${_src}" OR IS_SYMLINK "${_src}")
    message(FATAL_ERROR "invalid consultation contact-readiness source: ${_rel}")
  endif()
  file(READ "${_src}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\r\n])ACCESS_TOKEN[ ]*=|(^|[\r\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in consultation contact-readiness source: ${_rel}")
  endif()
  get_filename_component(_dir "${_out}/${_rel}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dir}")
  file(COPY_FILE "${_src}" "${_out}/${_rel}" ONLY_IF_DIFFERENT)
  list(APPEND _staged "${_rel}")
endforeach()
list(LENGTH _staged _count)
if(NOT _count EQUAL ANO_CRYPTO_CCR_DOCUMENT_COUNT)
  message(FATAL_ERROR "consultation contact-readiness package count drifted: ${_count}")
endif()
list(SORT _staged)
file(WRITE "${_out}/CONSULTATION_CONTACT_READINESS_MANIFEST.sha256" "")
foreach(_rel IN LISTS _staged)
  file(SHA256 "${_out}/${_rel}" _hash)
  file(APPEND "${_out}/CONSULTATION_CONTACT_READINESS_MANIFEST.sha256" "${_hash}  ${_rel}\n")
endforeach()
file(WRITE "${_out}/PACKAGE_STATUS.txt"
  "AnoCrypto-C P2-12 consultation contact-readiness package\n"
  "policy_id=${ANO_CRYPTO_CCR_POLICY_ID}\n"
  "engineering_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
  "package_class=${ANO_CRYPTO_CCR_PACKAGE_CLASS}\n"
  "readiness_status=${ANO_CRYPTO_CCR_STATUS}\n"
  "independent_human_rehearsal=${ANO_CRYPTO_CCR_INDEPENDENT_HUMAN_REHEARSAL}\n"
  "project_owner_authorization=${ANO_CRYPTO_CCR_PROJECT_OWNER_AUTHORIZATION}\n"
  "selected_institution=${ANO_CRYPTO_CCR_SELECTED_INSTITUTION}\n"
  "authorized_sender=${ANO_CRYPTO_CCR_AUTHORIZED_SENDER}\n"
  "contact_channel=${ANO_CRYPTO_CCR_CONTACT_CHANNEL}\n"
  "first_contact=${ANO_CRYPTO_CCR_FIRST_CONTACT}\n"
  "external_consultation=${ANO_CRYPTO_CCR_EXTERNAL_CONSULTATION}\n"
  "institution_responses=${ANO_CRYPTO_CCR_INSTITUTION_RESPONSES}\n"
  "formal_validation=${ANO_CRYPTO_CCR_FORMAL_VALIDATION}\n"
  "hosted_status=${ANO_CRYPTO_CCR_HOSTED_STATUS}\n"
  "hosted_review=${ANO_CRYPTO_CCR_HOSTED_REVIEW}\n"
  "repository_signing=${ANO_CRYPTO_CCR_REPOSITORY_SIGNING}\n"
  "signing_handoff=${ANO_CRYPTO_CCR_SIGNING_HANDOFF}\n"
  "documents=${_count}\n"
  "contains_source_or_binary=NO\n"
  "contains_sensitive_material=NO\n"
  "submission_authorization=${ANO_CRYPTO_CCR_SUBMISSION_AUTHORIZATION}\n"
  "external_authorization=${ANO_CRYPTO_CCR_EXTERNAL_AUTHORIZATION}\n"
  "next_implementation_authorization=${ANO_CRYPTO_CCR_NEXT_IMPLEMENTATION_AUTHORIZATION}\n")
message(STATUS "Created P2-12 consultation contact-readiness package at ${_out}")
