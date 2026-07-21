# anocrypto-c release evidence collection draft.
# Usage:
#   cmake -DANO_CRYPTO_SOURCE_DIR=. \
#         -DANO_CRYPTO_BUILD_DIR=build \
#         -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p3-09 \
#         -DANO_CRYPTO_RUN_CTEST=OFF \
#         -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
#         -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
#         -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON \
#         -P tools/collect_release_evidence.cmake
#
# This script creates a local evidence draft for engineering review. It does
# not create official validation evidence and should be reviewed before sharing.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()

if(NOT DEFINED ANO_CRYPTO_EVIDENCE_DIR)
    set(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/evidence/p3-09")
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CTEST)
    set(ANO_CRYPTO_RUN_CTEST OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_CREATE_SOURCE_ARCHIVE)
    set(ANO_CRYPTO_CREATE_SOURCE_ARCHIVE ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_INSTALL_SMOKE)
    set(ANO_CRYPTO_RUN_INSTALL_SMOKE OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK)
    set(ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_VALIDATION_CANDIDATE_CHECK)
    set(ANO_CRYPTO_RUN_VALIDATION_CANDIDATE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SELF_TEST_INTEGRITY_CHECK)
    set(ANO_CRYPTO_RUN_SELF_TEST_INTEGRITY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE)
    set(ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK)
    set(ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SHA512_FAMILY_CHECK)
    set(ANO_CRYPTO_RUN_SHA512_FAMILY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_AES_CBC_CHECK)
    set(ANO_CRYPTO_RUN_AES_CBC_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_AES_HARDENING_CHECK)
    set(ANO_CRYPTO_RUN_AES_HARDENING_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CMAC_CHECK)
    set(ANO_CRYPTO_RUN_CMAC_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CMAC_HARDENING_CHECK)
    set(ANO_CRYPTO_RUN_CMAC_HARDENING_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_AES_COMPILER_REVIEW)
    set(ANO_CRYPTO_RUN_AES_COMPILER_REVIEW ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CMAC_COMPILER_REVIEW)
    set(ANO_CRYPTO_RUN_CMAC_COMPILER_REVIEW ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_RANDOM_SERVICE_CHECK)
    set(ANO_CRYPTO_RUN_RANDOM_SERVICE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_RANDOM_LIFECYCLE_CHECK)
    set(ANO_CRYPTO_RUN_RANDOM_LIFECYCLE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_EXPORTED_SYMBOL_CHECK)
    set(ANO_CRYPTO_RUN_EXPORTED_SYMBOL_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_ENTROPY_HEALTH_CHECK)
    set(ANO_CRYPTO_RUN_ENTROPY_HEALTH_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK)
    set(ANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_ENTROPY_DRBG_POLICY_CHECK)
    set(ANO_CRYPTO_RUN_ENTROPY_DRBG_POLICY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CSP_ZEROIZATION_POLICY_CHECK)
    set(ANO_CRYPTO_RUN_CSP_ZEROIZATION_POLICY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_ZEROIZATION_COMPILER_REVIEW)
    set(ANO_CRYPTO_RUN_ZEROIZATION_COMPILER_REVIEW ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SECURE_CODING_POLICY_CHECK)
    set(ANO_CRYPTO_RUN_SECURE_CODING_POLICY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SECURE_CODING_COMPILER_REVIEW)
    set(ANO_CRYPTO_RUN_SECURE_CODING_COMPILER_REVIEW ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SUBMISSION_DOCUMENT_POLICY_CHECK)
    set(ANO_CRYPTO_RUN_SUBMISSION_DOCUMENT_POLICY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_PRECONSULTATION_REHEARSAL_CHECK)
    set(ANO_CRYPTO_RUN_PRECONSULTATION_REHEARSAL_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_P2_CLOSEOUT_CHECK)
    set(ANO_CRYPTO_RUN_P2_CLOSEOUT_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_LIVE_REHEARSAL_CHECK)
    set(ANO_CRYPTO_RUN_LIVE_REHEARSAL_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_HUMAN_REHEARSAL_HANDOFF_CHECK)
    set(ANO_CRYPTO_RUN_HUMAN_REHEARSAL_HANDOFF_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CONSULTATION_CONTACT_READINESS_CHECK)
    set(ANO_CRYPTO_RUN_CONSULTATION_CONTACT_READINESS_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_DOWNSTREAM_REQUIREMENT_ANALYSIS_CHECK)
    set(ANO_CRYPTO_RUN_DOWNSTREAM_REQUIREMENT_ANALYSIS_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_HKDF_SHA256_CHECK)
    set(ANO_CRYPTO_RUN_HKDF_SHA256_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_AES256_GCM_CHECK)
    set(ANO_CRYPTO_RUN_AES256_GCM_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_AES256_GCM_STREAMING_CHECK)
    set(ANO_CRYPTO_RUN_AES256_GCM_STREAMING_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_SCRYPT_CHECK)
    set(ANO_CRYPTO_RUN_SCRYPT_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_RSA_PKCS1_V15_CHECK)
    set(ANO_CRYPTO_RUN_RSA_PKCS1_V15_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_ECDSA_P256_CHECK)
    set(ANO_CRYPTO_RUN_ECDSA_P256_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK)
    set(ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK)
    set(ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_RELEASE_COMPATIBILITY_CHECK)
    set(ANO_CRYPTO_RUN_RELEASE_COMPATIBILITY_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_RELEASE_PROVENANCE_CHECK)
    set(ANO_CRYPTO_RUN_RELEASE_PROVENANCE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_TAGGED_COMPATIBILITY_CHECK)
    set(ANO_CRYPTO_RUN_TAGGED_COMPATIBILITY_CHECK ON)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_CONFIG)
    set(ANO_CRYPTO_BUILD_CONFIG "")
endif()
if(NOT DEFINED ANO_CRYPTO_MULTI_CONFIG_BUILD)
    set(ANO_CRYPTO_MULTI_CONFIG_BUILD OFF)
endif()
if(NOT DEFINED ANO_CRYPTO_OPENSSL_DIFFERENTIAL_AVAILABLE)
    set(ANO_CRYPTO_OPENSSL_DIFFERENTIAL_AVAILABLE OFF)
endif()
if(ANO_CRYPTO_MULTI_CONFIG_BUILD AND ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
    message(FATAL_ERROR "A multi-config evidence build requires ANO_CRYPTO_BUILD_CONFIG")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
file(RELATIVE_PATH _evidence_relative "${ANO_CRYPTO_BUILD_DIR}" "${ANO_CRYPTO_EVIDENCE_DIR}")
if(_evidence_relative STREQUAL "" OR _evidence_relative STREQUAL "." OR
   _evidence_relative MATCHES "^([.][.]|[/\\])")
    message(FATAL_ERROR "ANO_CRYPTO_EVIDENCE_DIR must be a child of ANO_CRYPTO_BUILD_DIR")
endif()

if(NOT DEFINED ANO_CRYPTO_INSTALL_PREFIX)
    set(ANO_CRYPTO_INSTALL_PREFIX "${ANO_CRYPTO_EVIDENCE_DIR}/install-prefix")
endif()
if(NOT DEFINED ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR)
    set(ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR "${ANO_CRYPTO_EVIDENCE_DIR}/install-smoke-build")
endif()
if(NOT DEFINED ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR)
    set(ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}/tests/install_smoke")
endif()
if(NOT DEFINED ANO_CRYPTO_POC_SMOKE_DIR)
    set(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}/poc-package-smoke")
endif()

get_filename_component(ANO_CRYPTO_INSTALL_PREFIX "${ANO_CRYPTO_INSTALL_PREFIX}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_POC_SMOKE_DIR}" ABSOLUTE)

file(REMOVE_RECURSE "${ANO_CRYPTO_EVIDENCE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_EVIDENCE_DIR}")

function(ano_crypto_run_logged log_file description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _cmd_result
        OUTPUT_VARIABLE _cmd_stdout
        ERROR_VARIABLE _cmd_stderr
        TIMEOUT 600)
    file(WRITE "${log_file}" "# ${description}\n")
    file(APPEND "${log_file}" "# Command:")
    foreach(_arg IN LISTS ARGN)
        file(APPEND "${log_file}" " ${_arg}")
    endforeach()
    file(APPEND "${log_file}" "\n# Result: ${_cmd_result}\n\n")
    file(APPEND "${log_file}" "${_cmd_stdout}${_cmd_stderr}")
    if(NOT _cmd_result EQUAL 0)
        message(FATAL_ERROR "${description} failed; see ${log_file}")
    endif()
endfunction()

# CMake's recursive glob is unusually slow on WSL-mounted Windows trees.
# Prefer one native directory walk where POSIX find is available; retain the
# CMake fallback for Windows and extracted-source environments.
function(_ano_crypto_list_tree_files _root _output)
    set(_files "")
    set(_allow_symlinks OFF)
    if(ARGC GREATER 2 AND ARGV2)
        set(_allow_symlinks ON)
    endif()
    if(UNIX)
        find_program(_ano_crypto_find_executable NAMES find)
        if(_ano_crypto_find_executable)
            execute_process(
                COMMAND "${_ano_crypto_find_executable}" "${_root}"
                    -type l -print
                RESULT_VARIABLE _symlink_find_result
                OUTPUT_VARIABLE _symlink_find_output
                ERROR_VARIABLE _symlink_find_error
                OUTPUT_STRIP_TRAILING_WHITESPACE
                TIMEOUT 60)
            if(NOT "${_symlink_find_result}" STREQUAL "0")
                message(FATAL_ERROR "Artifact symlink enumeration failed: ${_symlink_find_error}")
            elseif(NOT _allow_symlinks AND NOT _symlink_find_output STREQUAL "")
                message(FATAL_ERROR "Artifact tree rejects symlinks: ${_symlink_find_output}")
            endif()
            if(_allow_symlinks)
                set(_find_type_arguments "(" -type f -o -type l ")")
            else()
                set(_find_type_arguments -type f)
            endif()
            execute_process(
                COMMAND "${_ano_crypto_find_executable}" "${_root}"
                    ${_find_type_arguments} -print
                RESULT_VARIABLE _find_result
                OUTPUT_VARIABLE _find_output
                ERROR_VARIABLE _find_error
                OUTPUT_STRIP_TRAILING_WHITESPACE
                TIMEOUT 60)
            if("${_find_result}" STREQUAL "0" AND NOT _find_output STREQUAL "")
                string(REPLACE "\r\n" "\n" _find_output "${_find_output}")
                string(REPLACE "\n" ";" _files "${_find_output}")
            elseif(NOT "${_find_result}" STREQUAL "0")
                message(FATAL_ERROR "Artifact file enumeration failed: ${_find_error}")
            endif()
        endif()
    endif()
    if(NOT _files)
        file(GLOB_RECURSE _files LIST_DIRECTORIES false "${_root}/*")
        foreach(_file IN LISTS _files)
            if(NOT _allow_symlinks AND IS_SYMLINK "${_file}")
                message(FATAL_ERROR "Artifact tree rejects symlinks: ${_file}")
            endif()
        endforeach()
    endif()
    list(SORT _files)
    set(${_output} "${_files}" PARENT_SCOPE)
endfunction()

string(TIMESTAMP ANO_CRYPTO_EVIDENCE_UTC "%Y-%m-%dT%H:%M:%SZ" UTC)

set(_git_commit "unknown")
set(_git_status "git unavailable or not a repository")
set(_git_worktree_state "UNKNOWN")
set(_source_is_git_root FALSE)
find_program(_git_exe git)
if(_git_exe)
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" rev-parse --show-toplevel
        RESULT_VARIABLE _git_root_result
        OUTPUT_VARIABLE _git_root_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 30)
    if(_git_root_result EQUAL 0)
        get_filename_component(_source_real "${ANO_CRYPTO_SOURCE_DIR}" REALPATH)
        get_filename_component(_git_root_real "${_git_root_output}" REALPATH)
        if(WIN32)
            string(TOLOWER "${_source_real}" _source_real)
            string(TOLOWER "${_git_root_real}" _git_root_real)
        endif()
        if(_source_real STREQUAL _git_root_real)
            set(_source_is_git_root TRUE)
        endif()
    endif()
endif()
if(_git_exe AND _source_is_git_root)
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            rev-parse --verify "HEAD^{commit}"
        RESULT_VARIABLE _git_commit_result
        OUTPUT_VARIABLE _git_commit_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 30)
    if(_git_commit_result EQUAL 0)
        set(_git_commit "${_git_commit_output}")
    endif()

    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" status --short
        RESULT_VARIABLE _git_status_result
        OUTPUT_VARIABLE _git_status_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 30)
    if(_git_status_result EQUAL 0)
        if(_git_status_output STREQUAL "")
            set(_git_status "clean")
            set(_git_worktree_state "CLEAN")
        else()
            set(_git_status "${_git_status_output}")
            set(_git_worktree_state "DIRTY")
        endif()
    endif()
endif()
if(_source_is_git_root AND
   (_git_commit STREQUAL "unknown" OR NOT _git_worktree_state STREQUAL "CLEAN"))
    message(FATAL_ERROR
        "Git-backed P1-33 evidence collection requires a known commit and clean source root")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
"AnoCrypto-C Release Evidence Draft\n"
"Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
"Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
"Build directory: ${ANO_CRYPTO_BUILD_DIR}\n"
"Git commit: ${_git_commit}\n"
"Git status: ${_git_status}\n"
"CMake command: ${CMAKE_COMMAND}\n"
"CMake version: ${CMAKE_VERSION}\n"
"Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
"Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
"Compatibility baseline: ${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\n"
"ABI line and symbol count: ${ANO_CRYPTO_RELEASE_ABI_LINE} / ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
"Reproducible build status: P1-33 retained draft procedure; binary byte identity not claimed\n"
"Evidence status: engineering draft only\n"
"Claims boundary: not official validation evidence\n")

foreach(_required_prepared_var
        ANO_CRYPTO_PREPARED_PROVENANCE_DIR
        ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_DIR
        ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK)
    if(NOT DEFINED ${_required_prepared_var} OR
       "${${_required_prepared_var}}" STREQUAL "")
        message(FATAL_ERROR
            "${_required_prepared_var} is required; evidence assembly is status-only")
    endif()
    get_filename_component(${_required_prepared_var}
        "${${_required_prepared_var}}" ABSOLUTE)
endforeach()

set(_prepared_provenance_files
    SOURCE_FILES.sha256
    SOURCE_ARCHIVE_CONTENTS.txt
    SOURCE_ARCHIVE.log
    SOURCE_ARCHIVE.sha256
    anocrypto-c-source.tar.gz
    RELEASE_PROVENANCE.log
    RELEASE_PROVENANCE_CHECK.txt
    RELEASE_PROVENANCE_POLICY_CHECK.txt
    RELEASE_PROVENANCE_POLICY_CHECK.log
    SBOM_STATUS.txt
    SIGNING_STATUS.txt
    PREPARED_RELEASE_PROVENANCE_SUMMARY.txt)
foreach(_artifact IN LISTS _prepared_provenance_files)
    set(_prepared "${ANO_CRYPTO_PREPARED_PROVENANCE_DIR}/${_artifact}")
    if(NOT EXISTS "${_prepared}" OR IS_DIRECTORY "${_prepared}" OR
       IS_SYMLINK "${_prepared}")
        message(FATAL_ERROR "Prepared release provenance artifact is missing or unsafe: ${_artifact}")
    endif()
    configure_file("${_prepared}" "${ANO_CRYPTO_EVIDENCE_DIR}/${_artifact}" COPYONLY)
endforeach()
if(NOT IS_DIRECTORY "${ANO_CRYPTO_PREPARED_PROVENANCE_DIR}/release-provenance" OR
   IS_SYMLINK "${ANO_CRYPTO_PREPARED_PROVENANCE_DIR}/release-provenance")
    message(FATAL_ERROR "Prepared release provenance bundle is missing or unsafe")
