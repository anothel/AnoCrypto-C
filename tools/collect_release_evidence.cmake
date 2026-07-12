# anocrypto-c release evidence collection draft.
# Usage:
#   cmake -DANO_CRYPTO_SOURCE_DIR=. \
#         -DANO_CRYPTO_BUILD_DIR=build \
#         -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-28 \
#         -DANO_CRYPTO_RUN_CTEST=ON \
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
    set(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/evidence/p1-28")
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

if(NOT DEFINED ANO_CRYPTO_RUN_AES_COMPILER_REVIEW)
    set(ANO_CRYPTO_RUN_AES_COMPILER_REVIEW ON)
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

if(NOT DEFINED ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK)
    set(ANO_CRYPTO_RUN_CONCURRENCY_SANITIZER_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK)
    set(ANO_CRYPTO_RUN_DIFFERENTIAL_PORTABILITY_CHECK ON)
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}" ABSOLUTE)

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

file(MAKE_DIRECTORY "${ANO_CRYPTO_EVIDENCE_DIR}")

function(ano_crypto_run_logged log_file description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _cmd_result
        OUTPUT_VARIABLE _cmd_stdout
        ERROR_VARIABLE _cmd_stderr)
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

string(TIMESTAMP ANO_CRYPTO_EVIDENCE_UTC "%Y-%m-%dT%H:%M:%SZ" UTC)

set(_git_commit "unknown")
set(_git_status "git unavailable or not a repository")
find_program(_git_exe git)
if(_git_exe)
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" rev-parse --verify HEAD
        RESULT_VARIABLE _git_commit_result
        OUTPUT_VARIABLE _git_commit_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_git_commit_result EQUAL 0)
        set(_git_commit "${_git_commit_output}")
    endif()

    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" status --short
        RESULT_VARIABLE _git_status_result
        OUTPUT_VARIABLE _git_status_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_git_status_result EQUAL 0)
        if(_git_status_output STREQUAL "")
            set(_git_status "clean")
        else()
            set(_git_status "${_git_status_output}")
        endif()
    endif()
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
"Reproducible build status: P1-28 retained draft procedure; binary byte identity not claimed\n"
"Evidence status: engineering draft only\n"
"Claims boundary: not official validation evidence\n")

file(GLOB_RECURSE _source_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
    "${ANO_CRYPTO_SOURCE_DIR}/*")
list(SORT _source_files)

set(_included_source_files "")
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "")
foreach(_rel IN LISTS _source_files)
    set(_skip OFF)
    if(_rel MATCHES "(^|/)(\\.git|build|build-[^/]*|cmake-build-[^/]*|\\.cache)(/|$)")
        set(_skip ON)
    endif()
    if(_rel MATCHES "^evidence/(generated|out|p1-[0-9][^/]*|release-[^/]*)/")
        set(_skip ON)
    endif()
    if(_rel MATCHES "\\.(zip|patch|tar|tgz|gz)$")
        set(_skip ON)
    endif()

    if(NOT _skip)
        list(APPEND _included_source_files "${_rel}")
        file(SHA256 "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "${_hash}  ${_rel}\n")
    endif()
endforeach()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt" "")
foreach(_rel IN LISTS _included_source_files)
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt" "${_rel}\n")
endforeach()

if(ANO_CRYPTO_CREATE_SOURCE_ARCHIVE)
    set(_archive_root "${ANO_CRYPTO_EVIDENCE_DIR}/source-archive-work")
    set(_archive_tree "${_archive_root}/anocrypto-c")
    set(_archive_path "${ANO_CRYPTO_EVIDENCE_DIR}/anocrypto-c-source.tar.gz")
    file(REMOVE_RECURSE "${_archive_root}")
    file(MAKE_DIRECTORY "${_archive_tree}")

    foreach(_rel IN LISTS _included_source_files)
        get_filename_component(_rel_dir "${_rel}" DIRECTORY)
        if(_rel_dir STREQUAL "" OR _rel_dir STREQUAL ".")
            set(_dest_dir "${_archive_tree}")
        else()
            set(_dest_dir "${_archive_tree}/${_rel_dir}")
        endif()
        file(MAKE_DIRECTORY "${_dest_dir}")
        file(COPY "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" DESTINATION "${_dest_dir}")
    endforeach()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_archive_path}" "anocrypto-c"
        WORKING_DIRECTORY "${_archive_root}"
        RESULT_VARIABLE _archive_result
        OUTPUT_VARIABLE _archive_stdout
        ERROR_VARIABLE _archive_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.log"
        "# Create source archive\n# Result: ${_archive_result}\n\n${_archive_stdout}${_archive_stderr}")
    if(NOT _archive_result EQUAL 0)
        message(FATAL_ERROR "source archive creation failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.log")
    endif()

    file(SHA256 "${_archive_path}" _archive_hash)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256"
        "${_archive_hash}  anocrypto-c-source.tar.gz\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Source archive: anocrypto-c-source.tar.gz\n"
        "Source archive SHA-256: ${_archive_hash}\n")
    file(REMOVE_RECURSE "${_archive_root}")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256"
        "Source archive creation was not requested. Set ANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/BINARY_FILES.sha256" "")
