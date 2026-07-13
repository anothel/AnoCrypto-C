# Assemble the final P1-32 evidence directory from one complete, hash-bound
# predecessor artifact. This script intentionally launches no child process.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_BUILD_DIR
        ANO_CRYPTO_PREPARED_EVIDENCE_DIR
        ANO_CRYPTO_EVIDENCE_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PREPARED_EVIDENCE_DIR
    "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}" ABSOLUTE)

foreach(_path IN ITEMS
        "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}"
        "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(RELATIVE_PATH _relative "${ANO_CRYPTO_BUILD_DIR}" "${_path}")
    if(_relative STREQUAL "" OR _relative STREQUAL "." OR
       IS_ABSOLUTE "${_relative}" OR _relative MATCHES "^([.][.]|[/\\])")
        message(FATAL_ERROR "Evidence paths must be distinct children of ANO_CRYPTO_BUILD_DIR")
    endif()
endforeach()
if(ANO_CRYPTO_PREPARED_EVIDENCE_DIR STREQUAL ANO_CRYPTO_EVIDENCE_DIR)
    message(FATAL_ERROR "Prepared and final evidence directories must differ")
endif()
file(RELATIVE_PATH _prepared_from_final
    "${ANO_CRYPTO_EVIDENCE_DIR}" "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}")
file(RELATIVE_PATH _final_from_prepared
    "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}" "${ANO_CRYPTO_EVIDENCE_DIR}")
if((NOT IS_ABSOLUTE "${_prepared_from_final}" AND
    NOT _prepared_from_final MATCHES "^([.][.]|[/\\])") OR
   (NOT IS_ABSOLUTE "${_final_from_prepared}" AND
    NOT _final_from_prepared MATCHES "^([.][.]|[/\\])") OR
   IS_SYMLINK "${ANO_CRYPTO_EVIDENCE_DIR}")
    message(FATAL_ERROR
        "Prepared/final evidence paths must not overlap or use a final-directory symlink")
endif()
if(NOT IS_DIRECTORY "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}" OR
   IS_SYMLINK "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}")
    message(FATAL_ERROR "Prepared evidence directory is missing or unsafe")
endif()

function(_ano_crypto_validate_prepared_file _path _relative)
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}")
        message(FATAL_ERROR "Prepared evidence file is missing or unsafe: ${_relative}")
    endif()
    if(IS_SYMLINK "${_path}")
        get_filename_component(_resolved "${_path}" REALPATH)
        file(RELATIVE_PATH _resolved_relative
            "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}" "${_resolved}")
        if(_resolved_relative STREQUAL "" OR
           IS_ABSOLUTE "${_resolved_relative}" OR
           _resolved_relative MATCHES "^([.][.]|[/\\])" OR
           NOT EXISTS "${_resolved}" OR IS_DIRECTORY "${_resolved}")
            message(FATAL_ERROR
                "Prepared evidence symlink escapes its root or has an unsafe target: ${_relative}")
        endif()
    endif()
endfunction()

set(_manifest "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256")
if(NOT EXISTS "${_manifest}" OR IS_DIRECTORY "${_manifest}" OR IS_SYMLINK "${_manifest}")
    message(FATAL_ERROR "Prepared evidence manifest is missing or unsafe")
endif()
file(SHA256 "${_manifest}" _validated_manifest_hash)

foreach(_status_check IN ITEMS
        "CTEST_EVIDENCE_SUMMARY.txt|CTest result: 0"
        "SENSITIVE_ARTIFACT_SCAN.txt|Result: PASS"
        "RELEASE_PROVENANCE_CHECK.txt|Result: PASS"
        "RELEASE_PROVENANCE_POLICY_CHECK.txt|Result: PASS"
        "RELEASE_SIGNING_HANDOFF_CHECK.txt|Result: PASS"
        "release-signing-handoff/SIGNING_HANDOFF_STATUS.txt|Readiness: BLOCKED"
        "release-signing-handoff/SIGNING_HANDOFF_STATUS.txt|External release authorized: NO")
    string(REPLACE "|" ";" _status_parts "${_status_check}")
    list(GET _status_parts 0 _status_relative)
    list(GET _status_parts 1 _status_expected)
    set(_status_path "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/${_status_relative}")
    if(NOT EXISTS "${_status_path}" OR IS_DIRECTORY "${_status_path}" OR
       IS_SYMLINK "${_status_path}")
        message(FATAL_ERROR "Prepared status artifact is missing or unsafe: ${_status_relative}")
    endif()
    file(READ "${_status_path}" _status_text)
    string(FIND "${_status_text}" "${_status_expected}" _status_position)
    if(_status_position EQUAL -1)
        message(FATAL_ERROR "Prepared status artifact did not pass: ${_status_relative}")
    endif()
