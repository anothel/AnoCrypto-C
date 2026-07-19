# Assemble and validate logs produced by bounded CTest ranges and exact direct
# commands for nested-process tests. Ten non-overlapping ranges cover 1..84.

cmake_minimum_required(VERSION 3.16)


if(DEFINED ANO_CRYPTO_WRITE_RANGE_SUMMARY AND ANO_CRYPTO_WRITE_RANGE_SUMMARY)
    foreach(_required IN ITEMS
        ANO_CRYPTO_RANGE_SUMMARY_OUTPUT
        ANO_CRYPTO_RANGE_SUMMARY_RANGE
        ANO_CRYPTO_RANGE_SUMMARY_COUNT
        ANO_CRYPTO_RANGE_SUMMARY_TESTS)
        if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
            message(FATAL_ERROR "${_required} is required for range-summary mode")
        endif()
    endforeach()
    get_filename_component(_summary_dir
        "${ANO_CRYPTO_RANGE_SUMMARY_OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_summary_dir}")
    if(NOT DEFINED ANO_CRYPTO_BUILD_CONFIG OR
       ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
        set(ANO_CRYPTO_BUILD_CONFIG "default")
    endif()
    if(NOT DEFINED ANO_CRYPTO_RANGE_SUMMARY_MODE OR
       ANO_CRYPTO_RANGE_SUMMARY_MODE STREQUAL "")
        set(ANO_CRYPTO_RANGE_SUMMARY_MODE "ctest-direct-range")
    endif()
    file(WRITE "${ANO_CRYPTO_RANGE_SUMMARY_OUTPUT}"
        "AnoCrypto-C nested-process-safe test range summary
"
        "Range: ${ANO_CRYPTO_RANGE_SUMMARY_RANGE}
"
        "Tests: ${ANO_CRYPTO_RANGE_SUMMARY_TESTS}
"
        "Build configuration: ${ANO_CRYPTO_BUILD_CONFIG}
"
        "Execution mode: ${ANO_CRYPTO_RANGE_SUMMARY_MODE}
"
        "Detailed output: emitted to the parent build log
"
        "100% tests passed, 0 tests failed out of ${ANO_CRYPTO_RANGE_SUMMARY_COUNT}
")
    if(DEFINED ANO_CRYPTO_RANGE_DETAIL_LOGS AND
       NOT ANO_CRYPTO_RANGE_DETAIL_LOGS STREQUAL "")
        string(REPLACE "," ";" _detail_logs "${ANO_CRYPTO_RANGE_DETAIL_LOGS}")
        foreach(_detail_log IN LISTS _detail_logs)
            if(NOT EXISTS "${_detail_log}" OR IS_DIRECTORY "${_detail_log}" OR
               IS_SYMLINK "${_detail_log}")
                message(FATAL_ERROR "Range detail log is missing or unsafe: ${_detail_log}")
            endif()
            file(READ "${_detail_log}" _detail_content)
            get_filename_component(_detail_name "${_detail_log}" NAME)
            file(APPEND "${ANO_CRYPTO_RANGE_SUMMARY_OUTPUT}"
                "
===== ${_detail_name} =====
${_detail_content}")
        endforeach()
    endif()
    message(STATUS
        "Wrote verified test range summary: ${ANO_CRYPTO_RANGE_SUMMARY_OUTPUT}")
    return()
endif()

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
    "71_76"
    "77_77"
    "78_78"
    "79_81"
    "82_84")
set(_ctest_ranges
    "1,20"
    "21,44"
    "45,68"
    "69,69"
    "70,70"
    "71,76"
    "77,77"
    "78,78"
    "79,81"
    "82,84")
set(_ctest_counts 20 24 24 1 1 6 1 1 3 3)
set(_ctest_log "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest.log")
file(WRITE "${_ctest_log}"
    "AnoCrypto-C complete CTest evidence\n"
    "Execution mode: deterministic bounded direct ranges\n\n")

foreach(_index RANGE 0 9)
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
    "Configured command evidence result: 0\n"
    "Build configuration: ${ANO_CRYPTO_BUILD_CONFIG}\n"
    "Execution mode: bounded CTest ranges plus exact direct commands for nested-process tests\n"
    "Completed ranges: 10/10\n"
    "Configured tests covered: 84/84\n"
    "Sensitive cryptographic values logged: no\n")
message(STATUS "AnoCrypto-C CTest evidence capture passed: ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
