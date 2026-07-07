# AnoCrypto-C release evidence collection draft.
# Usage:
#   cmake -DANO_CRYPTO_SOURCE_DIR=. \
#         -DANO_CRYPTO_BUILD_DIR=build \
#         -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-7 \
#         -DANO_CRYPTO_RUN_CTEST=ON \
#         -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
#         -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
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
    set(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/evidence/p1-7")
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

get_filename_component(ANO_CRYPTO_INSTALL_PREFIX "${ANO_CRYPTO_INSTALL_PREFIX}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}" ABSOLUTE)

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
    set(_archive_tree "${_archive_root}/AnoCrypto-C")
    set(_archive_path "${ANO_CRYPTO_EVIDENCE_DIR}/AnoCrypto-C-source.tar.gz")
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
        COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_archive_path}" "AnoCrypto-C"
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
        "${_archive_hash}  AnoCrypto-C-source.tar.gz\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Source archive: AnoCrypto-C-source.tar.gz\n"
        "Source archive SHA-256: ${_archive_hash}\n")
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
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" "CTest was not requested. Set ANO_CRYPTO_RUN_CTEST=ON to capture test output.\n")
endif()

if(ANO_CRYPTO_RUN_INSTALL_SMOKE)
    if(NOT EXISTS "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "install smoke source directory is missing CMakeLists.txt: ${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}")
    endif()

    file(REMOVE_RECURSE "${ANO_CRYPTO_INSTALL_PREFIX}" "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")
    file(MAKE_DIRECTORY "${ANO_CRYPTO_INSTALL_PREFIX}" "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_INSTALL.log"
        "Install AnoCrypto-C package prefix"
        "${CMAKE_COMMAND}" --install "${ANO_CRYPTO_BUILD_DIR}" --prefix "${ANO_CRYPTO_INSTALL_PREFIX}")

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_CONFIGURE.log"
        "Configure install smoke consumer"
        "${CMAKE_COMMAND}" -S "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}" -B "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" -DCMAKE_PREFIX_PATH=${ANO_CRYPTO_INSTALL_PREFIX})

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_BUILD.log"
        "Build install smoke consumer"
        "${CMAKE_COMMAND}" --build "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" --parallel)

    set(_smoke_exe "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke")
    if(NOT EXISTS "${_smoke_exe}")
        set(_smoke_exe "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke.exe")
    endif()
    if(NOT EXISTS "${_smoke_exe}")
        file(GLOB_RECURSE _smoke_candidates
            LIST_DIRECTORIES false
            "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke*")
        foreach(_candidate IN LISTS _smoke_candidates)
            if(NOT _candidate MATCHES "CMakeFiles")
                set(_smoke_exe "${_candidate}")
                break()
            endif()
        endforeach()
    endif()
    if(NOT EXISTS "${_smoke_exe}")
        message(FATAL_ERROR "install smoke executable not found under ${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")
    endif()

    execute_process(
        COMMAND "${_smoke_exe}"
        WORKING_DIRECTORY "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}"
        RESULT_VARIABLE _smoke_result
        OUTPUT_VARIABLE _smoke_stdout
        ERROR_VARIABLE _smoke_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "# Run install smoke consumer\n# Executable: ${_smoke_exe}\n# Result: ${_smoke_result}\n\n${_smoke_stdout}${_smoke_stderr}")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_SUMMARY.txt"
        "Install prefix: ${ANO_CRYPTO_INSTALL_PREFIX}\n"
        "Install smoke source: ${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}\n"
        "Install smoke build: ${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}\n"
        "Install smoke executable: ${_smoke_exe}\n"
        "Install smoke result: ${_smoke_result}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Install smoke result: ${_smoke_result}\n")
    if(NOT _smoke_result EQUAL 0)
        message(FATAL_ERROR "install smoke failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "Install smoke was not requested. Set ANO_CRYPTO_RUN_INSTALL_SMOKE=ON to capture installed-package smoke output.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/README.md"
"# AnoCrypto-C Evidence Draft\n\n"
"Generated: ${ANO_CRYPTO_EVIDENCE_UTC}\n\n"
"This directory is an engineering evidence draft for the current local build.\n"
"It is not official validation evidence. Review path disclosure, source provenance,\n"
"test-vector provenance, compiler identity, binary provenance, and smoke logs before sharing.\n\n"
"## Files\n\n"
"- `BUILD_METADATA.txt`: local build and source metadata.\n"
"- `SOURCE_FILES.sha256`: SHA-256 hashes of repository source files included by the script.\n"
"- `SOURCE_ARCHIVE_CONTENTS.txt`: file list used for the local source archive.\n"
"- `SOURCE_ARCHIVE.sha256`: SHA-256 hash of `AnoCrypto-C-source.tar.gz` when archive creation is enabled.\n"
"- `BINARY_FILES.sha256`: SHA-256 hashes of build outputs matched by the script.\n"
"- `ctest.log`: captured CTest output when requested.\n"
"- `INSTALL_SMOKE_*.log`: installed-package configure/build/run logs when requested.\n"
"- `INSTALL_SMOKE_SUMMARY.txt`: install smoke paths and result when requested.\n"
"- Platform entropy provider status appears in `ctest.log` when the provider test output is shown by CTest.\n\n"
"## Review before release-candidate use\n\n"
"- Confirm source archive contents match the intended release scope.\n"
"- Confirm install smoke paths do not disclose internal deployment details before sharing.\n"
"- Add SBOM/signing status when available.\n"
"- Confirm no secrets, entropy bytes, generated secret bytes, private keys, tokens, or internal deployment details are present.\n"
"- Confirm platform entropy evidence records provider/status only and does not contain entropy bytes.\n")

message(STATUS "AnoCrypto-C evidence draft written to ${ANO_CRYPTO_EVIDENCE_DIR}")