if(EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    file(GLOB_RECURSE _binary_files
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_BUILD_DIR}"
        "${ANO_CRYPTO_BUILD_DIR}/libanocrypto_c*"
        "${ANO_CRYPTO_BUILD_DIR}/anocrypto_c*"
        "${ANO_CRYPTO_BUILD_DIR}/ano_crypto_*")
    list(SORT _binary_files)
    foreach(_rel IN LISTS _binary_files)
        if(NOT _rel MATCHES "CMakeFiles/" AND NOT _rel MATCHES "^evidence/")
            file(SHA256 "${ANO_CRYPTO_BUILD_DIR}/${_rel}" _hash)
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
        ERROR_VARIABLE _policy_stderr)
    file(APPEND "${_policy_check_output}" "\nCommand output:\n${_policy_stdout}${_policy_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Public surface check result: ${_policy_result}\n")
    if(NOT _policy_result EQUAL 0)
        message(FATAL_ERROR "public surface check failed; see ${_policy_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/PUBLIC_SURFACE_CHECK.txt"
        "Public surface check was not requested. Set ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON.\n")
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
        ERROR_VARIABLE _thread_check_stderr)
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
        ERROR_VARIABLE _concurrency_sanitizer_stderr)
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
        ERROR_VARIABLE _differential_portability_stderr)
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
        ERROR_VARIABLE _sha512_family_check_stderr)
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
        ERROR_VARIABLE _aes_cbc_check_stderr)
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
        ERROR_VARIABLE _aes_hardening_check_stderr)
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
        ERROR_VARIABLE _cmac_check_stderr)
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

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_CMAC_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-28 AES-CMAC review notes
"
    "Status: engineering implementation evidence; not official validation evidence.
"
    "Implemented scope: AES-CMAC with AES-128/192/256, full 16-byte tags, streaming compute, one-shot compute, and one-shot verify.
"
    "Key-purpose rule: only dedicated ANO_CRYPTO_KEY_TYPE_CMAC handles are accepted; CBC and CMAC reject the other purpose.
"
    "Startup and extended KAT source: public NIST AES-CMAC examples associated with SP 800-38B.
"
    "Differential provider: OpenSSL 3.x test-only; no SDK or PoC runtime dependency.
"
    "Public API added by P1-28: eight CMAC functions. Public export baseline: 69 symbols.
"
    "Deferred: tag truncation, AES-CTR, GCM/GMAC, protocol formatting, persistence, export, and target-specific side-channel conclusions.
"
    "Sensitive values recorded: none.
")

