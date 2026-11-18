cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_TEST_WORK_DIR OR ANO_CRYPTO_TEST_WORK_DIR STREQUAL "")
    message(FATAL_ERROR "ANO_CRYPTO_TEST_WORK_DIR is required")
endif()

set(_validator "${CMAKE_CURRENT_LIST_DIR}/../../tools/run_ctest_evidence.cmake")

function(_check_evidence _name _suffix _replacement _missing_last _expected_error)
    set(_dir "${ANO_CRYPTO_TEST_WORK_DIR}/${_name}")
    file(REMOVE_RECURSE "${_dir}")
    file(MAKE_DIRECTORY "${_dir}")
    foreach(_entry IN ITEMS
        "1_20=20" "21_44=24" "45_68=24" "69_84=16" "85_90=6"
        "91_94=4" "95_95=1" "96_96=1" "97_99=3" "100_102=3")
        string(REPLACE "=" ";" _fields "${_entry}")
        list(GET _fields 0 _range)
        list(GET _fields 1 _count)
        if(_missing_last AND _range STREQUAL "100_102")
            continue()
        endif()
        set(_summary "100% tests passed${_suffix} out of ${_count}")
        if(_range STREQUAL "1_20" AND NOT _replacement STREQUAL "")
            set(_summary "${_replacement}")
        endif()
        file(WRITE "${_dir}/ctest-${_range}.log" "CTest range output\n\n${_summary}\n\nTotal Test time (real) = 0.01 sec\n")
    endforeach()

    execute_process(COMMAND "${CMAKE_COMMAND}" "-DANO_CRYPTO_CTEST_EVIDENCE_DIR=${_dir}"
        -P "${_validator}" RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 30)
    set(_summary_file "${_dir}/CTEST_EVIDENCE_SUMMARY.txt")
    if(_expected_error STREQUAL "")
        if(NOT _result STREQUAL "0")
            message(FATAL_ERROR "${_name} failed unexpectedly:\n${_stdout}${_stderr}")
        endif()
        file(READ "${_summary_file}" _report)
        if(NOT _report MATCHES "Completed ranges: 10/10\nConfigured tests covered: 102/102\n")
            message(FATAL_ERROR "${_name} did not verify all 102 tests")
        endif()
    else()
        string(FIND "${_stderr}" "${_expected_error}" _diagnostic)
        if(NOT _result STREQUAL "1" OR _diagnostic EQUAL -1 OR EXISTS "${_summary_file}")
            message(FATAL_ERROR "${_name} did not reject invalid evidence as expected:\n${_stdout}${_stderr}")
        endif()
    endif()
endfunction()

set(_invalid_summary "CTest range 1,20 does not contain the expected pass summary")
foreach(_format IN ITEMS legacy modern)
    set(_suffix "")
    if(_format STREQUAL "legacy")
        set(_suffix ", 0 tests failed")
    endif()
    _check_evidence("${_format}-complete" "${_suffix}" "" FALSE "")
    _check_evidence("${_format}-failed-tests" "${_suffix}"
        "95% tests passed, 1 tests failed out of 20" FALSE "${_invalid_summary}")
    _check_evidence("${_format}-wrong-count" "${_suffix}"
        "100% tests passed${_suffix} out of 200" FALSE "${_invalid_summary}")
    _check_evidence("${_format}-incomplete-count" "${_suffix}"
        "100% tests passed${_suffix} out of 19" FALSE "${_invalid_summary}")
    _check_evidence("${_format}-skipped-test" "${_suffix}"
        "100% tests passed${_suffix} out of 20\n\nThe following tests did not run:\n\t2 - fixture (Skipped)"
        FALSE "CTest range 1,20 reports tests that did not run")
    _check_evidence("${_format}-no-summary" "${_suffix}"
        "Test run was interrupted" FALSE "${_invalid_summary}")
    _check_evidence("${_format}-embedded-summary" "${_suffix}"
        "tool output: 100% tests passed${_suffix} out of 20" FALSE "${_invalid_summary}")
    _check_evidence("${_format}-conflicting-summaries" "${_suffix}"
        "100% tests passed${_suffix} out of 20\n95% tests passed, 1 tests failed out of 20"
        FALSE "${_invalid_summary}")
    _check_evidence("${_format}-missing-final-range" "${_suffix}" "" TRUE
        "CTest range log is missing or unsafe:")
endforeach()
message(STATUS "CTest evidence summary regressions passed for legacy and modern formats")