endforeach()

file(STRINGS "${_manifest}" _manifest_lines)
set(_manifest_paths "")
set(_manifest_records "")
foreach(_line IN LISTS _manifest_lines)
    if(NOT _line MATCHES "^([0-9a-fA-F]+)  (.+)$")
        message(FATAL_ERROR "Malformed prepared evidence manifest line")
    endif()
    set(_expected_hash "${CMAKE_MATCH_1}")
    set(_relative "${CMAKE_MATCH_2}")
    string(LENGTH "${_expected_hash}" _hash_length)
    if(NOT _hash_length EQUAL 64 OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\]|(^|/)[.][.](/|$)|[|;\r\n\t])" OR
       _relative MATCHES "\\\\" OR
       _relative STREQUAL "EVIDENCE_MANIFEST.sha256" OR
       _relative IN_LIST _manifest_paths)
        message(FATAL_ERROR "Unsafe or duplicate prepared evidence manifest entry: ${_relative}")
    endif()
    set(_prepared_file "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/${_relative}")
    _ano_crypto_validate_prepared_file("${_prepared_file}" "${_relative}")
    file(SHA256 "${_prepared_file}" _actual_hash)
    string(TOLOWER "${_expected_hash}" _expected_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Prepared evidence hash mismatch: ${_relative}")
    endif()
    list(APPEND _manifest_paths "${_relative}")
    list(APPEND _manifest_records "${_relative}|${_expected_hash}")
endforeach()
if(NOT _manifest_paths)
    message(FATAL_ERROR "Prepared evidence manifest is empty")
endif()

file(GLOB_RECURSE _prepared_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}"
    "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/*")
foreach(_relative IN LISTS _prepared_files)
    _ano_crypto_validate_prepared_file(
        "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/${_relative}" "${_relative}")
    if(NOT _relative STREQUAL "EVIDENCE_MANIFEST.sha256" AND
       NOT _relative IN_LIST _manifest_paths)
        message(FATAL_ERROR "Prepared evidence file is not manifest-covered: ${_relative}")
    endif()
endforeach()
list(LENGTH _prepared_files _prepared_file_count)
list(LENGTH _manifest_paths _manifest_file_count)
math(EXPR _expected_prepared_count "${_manifest_file_count} + 1")
if(NOT _prepared_file_count EQUAL _expected_prepared_count)
    message(FATAL_ERROR "Prepared evidence manifest coverage is incomplete")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_EVIDENCE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_EVIDENCE_DIR}")
set(_copy_paths ${_manifest_paths} EVIDENCE_MANIFEST.sha256)
foreach(_relative IN LISTS _copy_paths)
    get_filename_component(_copy_parent "${ANO_CRYPTO_EVIDENCE_DIR}/${_relative}" DIRECTORY)
    file(MAKE_DIRECTORY "${_copy_parent}")
    # COPYONLY dereferences safe in-root library symlinks. Final evidence is a
    # regular-file, manifest-equivalent snapshot rather than an active link tree.
    configure_file(
        "${ANO_CRYPTO_PREPARED_EVIDENCE_DIR}/${_relative}"
        "${ANO_CRYPTO_EVIDENCE_DIR}/${_relative}" COPYONLY)
endforeach()

foreach(_record IN LISTS _manifest_records)
    string(REPLACE "|" ";" _record_parts "${_record}")
    list(GET _record_parts 0 _relative)
    list(GET _record_parts 1 _expected_hash)
    set(_copied_file "${ANO_CRYPTO_EVIDENCE_DIR}/${_relative}")
    if(NOT EXISTS "${_copied_file}" OR IS_DIRECTORY "${_copied_file}" OR
       IS_SYMLINK "${_copied_file}")
        message(FATAL_ERROR "Final evidence copy is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_copied_file}" _copied_hash)
    if(NOT _copied_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Final evidence copy hash mismatch: ${_relative}")
    endif()
endforeach()
file(SHA256 "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" _copied_manifest_hash)
if(NOT _copied_manifest_hash STREQUAL _validated_manifest_hash)
    message(FATAL_ERROR "Final evidence manifest copy hash mismatch")
endif()

message(STATUS
    "AnoCrypto-C evidence draft assembled from prepared artifacts: ${ANO_CRYPTO_EVIDENCE_DIR}")