if(ANO_CRYPTO_RUN_AES_COMPILER_REVIEW)
    set(_aes_compiler_review_source "${ANO_CRYPTO_BUILD_DIR}/policy-check/aes-compiler-review")
    set(_aes_compiler_review_result 0)
    set(_aes_compiler_review_stdout "Reused compiler-output artifacts built by the evidence target dependency.\n")
    set(_aes_compiler_review_stderr "")

    # The normal evidence target already depends on ano_crypto_aes_compiler_review.
    # Avoid recursively building a target in the same build tree because Make/Ninja
    # jobserver ownership can deadlock the outer evidence build.  Keep a fallback
    # for direct script invocation when the artifacts have not been prepared.
    if(NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_REVIEW.txt" OR
       NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" --build "${ANO_CRYPTO_BUILD_DIR}" --target ano_crypto_aes_compiler_review
            RESULT_VARIABLE _aes_compiler_review_result
            OUTPUT_VARIABLE _aes_compiler_review_stdout
            ERROR_VARIABLE _aes_compiler_review_stderr)
    endif()

    if(NOT _aes_compiler_review_result EQUAL 0 OR
       NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_REVIEW.txt" OR
       NOT EXISTS "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt")
        file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_REVIEW.txt"
            "AES compiler-output capture failed.\n${_aes_compiler_review_stdout}${_aes_compiler_review_stderr}")
        message(FATAL_ERROR "AES compiler-output review capture failed")
    endif()
    file(COPY "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_REVIEW.txt"
              "${_aes_compiler_review_source}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt"
         DESTINATION "${ANO_CRYPTO_EVIDENCE_DIR}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_REVIEW.txt"
        "\nCapture command output:\n${_aes_compiler_review_stdout}${_aes_compiler_review_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "AES compiler-output review capture result: ${_aes_compiler_review_result}\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_REVIEW.txt"
        "AES compiler-output review was not requested. Set ANO_CRYPTO_RUN_AES_COMPILER_REVIEW=ON.\n")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/AES_COMPILER_OUTPUT_DISASSEMBLY.txt"
        "AES compiler-output disassembly was not requested.\n")
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
        ERROR_VARIABLE _random_service_check_stderr)
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
        ERROR_VARIABLE _random_lifecycle_check_stderr)
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
    set(_symbol_check_work_dir "${ANO_CRYPTO_EVIDENCE_DIR}/clean-release-symbol-work")
    set(_symbol_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/EXPORTED_SYMBOL_BASELINE_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_SYMBOL_WORK_DIR=${_symbol_check_work_dir}
            -DANO_CRYPTO_SYMBOL_REPORT=${_symbol_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_clean_release_symbols.cmake"
        RESULT_VARIABLE _symbol_check_result
        OUTPUT_VARIABLE _symbol_check_stdout
        ERROR_VARIABLE _symbol_check_stderr)
    file(APPEND "${_symbol_check_output}" "\nCommand output:\n${_symbol_check_stdout}${_symbol_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Clean Release exported-symbol baseline result: ${_symbol_check_result}\n")
    file(REMOVE_RECURSE "${_symbol_check_work_dir}")
    if(NOT _symbol_check_result EQUAL 0)
        message(FATAL_ERROR "clean Release exported-symbol baseline check failed; see ${_symbol_check_output}")
    endif()
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
        ERROR_VARIABLE _entropy_health_check_stderr)
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
        ERROR_VARIABLE _entropy_operational_check_stderr)
    file(APPEND "${_entropy_operational_check_output}" "\nCommand output:\n${_entropy_operational_check_stdout}${_entropy_operational_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Entropy operational policy check result: ${_entropy_operational_check_result}\n")
    if(NOT _entropy_operational_check_result EQUAL 0)
        message(FATAL_ERROR "entropy operational policy check failed; see ${_entropy_operational_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_OPERATIONAL_POLICY_CHECK.txt"
        "Entropy operational policy check was not requested. Set ANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK=ON.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/REPRODUCIBLE_BUILD_NOTES.txt"
    "AnoCrypto-C reproducible build evidence notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-28 retained draft procedure; binary byte identity is not claimed.\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n"
    "Source archive content list: SOURCE_ARCHIVE_CONTENTS.txt\n"
    "Source hash list: SOURCE_FILES.sha256\n"
    "Binary hash list: BINARY_FILES.sha256\n"
    "Public surface check: PUBLIC_SURFACE_CHECK.txt\n"
    "Thread-safety policy check: THREAD_SAFETY_CHECK.txt\n"
    "SHA-512-family implementation check: SHA512_FAMILY_CHECK.txt\n"
    "Random-service implementation policy check: RANDOM_SERVICE_CHECK.txt\n"
    "Random lifecycle hardening policy check: RANDOM_LIFECYCLE_HARDENING_CHECK.txt\n"
    "Clean Release exported-symbol baseline: EXPORTED_SYMBOL_BASELINE_CHECK.txt\n"
    "Entropy/health policy check: ENTROPY_HEALTH_CHECK.txt\n"
    "Entropy operational policy check: ENTROPY_OPERATIONAL_POLICY_CHECK.txt\n"
    "Differential/portability policy check: DIFFERENTIAL_PORTABILITY_CHECK.txt\n"
    "AES/CBC implementation policy check: AES_CBC_IMPLEMENTATION_CHECK.txt\n"
    "AES/CBC hardening policy check: AES_HARDENING_CHECK.txt\n"
    "AES-CMAC implementation policy check: AES_CMAC_IMPLEMENTATION_CHECK.txt\n"
    "AES compiler-output review: AES_COMPILER_OUTPUT_REVIEW.txt\n"
    "TODO: normalize archive timestamps if byte-for-byte source archives become a release requirement.\n"
    "TODO: pin generator, compiler, locale, timezone, debug-info, and path-prefix-map policies for release-candidate builds.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SCOPE_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-28 evidence and scope-review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-28 engineering scope artifact covering implemented SHA-2/HMAC, random, AES-CBC, and AES-CMAC services; not official validation evidence.\n"
    "Development workflow: docs/roadmap/DEVELOPMENT_WORKFLOW.md\n"
    "Service scope matrix: docs/kcmvp/SERVICE_SCOPE_MATRIX.md\n"
    "Algorithm scope: docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md\n"
    "SHA-512-family decision: docs/design/SHA2_384_512_SCOPE_DECISION.md\n"
    "SHA-512-family KAT record: docs/kcmvp/SHA2_384_512_KAT_PLAN.md\n"
    "SHA-512-family implementation check: SHA512_FAMILY_CHECK.txt\n"
    "Current implemented algorithm slice includes SHA-256/SHA-384/SHA-512, their HMAC variants, and HMAC_DRBG-SHA-256.\n"
    "The existing platform-provider-backed random service and CSP HMAC key lifecycle remain in the candidate engineering slice.\n"
    "AES-128/192/256, CBC without padding, and full-tag AES-CMAC are implemented. AES-CTR, AEAD, padding helpers, GMAC, hardware dispatch, tag truncation, and additional algorithms remain deferred.\n"
    "OpenSSL 3.x test-only differential automation covers SHA-2/HMAC, AES-CBC, and AES-CMAC; actual hosted 32-bit, Windows, macOS, big-endian, and target-specific side-channel artifacts remain review items.\n"
    "Do not include digest values, MAC values, keys, entropy bytes, seed material, DRBG state, generated random bytes, private keys, tokens, customer data, or internal deployment details in scope evidence.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Scope review notes: SCOPE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SHA512_FAMILY_REVIEW_NOTES.txt"
    "AnoCrypto-C SHA-384/SHA-512 family review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-28 retained implementation, KAT, differential, and portability evidence; not official validation evidence.\n"
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
    "Status: P1-28 retained engineering comparison and portability evidence; not official validation evidence.\n"
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
    "Status: P1-28 retained engineering lifecycle-hardening evidence; not official validation evidence.\n"
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
    "Status: P1-28 retained engineering regression and policy evidence; not official validation evidence.\n"
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
    "Status: P1-28 clean Release ABI guardrail; not a complete ABI compatibility claim.\n"
    "Baseline: abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt\n"
    "Expected public symbol count: 61\n"
    "Automated result: EXPORTED_SYMBOL_BASELINE_CHECK.txt\n"
    "The check compares a clean shared Release build with test hooks disabled and internal symbol hiding enabled.\n"
    "Public struct layout, calling convention, type size, semantic compatibility, and platform-specific binary details remain separate review items.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Exported symbol baseline review notes: EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ENTROPY_HEALTH_REVIEW_NOTES.txt"
    "AnoCrypto-C entropy failure-injection and health-test review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-28 retained engineering testability and state-transition evidence only.\n"
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
    "Status: P1-28 retained engineering policy and regression evidence only.\n"
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

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SBOM_STATUS.txt"
    "AnoCrypto-C SBOM status\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: placeholder only; no complete SBOM is generated by P1-28.\n"
    "Hash manifests available: SOURCE_FILES.sha256, SOURCE_ARCHIVE.sha256, BINARY_FILES.sha256, EVIDENCE_MANIFEST.sha256.\n"
    "Candidate SBOM formats: SPDX JSON or CycloneDX JSON after tool and schema review.\n"
    "SBOM plan: docs/kcmvp/SBOM_SIGNING_PLAN.md\n"
    "Do not add secrets, private keys, tokens, entropy captures, generated secret bytes, customer data, or internal deployment details to SBOM inputs.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "SBOM status: SBOM_STATUS.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SIGNING_STATUS.txt"
    "AnoCrypto-C signing status\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: signing not performed by P1-28 evidence collection.\n"
    "Private signing keys and signing tokens must not be stored in this repository, evidence directory, or PoC package.\n"
    "Candidate signed artifacts: source archive, source hash file, binary hash file, PoC package archive, SBOM, and evidence manifest after policy review.\n"
    "Preferred future approach: detached signatures with separately reviewed verification instructions.\n"
    "Signing plan: docs/kcmvp/SBOM_SIGNING_PLAN.md\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Signing status: SIGNING_STATUS.txt\n")

