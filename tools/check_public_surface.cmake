# AnoCrypto-C public surface and repository policy check.
# This is an engineering guardrail, not formal validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_POLICY_CHECK_OUTPUT)
    set(ANO_CRYPTO_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/PUBLIC_SURFACE_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_policy_dir "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_policy_dir}")

set(_violations "")
set(_warnings "")

string(CONCAT _retired_a "crypto" "-" "core")
string(CONCAT _retired_b "crypto" "_" "core")
string(CONCAT _retired_c "CRYPTO" "_" "CORE")
string(CONCAT _claim_a "검증" "필")
string(CONCAT _claim_b "인증" " " "완료")
string(CONCAT _claim_c "production" "-" "ready")
string(CONCAT _claim_d "audit" "ed")
string(CONCAT _claim_e "constant" "-" "time certified")
string(CONCAT _claim_f "FIPS" "-" "compliant")
string(CONCAT _cpp_a "std" "::")
string(CONCAT _cpp_b "template" "<")
string(CONCAT _cpp_c "<" "vector" ">")
string(CONCAT _cpp_d "<" "string" ">")
string(CONCAT _cpp_e "throw" "(")

set(_cpp_terms
    "${_cpp_a}"
    "${_cpp_b}"
    "${_cpp_c}"
    "${_cpp_d}"
    "${_cpp_e}")

set(_blocked_terms
    "${_retired_a}"
    "${_retired_b}"
    "${_retired_c}"
    "${_claim_a}"
    "${_claim_b}"
    "${_claim_c}"
    "${_claim_d}"
    "${_claim_e}"
    "${_claim_f}")

set(_repo_files "")
find_program(_git_executable git)
if(_git_executable)
    execute_process(
        COMMAND "${_git_executable}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            ls-files --cached --others --exclude-standard
        RESULT_VARIABLE _git_files_result
        OUTPUT_VARIABLE _git_files_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 60)
    if("${_git_files_result}" STREQUAL "0")
        string(REPLACE "\r\n" "\n" _git_files_output "${_git_files_output}")
        string(REPLACE "\n" ";" _repo_files "${_git_files_output}")
    endif()
endif()
if(NOT _repo_files)
    file(GLOB_RECURSE _repo_files
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
        "${ANO_CRYPTO_SOURCE_DIR}/*")
endif()
list(SORT _repo_files)

foreach(_rel IN LISTS _repo_files)
    set(_skip OFF)
    if(_rel MATCHES "(^|/)(\\.git|build|build-[^/]*|cmake-build-[^/]*|\\.cache)(/|$)")
        set(_skip ON)
    endif()
    if(_rel MATCHES "^evidence/(generated|out|p1-[0-9][^/]*|release-[^/]*)/")
        set(_skip ON)
    endif()
    if(_rel MATCHES "\\.(zip|patch|tar|tgz|gz|so|dylib|dll|lib|a|o|obj|exe|pdb)$")
        set(_skip ON)
    endif()

    if(NOT _skip)
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _content)
        foreach(_term IN LISTS _blocked_terms)
            string(FIND "${_content}" "${_term}" _term_pos)
            if(NOT _term_pos EQUAL -1)
                list(APPEND _violations "blocked term class found in ${_rel}")
            endif()
        endforeach()
    endif()
endforeach()

file(GLOB _public_headers
    LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(SORT _public_headers)

if(_public_headers STREQUAL "")
    list(APPEND _violations "no public headers found under include/anocrypto")
endif()

foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_content)
    file(RELATIVE_PATH _header_rel "${ANO_CRYPTO_SOURCE_DIR}" "${_header}")

    if(_header_content MATCHES "typedef[ \t\r\n]+struct[^;]*\\{")
        if(_header_rel STREQUAL "include/anocrypto/ac_build.h")
            list(APPEND _warnings "public build identity struct layout remains a tracked ABI TODO: ${_header_rel}")
        else()
            list(APPEND _violations "public struct layout exposed in ${_header_rel}")
        endif()
    endif()

    foreach(_cpp_term IN LISTS _cpp_terms)
        string(FIND "${_header_content}" "${_cpp_term}" _cpp_pos)
        if(NOT _cpp_pos EQUAL -1)
            list(APPEND _violations "C++ core dependency token found in ${_header_rel}")
        endif()
    endforeach()
