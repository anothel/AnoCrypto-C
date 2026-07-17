cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_DIR)
  message(FATAL_ERROR "ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_DIR is required")
endif()
get_filename_component(_root "${ANO_CRYPTO_PRECONSULTATION_CLOSEOUT_DIR}" ABSOLUTE)
set(_manifest "${_root}/PRECONSULTATION_CLOSEOUT_MANIFEST.sha256")
set(_status "${_root}/PACKAGE_STATUS.txt")
if(NOT EXISTS "${_manifest}" OR NOT EXISTS "${_status}")
  message(FATAL_ERROR "closeout manifest/status missing")
endif()
file(READ "${_status}" _status_text)
foreach(_marker IN ITEMS
  "policy_id=P2C-P2-09-R1"
  "package_class=STATUS_ONLY_PRECONSULTATION_CLOSEOUT"
  "closeout_status=COMPLETE_LOCAL_CLOSEOUT"
  "external_consultation=NOT_CONDUCTED"
  "institution_responses=NOT_RECEIVED"
  "formal_validation=NOT_STARTED"
  "hosted_status=NOT_RUN"
  "hosted_review=UNREVIEWED"
  "repository_signing=UNSIGNED"
  "signing_handoff=BLOCKED"
  "documents=17"
  "contains_source_or_binary=NO"
  "contains_sensitive_material=NO"
  "submission_authorization=NO"
  "external_authorization=NO"
  "next_implementation_authorization=NO")
  string(FIND "${_status_text}" "${_marker}" _pos)
  if(_pos EQUAL -1)
    message(FATAL_ERROR "closeout package status missing ${_marker}")
  endif()
endforeach()
file(STRINGS "${_manifest}" _lines)
set(_count 0)
foreach(_line IN LISTS _lines)
  string(LENGTH "${_line}" _len)
  if(_len LESS 67)
    message(FATAL_ERROR "malformed manifest line: ${_line}")
  endif()
  string(SUBSTRING "${_line}" 0 64 _expected)
  string(SUBSTRING "${_line}" 64 2 _separator)
  math(EXPR _path_len "${_len}-66")
  string(SUBSTRING "${_line}" 66 ${_path_len} _rel)
  if(NOT _expected MATCHES "^[0-9a-f]+$" OR NOT _separator STREQUAL "  " OR _rel STREQUAL "")
    message(FATAL_ERROR "malformed manifest line: ${_line}")
  endif()
  if(_rel MATCHES "(^|/)[.][.]($|/)" OR _rel MATCHES "[.](c|h|o|a|so|dll|dylib|exe|key|pem|p12|pfx|sig|tsr)$")
    message(FATAL_ERROR "unsafe artifact in closeout package: ${_rel}")
  endif()
  if(NOT EXISTS "${_root}/${_rel}" OR IS_DIRECTORY "${_root}/${_rel}" OR IS_SYMLINK "${_root}/${_rel}")
    message(FATAL_ERROR "manifest file missing/invalid: ${_rel}")
  endif()
  file(SHA256 "${_root}/${_rel}" _actual)
  if(NOT _actual STREQUAL _expected)
    message(FATAL_ERROR "manifest hash mismatch: ${_rel}")
  endif()
  file(READ "${_root}/${_rel}" _text)
  if(_text MATCHES "-----BEGIN ([A-Z0-9 ]+ )?PRIVATE[ ]KEY-----|(^|[\\r\\n])ACCESS_TOKEN[ ]*=|(^|[\\r\\n])SECRET_KEY[ ]*=")
    message(FATAL_ERROR "sensitive marker in staged closeout document: ${_rel}")
  endif()
  math(EXPR _count "${_count}+1")
endforeach()
if(NOT _count EQUAL 17)
  message(FATAL_ERROR "closeout manifest count drifted: ${_count}")
endif()
file(GLOB_RECURSE _all LIST_DIRECTORIES false "${_root}/*")
list(LENGTH _all _all_count)
if(NOT _all_count EQUAL 19)
  message(FATAL_ERROR "unexpected closeout package file count: ${_all_count}")
endif()
message(STATUS "P2-09 pre-consultation closeout package check passed")