endif()
file(COPY "${ANO_CRYPTO_PREPARED_PROVENANCE_DIR}/release-provenance"
    DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")

if(NOT IS_DIRECTORY "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_DIR}" OR
   IS_SYMLINK "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_DIR}" OR
   NOT EXISTS "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK}" OR
   IS_DIRECTORY "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK}" OR
   IS_SYMLINK "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK}")
    message(FATAL_ERROR "Prepared release signing handoff is missing or unsafe")
endif()
file(COPY "${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_DIR}"
    DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
configure_file("${ANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_SIGNING_HANDOFF_CHECK.txt" COPYONLY)

function(_ano_read_unique_prepared_key _path _key _out)
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}" OR IS_SYMLINK "${_path}")
        message(FATAL_ERROR "Prepared binding input is missing or unsafe: ${_path}")
    endif()
    file(STRINGS "${_path}" _matches REGEX "^${_key}=")
    list(LENGTH _matches _match_count)
    if(NOT _match_count EQUAL 1)
        message(FATAL_ERROR
            "Prepared binding key ${_key} must occur exactly once in ${_path}")
    endif()
    list(GET _matches 0 _line)
    string(REGEX REPLACE "^[^=]*=" "" _value "${_line}")
    set(${_out} "${_value}" PARENT_SCOPE)
endfunction()

set(_collected_provenance
    "${ANO_CRYPTO_EVIDENCE_DIR}/release-provenance/RELEASE_PROVENANCE.txt")
foreach(_key IN ITEMS release_version expected_tag actual_tag source_commit
                       worktree_state signing_status release_authorization
                       external_release_authorized result)
    _ano_read_unique_prepared_key(
        "${_collected_provenance}" "${_key}" "_collected_prov_${_key}")
endforeach()
if(NOT _collected_prov_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _collected_prov_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _collected_prov_actual_tag STREQUAL "not-recorded" OR
   NOT _collected_prov_source_commit STREQUAL _git_commit OR
   NOT _collected_prov_worktree_state STREQUAL _git_worktree_state OR
   NOT _collected_prov_signing_status STREQUAL "UNSIGNED" OR
   NOT _collected_prov_release_authorization STREQUAL "BLOCKED" OR
   NOT _collected_prov_external_release_authorized STREQUAL "NO" OR
   NOT _collected_prov_result STREQUAL "PASS")
    message(FATAL_ERROR
        "Prepared provenance does not match the current P1-33 source/release boundary")
endif()

set(_collected_handoff_request
    "${ANO_CRYPTO_EVIDENCE_DIR}/release-signing-handoff/SIGNING_REQUEST.txt")
foreach(_key IN ITEMS release_version expected_tag source_commit subject readiness
                       external_release_authorized)
    _ano_read_unique_prepared_key(
        "${_collected_handoff_request}" "${_key}" "_collected_request_${_key}")
endforeach()
if(NOT _collected_request_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _collected_request_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _collected_request_source_commit STREQUAL _git_commit OR
   NOT _collected_request_subject STREQUAL "RELEASE_ARTIFACTS.sha256" OR
   NOT _collected_request_readiness STREQUAL "BLOCKED" OR
   NOT _collected_request_external_release_authorized STREQUAL "NO")
    message(FATAL_ERROR
        "Prepared signing request does not match the current P2-02 provenance boundary")
endif()

set(_collected_handoff_disposition
    "${ANO_CRYPTO_EVIDENCE_DIR}/release-signing-handoff/HOSTED_ARTIFACT_DISPOSITION.txt")
foreach(_key IN ITEMS review_status reviewer review_date disposition signing_status
                       handoff_status external_release_authorized result)
    _ano_read_unique_prepared_key(
        "${_collected_handoff_disposition}" "${_key}"
        "_collected_disposition_${_key}")
endforeach()
if(NOT _collected_disposition_review_status STREQUAL "UNREVIEWED" OR
   NOT _collected_disposition_reviewer STREQUAL "UNPROVISIONED" OR
   NOT _collected_disposition_review_date STREQUAL "UNREVIEWED" OR
   NOT _collected_disposition_disposition STREQUAL "UNREVIEWED" OR
   NOT _collected_disposition_signing_status STREQUAL "UNSIGNED" OR
   NOT _collected_disposition_handoff_status STREQUAL "BLOCKED" OR
   NOT _collected_disposition_external_release_authorized STREQUAL "NO" OR
   NOT _collected_disposition_result STREQUAL "BLOCKED")
    message(FATAL_ERROR
        "Prepared hosted disposition crosses the P1-33 unreviewed/blocked boundary")
endif()

file(READ "${ANO_CRYPTO_EVIDENCE_DIR}/PREPARED_RELEASE_PROVENANCE_SUMMARY.txt"
    _prepared_summary_text)
foreach(_summary_line IN ITEMS
        "Release version: ${ANO_CRYPTO_RELEASE_VERSION}"
        "Git commit: ${_git_commit}"
        "Git status: ${_git_status}"
        "Worktree state: ${_git_worktree_state}"
        "Provenance verification: PASS"
        "Signing status: UNSIGNED"
        "External release authorized: NO"
        "Result: PASS")
    string(FIND "${_prepared_summary_text}" "${_summary_line}" _summary_position)
    if(_summary_position EQUAL -1)
        message(FATAL_ERROR
            "Prepared provenance summary does not match current evidence: ${_summary_line}")
    endif()
endforeach()

foreach(_binding_name IN ITEMS
        RELEASE_PROVENANCE.txt HOSTED_ARTIFACT_DISPOSITION.txt
        RELEASE_ARTIFACTS.sha256)
    file(SHA256
        "${ANO_CRYPTO_EVIDENCE_DIR}/release-provenance/${_binding_name}"
        _provenance_binding_hash)
    file(SHA256
        "${ANO_CRYPTO_EVIDENCE_DIR}/release-signing-handoff/${_binding_name}"
        _handoff_binding_hash)
    if(NOT _provenance_binding_hash STREQUAL _handoff_binding_hash)
        message(FATAL_ERROR
            "Prepared provenance/signing-handoff binding mismatch: ${_binding_name}")
    endif()
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${ANO_CRYPTO_EVIDENCE_DIR}/release-provenance
        -DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_PROVENANCE_CHECK.txt
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_bundle.cmake"
    RESULT_VARIABLE _current_provenance_check_result
    OUTPUT_VARIABLE _current_provenance_check_stdout
    ERROR_VARIABLE _current_provenance_check_stderr
    TIMEOUT 300)
if(NOT _current_provenance_check_result EQUAL 0)
    message(FATAL_ERROR
        "Current-source provenance recheck failed:\n${_current_provenance_check_stdout}${_current_provenance_check_stderr}")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_SIGNING_HANDOFF_DIR=${ANO_CRYPTO_EVIDENCE_DIR}/release-signing-handoff
        -DANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT=${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_SIGNING_HANDOFF_CHECK.txt
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_signing_handoff.cmake"
    RESULT_VARIABLE _current_handoff_check_result
    OUTPUT_VARIABLE _current_handoff_check_stdout
    ERROR_VARIABLE _current_handoff_check_stderr
    TIMEOUT 300)
if(NOT _current_handoff_check_result EQUAL 0)
    message(FATAL_ERROR
        "Current-source signing-handoff recheck failed:\n${_current_handoff_check_stdout}${_current_handoff_check_stderr}")
endif()
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Prepared provenance/current source binding: PASS\n"
    "Prepared signing handoff/current provenance binding: PASS\n")

foreach(_passed_artifact IN ITEMS
        PREPARED_RELEASE_PROVENANCE_SUMMARY.txt
        RELEASE_PROVENANCE_CHECK.txt
        RELEASE_PROVENANCE_POLICY_CHECK.txt
        RELEASE_SIGNING_HANDOFF_CHECK.txt)
    file(READ "${ANO_CRYPTO_EVIDENCE_DIR}/${_passed_artifact}" _passed_text)
    if(NOT _passed_text MATCHES "Result: PASS")
        message(FATAL_ERROR "Prepared release artifact did not pass: ${_passed_artifact}")
    endif()
endforeach()
file(READ "${ANO_CRYPTO_EVIDENCE_DIR}/release-signing-handoff/SIGNING_HANDOFF_STATUS.txt"
    _handoff_status)
if(NOT _handoff_status MATCHES "Readiness: BLOCKED" OR
   NOT _handoff_status MATCHES "External release authorized: NO")
    message(FATAL_ERROR "Prepared signing handoff does not preserve the blocked release boundary")
endif()

if(NOT ANO_CRYPTO_CREATE_SOURCE_ARCHIVE)
    message(FATAL_ERROR "Prepared source archive is required for release evidence")
endif()
set(_archive_path "${ANO_CRYPTO_EVIDENCE_DIR}/anocrypto-c-source.tar.gz")
file(SHA256 "${_archive_path}" _archive_hash)
file(READ "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256" _archive_hash_record)
if(NOT _archive_hash_record STREQUAL
   "${_archive_hash}  anocrypto-c-source.tar.gz\n")
    message(FATAL_ERROR "Prepared source archive checksum mismatch")
endif()
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Source archive: anocrypto-c-source.tar.gz\n"
    "Source archive SHA-256: ${_archive_hash}\n")

if(NOT ANO_CRYPTO_RUN_RELEASE_PROVENANCE_CHECK)
    message(FATAL_ERROR "Prepared release provenance verification is required")
endif()
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Release provenance bundle: release-provenance/\n"
    "Release provenance check: RELEASE_PROVENANCE_CHECK.txt\n"
    "Release provenance policy check: RELEASE_PROVENANCE_POLICY_CHECK.txt\n"
    "Source SBOM: release-provenance/SOURCE_SBOM.spdx.json\n"
    "Signing handoff: release-signing-handoff/\n"
    "Signing handoff check: RELEASE_SIGNING_HANDOFF_CHECK.txt\n"
    "Signing mode: ${ANO_CRYPTO_RELEASE_SIGNING_MODE}\n"
    "Signing handoff readiness: BLOCKED\n"
    "External release authorized: NO\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/BINARY_FILES.sha256" "")