endforeach()

if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/cmake/AnoCryptoCConfig.cmake.in")
    list(APPEND _violations "CMake package config template missing: cmake/AnoCryptoCConfig.cmake.in")
endif()

if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/CMakeLists.txt")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/CMakeLists.txt" _cmake_content)
    string(FIND "${_cmake_content}" "project(AnoCryptoC" _project_pos)
    string(FIND "${_cmake_content}" "AnoCryptoC::AnoCryptoC" _target_pos)
    string(FIND "${_cmake_content}" "OUTPUT_NAME anocrypto_c" _output_pos)
    if(_project_pos EQUAL -1)
        list(APPEND _violations "CMake project identity is not AnoCryptoC")
    endif()
    if(_target_pos EQUAL -1)
        list(APPEND _violations "CMake imported target identity is missing")
    endif()
    if(_output_pos EQUAL -1)
        list(APPEND _violations "CMake library output name is not anocrypto_c")
    endif()
endif()

set(_shared_lib "${ANO_CRYPTO_BUILD_DIR}/libanocrypto_c.so")
if(EXISTS "${_shared_lib}")
    find_program(_nm_exe nm)
    if(_nm_exe)
        execute_process(
            COMMAND "${_nm_exe}" -D --defined-only "${_shared_lib}"
            RESULT_VARIABLE _nm_result
            OUTPUT_VARIABLE _nm_stdout
            ERROR_VARIABLE _nm_stderr
            TIMEOUT 60)
        if(_nm_result EQUAL 0)
            string(REPLACE "\n" ";" _nm_lines "${_nm_stdout}")
            foreach(_line IN LISTS _nm_lines)
                if(NOT _line STREQUAL "")
                    string(REGEX MATCH "[^ \t]+$" _symbol "${_line}")
                    if(_symbol MATCHES "^ANOCRYPTO_C_[0-9]+$")
                        continue()
                    endif()
                    string(REGEX REPLACE "@.*$" "" _base_symbol "${_symbol}")
                    if(NOT _base_symbol MATCHES "^ano_crypto_.*")
                        list(APPEND _violations "unexpected exported symbol: ${_symbol}")
                    endif()
                    if(_base_symbol MATCHES "^ano_crypto_internal_.*")
                        list(APPEND _warnings "internal exported symbol requires review: ${_symbol}")
                    endif()
                endif()
            endforeach()
        else()
            message(FATAL_ERROR
                "Exported-symbol scan failed for ${_shared_lib}: ${_nm_stderr}")
        endif()
    else()
        message(FATAL_ERROR "Exported-symbol scan requires nm when ${_shared_lib} exists")
    endif()
endif()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}"
    "AnoCrypto-C public surface check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- Retired naming scan\n"
    "- Premature assurance wording scan\n"
    "- Public header struct-layout scan\n"
    "- C++ dependency token scan\n"
    "- CMake package identity scan\n"
    "- Linux exported symbol scan when a shared object is present\n\n")

if(_warnings)
    list(LENGTH _warnings _warning_count)
    file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "Warnings / tracked TODOs:\n")
    foreach(_warning IN LISTS _warnings)
        file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "- ${_warning}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "\n")
    message(WARNING "AnoCrypto-C public surface check has ${_warning_count} tracked warning/TODO item(s); see ${ANO_CRYPTO_POLICY_CHECK_OUTPUT}")
else()
    file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "Warnings / tracked TODOs: none\n\n")
endif()

if(_violations)
    file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C public surface check failed; see ${ANO_CRYPTO_POLICY_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_POLICY_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C public surface check passed: ${ANO_CRYPTO_POLICY_CHECK_OUTPUT}")