if(ANO_CRYPTO_RUN_CTEST)
    find_program(_ctest_exe ctest)
    if(NOT _ctest_exe)
        message(FATAL_ERROR "ctest executable not found")
    endif()
    if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}/CTestTestfile.cmake")
        message(FATAL_ERROR "CTestTestfile.cmake not found in ANO_CRYPTO_BUILD_DIR")
    endif()

    execute_process(
        COMMAND "${_ctest_exe}" --output-on-failure
        WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
        RESULT_VARIABLE _ctest_result
        OUTPUT_VARIABLE _ctest_stdout
        ERROR_VARIABLE _ctest_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" "${_ctest_stdout}${_ctest_stderr}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "CTest result: ${_ctest_result}\n")
    if(NOT _ctest_result EQUAL 0)
        message(FATAL_ERROR "ctest failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log")
    endif()
else()
    set(_prepared_ctest_evidence_dir "${ANO_CRYPTO_BUILD_DIR}/ctest-evidence")
    set(_prepared_ctest_log "${_prepared_ctest_evidence_dir}/ctest.log")
    set(_prepared_ctest_summary "${_prepared_ctest_evidence_dir}/CTEST_EVIDENCE_SUMMARY.txt")
    if(EXISTS "${_prepared_ctest_log}" AND EXISTS "${_prepared_ctest_summary}")
        configure_file("${_prepared_ctest_log}" "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" COPYONLY)
        configure_file("${_prepared_ctest_summary}" "${ANO_CRYPTO_EVIDENCE_DIR}/CTEST_EVIDENCE_SUMMARY.txt" COPYONLY)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "CTest result: 0 (reused standalone evidence capture)\n")
    else()
        file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" "CTest was not requested. Set ANO_CRYPTO_RUN_CTEST=ON to capture test output.\n")
    endif()
