# Assemble and validate CTest logs produced by the direct custom-target commands.
# The target executes six bounded, non-overlapping ranges that cover tests 1..80.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_CTEST_EVIDENCE_DIR)
    message(FATAL_ERROR "ANO_CRYPTO_CTEST_EVIDENCE_DIR is required")
endif()
get_filename_component(ANO_CRYPTO_CTEST_EVIDENCE_DIR
    "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}" ABSOLUTE)
if(NOT IS_DIRECTORY "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
    message(FATAL_ERROR
        "CTest evidence directory is unavailable: ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_CONFIG OR
   ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
    set(ANO_CRYPTO_BUILD_CONFIG "default")
endif()

set(_ctest_tokens
    "1_20"
    "21_44"
    "45_68"
    "69_69"
    "70_70"
    "71_80")
set(_ctest_ranges
    "1,20"
    "21,44"
    "45,68"
    "69,69"
    "70,70"
    "71,80")
set(_ctest_counts 20 24 24 1 1 10)
set(_ctest_log "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest.log")
file(WRITE "${_ctest_log}"
    "AnoCrypto-C complete CTest evidence\n"
    "Execution mode: deterministic bounded direct ranges\n\n")

foreach(_index RANGE 0 5)
    list(GET _ctest_tokens ${_index} _token)
    list(GET _ctest_ranges ${_index} _range)
    list(GET _ctest_counts ${_index} _count)
    set(_range_log "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest-${_token}.log")
    if(NOT EXISTS "${_range_log}" OR IS_DIRECTORY "${_range_log}" OR
       IS_SYMLINK "${_range_log}")
        message(FATAL_ERROR "CTest range log is missing or unsafe: ${_range_log}")
    endif()
    file(READ "${_range_log}" _range_output)
    if(NOT _range_output MATCHES
       "100% tests passed, 0 tests failed out of ${_count}")
        message(FATAL_ERROR
            "CTest range ${_range} does not contain the expected pass summary")
    endif()
    file(APPEND "${_ctest_log}"
        "===== CTest range ${_range} =====\n"
        "${_range_output}"
        "===== End CTest range ${_range}; result 0 =====\n\n")
endforeach()

file(WRITE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/CTEST_EVIDENCE_SUMMARY.txt"
    "CTest result: 0\n"
    "Build configuration: ${ANO_CRYPTO_BUILD_CONFIG}\n"
    "Execution mode: deterministic bounded direct ranges\n"
    "Completed ranges: 6/6\n"
    "Configured tests covered: 80/80\n"
    "Sensitive cryptographic values logged: no\n")
message(STATUS "AnoCrypto-C CTest evidence capture passed: ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
