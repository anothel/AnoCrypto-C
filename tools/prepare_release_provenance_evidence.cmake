# Prepare the source archive and verified unsigned provenance artifacts before
# final evidence assembly. This script never signs or authorizes a release.
cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_BUILD_DIR
        ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR
    "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")

file(RELATIVE_PATH _provenance_evidence_relative
    "${ANO_CRYPTO_BUILD_DIR}" "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}")
if(_provenance_evidence_relative STREQUAL "" OR
   _provenance_evidence_relative STREQUAL "." OR
   IS_ABSOLUTE "${_provenance_evidence_relative}" OR
   _provenance_evidence_relative MATCHES "^([.][.]|[/\\])" OR
   IS_SYMLINK "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}")
    message(FATAL_ERROR
        "ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR must be a non-symlink child of ANO_CRYPTO_BUILD_DIR")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}")

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
    if("${_git_root_result}" STREQUAL "0")
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
    if("${_git_commit_result}" STREQUAL "0")
        set(_git_commit "${_git_commit_output}")
    endif()
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" status --short
        RESULT_VARIABLE _git_status_result
        OUTPUT_VARIABLE _git_status_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 30)
    if("${_git_status_result}" STREQUAL "0")
        if(_git_status_output STREQUAL "")
            set(_git_status "clean")
            set(_git_worktree_state "CLEAN")
        else()
            set(_git_status "${_git_status_output}")
            set(_git_worktree_state "DIRTY")
        endif()
    endif()
endif()

set(_source_files "")
if(_git_exe AND _source_is_git_root)
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            ls-files --cached --others --exclude-standard
        RESULT_VARIABLE _git_files_result
        OUTPUT_VARIABLE _git_files_output
        ERROR_VARIABLE _git_files_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 60)
    if("${_git_files_result}" STREQUAL "0")
        string(REPLACE "\r\n" "\n" _git_files_output "${_git_files_output}")
        string(REPLACE "\n" ";" _source_files "${_git_files_output}")
    endif()
endif()
if(NOT _source_files)
    # Extracted source archives have no Git metadata and no ignored build tree.
    file(GLOB_RECURSE _source_files
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
        "${ANO_CRYPTO_SOURCE_DIR}/*")
endif()
list(SORT _source_files)
set(_included_source_files "")
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_FILES.sha256" "")
foreach(_relative IN LISTS _source_files)
    if(_relative MATCHES "(^|/)([.]git|build|build-[^/]*|cmake-build-[^/]*|out|hosted-[^/]*|downloaded-hosted-artifacts|[.]cache)(/|$)" OR
       _relative MATCHES "^evidence/(generated|out|p1-[0-9][^/]*|release-[^/]*)/" OR
       _relative MATCHES "[.](zip|patch|tar|tgz|gz)$")
        continue()
    endif()
    list(APPEND _included_source_files "${_relative}")
    file(SHA256 "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _hash)
    file(APPEND "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_FILES.sha256"
        "${_hash}  ${_relative}\n")
endforeach()
list(LENGTH _included_source_files _source_file_count)
if(_source_file_count EQUAL 0)
    message(FATAL_ERROR "Release provenance source-file set is empty")
endif()

file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt" "")
set(_archive_root "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/source-archive-work")
set(_archive_tree "${_archive_root}/anocrypto-c")
set(_archive_path "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/anocrypto-c-source.tar.gz")
file(MAKE_DIRECTORY "${_archive_tree}")
foreach(_relative IN LISTS _included_source_files)
    get_filename_component(_relative_dir "${_relative}" DIRECTORY)
    if(_relative_dir STREQUAL "" OR _relative_dir STREQUAL ".")
        set(_destination "${_archive_tree}")
    else()
        set(_destination "${_archive_tree}/${_relative_dir}")
    endif()
    file(MAKE_DIRECTORY "${_destination}")
    file(COPY "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" DESTINATION "${_destination}")
    file(APPEND "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt"
        "${_relative}\n")
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_archive_path}" anocrypto-c
    WORKING_DIRECTORY "${_archive_root}"
    RESULT_VARIABLE _archive_result
    OUTPUT_VARIABLE _archive_stdout
    ERROR_VARIABLE _archive_stderr
    TIMEOUT 300)
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE.log"
    "# Create source archive\n# Result: ${_archive_result}\n\n${_archive_stdout}${_archive_stderr}")