endif()

function(_ano_crypto_find_evidence_test_executable _test_name _out_var)
    set(_candidates
        "${ANO_CRYPTO_BUILD_DIR}/${_test_name}"
        "${ANO_CRYPTO_BUILD_DIR}/${_test_name}.exe"
        "${ANO_CRYPTO_BUILD_DIR}/Debug/${_test_name}"
        "${ANO_CRYPTO_BUILD_DIR}/Debug/${_test_name}.exe"
        "${ANO_CRYPTO_BUILD_DIR}/Release/${_test_name}"
        "${ANO_CRYPTO_BUILD_DIR}/Release/${_test_name}.exe"
        "${ANO_CRYPTO_BUILD_DIR}/RelWithDebInfo/${_test_name}"
        "${ANO_CRYPTO_BUILD_DIR}/RelWithDebInfo/${_test_name}.exe"
        "${ANO_CRYPTO_BUILD_DIR}/MinSizeRel/${_test_name}"
        "${ANO_CRYPTO_BUILD_DIR}/MinSizeRel/${_test_name}.exe")
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
    execute_process(
        COMMAND "${_test_executable}"
        WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
        RESULT_VARIABLE _test_result
        OUTPUT_VARIABLE _test_stdout
        ERROR_VARIABLE _test_stderr
        TIMEOUT 120)
    set(${_result_var} "${_test_result}" PARENT_SCOPE)
    set(${_stdout_var} "${_test_stdout}" PARENT_SCOPE)
    set(${_stderr_var} "${_test_stderr}" PARENT_SCOPE)
