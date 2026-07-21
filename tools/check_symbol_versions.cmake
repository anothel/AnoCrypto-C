cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_LIBRARY_FILE ANO_CRYPTO_READELF ANO_CRYPTO_ABI0_BASELINE ANO_CRYPTO_ABI1_BASELINE ANO_CRYPTO_ABI2_BASELINE ANO_CRYPTO_ABI3_BASELINE ANO_CRYPTO_ABI4_BASELINE ANO_CRYPTO_ABI5_BASELINE ANO_CRYPTO_SYMBOL_VERSION_REPORT)
    if(NOT DEFINED ${_required})
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

execute_process(
    COMMAND "${ANO_CRYPTO_READELF}" --dyn-syms --wide "${ANO_CRYPTO_LIBRARY_FILE}"
    RESULT_VARIABLE _symbols_result
    OUTPUT_VARIABLE _symbols_output
    ERROR_VARIABLE _symbols_error)
execute_process(
    COMMAND "${ANO_CRYPTO_READELF}" --version-info --wide "${ANO_CRYPTO_LIBRARY_FILE}"
    RESULT_VARIABLE _versions_result
    OUTPUT_VARIABLE _versions_output
    ERROR_VARIABLE _versions_error)

get_filename_component(_report_dir "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}" DIRECTORY)
file(MAKE_DIRECTORY "${_report_dir}")
file(WRITE "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}"
    "AnoCrypto-C ELF symbol-version check\nLibrary: ${ANO_CRYPTO_LIBRARY_FILE}\n")

if(NOT _symbols_result EQUAL 0 OR NOT _versions_result EQUAL 0)
    file(APPEND "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}"
        "Result: FAIL\n${_symbols_error}${_versions_error}\n")
    message(FATAL_ERROR "readelf symbol-version inspection failed")
endif()

foreach(_pair IN ITEMS
    "${ANO_CRYPTO_ABI0_BASELINE}|ANOCRYPTO_C_0"
    "${ANO_CRYPTO_ABI1_BASELINE}|ANOCRYPTO_C_1"
    "${ANO_CRYPTO_ABI2_BASELINE}|ANOCRYPTO_C_2"
    "${ANO_CRYPTO_ABI3_BASELINE}|ANOCRYPTO_C_3"
    "${ANO_CRYPTO_ABI4_BASELINE}|ANOCRYPTO_C_4"
    "${ANO_CRYPTO_ABI5_BASELINE}|ANOCRYPTO_C_5")
    string(REPLACE "|" ";" _parts "${_pair}")
    list(GET _parts 0 _baseline)
    list(GET _parts 1 _node)
    file(STRINGS "${_baseline}" _lines)
    set(_count 0)
    foreach(_line IN LISTS _lines)
        string(STRIP "${_line}" _symbol)
        if(_symbol STREQUAL "" OR _symbol MATCHES "^#")
            continue()
        endif()
        math(EXPR _count "${_count}+1")
        string(FIND "${_symbols_output}" "${_symbol}@@${_node}" _found)
        if(_found EQUAL -1)
            file(APPEND "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}"
                "Missing/default-version mismatch: ${_symbol}@@${_node}\nResult: FAIL\n")
            message(FATAL_ERROR "Symbol ${_symbol} is not default-versioned at ${_node}")
        endif()
    endforeach()
    file(APPEND "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}" "${_node} symbol count: ${_count}\n")
    string(FIND "${_versions_output}" "Name: ${_node}" _node_found)
    if(_node_found EQUAL -1)
        file(APPEND "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}" "Missing version node: ${_node}\nResult: FAIL\n")
        message(FATAL_ERROR "Missing ELF version node ${_node}")
    endif()
endforeach()

file(APPEND "${ANO_CRYPTO_SYMBOL_VERSION_REPORT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C ELF symbol-version check passed")
