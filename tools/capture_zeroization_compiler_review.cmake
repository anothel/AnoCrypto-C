cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_LIBRARY_FILE)
    message(FATAL_ERROR "ANO_CRYPTO_LIBRARY_FILE is required")
endif()
if(NOT DEFINED ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR)
    message(FATAL_ERROR "ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR is required")
endif()
if(NOT DEFINED ANO_CRYPTO_REQUIRE_ZEROIZATION_DISASSEMBLY)
    set(ANO_CRYPTO_REQUIRE_ZEROIZATION_DISASSEMBLY OFF)
endif()
if(NOT DEFINED ANO_CRYPTO_OBJDUMP_STYLE)
    set(ANO_CRYPTO_OBJDUMP_STYLE "GNU")
endif()

file(MAKE_DIRECTORY "${ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR}")
set(_summary "${ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR}/ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt")
set(_disassembly "${ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR}/ZEROIZATION_COMPILER_OUTPUT_DISASSEMBLY.txt")

file(WRITE "${_summary}"
    "AnoCrypto-C P2-05 zeroization compiler-output capture\n"
    "Policy ID: CZP-P2-05-R1\n"
    "Status: LOCAL_ENGINEERING_ONLY; manual review required; not selected-OE evidence.\n"
    "Compiler ID: ${ANO_CRYPTO_COMPILER_ID}\n"
    "Compiler version: ${ANO_CRYPTO_COMPILER_VERSION}\n"
    "Build type: ${ANO_CRYPTO_BUILD_TYPE}\n"
    "Target system: ${ANO_CRYPTO_SYSTEM_NAME}\n"
    "Target processor: ${ANO_CRYPTO_SYSTEM_PROCESSOR}\n"
    "Library: ${ANO_CRYPTO_LIBRARY_FILE}\n")

if(NOT EXISTS "${ANO_CRYPTO_LIBRARY_FILE}")
    file(APPEND "${_summary}" "Result: FAIL\nReason: library file does not exist\n")
    message(FATAL_ERROR "Zeroization compiler-output review library does not exist")
endif()

file(SHA256 "${ANO_CRYPTO_LIBRARY_FILE}" _library_sha256)
file(APPEND "${_summary}" "Library SHA-256: ${_library_sha256}\n")

if(NOT DEFINED ANO_CRYPTO_OBJDUMP OR ANO_CRYPTO_OBJDUMP STREQUAL "" OR
   NOT EXISTS "${ANO_CRYPTO_OBJDUMP}")
    file(WRITE "${_disassembly}" "Disassembler unavailable; no machine-code artifact captured.\n")
    file(APPEND "${_summary}"
        "Disassembler: unavailable\n"
        "Result: NOT_CAPTURED\n"
        "Manual review required: yes\n"
        "Selected OE review status: NOT_RUN\n"
        "Sensitive runtime values captured: none\n")
    if(ANO_CRYPTO_REQUIRE_ZEROIZATION_DISASSEMBLY)
        message(FATAL_ERROR "Zeroization disassembly was required but no tool was available")
    endif()
    message(STATUS "Zeroization compiler-output review was not captured")
    return()
endif()

if(ANO_CRYPTO_OBJDUMP_STYLE STREQUAL "MSVC")
    execute_process(
        COMMAND "${ANO_CRYPTO_OBJDUMP}" /DISASM "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 120)
else()
    execute_process(
        COMMAND "${ANO_CRYPTO_OBJDUMP}" -d "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 120)
endif()

file(WRITE "${_disassembly}" "${_output}${_error}")
file(APPEND "${_summary}" "Disassembler: ${ANO_CRYPTO_OBJDUMP}\n")
if(NOT _result EQUAL 0)
    file(APPEND "${_summary}"
        "Result: FAIL\nReason: disassembler returned ${_result}\nManual review required: yes\n")
    message(FATAL_ERROR "Zeroization compiler-output disassembly failed")
endif()

set(_required_symbols
    ano_crypto_internal_zeroize
    ano_crypto_internal_csp_zeroize_all_unlocked
    ano_crypto_internal_csp_destroy)
set(_found_count 0)
foreach(_symbol IN LISTS _required_symbols)
    string(FIND "${_output}" "${_symbol}" _position)
    if(NOT _position EQUAL -1)
        math(EXPR _found_count "${_found_count} + 1")
        file(APPEND "${_summary}" "Symbol marker ${_symbol}: found\n")
    else()
        file(APPEND "${_summary}" "Symbol marker ${_symbol}: not found (inlining/stripping may apply)\n")
    endif()
endforeach()

file(APPEND "${_summary}"
    "Required symbol markers found: ${_found_count}/3\n"
    "Automated proof of effective zeroization: no\n"
    "Manual review required: yes\n"
    "Review scope: store loop/equivalent, return ordering, call sites, inlining, spills, and early-return bypasses.\n"
    "Selected OE review status: NOT_RUN\n"
    "Result: CAPTURED\n"
    "Sensitive runtime values captured: none\n")

message(STATUS "Zeroization compiler-output review captured at ${ANO_CRYPTO_ZEROIZATION_COMPILER_REVIEW_DIR}")