endfunction()

# The complete CTest pass above already executes every configured test.  Run the
# status-only evidence programs directly here instead of starting a second
# filtered CTest process from an outer build target.  This avoids nested CTest /
# build-runner jobserver interactions while preserving independent exit-status
# and output validation for each evidence artifact.
_ano_crypto_run_evidence_test(
    ano_crypto_sha2_hmac_openssl_differential_tests
    _sha2_diff_result _sha2_diff_stdout _sha2_diff_stderr)
_ano_crypto_run_evidence_test(
    ano_crypto_aes_cbc_openssl_differential_tests
    _aes_cbc_diff_result _aes_cbc_diff_stdout _aes_cbc_diff_stderr)
_ano_crypto_run_evidence_test(
    ano_crypto_aes_cmac_openssl_differential_tests
    _aes_cmac_diff_result _aes_cmac_diff_stdout _aes_cmac_diff_stderr)

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_VALIDATION.log"
    "${_sha2_diff_stdout}${_sha2_diff_stderr}"
    "${_aes_cbc_diff_stdout}${_aes_cbc_diff_stderr}"
    "${_aes_cmac_diff_stdout}${_aes_cmac_diff_stderr}")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
    "OpenSSL differential validation results: sha2-hmac=${_sha2_diff_result}; aes-cbc=${_aes_cbc_diff_result}; aes-cmac=${_aes_cmac_diff_result}\n")
if(NOT _sha2_diff_result EQUAL 0 OR
   NOT _aes_cbc_diff_result EQUAL 0 OR
   NOT _aes_cmac_diff_result EQUAL 0 OR
   NOT _sha2_diff_stdout MATCHES "sha2-hmac-differential status=pass" OR
   NOT _aes_cbc_diff_stdout MATCHES "AES-CBC OpenSSL differential comparisons passed" OR
   NOT _aes_cmac_diff_stdout MATCHES "OpenSSL 3.x AES-CMAC differential validation passed")
    message(FATAL_ERROR "OpenSSL differential validation failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/DIFFERENTIAL_VALIDATION.log")
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
    "AnoCrypto-C P1-28 concurrency and sanitizer review notes\n"
    "Status: engineering evidence only; not official validation evidence.\n"
    "Policy check: CONCURRENCY_SANITIZER_CHECK.txt\n"
    "Platform status: THREAD_PLATFORM_REVIEW.log\n"
    "Selected required sanitizer lane: Linux GCC address+undefined in hosted CI; Clang runtime review remains TODO.\n"
    "ThreadSanitizer: not selected in P1-28; runtime/toolchain review remains TODO.\n"
    "Build-identity ABI decision: docs/design/BUILD_IDENTITY_ABI_0_DECISION.md\n"
    "Actual hosted Linux/macOS/Windows artifacts require separate review before support wording is widened.\n"
    "Logs must not contain key bytes, MAC output, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data.\n")