if(EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    _ano_crypto_list_tree_files("${ANO_CRYPTO_BUILD_DIR}" _build_files TRUE)
    foreach(_binary_file IN LISTS _build_files)
        file(RELATIVE_PATH _rel "${ANO_CRYPTO_BUILD_DIR}" "${_binary_file}")
        get_filename_component(_binary_name "${_binary_file}" NAME)
        if(_binary_name MATCHES "^(libanocrypto_c|anocrypto_c|ano_crypto_)" AND
           NOT _rel MATCHES "CMakeFiles/" AND NOT _rel MATCHES "^evidence/")
            file(SHA256 "${_binary_file}" _hash)
            file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BINARY_FILES.sha256" "${_hash}  ${_rel}\n")
        endif()
    endforeach()
endif()

if(ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK)
    set(_policy_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/PUBLIC_SURFACE_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_POLICY_CHECK_OUTPUT=${_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_public_surface.cmake"
        RESULT_VARIABLE _policy_result
        OUTPUT_VARIABLE _policy_stdout
        ERROR_VARIABLE _policy_stderr
        TIMEOUT 300)
    file(APPEND "${_policy_check_output}" "\nCommand output:\n${_policy_stdout}${_policy_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Public surface check result: ${_policy_result}\n")
    if(NOT _policy_result EQUAL 0)
        message(FATAL_ERROR "public surface check failed; see ${_policy_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/PUBLIC_SURFACE_CHECK.txt"
        "Public surface check was not requested. Set ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_VALIDATION_CANDIDATE_CHECK)
    set(_validation_candidate_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_VALIDATION_CANDIDATE_CHECK_OUTPUT=${_validation_candidate_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_validation_candidate_policy.cmake"
        RESULT_VARIABLE _validation_candidate_result
        OUTPUT_VARIABLE _validation_candidate_stdout
        ERROR_VARIABLE _validation_candidate_stderr
        TIMEOUT 300)
    file(APPEND "${_validation_candidate_output}"
        "\nCommand output:\n${_validation_candidate_stdout}${_validation_candidate_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Validation candidate policy result: ${_validation_candidate_result}\n")
    if(NOT _validation_candidate_result EQUAL 0)
        message(FATAL_ERROR
            "validation candidate policy check failed; see ${_validation_candidate_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt"
        "Validation candidate policy check was not requested. Set ANO_CRYPTO_RUN_VALIDATION_CANDIDATE_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_SELF_TEST_INTEGRITY_CHECK)
    set(_self_test_integrity_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/SELF_TEST_INTEGRITY_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_SELF_TEST_POLICY_CHECK_OUTPUT=${_self_test_integrity_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_self_test_integrity_policy.cmake"
        RESULT_VARIABLE _self_test_integrity_result
        OUTPUT_VARIABLE _self_test_integrity_stdout
        ERROR_VARIABLE _self_test_integrity_stderr
        TIMEOUT 300)
    file(APPEND "${_self_test_integrity_output}"
        "\nCommand output:\n${_self_test_integrity_stdout}${_self_test_integrity_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Self-test/integrity policy result: ${_self_test_integrity_result}\n")
    if(NOT _self_test_integrity_result EQUAL 0)
        message(FATAL_ERROR
            "self-test/integrity policy check failed; see ${_self_test_integrity_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SELF_TEST_INTEGRITY_POLICY_CHECK.txt"
        "Self-test/integrity policy check was not requested. Set ANO_CRYPTO_RUN_SELF_TEST_INTEGRITY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_RELEASE_COMPATIBILITY_CHECK)
    set(_release_compatibility_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_COMPATIBILITY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT=${_release_compatibility_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_compatibility.cmake"
        RESULT_VARIABLE _release_compatibility_result
        OUTPUT_VARIABLE _release_compatibility_stdout
        ERROR_VARIABLE _release_compatibility_stderr
        TIMEOUT 300)
    file(APPEND "${_release_compatibility_output}"
        "\nCommand output:\n${_release_compatibility_stdout}${_release_compatibility_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Release compatibility policy result: ${_release_compatibility_result}\n")
    if(NOT _release_compatibility_result EQUAL 0)
        message(FATAL_ERROR
            "release compatibility check failed; see ${_release_compatibility_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_COMPATIBILITY_CHECK.txt"
        "Release compatibility check was not requested. Set ANO_CRYPTO_RUN_RELEASE_COMPATIBILITY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_TAGGED_COMPATIBILITY_CHECK)
    set(_prepared_tagged_compatibility_dir
        "${ANO_CRYPTO_BUILD_DIR}/tagged-release-compatibility")
    set(_prepared_tagged_compatibility_summary
        "${_prepared_tagged_compatibility_dir}/TAGGED_COMPAT_SUMMARY.txt")
    if(NOT EXISTS "${_prepared_tagged_compatibility_summary}")
        message(FATAL_ERROR
            "prepared tagged compatibility smoke is unavailable: ${_prepared_tagged_compatibility_summary}")
    endif()
    configure_file("${_prepared_tagged_compatibility_summary}"
        "${ANO_CRYPTO_EVIDENCE_DIR}/TAGGED_COMPATIBILITY_SMOKE.txt" COPYONLY)
    set(_prepared_tag_identity
        "${_prepared_tagged_compatibility_dir}/TAG_COMMIT_IDENTITY_CHECK.txt")
    if(NOT EXISTS "${_prepared_tag_identity}" OR
       IS_DIRECTORY "${_prepared_tag_identity}" OR
       IS_SYMLINK "${_prepared_tag_identity}")
        message(FATAL_ERROR
            "prepared tag/commit identity status is unavailable: ${_prepared_tag_identity}")
    endif()
    configure_file("${_prepared_tag_identity}"
        "${ANO_CRYPTO_EVIDENCE_DIR}/TAG_COMMIT_IDENTITY_CHECK.txt" COPYONLY)
    foreach(_compat_log IN ITEMS
            CONSUMER_CTEST.log
            PACKAGE_VERSION_EXACT_CTEST.log
            PACKAGE_VERSION_INCOMPATIBLE_CONFIGURE.log)
        if(EXISTS "${_prepared_tagged_compatibility_dir}/${_compat_log}")
            configure_file("${_prepared_tagged_compatibility_dir}/${_compat_log}"
                "${ANO_CRYPTO_EVIDENCE_DIR}/TAGGED_COMPAT_${_compat_log}" COPYONLY)
        endif()
    endforeach()
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Tagged compatibility smoke: TAGGED_COMPATIBILITY_SMOKE.txt\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/TAGGED_COMPATIBILITY_SMOKE.txt"
        "Tagged compatibility smoke was not requested. Set ANO_CRYPTO_RUN_TAGGED_COMPATIBILITY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK)
    set(_thread_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_SAFETY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT=${_thread_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_thread_safety_policy.cmake"
        RESULT_VARIABLE _thread_check_result
        OUTPUT_VARIABLE _thread_check_stdout
        ERROR_VARIABLE _thread_check_stderr
        TIMEOUT 300)
    file(APPEND "${_thread_check_output}" "\nCommand output:\n${_thread_check_stdout}${_thread_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Thread-safety policy check result: ${_thread_check_result}\n")
    if(NOT _thread_check_result EQUAL 0)
        message(FATAL_ERROR "thread-safety policy check failed; see ${_thread_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_SAFETY_CHECK.txt"
        "Thread-safety policy check was not requested. Set ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK)
    set(_concurrency_sanitizer_output "${ANO_CRYPTO_EVIDENCE_DIR}/CONCURRENCY_SANITIZER_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_CONCURRENCY_SANITIZER_CHECK_OUTPUT=${_concurrency_sanitizer_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_concurrency_sanitizer_policy.cmake"
        RESULT_VARIABLE _concurrency_sanitizer_result
        OUTPUT_VARIABLE _concurrency_sanitizer_stdout
        ERROR_VARIABLE _concurrency_sanitizer_stderr
        TIMEOUT 300)
    file(APPEND "${_concurrency_sanitizer_output}" "\nCommand output:\n${_concurrency_sanitizer_stdout}${_concurrency_sanitizer_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Concurrency/sanitizer policy check result: ${_concurrency_sanitizer_result}\n")
    if(NOT _concurrency_sanitizer_result EQUAL 0)
        message(FATAL_ERROR "concurrency/sanitizer policy check failed; see ${_concurrency_sanitizer_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CONCURRENCY_SANITIZER_CHECK.txt"
        "Concurrency/sanitizer policy check was not requested. Set ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK)
    set(_differential_portability_output "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_PORTABILITY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT=${_differential_portability_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_differential_portability_policy.cmake"
        RESULT_VARIABLE _differential_portability_result
        OUTPUT_VARIABLE _differential_portability_stdout
        ERROR_VARIABLE _differential_portability_stderr
        TIMEOUT 300)
    file(APPEND "${_differential_portability_output}" "\nCommand output:\n${_differential_portability_stdout}${_differential_portability_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Differential/portability policy check result: ${_differential_portability_result}\n")
    if(NOT _differential_portability_result EQUAL 0)
        message(FATAL_ERROR "differential/portability policy check failed; see ${_differential_portability_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_PORTABILITY_CHECK.txt"
        "Differential/portability policy check was not requested. Set ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_SHA512_FAMILY_CHECK)
    set(_sha512_family_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_FAMILY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_SHA512_FAMILY_CHECK_OUTPUT=${_sha512_family_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_sha512_family_policy.cmake"
        RESULT_VARIABLE _sha512_family_check_result
        OUTPUT_VARIABLE _sha512_family_check_stdout
        ERROR_VARIABLE _sha512_family_check_stderr
        TIMEOUT 300)
    file(APPEND "${_sha512_family_check_output}" "\nCommand output:\n${_sha512_family_check_stdout}${_sha512_family_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "SHA-512-family implementation check result: ${_sha512_family_check_result}\n")
    if(NOT _sha512_family_check_result EQUAL 0)
        message(FATAL_ERROR "SHA-512-family implementation check failed; see ${_sha512_family_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_FAMILY_CHECK.txt"
        "SHA-512-family implementation check was not requested. Set ANO_CRYPTO_RUN_SHA512_FAMILY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_AES_CBC_CHECK)
    set(_aes_cbc_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CBC_IMPLEMENTATION_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_AES_CBC_CHECK_OUTPUT=${_aes_cbc_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_aes_cbc_policy.cmake"
        RESULT_VARIABLE _aes_cbc_check_result
        OUTPUT_VARIABLE _aes_cbc_check_stdout
        ERROR_VARIABLE _aes_cbc_check_stderr
        TIMEOUT 300)
    file(APPEND "${_aes_cbc_check_output}" "\nCommand output:\n${_aes_cbc_check_stdout}${_aes_cbc_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "AES/CBC implementation policy check result: ${_aes_cbc_check_result}
")
    if(NOT _aes_cbc_check_result EQUAL 0)
        message(FATAL_ERROR "AES/CBC implementation policy check failed; see ${_aes_cbc_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CBC_IMPLEMENTATION_CHECK.txt"
        "AES/CBC implementation policy check was not requested. Set ANO_CRYPTO_RUN_AES_CBC_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_AES_HARDENING_CHECK)
    set(_aes_hardening_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES_HARDENING_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT=${_aes_hardening_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_aes_hardening_policy.cmake"
        RESULT_VARIABLE _aes_hardening_check_result
        OUTPUT_VARIABLE _aes_hardening_check_stdout
        ERROR_VARIABLE _aes_hardening_check_stderr
        TIMEOUT 300)
    file(APPEND "${_aes_hardening_check_output}" "\nCommand output:\n${_aes_hardening_check_stdout}${_aes_hardening_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "AES/CBC hardening policy check result: ${_aes_hardening_check_result}\n")
    if(NOT _aes_hardening_check_result EQUAL 0)
        message(FATAL_ERROR "AES/CBC hardening policy check failed; see ${_aes_hardening_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_HARDENING_CHECK.txt"
        "AES/CBC hardening policy check was not requested. Set ANO_CRYPTO_RUN_AES_HARDENING_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_CMAC_CHECK)
    set(_cmac_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_IMPLEMENTATION_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_CMAC_CHECK_OUTPUT=${_cmac_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_cmac_policy.cmake"
        RESULT_VARIABLE _cmac_check_result
        OUTPUT_VARIABLE _cmac_check_stdout
        ERROR_VARIABLE _cmac_check_stderr
        TIMEOUT 300)
    file(APPEND "${_cmac_check_output}" "
Command output:
${_cmac_check_stdout}${_cmac_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES-CMAC implementation policy check result: ${_cmac_check_result}
")
    if(NOT _cmac_check_result EQUAL 0)
        message(FATAL_ERROR "AES-CMAC implementation policy check failed; see ${_cmac_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_IMPLEMENTATION_CHECK.txt"
        "AES-CMAC implementation policy check was not requested. Set ANO_CRYPTO_RUN_CMAC_CHECK=ON.
")
endif()

if(ANO_CRYPTO_RUN_HKDF_SHA256_CHECK)
    set(_hkdf_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/HKDF_SHA256_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT=${_hkdf_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_hkdf_sha256_policy.cmake"
        RESULT_VARIABLE _hkdf_check_result
        OUTPUT_VARIABLE _hkdf_check_stdout
        ERROR_VARIABLE _hkdf_check_stderr
        TIMEOUT 300)
    file(APPEND "${_hkdf_check_output}"
        "\nCommand output:\n${_hkdf_check_stdout}${_hkdf_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "HKDF-SHA-256 P3-02 policy check result: ${_hkdf_check_result}\n")
    if(NOT _hkdf_check_result EQUAL 0)
        message(FATAL_ERROR "HKDF-SHA-256 policy check failed; see ${_hkdf_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/HKDF_SHA256_POLICY_CHECK.txt"
        "HKDF-SHA-256 policy check was not requested. Set ANO_CRYPTO_RUN_HKDF_SHA256_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_AES256_GCM_CHECK)
    set(_aes256_gcm_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT=${_aes256_gcm_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_aes256_gcm_policy.cmake"
        RESULT_VARIABLE _aes256_gcm_check_result
        OUTPUT_VARIABLE _aes256_gcm_check_stdout
        ERROR_VARIABLE _aes256_gcm_check_stderr
        TIMEOUT 300)
    file(APPEND "${_aes256_gcm_check_output}"
        "\nCommand output:\n${_aes256_gcm_check_stdout}${_aes256_gcm_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES-256-GCM P3-03 policy check result: ${_aes256_gcm_check_result}\n")
    if(NOT _aes256_gcm_check_result EQUAL 0)
        message(FATAL_ERROR "AES-256-GCM policy check failed; see ${_aes256_gcm_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_POLICY_CHECK.txt"
        "AES-256-GCM policy check was not requested. Set ANO_CRYPTO_RUN_AES256_GCM_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_AES256_GCM_STREAMING_CHECK)
    set(_aes256_gcm_streaming_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_STREAMING_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT=${_aes256_gcm_streaming_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_aes256_gcm_streaming_policy.cmake"
        RESULT_VARIABLE _aes256_gcm_streaming_check_result
        OUTPUT_VARIABLE _aes256_gcm_streaming_check_stdout
        ERROR_VARIABLE _aes256_gcm_streaming_check_stderr
        TIMEOUT 300)
    file(APPEND "${_aes256_gcm_streaming_check_output}"
        "\nCommand output:\n${_aes256_gcm_streaming_check_stdout}${_aes256_gcm_streaming_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES-256-GCM P3-04 streaming policy check result: ${_aes256_gcm_streaming_check_result}\n")
    if(NOT _aes256_gcm_streaming_check_result EQUAL 0)
        message(FATAL_ERROR "AES-256-GCM streaming policy check failed; see ${_aes256_gcm_streaming_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_STREAMING_POLICY_CHECK.txt"
        "AES-256-GCM streaming policy check was not requested. Set ANO_CRYPTO_RUN_AES256_GCM_STREAMING_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_SCRYPT_CHECK)
    set(_scrypt_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/SCRYPT_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT=${_scrypt_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_scrypt_policy.cmake"
        RESULT_VARIABLE _scrypt_check_result
        OUTPUT_VARIABLE _scrypt_check_stdout
        ERROR_VARIABLE _scrypt_check_stderr
        TIMEOUT 300)
    file(APPEND "${_scrypt_check_output}"
        "\nCommand output:\n${_scrypt_check_stdout}${_scrypt_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "scrypt P3-05 policy check result: ${_scrypt_check_result}\n")
    if(NOT _scrypt_check_result EQUAL 0)
        message(FATAL_ERROR "scrypt policy check failed; see ${_scrypt_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SCRYPT_POLICY_CHECK.txt"
        "scrypt policy check was not requested. Set ANO_CRYPTO_RUN_SCRYPT_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_RSA_PKCS1_V15_CHECK)
    set(_rsa_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/RSA_PKCS1_V15_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT=${_rsa_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_rsa_pkcs1_v15_policy.cmake"
        RESULT_VARIABLE _rsa_check_result
        OUTPUT_VARIABLE _rsa_check_stdout
        ERROR_VARIABLE _rsa_check_stderr
        TIMEOUT 300)
    file(APPEND "${_rsa_check_output}"
        "
Command output:
${_rsa_check_stdout}${_rsa_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "RSA PKCS#1 v1.5 P3-08 policy check result: ${_rsa_check_result}
")
    if(NOT _rsa_check_result EQUAL 0)
        message(FATAL_ERROR "RSA PKCS#1 v1.5 policy check failed; see ${_rsa_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RSA_PKCS1_V15_POLICY_CHECK.txt"
        "RSA PKCS#1 v1.5 policy check was not requested. Set ANO_CRYPTO_RUN_RSA_PKCS1_V15_CHECK=ON.
")
endif()

if(ANO_CRYPTO_RUN_ECDSA_P256_CHECK)
    set(_ecdsa_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/ECDSA_P256_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT=${_ecdsa_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_ecdsa_p256_policy.cmake"
        RESULT_VARIABLE _ecdsa_check_result
        OUTPUT_VARIABLE _ecdsa_check_stdout
        ERROR_VARIABLE _ecdsa_check_stderr
        TIMEOUT 300)
    file(APPEND "${_ecdsa_check_output}"
        "
Command output:
${_ecdsa_check_stdout}${_ecdsa_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "ECDSA P-256 P3-09 policy check result: ${_ecdsa_check_result}
")
    if(NOT _ecdsa_check_result EQUAL 0)
        message(FATAL_ERROR "ECDSA P-256 policy check failed; see ${_ecdsa_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ECDSA_P256_POLICY_CHECK.txt"
        "ECDSA P-256 policy check was not requested. Set ANO_CRYPTO_RUN_ECDSA_P256_CHECK=ON.
")
endif()

if(ANO_CRYPTO_RUN_CMAC_HARDENING_CHECK)
    set(_cmac_hardening_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_HARDENING_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT=${_cmac_hardening_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_cmac_hardening_policy.cmake"
        RESULT_VARIABLE _cmac_hardening_check_result
        OUTPUT_VARIABLE _cmac_hardening_check_stdout
        ERROR_VARIABLE _cmac_hardening_check_stderr
        TIMEOUT 300)
    file(APPEND "${_cmac_hardening_check_output}"
        "\nCommand output:\n${_cmac_hardening_check_stdout}${_cmac_hardening_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES-CMAC hardening policy check result: ${_cmac_hardening_check_result}\n")
    if(NOT _cmac_hardening_check_result EQUAL 0)
        message(FATAL_ERROR "AES-CMAC hardening policy check failed; see ${_cmac_hardening_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_HARDENING_CHECK.txt"
        "AES-CMAC hardening policy check was not requested. Set ANO_CRYPTO_RUN_CMAC_HARDENING_CHECK=ON.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-29 AES-CMAC hardening review notes\n"
    "Status: engineering implementation and hardening evidence; not official validation evidence.\n"
    "Implemented scope: AES-CMAC with AES-128/192/256, full 16-byte tags, streaming compute, one-shot compute, and one-shot verify.\n"
    "Key-purpose rule: only dedicated ANO_CRYPTO_KEY_TYPE_CMAC handles are accepted; CBC and CMAC reject the other purpose.\n"
    "Hardening: private state/version/AES-schedule guards, fail-closed reinitialization, bounded cleanup, alias rejection, and failure output clearing.\n"
    "Lifecycle/concurrency: 128 lifecycle iterations plus per-thread and shared-handle CMAC regression.\n"
    "Differential provider: OpenSSL 3.x test-only; 1728 CMAC comparisons; no SDK or PoC runtime dependency.\n"
    "Public API: eight CMAC functions. Public export baseline remains 69 symbols with no P1-29 delta.\n"
    "Compiler-output interpretation: target-specific human review required; no automated side-channel verdict.\n"
    "Deferred: tag truncation, AES-CTR, GCM/GMAC, protocol formatting, persistence, export, and completed timing/cache/fault conclusions.\n"
    "Sensitive values recorded: none.\n")

if(ANO_CRYPTO_RUN_AES_COMPILER_REVIEW)
    set(_aes_compiler_review_source "${ANO_CRYPTO_BUILD_DIR}/policy-check/aes-compiler-review")
    if(NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_REVIEW.txt" OR
       NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt")
        message(FATAL_ERROR "Prepared AES compiler-output review artifact is incomplete")
    endif()
    file(COPY "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_REVIEW.txt"
              "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt"
         DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES compiler-output review capture result: 0 (prepared predecessor artifact)\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_REVIEW.txt"
        "AES compiler-output review was not requested. Set ANO_CRYPTO_RUN_AES_COMPILER_REVIEW=ON.\n")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt"
        "AES compiler-output disassembly was not requested.\n")
endif()

if(ANO_CRYPTO_RUN_CMAC_COMPILER_REVIEW)
    set(_cmac_compiler_review_source "${ANO_CRYPTO_BUILD_DIR}/policy-check/cmac-compiler-review")
    if(NOT EXISTS "${_cmac_compiler_review_source}/CMAC_COMPILER_OUTPUT_REVIEW.txt" OR
       NOT EXISTS "${_cmac_compiler_review_source}/CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt")
        message(FATAL_ERROR "Prepared CMAC compiler-output review artifact is incomplete")
    endif()
    file(COPY "${_cmac_compiler_review_source}/CMAC_COMPILER_OUTPUT_REVIEW.txt"
              "${_cmac_compiler_review_source}/CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt"
         DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "CMAC compiler-output review capture result: 0 (prepared predecessor artifact)\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CMAC_COMPILER_OUTPUT_REVIEW.txt"
        "CMAC compiler-output review was not requested. Set ANO_CRYPTO_RUN_CMAC_COMPILER_REVIEW=ON.\n")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt"
        "CMAC compiler-output disassembly was not requested.\n")
endif()

if(ANO_CRYPTO_RUN_ZEROIZATION_COMPILER_REVIEW)
    set(_zeroization_compiler_review_source "${ANO_CRYPTO_BUILD_DIR}/policy-check/zeroization-compiler-review")
    if(NOT EXISTS "${_zeroization_compiler_review_source}/ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt" OR
       NOT EXISTS "${_zeroization_compiler_review_source}/ZEROIZATION_COMPILER_OUTPUT_DISASSEMBLY.txt")
        message(FATAL_ERROR "Prepared zeroization compiler-output review artifact is incomplete")
    endif()
    file(COPY "${_zeroization_compiler_review_source}/ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt"
              "${_zeroization_compiler_review_source}/ZEROIZATION_COMPILER_OUTPUT_DISASSEMBLY.txt"
         DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-05 zeroization compiler-output capture result: 0 (prepared predecessor artifact)\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt"
        "Zeroization compiler-output review was not requested.\n")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ZEROIZATION_COMPILER_OUTPUT_DISASSEMBLY.txt"
        "Zeroization compiler-output disassembly was not requested.\n")
endif()

if(ANO_CRYPTO_RUN_SECURE_CODING_COMPILER_REVIEW)
    set(_secure_coding_review_source
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/secure-coding-review")
    if(NOT EXISTS "${_secure_coding_review_source}/SECURE_CODING_COMPILER_CPU_REVIEW.txt" OR
       NOT EXISTS "${_secure_coding_review_source}/ELF_HARDENING_REVIEW.txt")
        message(FATAL_ERROR "Prepared P2-06 compiler/build-hardening artifact is incomplete")
    endif()
    file(COPY
        "${_secure_coding_review_source}/SECURE_CODING_COMPILER_CPU_REVIEW.txt"
        "${_secure_coding_review_source}/ELF_HARDENING_REVIEW.txt"
        DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-06 compiler/CPU and ELF-hardening capture result: 0 (prepared predecessor artifact)\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SECURE_CODING_COMPILER_CPU_REVIEW.txt"
        "P2-06 compiler/CPU review was not requested.\n")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ELF_HARDENING_REVIEW.txt"
        "P2-06 ELF hardening review was not requested.\n")
endif()

if(ANO_CRYPTO_RUN_RANDOM_SERVICE_CHECK)
    set(_random_service_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_SERVICE_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_RANDOM_SERVICE_CHECK_OUTPUT=${_random_service_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_random_service_policy.cmake"
        RESULT_VARIABLE _random_service_check_result
        OUTPUT_VARIABLE _random_service_check_stdout
        ERROR_VARIABLE _random_service_check_stderr
        TIMEOUT 300)
    file(APPEND "${_random_service_check_output}" "\nCommand output:\n${_random_service_check_stdout}${_random_service_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Random-service implementation policy check result: ${_random_service_check_result}\n")
    if(NOT _random_service_check_result EQUAL 0)
        message(FATAL_ERROR "random-service implementation policy check failed; see ${_random_service_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_SERVICE_CHECK.txt"
        "Random-service implementation policy check was not requested. Set ANO_CRYPTO_RUN_RANDOM_SERVICE_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_RANDOM_LIFECYCLE_CHECK)
    set(_random_lifecycle_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_LIFECYCLE_HARDENING_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_RANDOM_LIFECYCLE_CHECK_OUTPUT=${_random_lifecycle_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_random_lifecycle_policy.cmake"
        RESULT_VARIABLE _random_lifecycle_check_result
        OUTPUT_VARIABLE _random_lifecycle_check_stdout
        ERROR_VARIABLE _random_lifecycle_check_stderr
        TIMEOUT 300)
    file(APPEND "${_random_lifecycle_check_output}" "\nCommand output:\n${_random_lifecycle_check_stdout}${_random_lifecycle_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Random lifecycle hardening policy check result: ${_random_lifecycle_check_result}\n")
    if(NOT _random_lifecycle_check_result EQUAL 0)
        message(FATAL_ERROR "random lifecycle hardening policy check failed; see ${_random_lifecycle_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_LIFECYCLE_HARDENING_CHECK.txt"
        "Random lifecycle hardening policy check was not requested. Set ANO_CRYPTO_RUN_RANDOM_LIFECYCLE_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_EXPORTED_SYMBOL_CHECK)
    set(_symbol_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/EXPORTED_SYMBOL_BASELINE_CHECK.txt")
    set(_prepared_symbol_check
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/CLEAN_RELEASE_EXPORTED_SYMBOL_BASELINE_CHECK.txt")
    if(NOT EXISTS "${_prepared_symbol_check}" OR IS_DIRECTORY "${_prepared_symbol_check}" OR
       IS_SYMLINK "${_prepared_symbol_check}")
        message(FATAL_ERROR "Prepared clean Release symbol report is unavailable")
    endif()
    file(READ "${_prepared_symbol_check}" _prepared_symbol_text)
    if(NOT _prepared_symbol_text MATCHES "Result: PASS")
        message(FATAL_ERROR "Prepared clean Release symbol report did not pass")
    endif()
    configure_file("${_prepared_symbol_check}" "${_symbol_check_output}" COPYONLY)
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Clean Release exported-symbol baseline result: 0 (prepared predecessor artifact)\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EXPORTED_SYMBOL_BASELINE_CHECK.txt"
        "Clean Release exported-symbol baseline check was not requested. Set ANO_CRYPTO_RUN_EXPORTED_SYMBOL_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_ENTROPY_HEALTH_CHECK)
    set(_entropy_health_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_HEALTH_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT=${_entropy_health_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_entropy_health_policy.cmake"
        RESULT_VARIABLE _entropy_health_check_result
        OUTPUT_VARIABLE _entropy_health_check_stdout
        ERROR_VARIABLE _entropy_health_check_stderr
        TIMEOUT 300)
    file(APPEND "${_entropy_health_check_output}" "\nCommand output:\n${_entropy_health_check_stdout}${_entropy_health_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Entropy/health policy check result: ${_entropy_health_check_result}\n")
    if(NOT _entropy_health_check_result EQUAL 0)
        message(FATAL_ERROR "entropy/health policy check failed; see ${_entropy_health_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_HEALTH_CHECK.txt"
        "Entropy/health policy check was not requested. Set ANO_CRYPTO_RUN_ENTROPY_HEALTH_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK)
    set(_entropy_operational_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_OPERATIONAL_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT=${_entropy_operational_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_entropy_operational_policy.cmake"
        RESULT_VARIABLE _entropy_operational_check_result
        OUTPUT_VARIABLE _entropy_operational_check_stdout
        ERROR_VARIABLE _entropy_operational_check_stderr
        TIMEOUT 300)
    file(APPEND "${_entropy_operational_check_output}" "\nCommand output:\n${_entropy_operational_check_stdout}${_entropy_operational_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Entropy operational policy check result: ${_entropy_operational_check_result}\n")
    if(NOT _entropy_operational_check_result EQUAL 0)
        message(FATAL_ERROR "entropy operational policy check failed; see ${_entropy_operational_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_OPERATIONAL_POLICY_CHECK.txt"
        "Entropy operational policy check was not requested. Set ANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK=ON.\n")
endif()


if(ANO_CRYPTO_RUN_ENTROPY_DRBG_POLICY_CHECK)
    set(_entropy_drbg_policy_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_DRBG_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_ENTROPY_DRBG_POLICY_CHECK_OUTPUT=${_entropy_drbg_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_entropy_drbg_policy.cmake"
        RESULT_VARIABLE _entropy_drbg_policy_check_result
        OUTPUT_VARIABLE _entropy_drbg_policy_check_stdout
        ERROR_VARIABLE _entropy_drbg_policy_check_stderr
        TIMEOUT 300)
    file(APPEND "${_entropy_drbg_policy_check_output}"
        "\nCommand output:\n${_entropy_drbg_policy_check_stdout}${_entropy_drbg_policy_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-04 entropy/DRBG policy check result: ${_entropy_drbg_policy_check_result}\n")
    if(NOT _entropy_drbg_policy_check_result EQUAL 0)
        message(FATAL_ERROR
            "P2-04 entropy/DRBG policy check failed; see ${_entropy_drbg_policy_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_DRBG_POLICY_CHECK.txt"
        "P2-04 entropy/DRBG policy check was not requested. Set ANO_CRYPTO_RUN_ENTROPY_DRBG_POLICY_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_CSP_ZEROIZATION_POLICY_CHECK)
    set(_csp_zeroization_policy_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/CSP_ZEROIZATION_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_CSP_ZEROIZATION_POLICY_CHECK_OUTPUT=${_csp_zeroization_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_csp_zeroization_policy.cmake"
        RESULT_VARIABLE _csp_zeroization_policy_check_result
        OUTPUT_VARIABLE _csp_zeroization_policy_check_stdout
        ERROR_VARIABLE _csp_zeroization_policy_check_stderr
        TIMEOUT 300)
    file(APPEND "${_csp_zeroization_policy_check_output}"
        "\nCommand output:\n${_csp_zeroization_policy_check_stdout}${_csp_zeroization_policy_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-05 CSP/zeroization policy check result: ${_csp_zeroization_policy_check_result}\n")
    if(NOT _csp_zeroization_policy_check_result EQUAL 0)
        message(FATAL_ERROR "P2-05 CSP/zeroization policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CSP_ZEROIZATION_POLICY_CHECK.txt"
        "P2-05 CSP/zeroization policy check was not requested.\n")
endif()

if(ANO_CRYPTO_RUN_SECURE_CODING_POLICY_CHECK)
    set(_secure_coding_policy_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_SECURE_CODING_CHECK_OUTPUT=${_secure_coding_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_secure_coding_side_channel_policy.cmake"
        RESULT_VARIABLE _secure_coding_policy_check_result
        OUTPUT_VARIABLE _secure_coding_policy_check_stdout
        ERROR_VARIABLE _secure_coding_policy_check_stderr
        TIMEOUT 300)
    file(APPEND "${_secure_coding_policy_check_output}"
        "\nCommand output:\n${_secure_coding_policy_check_stdout}${_secure_coding_policy_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-06 secure-coding/side-channel policy check result: ${_secure_coding_policy_check_result}\n")
    if(NOT _secure_coding_policy_check_result EQUAL 0)
        message(FATAL_ERROR "P2-06 secure-coding/side-channel policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt"
        "P2-06 secure-coding/side-channel policy check was not requested.\n")
endif()


if(ANO_CRYPTO_RUN_SUBMISSION_DOCUMENT_POLICY_CHECK)
    set(_submission_document_policy_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/SUBMISSION_DOCUMENT_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_SUBMISSION_DOCUMENT_CHECK_OUTPUT=${_submission_document_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_submission_document_policy.cmake"
        RESULT_VARIABLE _submission_document_policy_check_result
        OUTPUT_VARIABLE _submission_document_policy_check_stdout
        ERROR_VARIABLE _submission_document_policy_check_stderr
        TIMEOUT 300)
    file(APPEND "${_submission_document_policy_check_output}"
        "
Command output:
${_submission_document_policy_check_stdout}${_submission_document_policy_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-07 submission-document policy check result: ${_submission_document_policy_check_result}
")
    if(NOT _submission_document_policy_check_result EQUAL 0)
        message(FATAL_ERROR "P2-07 submission-document policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SUBMISSION_DOCUMENT_POLICY_CHECK.txt"
        "P2-07 submission-document policy check was not requested.
")
endif()

if(ANO_CRYPTO_RUN_PRECONSULTATION_REHEARSAL_CHECK)
    set(_preconsultation_policy_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/PRECONSULTATION_REHEARSAL_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_PRECONSULTATION_CHECK_OUTPUT=${_preconsultation_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_preconsultation_rehearsal_policy.cmake"
        RESULT_VARIABLE _preconsultation_policy_check_result
        OUTPUT_VARIABLE _preconsultation_policy_check_stdout
        ERROR_VARIABLE _preconsultation_policy_check_stderr
        TIMEOUT 300)
    file(APPEND "${_preconsultation_policy_check_output}"
        "
Command output:
${_preconsultation_policy_check_stdout}${_preconsultation_policy_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-08 pre-consultation rehearsal policy check result: ${_preconsultation_policy_check_result}
")
    if(NOT _preconsultation_policy_check_result EQUAL 0)
        message(FATAL_ERROR "P2-08 pre-consultation rehearsal policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/PRECONSULTATION_REHEARSAL_POLICY_CHECK.txt"
        "P2-08 pre-consultation rehearsal policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/PRECONSULTATION_REHEARSAL_NOTES.txt"
    "AnoCrypto-C P2-08 pre-consultation rehearsal notes
"
    "Policy ID: PCR-P2-08-R1
"
    "Status: COMPLETE_LOCAL_SCRIPTED; live internal rehearsal NOT_RUN.
"
    "External consultation: NOT_CONDUCTED.
"
    "Institution responses: NOT_RECEIVED.
"
    "Runtime integrity: NOT_IMPLEMENTED.
"
    "Exact OE-01: NOT_RUN.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/handoff/authorization: UNSIGNED/BLOCKED/NO.
"
    "Public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")

if(ANO_CRYPTO_RUN_P2_CLOSEOUT_CHECK)
    set(_p2_closeout_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/P2_CLOSEOUT_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_P2_CLOSEOUT_CHECK_OUTPUT=${_p2_closeout_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_p2_closeout_policy.cmake"
        RESULT_VARIABLE _p2_closeout_check_result
        OUTPUT_VARIABLE _p2_closeout_check_stdout
        ERROR_VARIABLE _p2_closeout_check_stderr
        TIMEOUT 300)
    file(APPEND "${_p2_closeout_check_output}"
        "
Command output:
${_p2_closeout_check_stdout}${_p2_closeout_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-09 cumulative closeout policy check result: ${_p2_closeout_check_result}
")
    if(NOT _p2_closeout_check_result EQUAL 0)
        message(FATAL_ERROR "P2-09 cumulative closeout policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/P2_CLOSEOUT_POLICY_CHECK.txt"
        "P2-09 cumulative closeout policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/P2_CLOSEOUT_NOTES.txt"
    "AnoCrypto-C P2-09 cumulative validation-readiness closeout notes
"
    "Policy ID: P2C-P2-09-R1
"
    "Status: COMPLETE_LOCAL_CLOSEOUT.
"
    "External consultation/institution responses: NOT_CONDUCTED/NOT_RECEIVED.
"
    "Formal validation: NOT_STARTED.
"
    "Runtime integrity/exact OE-01: NOT_IMPLEMENTED/NOT_RUN.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/handoff: UNSIGNED/BLOCKED.
"
    "Submission/external/next implementation authorization: NO/NO/NO.
"
    "Public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")

if(ANO_CRYPTO_RUN_LIVE_REHEARSAL_CHECK)
    set(_live_rehearsal_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/LIVE_REHEARSAL_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_LIVE_REHEARSAL_CHECK_OUTPUT=${_live_rehearsal_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_live_rehearsal_policy.cmake"
        RESULT_VARIABLE _live_rehearsal_check_result
        OUTPUT_VARIABLE _live_rehearsal_check_stdout
        ERROR_VARIABLE _live_rehearsal_check_stderr
        TIMEOUT 300)
    file(APPEND "${_live_rehearsal_check_output}"
        "
Command output:
${_live_rehearsal_check_stdout}${_live_rehearsal_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-10 assisted rehearsal policy check result: ${_live_rehearsal_check_result}
")
    if(NOT _live_rehearsal_check_result EQUAL 0)
        message(FATAL_ERROR "P2-10 assisted rehearsal policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/LIVE_REHEARSAL_POLICY_CHECK.txt"
        "P2-10 assisted rehearsal policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/LIVE_REHEARSAL_NOTES.txt"
    "AnoCrypto-C P2-10 assisted pre-consultation rehearsal notes
"
    "Policy ID: LAR-P2-10-R1
"
    "Status: COMPLETE_LOCAL_ASSISTED.
"
    "Independent human live rehearsal: NOT_RUN.
"
    "External consultation/institution responses: NOT_CONDUCTED/NOT_RECEIVED.
"
    "Institution-contact decision: CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION.
"
    "Runtime integrity/exact OE-01: NOT_IMPLEMENTED/NOT_RUN.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/handoff/authorization: UNSIGNED/BLOCKED/NO.
"
    "Public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")

if(ANO_CRYPTO_RUN_HUMAN_REHEARSAL_HANDOFF_CHECK)
    set(_hrh_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/HUMAN_REHEARSAL_HANDOFF_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_HRH_CHECK_OUTPUT=${_hrh_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_human_rehearsal_handoff_policy.cmake"
        RESULT_VARIABLE _hrh_check_result
        OUTPUT_VARIABLE _hrh_check_stdout
        ERROR_VARIABLE _hrh_check_stderr
        TIMEOUT 300)
    file(APPEND "${_hrh_check_output}"
        "
Command output:
${_hrh_check_stdout}${_hrh_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-11 human rehearsal handoff policy check result: ${_hrh_check_result}
")
    if(NOT _hrh_check_result EQUAL 0)
        message(FATAL_ERROR "P2-11 human rehearsal handoff policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/HUMAN_REHEARSAL_HANDOFF_POLICY_CHECK.txt"
        "P2-11 human rehearsal handoff policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/HUMAN_REHEARSAL_HANDOFF_NOTES.txt"
    "AnoCrypto-C P2-11 independent human rehearsal handoff notes
"
    "Policy ID: HRH-P2-11-R1
"
    "Status: READY_FOR_NAMED_HUMAN_EXECUTION.
"
    "Reviewer identity/independence: UNASSIGNED/NOT_RECORDED.
"
    "Independent human live rehearsal: NOT_RUN.
"
    "Project-owner/contact authorization: NOT_RECORDED/NO.
"
    "External consultation/institution responses: NOT_CONDUCTED/NOT_RECEIVED.
"
    "Runtime integrity/exact OE-01: NOT_IMPLEMENTED/NOT_RUN.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/handoff/authorization: UNSIGNED/BLOCKED/NO.
"
    "Public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")


if(ANO_CRYPTO_RUN_CONSULTATION_CONTACT_READINESS_CHECK)
    set(_ccr_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/CONSULTATION_CONTACT_READINESS_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_CCR_CHECK_OUTPUT=${_ccr_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_consultation_contact_readiness_policy.cmake"
        RESULT_VARIABLE _ccr_check_result
        OUTPUT_VARIABLE _ccr_check_stdout
        ERROR_VARIABLE _ccr_check_stderr
        TIMEOUT 300)
    file(APPEND "${_ccr_check_output}"
        "
Command output:
${_ccr_check_stdout}${_ccr_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P2-12 consultation contact-readiness policy check result: ${_ccr_check_result}
")
    if(NOT _ccr_check_result EQUAL 0)
        message(FATAL_ERROR "P2-12 consultation contact-readiness policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CONSULTATION_CONTACT_READINESS_POLICY_CHECK.txt"
        "P2-12 consultation contact-readiness policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CONSULTATION_CONTACT_READINESS_NOTES.txt"
    "AnoCrypto-C P2-12 consultation contact-readiness notes
"
    "Policy ID: CCR-P2-12-R1
"
    "Status: READY_FOR_OWNER_SELECTION_AFTER_HUMAN_REHEARSAL.
"
    "Human rehearsal/owner authorization: NOT_RUN/NOT_RECORDED.
"
    "Institution/sender/channel: UNSELECTED/UNASSIGNED/UNSELECTED.
"
    "First contact/consultation/responses: NOT_SENT/NOT_CONDUCTED/NOT_RECEIVED.
"
    "Runtime integrity/exact OE-01: NOT_IMPLEMENTED/NOT_RUN.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/handoff/authorization: UNSIGNED/BLOCKED/NO.
"
    "Public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")

if(ANO_CRYPTO_RUN_DOWNSTREAM_REQUIREMENT_ANALYSIS_CHECK)
    set(_dri_check_output
        "${ANO_CRYPTO_EVIDENCE_DIR}/DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_DRA_CHECK_OUTPUT=${_dri_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_downstream_requirement_analysis_policy.cmake"
        RESULT_VARIABLE _dri_check_result
        OUTPUT_VARIABLE _dri_check_stdout
        ERROR_VARIABLE _dri_check_stderr
        TIMEOUT 300)
    file(APPEND "${_dri_check_output}"
        "
Command output:
${_dri_check_stdout}${_dri_check_stderr}
")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "P3-01 downstream requirement analysis policy check result: ${_dri_check_result}
")
    if(NOT _dri_check_result EQUAL 0)
        message(FATAL_ERROR "P3-01 downstream requirement analysis policy check failed")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY_CHECK.txt"
        "P3-01 downstream requirement analysis policy check was not requested.
")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DOWNSTREAM_REQUIREMENT_ANALYSIS_NOTES.txt"
    "AnoCrypto-C P3-01 downstream requirement analysis notes
"
    "Policy ID: DRA-P3-01-R1
"
    "Status: DOWNSTREAM_REQUIREMENTS_CONSOLIDATED.
"
    "Imported source sets/conflicts: 4/1.
"
    "Accepted requirements/approved gaps: 20/8.
"
    "Selected implementation: P3-02_HKDF_SHA256_COMPLETE; P3-03_AES256_GCM_ONESHOT_COMPLETE; P3-04_AES256_GCM_STREAMING_COMPLETE; P3-05_SCRYPT_FIXED_PROFILE_COMPLETE; P3-08_RSA_PKCS1_V15_SHA2_COMPLETE; P3-09_ECDSA_P256_SHA256_COMPLETE.
"
    "P3-01 frozen public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Public API/new ABI authorization: P3_02_THROUGH_P3_09_ADDITIVE/ANOCRYPTO_C_1_HKDF_ANOCRYPTO_C_2_GCM_ONESHOT_ANOCRYPTO_C_3_GCM_STREAMING_ANOCRYPTO_C_4_SCRYPT_ANOCRYPTO_C_5_RSA_ANOCRYPTO_C_6_ECDSA.
"
    "KCMVP active lane: DEFERRED.
"
    "P1-34 hosted lanes/review: NOT_RUN/UNREVIEWED.
"
    "Repository signing/external authorization: UNSIGNED/NO.
"
    "P3-01 public ABI: 14 headers, 69 symbols, ANOCRYPTO_C_0.
"
    "Sensitive values recorded: none.
")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CSP_ZEROIZATION_REVIEW_NOTES.txt"
    "AnoCrypto-C P2-05 CSP and zeroization review notes\n"
    "Policy ID: CZP-P2-05-R1\n"
    "Status: internal engineering evidence; formal classification and selected-OE review pending.\n"
    "Implemented purposes: HMAC, BLOCK_CIPHER, CMAC.\n"
    "Module-owned table wipe: orderly shutdown and locked fatal transition implemented in P2-05.\n"
    "Caller-owned contexts: explicit uninit required; module does not retain addresses.\n"
    "Export, persistence, wrapping, and multi-process sharing: out of scope.\n"
    "Selected OE compiler review: NOT_RUN.\n"
    "Sensitive values recorded: none.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SECURE_CODING_REVIEW_NOTES.txt"
    "AnoCrypto-C P2-06 secure-coding and side-channel review notes\n"
    "Policy ID: SCP-P2-06-R1\n"
    "Status: local source/compiler/build review with open exact-OE and external findings.\n"
    "Source AES profile: table-free fixed-work candidate; no universal constant-time claim.\n"
    "CMAC verify profile: fixed 16-byte comparison candidate; empirical timing is NOT_RUN.\n"
    "Local analyzers: GCC reviewed false positive, Clang strict PASS after internal switch fix, Clang analyzer PASS.\n"
    "Build hardening: FORTIFY 3, strong stack protector, stack-clash protection, full RELRO/BIND_NOW, NX stack.\n"
    "Exact OE-01 GCC 13.3.0 review and named human disposition: NOT_RUN/TODO.\n"
    "Power, EM, transient-execution, and active-fault assessment: NOT_EVALUATED.\n"
    "Hosted lanes/review: NOT_RUN/UNREVIEWED.\n"
    "Sensitive values recorded: none.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/REPRODUCIBLE_BUILD_NOTES.txt"
    "AnoCrypto-C reproducible build evidence notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-33 retained draft procedure; binary byte identity is not claimed.\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n"
    "Source archive content list: SOURCE_ARCHIVE_CONTENTS.txt\n"
    "Source hash list: SOURCE_FILES.sha256\n"
    "Binary hash list: BINARY_FILES.sha256\n"
    "Public surface check: PUBLIC_SURFACE_CHECK.txt\n"
    "Validation candidate check: VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt\n"
    "Self-test/integrity policy check: SELF_TEST_INTEGRITY_POLICY_CHECK.txt\n"
    "Thread-safety policy check: THREAD_SAFETY_CHECK.txt\n"
    "SHA-512-family implementation check: SHA512_FAMILY_CHECK.txt\n"
    "Random-service implementation policy check: RANDOM_SERVICE_CHECK.txt\n"
    "Random lifecycle hardening policy check: RANDOM_LIFECYCLE_HARDENING_CHECK.txt\n"
    "Clean Release exported-symbol baseline: EXPORTED_SYMBOL_BASELINE_CHECK.txt\n"
    "Entropy/health policy check: ENTROPY_HEALTH_CHECK.txt\n"
    "Entropy operational policy check: ENTROPY_OPERATIONAL_POLICY_CHECK.txt\n"
    "P2-04 entropy/DRBG policy check: ENTROPY_DRBG_POLICY_CHECK.txt\n"
    "P2-05 CSP/zeroization policy check: CSP_ZEROIZATION_POLICY_CHECK.txt\n"
    "P2-06 secure-coding/side-channel policy check: SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt\n"
    "P2-07 submission-document policy check: SUBMISSION_DOCUMENT_POLICY_CHECK.txt\n"
    "P2-08 pre-consultation rehearsal policy check: PRECONSULTATION_REHEARSAL_POLICY_CHECK.txt\n"
    "P2-09 cumulative closeout policy check: P2_CLOSEOUT_POLICY_CHECK.txt\n"
    "P2-10 assisted rehearsal policy check: LIVE_REHEARSAL_POLICY_CHECK.txt\n"
    "P2-11 human rehearsal handoff policy check: HUMAN_REHEARSAL_HANDOFF_POLICY_CHECK.txt\n"
    "P2-12 consultation contact-readiness policy check: CONSULTATION_CONTACT_READINESS_POLICY_CHECK.txt\n"
    "P3-01 downstream requirement analysis policy check: DOWNSTREAM_REQUIREMENT_ANALYSIS_POLICY_CHECK.txt\n"
    "P3-02 HKDF-SHA-256 policy check: HKDF_SHA256_POLICY_CHECK.txt\n"
    "P3-03 AES-256-GCM one-shot policy check: AES256_GCM_POLICY_CHECK.txt\n"
    "P3-04 AES-256-GCM streaming policy check: AES256_GCM_STREAMING_POLICY_CHECK.txt\n"
    "P3-05 fixed-profile scrypt policy check: SCRYPT_POLICY_CHECK.txt\n"
    "Differential/portability policy check: DIFFERENTIAL_PORTABILITY_CHECK.txt\n"
    "AES/CBC implementation policy check: AES_CBC_IMPLEMENTATION_CHECK.txt\n"
    "AES/CBC hardening policy check: AES_HARDENING_CHECK.txt\n"
    "AES-CMAC implementation policy check: AES_CMAC_IMPLEMENTATION_CHECK.txt\n"
    "AES-CMAC hardening policy check: AES_CMAC_HARDENING_CHECK.txt\n"
    "AES compiler-output review: AES_COMPILER_OUTPUT_REVIEW.txt\n"
    "CMAC compiler-output review: CMAC_COMPILER_OUTPUT_REVIEW.txt\n"
    "Zeroization compiler-output review: ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt\n"
    "P2-06 compiler/CPU review: SECURE_CODING_COMPILER_CPU_REVIEW.txt\n"
    "P2-06 ELF hardening review: ELF_HARDENING_REVIEW.txt\n"
    "TODO: normalize archive timestamps if byte-for-byte source archives become a release requirement.\n"
    "TODO: pin generator, compiler, locale, timezone, debug-info, and path-prefix-map policies for release-candidate builds.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SCOPE_REVIEW_NOTES.txt"
    "AnoCrypto-C P3-09 evidence and scope-review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P3-09 ECDSA P-256 SHA-256 implementation and additive ABI evidence; P3-08 RSA remains retained. P3-04 streaming, P3-03 one-shot, and P3-02 HKDF-SHA-256 remain retained; P3-01 remains the accepted downstream requirement baseline.\n"
    "Development workflow: docs/roadmap/DEVELOPMENT_WORKFLOW.md\n"
    "Service scope matrix: docs/kcmvp/SERVICE_SCOPE_MATRIX.md\n"
    "Algorithm scope: docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md\n"
    "SHA-512-family decision: docs/design/SHA2_384_512_SCOPE_DECISION.md\n"
    "SHA-512-family KAT record: docs/kcmvp/SHA2_384_512_KAT_PLAN.md\n"
    "SHA-512-family implementation check: SHA512_FAMILY_CHECK.txt\n"
    "Current implemented algorithm slice includes SHA-256/SHA-384/SHA-512, their HMAC variants, HMAC_DRBG-SHA-256, generic HKDF-SHA-256, fixed-profile scrypt, and generic AES-256-GCM one-shot/streaming.\n"
    "The existing platform-provider-backed random service and CSP HMAC key lifecycle remain in the candidate engineering slice.\n"
    "AES-128/192/256, CBC without padding, full-tag AES-CMAC, fixed-profile AES-256-GCM one-shot/streaming, and fixed-profile scrypt are implemented. Generic nonce/tag or scrypt profiles, padding helpers, GMAC, hardware dispatch, tag truncation, and additional algorithms remain deferred.\n"
    "OpenSSL 3.x test-only differential automation covers SHA-2/HMAC, HKDF-SHA-256, fixed-profile scrypt, AES-CBC, AES-CMAC, AES-256-GCM, RSA, and ECDSA P-256; actual hosted 32-bit, Windows, macOS, big-endian, and target-specific side-channel artifacts remain review items.\n"
    "Do not include digest values, MAC values, keys, entropy bytes, seed material, DRBG state, generated random bytes, private keys, tokens, customer data, or internal deployment details in scope evidence.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Scope review notes: SCOPE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/HKDF_SHA256_REVIEW_NOTES.txt"
    "AnoCrypto-C P3-02 HKDF-SHA-256 review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: local engineering implementation and compatibility evidence; not formal validation evidence.\n"
    "Public API: one combined extract-and-expand function in include/anocrypto/ac_kdf.h.\n"
    "ABI: all 69 ANOCRYPTO_C_0 symbols retained; HKDF remains on ANOCRYPTO_C_1; one-shot GCM remains on ANOCRYPTO_C_2; seven streaming symbols remain on ANOCRYPTO_C_3; fixed-profile scrypt is on ANOCRYPTO_C_4; RSA import/sign/verify remain on ANOCRYPTO_C_5; ECDSA P-256 import is on ANOCRYPTO_C_6; current total is 84.\n"
    "Bounds: output lengths 0 through 8160 accepted; 8161 rejected without partial output.\n"
    "Semantics: RFC 5869 empty salt, empty IKM, empty info, zero-length output, range, alias, overlap, and failure-clearing rules are documented and tested.\n"
    "Compatibility: exact AnoSecureKit SKF1 v1 info bytes are covered by a fixed test vector; SKF1 framing and workflow remain AnoSecureKit-owned.\n"
    "Differential provider: OpenSSL 3.x is test-only and is not linked by the product library.\n"
    "KCMVP/hosted/signing/external release: DEFERRED/NOT_RUN-UNREVIEWED/UNSIGNED/NO.\n"
    "Sensitive values recorded in this review note: none.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "HKDF-SHA-256 review notes: HKDF_SHA256_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_REVIEW_NOTES.txt"
    "AnoCrypto-C P3-03 AES-256-GCM one-shot review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: local engineering implementation and compatibility evidence; not formal validation evidence.\n"
    "Public API: two one-shot functions in include/anocrypto/ac_aead.h for encrypt and decrypt.\n"
    "Fixed profile: 32-byte block-cipher CSP key, 12-byte nonce, and 16-byte tag.\n"
    "ABI: the P3-03 slice added two AES-256-GCM symbols on ANOCRYPTO_C_2 for a then-current total of 72; P3-09 retains them inside the current 84-symbol surface.\n"
    "Buffer rules: empty AAD/payload valid, exact payload in-place supported, partial overlap rejected, and length/overflow ranges checked.\n"
    "Authentication release rule: decrypt verifies the complete tag before writing plaintext; validation or authentication failure returns no partial output and clears the accepted output range.\n"
    "Compatibility: exact AnoSecureKit SKT1, SKF1, and SKP1 v1 GCM inputs/outputs are covered without implementing product framing, nonce construction, password derivation, or workflows.\n"
    "Differential provider: OpenSSL 3.x is test-only and is not linked by the product library.\n"
    "KCMVP/hosted/signing/external release: DEFERRED/NOT_RUN-UNREVIEWED/UNSIGNED/NO.\n"
    "Community dependency identity conflict: OPEN_DOWNSTREAM_SYNC_DECISION.\n"
    "Sensitive values recorded in this review note: none.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "AES-256-GCM review notes: AES256_GCM_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES256_GCM_STREAMING_REVIEW_NOTES.txt"
    "AnoCrypto-C P3-04 AES-256-GCM streaming review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: local engineering implementation and compatibility evidence; not formal validation evidence.\n"
    "Public API: seven caller-owned opaque-context functions in include/anocrypto/ac_aead.h.\n"
    "Context storage: malloc/calloc/realloc or equivalent C-object alignment is required; misaligned storage is rejected before internal access.\n"
    "State model: direction bound at init; AAD before non-empty payload; correct-direction final consumes and clears context.\n"
    "Decrypt release policy: update output is unverified until final succeeds; caller must buffer, withhold, and wipe it on failure.\n"
    "Buffer policy: exact payload in-place is supported; partial overlap and out_written overlap with context/input/output are rejected.\n"
    "ABI: 69 ANOCRYPTO_C_0, one ANOCRYPTO_C_1, two ANOCRYPTO_C_2, seven ANOCRYPTO_C_3, one ANOCRYPTO_C_4, three ANOCRYPTO_C_5 symbols, and one ANOCRYPTO_C_6 symbol; current total 84.\n"
    "Compatibility: exact AnoSecureKit SKT1 v1 primitive bytes and arbitrary chunk boundaries are covered without implementing framing or product workflow.\n"
    "Differential provider: OpenSSL 3.x is test-only and is not linked by the product library.\n"
    "KCMVP/hosted/signing/external release: DEFERRED/NOT_RUN-UNREVIEWED/UNSIGNED/NO.\n"
    "Community dependency identity conflict: OPEN_DOWNSTREAM_SYNC_DECISION.\n"
    "Sensitive values recorded in this review note: none.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "AES-256-GCM streaming review notes: AES256_GCM_STREAMING_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SCRYPT_REVIEW_NOTES.txt"
    "AnoCrypto-C P3-05 fixed-profile scrypt review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: local engineering implementation, resource, and compatibility evidence; not formal validation evidence.\n"
    "Public API: one fixed-profile function in include/anocrypto/ac_kdf.h.\n"
    "Profile: N=32768, r=8, p=1, 32-byte output, and 33,556,544-byte caller workspace.\n"
    "Resource boundary: no production heap allocation; undersized or misaligned workspace and all prohibited overlaps fail closed.\n"
    "Clearing: accepted output is cleared on failure and the complete reviewed workspace extent is cleared after every derivation attempt that begins using it.\n"
    "ABI: one symbol on ANOCRYPTO_C_4; retained ABI-0/1/2/3 lines unchanged; RSA remains on ANOCRYPTO_C_5; ECDSA import adds one symbol on ANOCRYPTO_C_6; current total 84.\n"
    "Compatibility: exact AnoSecureKit SKP1 v1 key derivation and record decryption are covered without implementing framing, prompting, password policy, storage, or workflow.\n"
    "Differential provider: OpenSSL 3.x EVP_PBE_scrypt is test-only and is not linked by the product library.\n"
    "KCMVP/hosted/signing/external release: DEFERRED/NOT_RUN-UNREVIEWED/UNSIGNED/NO.\n"
    "Community dependency identity conflict: OPEN_DOWNSTREAM_SYNC_DECISION.\n"
    "Sensitive values recorded in this review note: none.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "scrypt review notes: SCRYPT_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_FAMILY_REVIEW_NOTES.txt"
    "AnoCrypto-C SHA-384/SHA-512 family review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained implementation, KAT, differential, and portability evidence; not official validation evidence.\n"
    "Implementation decision: docs/design/SHA2_384_512_SCOPE_DECISION.md\n"
    "KAT record: docs/kcmvp/SHA2_384_512_KAT_PLAN.md\n"
    "Implementation: src/hash/sha512.c, src/hash/hash.c, src/mac/hmac.c\n"
    "Startup self-tests: src/selftest/selftest.c\n"
    "Extended KAT: tests/kat/sha512_family_kat_tests.c\n"
    "Negative API test: tests/negative/sha512_family_api_negative_tests.c\n"
    "CSP handle coverage: tests/kat/hmac_key_handle_kat_tests.c\n"
    "Policy check: SHA512_FAMILY_CHECK.txt\n"
    "Current behavior: SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 one-shot, streaming, and HMAC key-handle paths are implemented.\n"
    "Public sources: SRC-1001 NIST FIPS 180-4 and SRC-1002 RFC 4231.\n"
    "OpenSSL 3.x test-only differential automation is recorded separately in DIFFERENTIAL_VALIDATION.log and does not replace KAT provenance.\n"
    "Evidence stores statuses and source identifiers only; digest values, MAC values, and HMAC key bytes are not retained.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "SHA-512-family review notes: SHA512_FAMILY_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CBC_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-26 AES/CBC hardening review notes\n"
    "Status: engineering evidence only; not official validation evidence.\n"
    "Implementation policy check: AES_CBC_IMPLEMENTATION_CHECK.txt\n"
    "Hardening policy check: AES_HARDENING_CHECK.txt\n"
    "Runtime hardening review: AES_HARDENING_REVIEW.log\n"
    "Compiler-output summary: AES_COMPILER_OUTPUT_REVIEW.txt\n"
    "Public AES/CBC API: include/anocrypto/ac_cipher.h; eight reviewed cipher functions. Current overall baseline: 69 symbols after CMAC addition.\n"
    "Context policy: private magic and exact round-key-size guard; failed reinitialization clears an earlier schedule.\n"
    "Lifecycle: CSP handle may be destroyed after successful init; caller-owned expanded context remains valid until uninit and is then cleared.\n"
    "Differential provider: OpenSSL 3.x test-only with 2,016 AES-CBC comparisons; no SDK or PoC runtime dependency.\n"
    "Compiler-output interpretation: manual target-specific review required; no automatic side-channel verdict.\n"
    "Sensitive values retained in evidence: none.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "AES/CBC review notes: AES_CBC_REVIEW_NOTES.txt
")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt"
    "AnoCrypto-C SHA-2/HMAC differential-validation and portability review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained engineering comparison and portability evidence; not official validation evidence.\n"
    "Provider decision: docs/kcmvp/OPENSSL_DIFFERENTIAL_PROVIDER_DECISION.md\n"
    "Portability policy: docs/design/SHA512_FAMILY_PORTABILITY_POLICY.md\n"
    "CI checklist: docs/ci/DIFFERENTIAL_PORTABILITY_CI_CHECKLIST.md\n"
    "Policy check: DIFFERENTIAL_PORTABILITY_CHECK.txt\n"
    "Differential result: DIFFERENTIAL_VALIDATION.log\n"
    "Portability result: SHA512_PORTABILITY_REVIEW.log\n"
    "OpenSSL 3.x is linked only to differential test executables and is not a product, installed-SDK, or PoC runtime dependency.\n"
    "The deterministic corpus covers SHA-256/SHA-384/SHA-512 and HMAC variants plus AES-CBC and AES-CMAC across one-shot, streaming, boundary, long-key, and intentionally unaligned-input cases.\n"
    "Logs retain provider/version, status, representation metadata, and comparison counts only; digest, MAC, key, message, entropy, seed, DRBG state, and generated random values are prohibited.\n"
    "Actual hosted 32-bit, Windows, macOS, and big-endian artifacts require explicit review before support wording changes.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Differential/portability review notes: DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_SERVICE_REVIEW_NOTES.txt"
    "AnoCrypto-C random-service implementation review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained engineering lifecycle-hardening evidence; not official validation evidence.\n"
    "Decision document: docs/design/PUBLIC_RANDOM_API_DECISION.md\n"
    "CI checklist: docs/ci/RANDOM_SERVICE_CI_CHECKLIST.md\n"
    "DRBG/entropy evidence notes: docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md\n"
    "Public header: include/anocrypto/ac_random.h\n"
    "Implementation: src/drbg/random.c\n"
    "Lifecycle/fork test: tests/unit/random_service_tests.c\n"
    "Failure-injection test: tests/negative/random_service_failure_injection_tests.c\n"
    "Policy check: RANDOM_SERVICE_CHECK.txt\n"
    "Current behavior: the public service instantiates and reseeds HMAC_DRBG-SHA-256 through the reviewed platform entropy seam where available.\n"
    "A detected process change requires explicit random_reseed before generation. Missing process-marker support fails closed; VM snapshot/restore and clone events require external discard-and-reinitialize policy because automatic detection is not implemented.\n"
    "Generated random bytes, raw entropy, nonce seed material, DRBG state, private keys, tokens, and customer data must not be stored in evidence.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Random service review notes: RANDOM_SERVICE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RANDOM_LIFECYCLE_REVIEW_NOTES.txt"
    "AnoCrypto-C random lifecycle and portability review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained engineering regression and policy evidence; not official validation evidence.\n"
    "Policy: docs/design/C99_RANDOM_OPERATIONAL_PORTABILITY_POLICY.md\n"
    "CI checklist: docs/ci/RANDOM_LIFECYCLE_HARDENING_CI_CHECKLIST.md\n"
    "Lifecycle stress: tests/stress/random_lifecycle_stress_tests.c\n"
    "Hosted platform status review: tests/operational/random_platform_review.c\n"
    "Dedicated reseed-limit regression: tests/negative/random_reseed_limit_tests.c\n"
    "Policy check: RANDOM_LIFECYCLE_HARDENING_CHECK.txt\n"
    "Current behavior: service availability requires both a reviewed platform provider and a supported process marker. Unsupported process-marker platforms fail closed.\n"
    "VM snapshot/restore or clone events are not detected automatically; known duplication events require discarding inherited state and independently reinitializing each instance.\n"
    "Suspend/resume handling remains a product integration policy. Strict threat models should discard and reinitialize state; any explicit reseed-only policy requires documented platform review.\n"
    "Evidence records status, operation counts, and policy only. Generated bytes, entropy, seed material, and DRBG state are prohibited.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Random lifecycle review notes: RANDOM_LIFECYCLE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt"
    "AnoCrypto-C exported-symbol baseline review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P3-09 additive clean Release ABI guardrail; not a complete ABI compatibility claim.\n"
    "Retained baseline: abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt\n"
    "Current manifest: abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt\n"
    "Expected retained/current symbol counts: 69/84\n"
    "Automated result: EXPORTED_SYMBOL_BASELINE_CHECK.txt\n"
    "The check compares a clean shared Release build with test hooks disabled and internal symbol hiding enabled.\n"
    "Public struct layout, calling convention, type size, semantic compatibility, and platform-specific binary details remain separate review items.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Exported symbol baseline review notes: EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/RELEASE_COMPATIBILITY_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-33 release compatibility review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Current version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Baseline: ${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\n"
    "Public headers: ${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}\n"
    "Current ABI line / total exports: ${ANO_CRYPTO_RELEASE_ABI_LINE} / ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "Automated policy: RELEASE_COMPATIBILITY_CHECK.txt\n"
    "Installed consumer smoke: TAGGED_COMPATIBILITY_SMOKE.txt\n"
    "Hosted artifact execution: NOT_RUN; review: UNREVIEWED. Local evidence does not establish Windows, macOS, Linux 32-bit, or big-endian compatibility.\n"
    "Actual tag: NOT_CREATED; tag verification: NOT_RUN. Expected metadata is recorded, while real tag/commit identity requires a tagged Git checkout.\n"
    "Limit: normalized source declarations and symbol references are engineering guardrails, not universal ABI or semantic compatibility proof.\n"
    "Sensitive cryptographic values retained by this review: no.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Release compatibility review notes: RELEASE_COMPATIBILITY_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_HEALTH_REVIEW_NOTES.txt"
    "AnoCrypto-C entropy failure-injection and health-test review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained engineering testability and state-transition evidence only.\n"
    "Design: docs/design/C99_ENTROPY_FAILURE_INJECTION_AND_HEALTH_TEST.md\n"
    "CI checklist: docs/ci/ENTROPY_FAILURE_CI_CHECKLIST.md\n"
    "Policy check: ENTROPY_HEALTH_CHECK.txt\n"
    "Tests: entropy_source_negative_tests, entropy_health_tests, entropy_health_error_state_tests, and dedicated platform failure-injection lane when enabled.\n"
    "Health-test candidates are internal, disabled by default, and are not a substitute for a reviewed noise-source-specific test design.\n"
    "Fatal health-test failure maps to ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED, zeroizes affected output/context, and latches module ERROR_STATE.\n"
    "Evidence must record status and policy only; raw entropy, deterministic hook bytes, seed material, DRBG state, and generated secret bytes are prohibited.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Entropy health review notes: ENTROPY_HEALTH_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_OPERATIONAL_REVIEW_NOTES.txt"
    "AnoCrypto-C entropy source and operational process-boundary review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-29 retained engineering policy and regression evidence only.\n"
    "Design: docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md\n"
    "CI checklist: docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md\n"
    "Policy check: ENTROPY_OPERATIONAL_POLICY_CHECK.txt\n"
    "Tests: entropy_operational_policy_tests, process_guard_tests, and the dedicated platform failure-injection lane.\n"
    "OS APIs are classified as external conditioned random providers; the internal raw-noise health skeleton is not attached to OS-provider output.\n"
    "The operational probe is status-only; its success, forced-failure, and partial-failure paths are exercised only in the dedicated hook lane.\n"
    "A detected process identity change causes source-backed DRBG generation to return ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED and clear output until reseed succeeds.\n"
    "Explicit-entropy DRBG state remains caller-managed across process inheritance; this is a boundary record, not a fork-safety claim.\n"
    "The public random service is implemented through source-backed HMAC_DRBG-SHA-256 contexts. Evidence records status and policy only, not provider output, seed material, nonce material, DRBG state, or generated output.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Entropy operational review notes: ENTROPY_OPERATIONAL_REVIEW_NOTES.txt\n")


file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_DRBG_REVIEW_NOTES.txt"
    "AnoCrypto-C P2-04 entropy/provider/DRBG responsibility review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: frozen internal engineering policy; formal provider and health-test acceptance remains EXTERNAL_CONFIRMATION_REQUIRED.\n"
    "Policy ID: EDP-P2-04-R1\n"
    "Specification: docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md\n"
    "Responsibility matrix: docs/kcmvp/KCMVP_ENTROPY_RESPONSIBILITY_MATRIX.md\n"
    "Health/recovery matrix: docs/kcmvp/KCMVP_ENTROPY_HEALTH_AND_RECOVERY_MATRIX.md\n"
    "Implementation delta: docs/kcmvp/P2_04_IMPLEMENTATION_TEST_DELTA.md\n"
    "Policy check: ENTROPY_DRBG_POLICY_CHECK.txt\n"
    "Selected provider profile: ENT-OE01-LINUX-GETRANDOM-R1 using Linux getrandom(flags=0) as an external conditioned provider.\n"
    "Direct project-owned raw noise source: OUT_OF_SCOPE_INITIAL_CANDIDATE.\n"
    "Module health tests on conditioned provider output: disabled; generic duplicate-block candidates remain test-only.\n"
    "HMAC_DRBG limits: 32-byte entropy input, 16-byte nonce, 65,536-byte maximum generate request, and 2^48 generate-request reseed interval.\n"
    "Lifecycle: detected process changes require reseed; VM/container checkpoint duplication and suspend/resume are not automatically detected.\n"
    "Evidence records provider/status/length/count/hash metadata only. Entropy, nonce, DRBG state, and generated output are prohibited.\n"
    "Actual selected-OE and P1-34 hosted evidence remain NOT_RUN/BLOCKED_EXTERNAL.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "P2-04 entropy/DRBG review notes: ENTROPY_DRBG_REVIEW_NOTES.txt\n")

file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "SBOM status: SBOM_STATUS.txt\n"
    "Signing status: SIGNING_STATUS.txt\n")

if(ANO_CRYPTO_RUN_CTEST)
    message(FATAL_ERROR
        "Release evidence assembly does not run nested CTest; use ano_crypto_ctest_evidence first")
endif()
set(_prepared_ctest_evidence_dir "${ANO_CRYPTO_BUILD_DIR}/ctest-evidence")
set(_prepared_ctest_log "${_prepared_ctest_evidence_dir}/ctest.log")
set(_prepared_ctest_summary "${_prepared_ctest_evidence_dir}/CTEST_EVIDENCE_SUMMARY.txt")
if(NOT EXISTS "${_prepared_ctest_log}" OR IS_DIRECTORY "${_prepared_ctest_log}" OR
   IS_SYMLINK "${_prepared_ctest_log}" OR
   NOT EXISTS "${_prepared_ctest_summary}" OR IS_DIRECTORY "${_prepared_ctest_summary}" OR
   IS_SYMLINK "${_prepared_ctest_summary}")
    message(FATAL_ERROR "Prepared CTest evidence is missing or unsafe")
endif()
file(READ "${_prepared_ctest_summary}" _prepared_ctest_text)
if(NOT _prepared_ctest_text MATCHES "CTest result: 0")
    message(FATAL_ERROR "Prepared CTest evidence did not pass")
endif()
configure_file("${_prepared_ctest_log}" "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" COPYONLY)
configure_file("${_prepared_ctest_summary}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/CTEST_EVIDENCE_SUMMARY.txt" COPYONLY)
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "CTest result: 0 (prepared predecessor artifact)\n")

set(_prepared_rehearsal_report
    "${ANO_CRYPTO_BUILD_DIR}/tagged-release-rehearsal-test/TAGGED_RELEASE_REHEARSAL.txt")
if(NOT EXISTS "${_prepared_rehearsal_report}" OR
   IS_DIRECTORY "${_prepared_rehearsal_report}" OR
   IS_SYMLINK "${_prepared_rehearsal_report}")
    message(FATAL_ERROR "Prepared tagged-release rehearsal report is missing or unsafe")
endif()
file(STRINGS "${_prepared_rehearsal_report}" _rehearsal_magic_lines LIMIT_COUNT 1)
list(GET _rehearsal_magic_lines 0 _rehearsal_magic)
if(NOT _rehearsal_magic STREQUAL "ANOCRYPTO_TAGGED_RELEASE_REHEARSAL_V1")
    message(FATAL_ERROR "Prepared tagged-release rehearsal has invalid magic")
endif()
foreach(_key IN ITEMS release_version expected_tag mode source_commit rehearsal_commit
                       source_worktree_state source_expected_tag_before
                       source_expected_tag_after source_repository_unchanged
                       synthetic_tag_scope tag_commit_head_identity
                       dirty_worktree_rejection moved_tag_rejection
                       moved_head_rejection user_repository_tag_created tag_signing
                       actual_signature timestamp_response hosted_runner
                       hosted_artifact_review github_attestation
                       repository_signing_status signing_handoff
                       external_release_authorized release_rehearsal_status result)
    _ano_read_unique_prepared_key(
        "${_prepared_rehearsal_report}" "${_key}" "_rehearsal_${_key}")
endforeach()
if(NOT _rehearsal_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _rehearsal_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _rehearsal_synthetic_tag_scope STREQUAL "ISOLATED_REPOSITORY_ONLY" OR
   NOT _rehearsal_tag_commit_head_identity STREQUAL "PASS" OR
   NOT _rehearsal_dirty_worktree_rejection STREQUAL "PASS" OR
   NOT _rehearsal_moved_tag_rejection STREQUAL "PASS" OR
   NOT _rehearsal_moved_head_rejection STREQUAL "PASS" OR
   NOT _rehearsal_user_repository_tag_created STREQUAL "NO" OR
   NOT _rehearsal_tag_signing STREQUAL "NOT_RUN" OR
   NOT _rehearsal_actual_signature STREQUAL "NOT_CREATED" OR
   NOT _rehearsal_timestamp_response STREQUAL "NOT_CREATED" OR
   NOT _rehearsal_hosted_runner STREQUAL "NOT_RUN" OR
   NOT _rehearsal_hosted_artifact_review STREQUAL "UNREVIEWED" OR
   NOT _rehearsal_github_attestation STREQUAL "NOT_RUN_SECONDARY_ONLY" OR
   NOT _rehearsal_repository_signing_status STREQUAL "UNSIGNED" OR
   NOT _rehearsal_signing_handoff STREQUAL "BLOCKED" OR
   NOT _rehearsal_external_release_authorized STREQUAL "NO" OR
   NOT _rehearsal_result STREQUAL "PASS")
    message(FATAL_ERROR "Prepared tagged-release rehearsal crosses the P1-33 boundary")
endif()
if(_source_is_git_root)
    if(NOT _rehearsal_mode STREQUAL "CLEAN_GIT_CLONE" OR
       NOT _rehearsal_source_commit STREQUAL _git_commit OR
       NOT _rehearsal_rehearsal_commit STREQUAL _git_commit OR
       NOT _rehearsal_source_worktree_state STREQUAL "CLEAN" OR
       NOT _rehearsal_source_expected_tag_before STREQUAL "ABSENT" OR
       NOT _rehearsal_source_expected_tag_after STREQUAL "ABSENT" OR
       NOT _rehearsal_source_repository_unchanged STREQUAL "YES" OR
       NOT _rehearsal_release_rehearsal_status STREQUAL "PASS")
        message(FATAL_ERROR
            "Clean Git evidence requires current commit, absent tag, and source-invariant rehearsal")
    endif()
else()
    string(LENGTH "${_rehearsal_rehearsal_commit}" _archive_rehearsal_commit_length)
    if(NOT _rehearsal_mode STREQUAL "SYNTHETIC_ARCHIVE_ONLY" OR
       NOT _rehearsal_source_commit STREQUAL "NOT_AVAILABLE" OR
       NOT _rehearsal_source_worktree_state STREQUAL "NOT_A_GIT_WORKTREE" OR
       NOT _rehearsal_source_expected_tag_before STREQUAL "NOT_APPLICABLE" OR
       NOT _rehearsal_source_expected_tag_after STREQUAL "NOT_APPLICABLE" OR
       NOT _rehearsal_source_repository_unchanged STREQUAL "NOT_APPLICABLE" OR
       NOT _rehearsal_release_rehearsal_status STREQUAL "SYNTHETIC_ARCHIVE_ONLY" OR
       NOT (_archive_rehearsal_commit_length EQUAL 40 OR
            _archive_rehearsal_commit_length EQUAL 64) OR
       NOT _rehearsal_rehearsal_commit MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR
            "Archive evidence must remain an explicit synthetic, non-Git rehearsal")
    endif()
endif()
configure_file("${_prepared_rehearsal_report}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/TAGGED_RELEASE_REHEARSAL.txt" COPYONLY)
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Tagged-release rehearsal: TAGGED_RELEASE_REHEARSAL.txt\n")

set(_prepared_hosted_review_dir
    "${ANO_CRYPTO_BUILD_DIR}/hosted-artifact-manifest-test")
set(_prepared_hosted_disposition
    "${_prepared_hosted_review_dir}/HOSTED_ARTIFACT_REVIEW_REGRESSION.txt")
set(_prepared_hosted_matrix
    "${_prepared_hosted_review_dir}/HOSTED_ARTIFACT_MATRIX_REGRESSION.txt")
foreach(_prepared_hosted_file IN ITEMS
        "${_prepared_hosted_disposition}" "${_prepared_hosted_matrix}")
    if(NOT EXISTS "${_prepared_hosted_file}" OR
       IS_DIRECTORY "${_prepared_hosted_file}" OR
       IS_SYMLINK "${_prepared_hosted_file}")
        message(FATAL_ERROR "Prepared hosted-artifact review regression is missing or unsafe")
    endif()
endforeach()
file(READ "${_prepared_hosted_disposition}" _prepared_hosted_disposition_text)
foreach(_required_hosted_text IN ITEMS
        "ANOCRYPTO_HOSTED_ARTIFACT_DISPOSITION_V2"
        "required_tuple_count=16"
        "required_hosted_tuple_count=0"
        "hosted_runner_bundle_count=0"
        "synthetic_local_bundle_count=16"
        "artifact_index_sha256="
        "artifact_files_manifest_sha256="
        "hosted_execution_status=NOT_RUN"
        "review_status=UNREVIEWED"
        "reviewer=UNPROVISIONED"
        "review_date=UNREVIEWED"
        "disposition=UNREVIEWED"
        "signing_status=UNSIGNED"
        "handoff_status=BLOCKED"
        "external_release_authorized=NO"
        "github_attestation_role=OPTIONAL_SECONDARY_NOT_TRUST_ROOT"
        "result=BLOCKED")
    string(FIND "${_prepared_hosted_disposition_text}" "${_required_hosted_text}"
        _required_hosted_position)
    if(_required_hosted_position EQUAL -1)
        message(FATAL_ERROR
            "Hosted-artifact review regression omits ${_required_hosted_text}")
    endif()
endforeach()
configure_file("${_prepared_hosted_disposition}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/HOSTED_ARTIFACT_REVIEW_REGRESSION.txt" COPYONLY)
configure_file("${_prepared_hosted_matrix}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/HOSTED_ARTIFACT_MATRIX_REGRESSION.txt" COPYONLY)
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Hosted-artifact review regression: synthetic NOT_RUN / UNREVIEWED / BLOCKED\n")

function(_ano_crypto_find_evidence_test_executable _test_name _out_var)
    if(ANO_CRYPTO_MULTI_CONFIG_BUILD)
        set(_candidates
            "${ANO_CRYPTO_BUILD_DIR}/${ANO_CRYPTO_BUILD_CONFIG}/${_test_name}"
            "${ANO_CRYPTO_BUILD_DIR}/${ANO_CRYPTO_BUILD_CONFIG}/${_test_name}.exe")
    else()
        set(_candidates
            "${ANO_CRYPTO_BUILD_DIR}/${_test_name}"
            "${ANO_CRYPTO_BUILD_DIR}/${_test_name}.exe")
    endif()
    foreach(_candidate IN LISTS _candidates)
        if(EXISTS "${_candidate}" AND NOT IS_DIRECTORY "${_candidate}")
            set(${_out_var} "${_candidate}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${_out_var} "" PARENT_SCOPE)
endfunction()

function(_ano_crypto_run_evidence_test _test_name _result_var _stdout_var _stderr_var)
    _ano_crypto_find_evidence_test_executable("${_test_name}" _test_executable)
    if(NOT _test_executable)
        message(FATAL_ERROR "required evidence test executable is unavailable: ${_test_name}")
    endif()
    set(_test_output_dir "${ANO_CRYPTO_BUILD_DIR}/evidence-test-output")
    file(MAKE_DIRECTORY "${_test_output_dir}")
    set(_test_stdout_file "${_test_output_dir}/${_test_name}.stdout")
    set(_test_stderr_file "${_test_output_dir}/${_test_name}.stderr")
    file(REMOVE "${_test_stdout_file}" "${_test_stderr_file}")
    execute_process(
        COMMAND "${_test_executable}"
        WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
        RESULT_VARIABLE _test_result
        OUTPUT_FILE "${_test_stdout_file}"
        ERROR_FILE "${_test_stderr_file}"
        TIMEOUT 120)
    set(_test_stdout "")
    set(_test_stderr "")
    if(EXISTS "${_test_stdout_file}")
        file(READ "${_test_stdout_file}" _test_stdout)
    endif()
    if(EXISTS "${_test_stderr_file}")
        file(READ "${_test_stderr_file}" _test_stderr)
    endif()
    set(${_result_var} "${_test_result}" PARENT_SCOPE)
    set(${_stdout_var} "${_test_stdout}" PARENT_SCOPE)
    set(${_stderr_var} "${_test_stderr}" PARENT_SCOPE)
endfunction()

# The complete CTest pass above already executes every configured test.  Run the
# status-only evidence programs directly here instead of starting a second
# filtered CTest process from an outer build target.  This avoids nested CTest /
# build-runner jobserver interactions while preserving independent exit-status
# and output validation for each evidence artifact.
_ano_crypto_find_evidence_test_executable(
    ano_crypto_sha2_hmac_openssl_differential_tests _sha2_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_hkdf_sha256_openssl_differential_tests _hkdf_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_scrypt_openssl_differential_tests _scrypt_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_rsa_openssl_differential_tests _rsa_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_ecdsa_p256_openssl_differential_tests _ecdsa_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_aes_cbc_openssl_differential_tests _aes_cbc_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_aes_cmac_openssl_differential_tests _aes_cmac_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_aes256_gcm_openssl_differential_tests _aes256_gcm_diff_executable)
_ano_crypto_find_evidence_test_executable(
    ano_crypto_aes256_gcm_streaming_openssl_differential_tests
    _aes256_gcm_streaming_diff_executable)

if(ANO_CRYPTO_OPENSSL_DIFFERENTIAL_AVAILABLE)
    if(NOT (_sha2_diff_executable AND
            _hkdf_diff_executable AND
            _scrypt_diff_executable AND
            _rsa_diff_executable AND
            _ecdsa_diff_executable AND
            _aes_cbc_diff_executable AND
            _aes_cmac_diff_executable AND
            _aes256_gcm_diff_executable AND
            _aes256_gcm_streaming_diff_executable))
        message(FATAL_ERROR
            "Configured OpenSSL differential validation executables are incomplete")
    endif()
    _ano_crypto_run_evidence_test(
        ano_crypto_sha2_hmac_openssl_differential_tests
        _sha2_diff_result _sha2_diff_stdout _sha2_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_hkdf_sha256_openssl_differential_tests
        _hkdf_diff_result _hkdf_diff_stdout _hkdf_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_scrypt_openssl_differential_tests
        _scrypt_diff_result _scrypt_diff_stdout _scrypt_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_rsa_openssl_differential_tests
        _rsa_diff_result _rsa_diff_stdout _rsa_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_ecdsa_p256_openssl_differential_tests
        _ecdsa_diff_result _ecdsa_diff_stdout _ecdsa_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_aes_cbc_openssl_differential_tests
        _aes_cbc_diff_result _aes_cbc_diff_stdout _aes_cbc_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_aes_cmac_openssl_differential_tests
        _aes_cmac_diff_result _aes_cmac_diff_stdout _aes_cmac_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_aes256_gcm_openssl_differential_tests
        _aes256_gcm_diff_result _aes256_gcm_diff_stdout _aes256_gcm_diff_stderr)
    _ano_crypto_run_evidence_test(
        ano_crypto_aes256_gcm_streaming_openssl_differential_tests
        _aes256_gcm_streaming_diff_result
        _aes256_gcm_streaming_diff_stdout
        _aes256_gcm_streaming_diff_stderr)

    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_VALIDATION.log"
        "${_sha2_diff_stdout}${_sha2_diff_stderr}"
        "${_hkdf_diff_stdout}${_hkdf_diff_stderr}"
        "${_scrypt_diff_stdout}${_scrypt_diff_stderr}"
        "${_rsa_diff_stdout}${_rsa_diff_stderr}"
        "${_ecdsa_diff_stdout}${_ecdsa_diff_stderr}"
        "${_aes_cbc_diff_stdout}${_aes_cbc_diff_stderr}"
        "${_aes_cmac_diff_stdout}${_aes_cmac_diff_stderr}"
        "${_aes256_gcm_diff_stdout}${_aes256_gcm_diff_stderr}"
        "${_aes256_gcm_streaming_diff_stdout}${_aes256_gcm_streaming_diff_stderr}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "OpenSSL differential validation results: sha2-hmac=${_sha2_diff_result}; hkdf-sha256=${_hkdf_diff_result}; scrypt=${_scrypt_diff_result}; rsa=${_rsa_diff_result}; ecdsa-p256=${_ecdsa_diff_result}; aes-cbc=${_aes_cbc_diff_result}; aes-cmac=${_aes_cmac_diff_result}; aes256-gcm=${_aes256_gcm_diff_result}; aes256-gcm-streaming=${_aes256_gcm_streaming_diff_result}\n")
    if(NOT _sha2_diff_result EQUAL 0 OR
       NOT _hkdf_diff_result EQUAL 0 OR
       NOT _scrypt_diff_result EQUAL 0 OR
       NOT _rsa_diff_result EQUAL 0 OR
       NOT _ecdsa_diff_result EQUAL 0 OR
       NOT _aes_cbc_diff_result EQUAL 0 OR
       NOT _aes_cmac_diff_result EQUAL 0 OR
       NOT _aes256_gcm_diff_result EQUAL 0 OR
       NOT _aes256_gcm_streaming_diff_result EQUAL 0 OR
       NOT _sha2_diff_stdout MATCHES "sha2-hmac-differential status=pass" OR
       NOT _hkdf_diff_stdout MATCHES "hkdf_sha256_openssl_differential_tests: PASS" OR
       NOT _scrypt_diff_stdout MATCHES "scrypt_openssl_differential_tests: PASS" OR
       NOT _rsa_diff_stdout MATCHES "OpenSSL RSA differential comparisons passed: 36" OR
       NOT _ecdsa_diff_stdout MATCHES "OpenSSL ECDSA P-256 differential comparisons passed: 56" OR
       NOT _aes_cbc_diff_stdout MATCHES "AES-CBC OpenSSL differential comparisons passed" OR
       NOT _aes_cmac_diff_stdout MATCHES "OpenSSL 3.x AES-CMAC differential validation passed" OR
       NOT _aes256_gcm_diff_stdout MATCHES "AES-256-GCM OpenSSL differential comparisons: 640" OR
       NOT _aes256_gcm_streaming_diff_stdout MATCHES "AES-256-GCM streaming OpenSSL differential comparisons: 256")
        message(FATAL_ERROR "OpenSSL differential validation failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_VALIDATION.log")
    endif()
else()
    if(_sha2_diff_executable OR _hkdf_diff_executable OR
       _scrypt_diff_executable OR _rsa_diff_executable OR _ecdsa_diff_executable OR _aes_cbc_diff_executable OR _aes_cmac_diff_executable OR
       _aes256_gcm_diff_executable OR _aes256_gcm_streaming_diff_executable)
        message(FATAL_ERROR
            "Stale OpenSSL differential executables are present although the provider is not configured")
    endif()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_VALIDATION.log"
        "OpenSSL differential validation status=unavailable\n"
        "Reason: optional OpenSSL differential provider was not configured.\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "OpenSSL differential validation results: unavailable (optional provider not configured)\n")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_sha512_portability_review
    _portability_review_result _portability_review_stdout _portability_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_PORTABILITY_REVIEW.log"
    "${_portability_review_stdout}${_portability_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "SHA-512 portability review result: ${_portability_review_result}\n")
if(NOT _portability_review_result EQUAL 0 OR
   NOT _portability_review_stdout MATCHES "sha512-portability status=pass")
    message(FATAL_ERROR "SHA-512 portability review failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_PORTABILITY_REVIEW.log")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_aes_cbc_hardening_review
    _aes_hardening_review_result _aes_hardening_review_stdout _aes_hardening_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_HARDENING_REVIEW.log"
    "${_aes_hardening_review_stdout}${_aes_hardening_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "AES/CBC hardening review result: ${_aes_hardening_review_result}\n")
if(NOT _aes_hardening_review_result EQUAL 0 OR
   NOT _aes_hardening_review_stdout MATCHES "AES-CBC hardening review passed")
    message(FATAL_ERROR "AES/CBC hardening review failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/AES_HARDENING_REVIEW.log")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_aes_cmac_hardening_review
    _cmac_hardening_review_result _cmac_hardening_review_stdout _cmac_hardening_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_HARDENING_REVIEW.log"
    "${_cmac_hardening_review_stdout}${_cmac_hardening_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "AES-CMAC hardening review result: ${_cmac_hardening_review_result}\n")
if(NOT _cmac_hardening_review_result EQUAL 0 OR
   NOT _cmac_hardening_review_stdout MATCHES "AES-CMAC hardening review passed")
    message(FATAL_ERROR "AES-CMAC hardening review failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_HARDENING_REVIEW.log")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_aes_cmac_lifecycle_stress_tests
    _cmac_lifecycle_result _cmac_lifecycle_stdout _cmac_lifecycle_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_LIFECYCLE_STRESS.log"
    "${_cmac_lifecycle_stdout}${_cmac_lifecycle_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "AES-CMAC lifecycle stress result: ${_cmac_lifecycle_result}\n")
if(NOT _cmac_lifecycle_result EQUAL 0 OR
   NOT _cmac_lifecycle_stdout MATCHES "AES-CMAC lifecycle stress passed")
    message(FATAL_ERROR "AES-CMAC lifecycle stress failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_LIFECYCLE_STRESS.log")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_entropy_drbg_policy_review
    _entropy_drbg_review_result _entropy_drbg_review_stdout _entropy_drbg_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_DRBG_POLICY_REVIEW.txt"
    "${_entropy_drbg_review_stdout}${_entropy_drbg_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "P2-04 entropy/DRBG operational review result: ${_entropy_drbg_review_result}\n")
if(NOT _entropy_drbg_review_result EQUAL 0 OR
   NOT _entropy_drbg_review_stdout MATCHES "policy_id=EDP-P2-04-R1" OR
   NOT _entropy_drbg_review_stdout MATCHES "generated_bytes_logged=no" OR
   NOT _entropy_drbg_review_stdout MATCHES "provider_probe_status=ANO_CRYPTO_OK")
    message(FATAL_ERROR "P2-04 entropy/DRBG operational review failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_DRBG_POLICY_REVIEW.txt")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_csp_zeroization_policy_review
    _csp_zeroization_review_result _csp_zeroization_review_stdout _csp_zeroization_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CSP_ZEROIZATION_POLICY_REVIEW.txt"
    "${_csp_zeroization_review_stdout}${_csp_zeroization_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "P2-05 CSP/zeroization operational review result: ${_csp_zeroization_review_result}\n")
if(NOT _csp_zeroization_review_result EQUAL 0 OR
   NOT _csp_zeroization_review_stdout MATCHES "policy_id=CZP-P2-05-R1" OR
   NOT _csp_zeroization_review_stdout MATCHES "sensitive_values_printed=0" OR
   NOT _csp_zeroization_review_stdout MATCHES "csp_zeroization_policy_review: PASS")
    message(FATAL_ERROR "P2-05 CSP/zeroization operational review failed")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_secure_coding_policy_review
    _secure_coding_review_result _secure_coding_review_stdout _secure_coding_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SECURE_CODING_POLICY_REVIEW.txt"
    "${_secure_coding_review_stdout}${_secure_coding_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "P2-06 secure-coding operational review result: ${_secure_coding_review_result}\n")
if(NOT _secure_coding_review_result EQUAL 0 OR
   NOT _secure_coding_review_stdout MATCHES "policy_id=SCP-P2-06-R1" OR
   NOT _secure_coding_review_stdout MATCHES "timing_claim=none" OR
   NOT _secure_coding_review_stdout MATCHES "sensitive_values_printed=0" OR
   NOT _secure_coding_review_stdout MATCHES "secure_coding_policy_review: PASS")
    message(FATAL_ERROR "P2-06 secure-coding operational review failed")
endif()

_ano_crypto_run_evidence_test(
    ano_crypto_thread_platform_review
    _thread_platform_review_result _thread_platform_review_stdout _thread_platform_review_stderr)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_PLATFORM_REVIEW.log"
    "${_thread_platform_review_stdout}${_thread_platform_review_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "Thread platform review result: ${_thread_platform_review_result}\n")
if(NOT _thread_platform_review_result EQUAL 0 OR
   NOT _thread_platform_review_stdout MATCHES "thread_backend=")
    message(FATAL_ERROR "thread platform review failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_PLATFORM_REVIEW.log")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/CONCURRENCY_SANITIZER_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-33 concurrency and sanitizer review notes\n"
    "Status: engineering evidence only; not official validation evidence.\n"
    "Policy check: CONCURRENCY_SANITIZER_CHECK.txt\n"
    "Platform status: THREAD_PLATFORM_REVIEW.log\n"
    "Selected required sanitizer lane: Linux GCC address+undefined in hosted CI; Clang runtime review remains TODO.\n"
    "ThreadSanitizer: not selected in P1-33; runtime/toolchain review remains TODO.\n"
    "Build-identity ABI decision: docs/design/BUILD_IDENTITY_ABI_0_DECISION.md\n"
    "Actual hosted Linux/macOS/Windows artifacts require separate review before support wording is widened.\n"
    "Logs must not contain key bytes, MAC output, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data.\n")

if(ANO_CRYPTO_RUN_INSTALL_SMOKE)
    set(_prepared_install_smoke_dir "${ANO_CRYPTO_BUILD_DIR}/install-package-smoke")
    set(_prepared_install_smoke_summary "${_prepared_install_smoke_dir}/INSTALL_SMOKE_SUMMARY.txt")
    foreach(_artifact IN ITEMS
        INSTALL_SMOKE_INSTALL.log
        INSTALL_SMOKE_CONFIGURE.log
        INSTALL_SMOKE_BUILD.log
        INSTALL_SMOKE_RUN.log
        P1_29_COMPAT_CONFIGURE.log
        P1_29_COMPAT_BUILD.log
        P1_29_COMPAT_CTEST.log
        INSTALL_SMOKE_SUMMARY.txt)
        if(NOT EXISTS "${_prepared_install_smoke_dir}/${_artifact}" OR
           IS_DIRECTORY "${_prepared_install_smoke_dir}/${_artifact}" OR
           IS_SYMLINK "${_prepared_install_smoke_dir}/${_artifact}")
            message(FATAL_ERROR "Prepared install-package smoke artifact is incomplete: ${_artifact}")
        endif()
        configure_file(
            "${_prepared_install_smoke_dir}/${_artifact}"
            "${ANO_CRYPTO_EVIDENCE_DIR}/${_artifact}"
            COPYONLY)
    endforeach()
    file(READ "${_prepared_install_smoke_summary}" _install_smoke_summary_text)
    if(NOT _install_smoke_summary_text MATCHES "Install smoke result: 0" OR
       NOT _install_smoke_summary_text MATCHES "Current/P1-29 header compatibility CTest result: 0")
        message(FATAL_ERROR "Prepared install-package smoke did not pass")
    endif()
    set(_smoke_result 0)
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Install smoke result: ${_smoke_result}\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "Install smoke was not requested. Set ANO_CRYPTO_RUN_INSTALL_SMOKE=ON to capture installed-package smoke output.\n")
endif()

if(ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE)
    set(_prepared_poc_smoke_dir "${ANO_CRYPTO_BUILD_DIR}/poc-package-smoke")
    set(_prepared_poc_summary "${_prepared_poc_smoke_dir}/POC_PACKAGE_SMOKE_SUMMARY.txt")
    if(NOT EXISTS "${_prepared_poc_summary}" OR IS_DIRECTORY "${_prepared_poc_summary}" OR
       IS_SYMLINK "${_prepared_poc_summary}")
        message(FATAL_ERROR "Prepared PoC package smoke artifact is missing or unsafe")
    endif()
    file(READ "${_prepared_poc_summary}" _poc_smoke_summary_text)
    if(NOT _poc_smoke_summary_text MATCHES "PoC package smoke result: 0")
        message(FATAL_ERROR "Prepared PoC package smoke did not pass")
    endif()
    set(_prepared_poc_package "${_prepared_poc_smoke_dir}/AnoCrypto-C-PoC-v0")
    if(NOT IS_DIRECTORY "${_prepared_poc_package}" OR IS_SYMLINK "${_prepared_poc_package}")
        message(FATAL_ERROR "Prepared PoC package is missing or unsafe")
    endif()
    file(REMOVE_RECURSE "${ANO_CRYPTO_POC_SMOKE_DIR}")
    file(MAKE_DIRECTORY "${ANO_CRYPTO_POC_SMOKE_DIR}")
    file(COPY "${_prepared_poc_package}" DESTINATION "${ANO_CRYPTO_POC_SMOKE_DIR}")
    foreach(_artifact IN ITEMS
            POC_EXAMPLE_BUILD.log
            POC_EXAMPLE_CONFIGURE.log
            POC_EXAMPLE_RUN.log
            POC_PACKAGE_CREATE.log
            POC_PACKAGE_SMOKE_SUMMARY.txt)
        if(NOT EXISTS "${_prepared_poc_smoke_dir}/${_artifact}" OR
           IS_DIRECTORY "${_prepared_poc_smoke_dir}/${_artifact}" OR
           IS_SYMLINK "${_prepared_poc_smoke_dir}/${_artifact}")
            message(FATAL_ERROR "Prepared PoC smoke artifact is incomplete: ${_artifact}")
        endif()
        configure_file("${_prepared_poc_smoke_dir}/${_artifact}"
            "${ANO_CRYPTO_POC_SMOKE_DIR}/${_artifact}" COPYONLY)
    endforeach()
    set(_poc_smoke_result 0)
    set(_poc_smoke_stdout "Reused prepared ano_crypto_poc_package_smoke artifact: ${_prepared_poc_smoke_dir}\n")
    set(_poc_smoke_stderr "")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log"
        "# Run customer PoC package smoke\n"
        "# Result: ${_poc_smoke_result}\n\n"
        "${_poc_smoke_stdout}${_poc_smoke_stderr}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "PoC package smoke result: ${_poc_smoke_result}\n")
    if(NOT _poc_smoke_result EQUAL 0)
        message(FATAL_ERROR "PoC package smoke failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log"
        "PoC package smoke was not requested. Set ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON to capture customer PoC package smoke output.\n")
endif()

set(_artifact_scan_roots
    "${ANO_CRYPTO_EVIDENCE_DIR}"
    "${ANO_CRYPTO_BUILD_DIR}/install-package-smoke/install-prefix"
    "${ANO_CRYPTO_POC_SMOKE_DIR}/AnoCrypto-C-PoC-v0")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_SENSITIVE_ARTIFACT_ROOTS=${_artifact_scan_roots}"
        "-DANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT=${ANO_CRYPTO_EVIDENCE_DIR}/SENSITIVE_ARTIFACT_SCAN.txt"
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_sensitive_release_artifacts.cmake"
    RESULT_VARIABLE _artifact_scan_result
    OUTPUT_VARIABLE _artifact_scan_stdout
    ERROR_VARIABLE _artifact_scan_stderr
    TIMEOUT 120)
if(NOT "${_artifact_scan_result}" STREQUAL "0")
    message(FATAL_ERROR
        "Sensitive release-artifact scan failed:\n${_artifact_scan_stdout}${_artifact_scan_stderr}")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/README.md"
"# anocrypto-c Evidence Draft\n\n"
"Generated: ${ANO_CRYPTO_EVIDENCE_UTC}\n\n"
"This directory is an engineering evidence draft for the current local build.\n"
"It is not official validation evidence. Review path disclosure, source provenance,\n"
"test-vector provenance, compiler identity, binary provenance, and smoke logs before sharing.\n\n"
"## Files\n\n"
"- `BUILD_METADATA.txt`: local build and source metadata.\n"
"- `SOURCE_FILES.sha256`: SHA-256 hashes of repository source files included by the script.\n"
"- `SOURCE_ARCHIVE_CONTENTS.txt`: file list used for the local source archive.\n"
"- `SOURCE_ARCHIVE.sha256`: SHA-256 hash of `anocrypto-c-source.tar.gz` when archive creation is enabled.\n"
"- `BINARY_FILES.sha256`: SHA-256 hashes of build outputs matched by the script.\n"
"- `PUBLIC_SURFACE_CHECK.txt`: repository naming, claims-boundary, header, and package identity check.\n"
"- `VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt`: P2-02 candidate boundary/environment policy record; not selected-OE execution evidence.\n"
"- `SELF_TEST_INTEGRITY_POLICY_CHECK.txt`: P2-03 startup/conditional/on-demand classification, integrity-candidate, finite-state, recovery, ABI, and blocker policy record.\n"
"- `RELEASE_COMPATIBILITY_CHECK.txt`: P1-29/P1-33 normalized public declaration, header-hash, release-identity, and stored-symbol comparison.\n"
"- `TAGGED_COMPATIBILITY_SMOKE.txt`: clean installed-package current-header/P1-29-header and package-version smoke summary.\n"
"- `TAG_COMMIT_IDENTITY_CHECK.txt`: strict tagged-workflow identity result or explicit NOT_RUN placeholder when no actual tag exists.\n"
"- `TAGGED_RELEASE_REHEARSAL.txt`: isolated synthetic-tag identity and dirty/moved-tag/moved-HEAD rejection results.\n"
"- `HOSTED_ARTIFACT_REVIEW_REGRESSION.txt`: synthetic/local fixture disposition, explicitly NOT_RUN/UNREVIEWED/BLOCKED.\n"
"- `HOSTED_ARTIFACT_MATRIX_REGRESSION.txt`: exact required lane/platform/architecture/compiler tuple regression index.\n"
"- `RELEASE_COMPATIBILITY_REVIEW_NOTES.txt`: tag, source/ABI guardrail, hosted-artifact, and remaining reviewer boundaries.\n"
"- `THREAD_SAFETY_CHECK.txt`: support-matrix, CI-checklist, lock-branch, and build-option check.\n"
"- `CONCURRENCY_SANITIZER_CHECK.txt`: P1-26 cross-platform concurrency, sanitizer-lane, and build-identity ABI policy check.\n"
"- `THREAD_PLATFORM_REVIEW.log`: status-only thread backend and concurrency-test availability review.\n"
"- `CONCURRENCY_SANITIZER_REVIEW_NOTES.txt`: sanitizer selection and hosted-artifact review boundary.\n"
"- `AES_CBC_IMPLEMENTATION_CHECK.txt`: AES-128/192/256 primitive, CBC no-padding, API, CSP, KAT, and package policy check.\n"
"- `AES_HARDENING_CHECK.txt`: P1-26 context integrity, lifecycle, differential-corpus, compiler-review, and unchanged-ABI policy check.\n"
"- `AES_HARDENING_REVIEW.log`: status-only AES portability and lifecycle review.\n"
"- `AES_COMPILER_OUTPUT_REVIEW.txt`: compiler/target/library metadata and manual-review boundary.\n"
"- `AES_COMPILER_OUTPUT_DISASSEMBLY.txt`: internal machine-code artifact for target-specific manual review.\n"
"- `AES_CBC_REVIEW_NOTES.txt`: AES/CBC scope, context lifecycle, differential boundary, and remaining side-channel review notes.\n"
"- `AES_CMAC_IMPLEMENTATION_CHECK.txt`: retained AES-CMAC API, CSP, KAT, differential, package, and ABI-0 policy check.\n"
"- `HKDF_SHA256_POLICY_CHECK.txt`: P3-02 one-function API, RFC 5869 bounds, additive ABI, exact SKF1 info-byte evidence, and blocker-state policy check.\n"
"- `HKDF_SHA256_REVIEW_NOTES.txt`: P3-02 semantics, compatibility boundary, differential-provider boundary, and retained external-status notes.\n"
"- `AES256_GCM_POLICY_CHECK.txt`: P3-03 two-function API, fixed profile, authenticate-before-release, additive ABI, exact AnoSecureKit compatibility, and blocker-state policy check.\n"
"- `AES256_GCM_REVIEW_NOTES.txt`: P3-03 semantics, product-boundary compatibility, differential-provider boundary, and retained external-status notes.\n"
"- `AES256_GCM_STREAMING_POLICY_CHECK.txt`: P3-04 seven-function state-machine, pre-final decrypt policy, additive ABI, exact SKT1 compatibility, and blocker-state policy check.\n"
"- `AES256_GCM_STREAMING_REVIEW_NOTES.txt`: P3-04 state, caller-buffering, product-boundary, differential-provider, and retained external-status notes.\n"
"- `SCRYPT_POLICY_CHECK.txt`: P3-05 one-function fixed-profile API, workspace/resource, clearing, additive ABI, exact SKP1 compatibility, and blocker-state policy check.\n"
"- `SCRYPT_REVIEW_NOTES.txt`: P3-05 profile, resource, clearing, product-boundary, differential-provider, and retained external-status notes.\n"
"- `AES_CMAC_HARDENING_CHECK.txt`: P1-29 context, cleanup, lifecycle, concurrency, differential, compiler-review, and unchanged-ABI policy check.\n"
"- `AES_CMAC_HARDENING_REVIEW.log`: status-only CMAC context, alias, cleanup, and mismatch-position review.\n"
"- `AES_CMAC_LIFECYCLE_STRESS.log`: 128-iteration CMAC lifecycle and handle-snapshot stress result.\n"
"- `CMAC_COMPILER_OUTPUT_REVIEW.txt`: compiler/target/library metadata and manual-review boundary for CMAC.\n"
"- `CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt`: internal CMAC machine-code artifact for target-specific manual review.\n"
"- `AES_CMAC_REVIEW_NOTES.txt`: full-tag CMAC scope, dedicated key-purpose, hardening boundary, and remaining review notes.\n"
"- `SHA512_FAMILY_CHECK.txt`: SHA-384/SHA-512 family implementation, KAT, negative-test, and package wiring check.\n"
"- `DIFFERENTIAL_PORTABILITY_CHECK.txt`: OpenSSL test-only provider, deterministic comparison, C99 representation, and portability policy check.\n"
"- `DIFFERENTIAL_VALIDATION.log`: status-only OpenSSL 3.x SHA-2/HMAC, HKDF-SHA-256, AES-CBC, AES-CMAC, AES-256-GCM one-shot/streaming, and fixed-profile scrypt, and RSA PKCS#1 v1.5 SHA-2 differential result.\n"
"- `SHA512_PORTABILITY_REVIEW.log`: status-only representation, endian, and unaligned-input review.\n"
"- `DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt`: provider/product boundary and remaining hosted-artifact review notes.\n"
"- `RANDOM_SERVICE_CHECK.txt`: public random-service implementation policy check.\n"
"- `RANDOM_SERVICE_REVIEW_NOTES.txt`: random-service implementation review notes.\n"
"- `RANDOM_LIFECYCLE_HARDENING_CHECK.txt`: P1-26 lifecycle, process-marker, VM policy, hook, and ABI-baseline policy check.\n"
"- `RANDOM_LIFECYCLE_REVIEW_NOTES.txt`: status-only lifecycle and portability review notes.\n"
"- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`: clean Release comparison against the retained 69-symbol ABI-0 baseline and current 84-symbol additive manifest.\n"
"- `EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt`: exported-symbol guardrail scope and limitations.\n"
"- `ENTROPY_HEALTH_CHECK.txt`: entropy failure-injection and health-test boundary policy check.\n"
"- `ENTROPY_HEALTH_REVIEW_NOTES.txt`: health-test state-transition and logging review notes.\n"
"- `ENTROPY_OPERATIONAL_POLICY_CHECK.txt`: source classification, health ownership, operational probe, and process-boundary policy check.\n"
"- `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`: entropy source and process-change review notes.\n"
"- `ENTROPY_DRBG_POLICY_CHECK.txt`: P2-04 provider, health ownership, limits, lifecycle, ABI, and blocker policy record.\n"
"- `ENTROPY_DRBG_POLICY_REVIEW.txt`: status-only runtime provider/classification/limit/capability review; contains no provider or generated bytes.\n"
"- `ENTROPY_DRBG_REVIEW_NOTES.txt`: P2-04 responsibility and remaining external-confirmation review notes.\n"
"- `CSP_ZEROIZATION_POLICY_CHECK.txt`: P2-05 CSP inventory, lifecycle, zeroization, ABI, and blocker policy record.\n"
"- `CSP_ZEROIZATION_POLICY_REVIEW.txt`: status-only public zeroize and ownership-boundary review.\n"
"- `CSP_ZEROIZATION_REVIEW_NOTES.txt`: P2-05 closure and remaining external-confirmation notes.\n"
"- `ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt`: local compiler/target/hash capture; manual review required.\n"
"- `ZEROIZATION_COMPILER_OUTPUT_DISASSEMBLY.txt`: internal machine-code artifact; not selected-OE evidence.\n"
"- `SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt`: P2-06 source, tool, hardening, ABI, claim, and blocker policy record.\n"
"- `SECURE_CODING_POLICY_REVIEW.txt`: status-only CMAC fixed-tag review; no key, message, or tag output.\n"
"- `SECURE_CODING_REVIEW_NOTES.txt`: local review status, limitations, and open selected-OE/external findings.\n"
"- `SECURE_CODING_COMPILER_CPU_REVIEW.txt`: local compiler/target/library hash and hardening disposition; exact OE-01 remains NOT_RUN.\n"
"- `ELF_HARDENING_REVIEW.txt`: local readelf/nm capture for stack, RELRO/BIND_NOW, RWX/TEXTREL, and stack-protector review.\n"
"- `REPRODUCIBLE_BUILD_NOTES.txt`: current reproducibility procedure notes and unresolved gaps.\n"
"- `ctest.log`: captured CTest output when requested.\n"
"- `INSTALL_SMOKE_*.log`: installed-package configure/build/run logs when requested.\n"
"- `INSTALL_SMOKE_SUMMARY.txt`: install smoke paths and result when requested.\n"
"- `POC_PACKAGE_SMOKE.log`: customer PoC package creation/configure/build/run summary when requested.\n"
"- `poc-package-smoke/`: detailed PoC package smoke logs and staged package when requested.\n"
"- `release-provenance/`: unsigned source archive, source-file hashes, SPDX 2.3 JSON source inventory, ABI/public-header inputs, signing request placeholder, and BLOCKED authorization bound by `RELEASE_ARTIFACTS.sha256`.\n"
"- `RELEASE_PROVENANCE_CHECK.txt`: generated bundle hash/path/identity/SBOM/signing-boundary verification.\n"
"- `RELEASE_PROVENANCE_POLICY_CHECK.txt`: static P1-33 integration and unsigned-release policy verification.\n"
"- `release-signing-handoff/`: key-free Ed25519/RFC 3161 offline-signing request and BLOCKED readiness metadata.\n"
"- `RELEASE_SIGNING_HANDOFF_CHECK.txt`: manifest, policy, role-separation, and sensitive-material verification.\n"
"- `SBOM_STATUS.txt`: source-inventory SBOM scope and limitation status.\n"
"- `SIGNING_STATUS.txt`: explicit unsigned signing and authorization boundary.\n"
"- `SENSITIVE_ARTIFACT_SCAN.txt`: evidence, installed SDK, and PoC credential/signature leakage scan.\n"
"- `EVIDENCE_MANIFEST_README.txt`: notes for the generated evidence manifest.\n"
"- `EVIDENCE_MANIFEST.sha256`: SHA-256 manifest of generated evidence files.\n"
"- Platform entropy provider status appears in `ctest.log` when the provider test output is shown by CTest.\n"
"- Random-service lifecycle and failure behavior appears in `ctest.log` when tests are run.\n"
"- Entropy failure and health-test logs must contain test/status metadata only.\n\n"
"## Review before release-candidate use\n\n"
"- Confirm source archive contents match the intended release scope.\n"
"- Confirm install smoke and PoC package smoke paths do not disclose internal deployment details before sharing.\n"
"- Confirm PoC package examples and manifests do not contain secrets, entropy bytes, generated secret bytes, private keys, tokens, or customer data.\n"
"- Confirm SBOM/signing placeholder status is accurate for this package.\n"
"- Confirm reproducibility gaps are marked as TODO instead of stronger claims.\n"
"- Confirm no secrets, entropy bytes, generated secret bytes, private keys, tokens, or internal deployment details are present.\n"
"- Confirm platform entropy evidence records provider/status only and does not contain entropy bytes.\n"
"- Confirm random-service evidence records status and lifecycle results only and does not contain generated bytes.\n"
"- Confirm lifecycle evidence records only counts, statuses, provider classification, and process-marker support; no generated output is retained.\n"
"- Confirm clean Release exports match `abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt` and any intended ABI change is reviewed before updating the baseline.\n"
"- Confirm P1-29 retained public headers and `release/PUBLIC_HEADERS.sha256` match their reviewed manifests.\n"
"- Confirm current-header and P1-29-header installed-package consumers pass on each supported toolchain before changing compatibility wording.\n"
"- Confirm the expected release tag exists and resolves to the reviewed commit; metadata alone is not sufficient.\n"
"- Confirm downloaded hosted bundles pass manifest verification and receive explicit human disposition before platform support wording changes.\n"
"- Confirm VM snapshot/restore, clone, and suspend/resume limitations remain explicit in customer-facing integration notes.\n"
"- Confirm entropy/health evidence contains no raw entropy, deterministic hook bytes, seed material, or DRBG state.\n"
"- Confirm operational evidence classifies OS providers as external conditioned services and contains no provider output.\n"
"- Confirm source-backed DRBG process-change evidence records only status and output-clearing results.\n"
"- Confirm PoC clean-build logs show entropy test hooks OFF and internal symbol hiding ON.\n"
"- Confirm thread-safety evidence records only status, platform branch, sanitizer mode, and support-matrix notes.\n"
"- Confirm actual hosted thread/sanitizer artifacts are not marked reviewed unless the referenced workflow run completed successfully.\n"
"- Confirm build-identity ABI version 1 matches `abi/ANOCRYPTO_C_0_BUILD_IDENTITY_LAYOUT.md`.\n"
"- Confirm differential logs retain status, provider/version, representation metadata, and comparison counts only; no digest, MAC, key, input, entropy, seed, DRBG state, or random value is present.\n"
"- Confirm OpenSSL remains a test-only dependency and does not appear in installed SDK or PoC runtime dependencies.\n"
"- Confirm AES compiler-output artifacts are interpreted only for the recorded compiler/target and have a human reviewer decision before any support wording changes.\n"
"- Confirm CMAC compiler-output artifacts are interpreted only for the recorded compiler/target and do not constitute an automated timing or side-channel conclusion.\n"
"- Confirm CMAC hardening and lifecycle logs contain counts/status only and no key, message, tag, subkey, chain, AES schedule, or reference-result bytes.\n"
"- Confirm AES logs contain no key, round key, IV, plaintext, ciphertext, or reference-result bytes.\n"
"- Confirm P2-06 hardening capture refers only to the recorded compiler/target/library hash and is not represented as exact OE-01 evidence.\n"
"- Confirm table-free/fixed-work source observations are not promoted to universal constant-time or side-channel-resistance claims.\n"
"- Confirm timing/cache, power/EM, transient-execution, and fault findings retain their NOT_RUN/NOT_EVALUATED/external-confirmation status.\n"
"- Confirm hosted 32-bit, Windows, macOS, and big-endian artifacts are not marked reviewed unless the referenced runs completed successfully.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST_README.txt"
    "AnoCrypto-C evidence manifest\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: engineering hash manifest for local evidence review.\n"
    "Manifest file: EVIDENCE_MANIFEST.sha256\n"
    "Policy document: docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md\n"
    "The manifest excludes itself, the temporary source-archive-work tree, and transient smoke build directories. Review local paths and logs before sharing.\n")

file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Evidence manifest: EVIDENCE_MANIFEST.sha256\n")

_ano_crypto_list_tree_files("${ANO_CRYPTO_EVIDENCE_DIR}" _evidence_files_absolute TRUE)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "")
foreach(_evidence_file IN LISTS _evidence_files_absolute)
    file(RELATIVE_PATH _rel "${ANO_CRYPTO_EVIDENCE_DIR}" "${_evidence_file}")
    set(_manifest_skip OFF)
    if(_rel STREQUAL "EVIDENCE_MANIFEST.sha256")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^source-archive-work/")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^install-smoke-build/")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^poc-package-smoke/core-poc-build/")
        set(_manifest_skip ON)
    endif()
    if(NOT _manifest_skip)
        file(SHA256 "${_evidence_file}" _evidence_hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "${_evidence_hash}  ${_rel}\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C evidence draft written to ${ANO_CRYPTO_EVIDENCE_DIR}")
