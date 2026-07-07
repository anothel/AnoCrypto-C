# AnoCrypto-C release evidence collection draft.
# Usage:
#   cmake -DANO_CRYPTO_SOURCE_DIR=. \
#         -DANO_CRYPTO_BUILD_DIR=build \
#         -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-4 \
#         -DANO_CRYPTO_RUN_CTEST=ON \
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
    set(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/evidence/p1-4")
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CTEST)
    set(ANO_CRYPTO_RUN_CTEST OFF)
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}" ABSOLUTE)

file(MAKE_DIRECTORY "${ANO_CRYPTO_EVIDENCE_DIR}")

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

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "")
foreach(_rel IN LISTS _source_files)
    set(_skip OFF)
    if(_rel MATCHES "(^|/)(\\.git|build|cmake-build-[^/]*|\\.cache)(/|$)")
        set(_skip ON)
    endif()
    if(_rel MATCHES "^evidence/(generated|out|p1-[0-9][^/]*|release-[^/]*)/")
        set(_skip ON)
    endif()
    if(_rel MATCHES "\\.(zip|patch|tar|tgz|gz)$")
        set(_skip ON)
    endif()

    if(NOT _skip)
        file(SHA256 "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "${_hash}  ${_rel}\n")
    endif()
endforeach()

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
        if(NOT _rel MATCHES "CMakeFiles/")
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

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/README.md"
"# AnoCrypto-C Evidence Draft\n\n"
"Generated: ${ANO_CRYPTO_EVIDENCE_UTC}\n\n"
"This directory is an engineering evidence draft for the current local build.\n"
"It is not official validation evidence. Review path disclosure, source provenance,\n"
"test-vector provenance, compiler identity, and binary provenance before sharing.\n\n"
"## Files\n\n"
"- `BUILD_METADATA.txt`: local build and source metadata.\n"
"- `SOURCE_FILES.sha256`: SHA-256 hashes of repository source files included by the script.\n"
"- `BINARY_FILES.sha256`: SHA-256 hashes of build outputs matched by the script.\n"
"- `ctest.log`: captured CTest output when requested.\n\n"
"## TODO before release-candidate use\n\n"
"- Add source archive SHA-256.\n"
"- Add install smoke log from an installed package prefix.\n"
"- Add SBOM/signing status.\n"
"- Confirm no secrets, entropy bytes, generated secret bytes, private keys, tokens, or internal deployment details are present.\n")

message(STATUS "AnoCrypto-C evidence draft written to ${ANO_CRYPTO_EVIDENCE_DIR}")
