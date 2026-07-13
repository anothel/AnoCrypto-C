# Fail-closed credential/signature scan shared by evidence preparation and its
# synthetic source-archive/install/PoC/evidence regressions.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_SENSITIVE_ARTIFACT_ROOTS
        ANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT
    "${ANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")

set(_scan_roots "")
foreach(_root IN LISTS ANO_CRYPTO_SENSITIVE_ARTIFACT_ROOTS)
    get_filename_component(_root "${_root}" ABSOLUTE)
    if(NOT IS_DIRECTORY "${_root}" OR IS_SYMLINK "${_root}")
        message(FATAL_ERROR "Required artifact-safety scan root is missing or unsafe: ${_root}")
    endif()
    list(APPEND _scan_roots "${_root}")
endforeach()

set(_scan_files "")
foreach(_root IN LISTS _scan_roots)
    file(GLOB_RECURSE _root_files LIST_DIRECTORIES false "${_root}/*")
    list(APPEND _scan_files ${_root_files})
endforeach()
list(REMOVE_DUPLICATES _scan_files)
list(SORT _scan_files)

set(_scan_count 0)
foreach(_scan_file IN LISTS _scan_files)
    math(EXPR _scan_count "${_scan_count} + 1")
    string(TOLOWER "${_scan_file}" _scan_file_lower)
    if(_scan_file_lower MATCHES "[.](key|pem|sig|asc|p12|pfx|jks|keystore|token|tsq|tsr|ed25519)$")
        message(FATAL_ERROR
            "Credential/signature-like path found in release artifact: ${_scan_file}")
    endif()
    set(_scan_content_file "${_scan_file}")
    if(IS_SYMLINK "${_scan_file}")
        get_filename_component(_scan_content_file "${_scan_file}" REALPATH)
        set(_safe_scan_target OFF)
        foreach(_root IN LISTS _scan_roots)
            file(RELATIVE_PATH _target_relative "${_root}" "${_scan_content_file}")
            if(NOT IS_ABSOLUTE "${_target_relative}" AND
               NOT _target_relative MATCHES "^([.][.]|[/\\])" AND
               EXISTS "${_scan_content_file}" AND
               NOT IS_DIRECTORY "${_scan_content_file}")
                set(_safe_scan_target ON)
            endif()
        endforeach()
        if(NOT _safe_scan_target)
            message(FATAL_ERROR
                "Artifact symlink escapes the scanned roots or has an unsafe target: ${_scan_file}")
        endif()
    endif()
    ano_crypto_detect_sensitive_signing_material_file(
        "${_scan_content_file}" ON _sensitive_reason)
    if(_sensitive_reason)
        message(FATAL_ERROR
            "Sensitive signing material found in release artifact ${_scan_file}: ${_sensitive_reason}")
    endif()
endforeach()

get_filename_component(_scan_output_parent
    "${ANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_scan_output_parent}")
file(WRITE "${ANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT}"
    "AnoCrypto-C P1-33 sensitive artifact scan\n"
    "Scanned files: ${_scan_count}\n"
    "Private-key marker present: no\n"
    "Credential/signature-like file present: no\n"
    "Evidence, install SDK, and PoC roots: PASS\n"
    "Result: PASS\n")

message(STATUS "AnoCrypto-C sensitive release-artifact scan passed")