if(ANO_CRYPTO_RUN_INSTALL_SMOKE)
    set(_prepared_install_smoke_dir "${ANO_CRYPTO_BUILD_DIR}/install-package-smoke")
    set(_prepared_install_smoke_summary "${_prepared_install_smoke_dir}/INSTALL_SMOKE_SUMMARY.txt")
    if(EXISTS "${_prepared_install_smoke_summary}")
        foreach(_artifact IN ITEMS
            INSTALL_SMOKE_INSTALL.log
            INSTALL_SMOKE_CONFIGURE.log
            INSTALL_SMOKE_BUILD.log
            INSTALL_SMOKE_RUN.log
            INSTALL_SMOKE_SUMMARY.txt)
            if(NOT EXISTS "${_prepared_install_smoke_dir}/${_artifact}")
                message(FATAL_ERROR "prepared install-package smoke artifact is incomplete: ${_artifact}")
            endif()
            configure_file(
                "${_prepared_install_smoke_dir}/${_artifact}"
                "${ANO_CRYPTO_EVIDENCE_DIR}/${_artifact}"
                COPYONLY)
        endforeach()
        set(_smoke_result 0)
    else()
        if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tools/run_install_package_smoke.cmake")
            message(FATAL_ERROR "install-package smoke script is missing")
        endif()
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E env
                --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
                "${CMAKE_COMMAND}"
                -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
                -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
                -DANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR=${_prepared_install_smoke_dir}
                -P "${ANO_CRYPTO_SOURCE_DIR}/tools/run_install_package_smoke.cmake"
            RESULT_VARIABLE _smoke_prepare_result
            OUTPUT_VARIABLE _smoke_prepare_stdout
            ERROR_VARIABLE _smoke_prepare_stderr)
        if(NOT _smoke_prepare_result EQUAL 0 OR NOT EXISTS "${_prepared_install_smoke_summary}")
            file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
                "${_smoke_prepare_stdout}${_smoke_prepare_stderr}")
            message(FATAL_ERROR "install-package smoke preparation failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log")
        endif()
        foreach(_artifact IN ITEMS
            INSTALL_SMOKE_INSTALL.log
            INSTALL_SMOKE_CONFIGURE.log
            INSTALL_SMOKE_BUILD.log
            INSTALL_SMOKE_RUN.log
            INSTALL_SMOKE_SUMMARY.txt)
            configure_file(
                "${_prepared_install_smoke_dir}/${_artifact}"
                "${ANO_CRYPTO_EVIDENCE_DIR}/${_artifact}"
                COPYONLY)
        endforeach()
        set(_smoke_result 0)
    endif()
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Install smoke result: ${_smoke_result}\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "Install smoke was not requested. Set ANO_CRYPTO_RUN_INSTALL_SMOKE=ON to capture installed-package smoke output.\n")
endif()