file(REMOVE_RECURSE "${_archive_root}")
if(NOT "${_archive_result}" STREQUAL "0" OR NOT EXISTS "${_archive_path}")
    message(FATAL_ERROR "Source archive creation failed; see ${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE.log")
endif()
file(SHA256 "${_archive_path}" _archive_hash)
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256"
    "${_archive_hash}  anocrypto-c-source.tar.gz\n")

string(TIMESTAMP _created_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
set(_bundle "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/release-provenance")
set(_bundle_check "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE_CHECK.txt")
set(_bundle_command
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${_bundle}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive_path}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=${_created_utc}"
    "-DANO_CRYPTO_PROVENANCE_COMMIT=${_git_commit}"
    "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=${_git_worktree_state}"
    "-DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_bundle_check}")
if(DEFINED ANO_CRYPTO_ACTUAL_RELEASE_TAG AND
   NOT ANO_CRYPTO_ACTUAL_RELEASE_TAG STREQUAL "")
    list(APPEND _bundle_command
        "-DANO_CRYPTO_PROVENANCE_ACTUAL_TAG=${ANO_CRYPTO_ACTUAL_RELEASE_TAG}")
endif()
list(APPEND _bundle_command
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")
execute_process(
    COMMAND ${_bundle_command}
    RESULT_VARIABLE _bundle_result
    OUTPUT_VARIABLE _bundle_stdout
    ERROR_VARIABLE _bundle_stderr
    TIMEOUT 300)
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE.log"
    "# Create and verify unsigned P2-02 release provenance bundle\n"
    "# Result: ${_bundle_result}\n\n${_bundle_stdout}${_bundle_stderr}")
if(NOT "${_bundle_result}" STREQUAL "0")
    message(FATAL_ERROR "Release provenance generation failed; see ${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE.log")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
        -DANO_CRYPTO_SKIP_GENERATED_PROVENANCE_CHECK=ON
        -DANO_CRYPTO_RELEASE_PROVENANCE_CHECK_OUTPUT=${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE_POLICY_CHECK.txt
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_policy.cmake"
    RESULT_VARIABLE _policy_result
    OUTPUT_VARIABLE _policy_stdout
    ERROR_VARIABLE _policy_stderr
    TIMEOUT 180)
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE_POLICY_CHECK.log"
    "# P2-02 release provenance policy check\n# Result: ${_policy_result}\n\n"
    "${_policy_stdout}${_policy_stderr}")
if(NOT "${_policy_result}" STREQUAL "0")
    message(FATAL_ERROR "Release provenance policy check failed; see ${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/RELEASE_PROVENANCE_POLICY_CHECK.log")
endif()

foreach(_status IN ITEMS SBOM_STATUS.txt SIGNING_STATUS.txt)
    configure_file("${_bundle}/${_status}"
        "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/${_status}" COPYONLY)
endforeach()
file(WRITE "${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}/PREPARED_RELEASE_PROVENANCE_SUMMARY.txt"
    "AnoCrypto-C P2-02 prepared release provenance evidence\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Git commit: ${_git_commit}\n"
    "Git status: ${_git_status}\n"
    "Worktree state: ${_git_worktree_state}\n"
    "Source files: ${_source_file_count}\n"
    "Source archive SHA-256: ${_archive_hash}\n"
    "Provenance verification: PASS\n"
    "Signing status: UNSIGNED\n"
    "External release authorized: NO\n"
    "Sensitive signing material present: no\n"
    "Result: PASS\n")
message(STATUS "AnoCrypto-C release provenance evidence prepared: ${ANO_CRYPTO_PROVENANCE_EVIDENCE_DIR}")
