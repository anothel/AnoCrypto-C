cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_LIBRARY_FILE ANO_CRYPTO_SYMBOL_BASELINE ANO_CRYPTO_SYMBOL_TOOL ANO_CRYPTO_SYMBOL_STYLE ANO_CRYPTO_SYMBOL_REPORT)
    if(NOT DEFINED ${_required})
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

if(NOT EXISTS "${ANO_CRYPTO_LIBRARY_FILE}")
    message(FATAL_ERROR "Shared library does not exist: ${ANO_CRYPTO_LIBRARY_FILE}")
endif()
if(NOT EXISTS "${ANO_CRYPTO_SYMBOL_BASELINE}")
    message(FATAL_ERROR "Symbol baseline does not exist: ${ANO_CRYPTO_SYMBOL_BASELINE}")
endif()

get_filename_component(_report_dir "${ANO_CRYPTO_SYMBOL_REPORT}" DIRECTORY)
file(MAKE_DIRECTORY "${_report_dir}")

if(ANO_CRYPTO_SYMBOL_STYLE STREQUAL "GNU")
    execute_process(
        COMMAND "${ANO_CRYPTO_SYMBOL_TOOL}" -D --defined-only "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _tool_result
        OUTPUT_VARIABLE _tool_output
        ERROR_VARIABLE _tool_error
        TIMEOUT 60)
elseif(ANO_CRYPTO_SYMBOL_STYLE STREQUAL "APPLE")
    execute_process(
        COMMAND "${ANO_CRYPTO_SYMBOL_TOOL}" -gU "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _tool_result
        OUTPUT_VARIABLE _tool_output
        ERROR_VARIABLE _tool_error
        TIMEOUT 60)
elseif(ANO_CRYPTO_SYMBOL_STYLE STREQUAL "MSVC")
    execute_process(
        COMMAND "${ANO_CRYPTO_SYMBOL_TOOL}" /nologo /exports "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _tool_result
        OUTPUT_VARIABLE _tool_output
        ERROR_VARIABLE _tool_error
        TIMEOUT 60)
else()
    message(FATAL_ERROR "Unsupported symbol tool style: ${ANO_CRYPTO_SYMBOL_STYLE}")
endif()

if(NOT _tool_result EQUAL 0)
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nTool result: ${_tool_result}\n${_tool_output}${_tool_error}\n")
    message(FATAL_ERROR "Exported-symbol tool failed; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

string(REGEX MATCHALL "_?ano_crypto_[A-Za-z0-9_]+" _actual_symbols "${_tool_output}")
set(_normalized_actual "")
foreach(_symbol IN LISTS _actual_symbols)
    string(REGEX REPLACE "^_" "" _symbol "${_symbol}")
    list(APPEND _normalized_actual "${_symbol}")
endforeach()
list(REMOVE_DUPLICATES _normalized_actual)
list(SORT _normalized_actual)

file(STRINGS "${ANO_CRYPTO_SYMBOL_BASELINE}" _baseline_lines)
set(_baseline_symbols "")
foreach(_line IN LISTS _baseline_lines)
    string(STRIP "${_line}" _line)
    if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
        list(APPEND _baseline_symbols "${_line}")
    endif()
endforeach()
list(REMOVE_DUPLICATES _baseline_symbols)
list(SORT _baseline_symbols)

set(_missing "")
foreach(_symbol IN LISTS _baseline_symbols)
    if(NOT _symbol IN_LIST _normalized_actual)
        list(APPEND _missing "${_symbol}")
    endif()
endforeach()
set(_unexpected "")
foreach(_symbol IN LISTS _normalized_actual)
    if(NOT _symbol IN_LIST _baseline_symbols)
        list(APPEND _unexpected "${_symbol}")
    endif()
endforeach()

list(LENGTH _baseline_symbols _baseline_count)
list(LENGTH _normalized_actual _actual_count)
file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
    "AnoCrypto-C clean Release exported-symbol baseline check\n"
    "Library: ${ANO_CRYPTO_LIBRARY_FILE}\n"
    "Baseline: ${ANO_CRYPTO_SYMBOL_BASELINE}\n"
    "Tool style: ${ANO_CRYPTO_SYMBOL_STYLE}\n"
    "Baseline count: ${_baseline_count}\n"
    "Actual count: ${_actual_count}\n")

if(_missing)
    file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "Missing symbols:\n")
    foreach(_symbol IN LISTS _missing)
        file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "  ${_symbol}\n")
    endforeach()
endif()
if(_unexpected)
    file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "Unexpected symbols:\n")
    foreach(_symbol IN LISTS _unexpected)
        file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "  ${_symbol}\n")
    endforeach()
endif()

if(_missing OR _unexpected)
    file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "Result: FAIL\n")
    message(FATAL_ERROR "Exported symbols differ from the stored ABI-0 baseline; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C exported symbols match ABI-0 baseline (${_actual_count} symbols)")