if(ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE)
    set(_prepared_poc_smoke_dir "${ANO_CRYPTO_BUILD_DIR}/poc-package-smoke")
    set(_prepared_poc_summary "${_prepared_poc_smoke_dir}/POC_PACKAGE_SMOKE_SUMMARY.txt")
    if(EXISTS "${_prepared_poc_summary}")
        file(REMOVE_RECURSE "${ANO_CRYPTO_POC_SMOKE_DIR}")
        file(MAKE_DIRECTORY "${ANO_CRYPTO_POC_SMOKE_DIR}")
        file(COPY "${_prepared_poc_smoke_dir}/" DESTINATION "${ANO_CRYPTO_POC_SMOKE_DIR}")
        set(_poc_smoke_result 0)
        set(_poc_smoke_stdout "Reused prebuilt ano_crypto_poc_package_smoke artifact: ${_prepared_poc_smoke_dir}\n")
        set(_poc_smoke_stderr "")
    else()
        if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake")
            message(FATAL_ERROR "PoC package smoke script is missing: ${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake")
        endif()
        execute_process(
            COMMAND "${CMAKE_COMMAND}"
                -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
                -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
                -DANO_CRYPTO_POC_SMOKE_DIR=${ANO_CRYPTO_POC_SMOKE_DIR}
                -P "${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake"
            RESULT_VARIABLE _poc_smoke_result
            OUTPUT_VARIABLE _poc_smoke_stdout
            ERROR_VARIABLE _poc_smoke_stderr)
    endif()
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
"- `AES_CMAC_IMPLEMENTATION_CHECK.txt`: AES-CMAC API, CSP, KAT, differential, package, and 69-symbol policy check.\n"
"- `AES_CMAC_REVIEW_NOTES.txt`: full-tag CMAC scope, dedicated key-purpose, differential boundary, and remaining review notes.\n"
"- `SHA512_FAMILY_CHECK.txt`: SHA-384/SHA-512 family implementation, KAT, negative-test, and package wiring check.\n"
"- `DIFFERENTIAL_PORTABILITY_CHECK.txt`: OpenSSL test-only provider, deterministic comparison, C99 representation, and portability policy check.\n"
"- `DIFFERENTIAL_VALIDATION.log`: status-only OpenSSL 3.x SHA-2/HMAC, AES-CBC, and AES-CMAC differential result.\n"
"- `SHA512_PORTABILITY_REVIEW.log`: status-only representation, endian, and unaligned-input review.\n"
"- `DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt`: provider/product boundary and remaining hosted-artifact review notes.\n"
"- `RANDOM_SERVICE_CHECK.txt`: public random-service implementation policy check.\n"
"- `RANDOM_SERVICE_REVIEW_NOTES.txt`: random-service implementation review notes.\n"
"- `RANDOM_LIFECYCLE_HARDENING_CHECK.txt`: P1-26 lifecycle, process-marker, VM policy, hook, and ABI-baseline policy check.\n"
"- `RANDOM_LIFECYCLE_REVIEW_NOTES.txt`: status-only lifecycle and portability review notes.\n"
"- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`: clean Release comparison against the stored 69-symbol ABI-0 baseline.\n"
"- `EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt`: exported-symbol guardrail scope and limitations.\n"
"- `ENTROPY_HEALTH_CHECK.txt`: entropy failure-injection and health-test boundary policy check.\n"
"- `ENTROPY_HEALTH_REVIEW_NOTES.txt`: health-test state-transition and logging review notes.\n"
"- `ENTROPY_OPERATIONAL_POLICY_CHECK.txt`: source classification, health ownership, operational probe, and process-boundary policy check.\n"
"- `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`: entropy source and process-change review notes.\n"
"- `REPRODUCIBLE_BUILD_NOTES.txt`: current reproducibility procedure notes and unresolved gaps.\n"
"- `ctest.log`: captured CTest output when requested.\n"
"- `INSTALL_SMOKE_*.log`: installed-package configure/build/run logs when requested.\n"
"- `INSTALL_SMOKE_SUMMARY.txt`: install smoke paths and result when requested.\n"
"- `POC_PACKAGE_SMOKE.log`: customer PoC package creation/configure/build/run summary when requested.\n"
"- `poc-package-smoke/`: detailed PoC package smoke logs and staged package when requested.\n"
"- `SBOM_STATUS.txt`: placeholder status for future SBOM generation.\n"
"- `SIGNING_STATUS.txt`: placeholder status for future detached-signature workflow.\n"
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
"- Confirm AES logs contain no key, round key, IV, plaintext, ciphertext, or reference-result bytes.\n"
"- Confirm hosted 32-bit, Windows, macOS, and big-endian artifacts are not marked reviewed unless the referenced runs completed successfully.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST_README.txt"
    "AnoCrypto-C evidence manifest\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: engineering hash manifest for local evidence review.\n"
    "Manifest file: EVIDENCE_MANIFEST.sha256\n"
    "Policy document: docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md\n"
    "The manifest excludes itself, the temporary source-archive-work tree, and transient smoke build directories. Review local paths and logs before sharing.\n")

file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Evidence manifest: EVIDENCE_MANIFEST.sha256\n")

file(GLOB_RECURSE _evidence_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_EVIDENCE_DIR}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/*")
list(SORT _evidence_files)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "")
foreach(_rel IN LISTS _evidence_files)
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
        file(SHA256 "${ANO_CRYPTO_EVIDENCE_DIR}/${_rel}" _evidence_hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "${_evidence_hash}  ${_rel}\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C evidence draft written to ${ANO_CRYPTO_EVIDENCE_DIR}")
