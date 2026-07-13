cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_LIBRARY_FILE)
    message(FATAL_ERROR "ANO_CRYPTO_LIBRARY_FILE is required")
endif()
if(NOT DEFINED ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR)
    message(FATAL_ERROR "ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR is required")
endif()
if(NOT DEFINED ANO_CRYPTO_REQUIRE_CMAC_DISASSEMBLY)
    set(ANO_CRYPTO_REQUIRE_CMAC_DISASSEMBLY OFF)
endif()
if(NOT DEFINED ANO_CRYPTO_OBJDUMP_STYLE)
    set(ANO_CRYPTO_OBJDUMP_STYLE "GNU")
endif()

file(MAKE_DIRECTORY "${ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR}")
set(_summary "${ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR}/CMAC_COMPILER_OUTPUT_REVIEW.txt")
set(_disassembly "${ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR}/CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt")

file(WRITE "${_summary}"
    "AnoCrypto-C CMAC compiler-output review capture\n"
    "Status: engineering artifact for manual review; not a side-channel proof.\n"
    "Compiler ID: ${ANO_CRYPTO_COMPILER_ID}\n"
    "Compiler version: ${ANO_CRYPTO_COMPILER_VERSION}\n"
    "Build type: ${ANO_CRYPTO_BUILD_TYPE}\n"
    "Target system: ${ANO_CRYPTO_SYSTEM_NAME}\n"
    "Target processor: ${ANO_CRYPTO_SYSTEM_PROCESSOR}\n"
    "Library: ${ANO_CRYPTO_LIBRARY_FILE}\n")

if(NOT EXISTS "${ANO_CRYPTO_LIBRARY_FILE}")
    file(APPEND "${_summary}" "Result: FAIL\nReason: library file does not exist\n")
    message(FATAL_ERROR "CMAC compiler-output review library does not exist")
endif()

file(SHA256 "${ANO_CRYPTO_LIBRARY_FILE}" _library_sha256)
file(APPEND "${_summary}" "Library SHA-256: ${_library_sha256}\n")

if(NOT DEFINED ANO_CRYPTO_OBJDUMP OR ANO_CRYPTO_OBJDUMP STREQUAL "" OR
   NOT EXISTS "${ANO_CRYPTO_OBJDUMP}")
    file(WRITE "${_disassembly}" "Disassembler unavailable; no machine-code artifact captured.\n")
    file(APPEND "${_summary}"
        "Disassembler: unavailable\n"
        "Result: NOT_CAPTURED\n"
        "Manual review required: yes\n")
    if(ANO_CRYPTO_REQUIRE_CMAC_DISASSEMBLY)
        message(FATAL_ERROR "CMAC compiler-output disassembly was required but no tool was available")
    endif()
    message(STATUS "CMAC compiler-output review was not captured because no disassembler was available")
    return()
endif()

if(ANO_CRYPTO_OBJDUMP_STYLE STREQUAL "MSVC")
    execute_process(
        COMMAND "${ANO_CRYPTO_OBJDUMP}" /DISASM "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
        TIMEOUT 120)
else()
    execute_process(
        COMMAND "${ANO_CRYPTO_OBJDUMP}" -d "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
        TIMEOUT 120)
endif()

file(WRITE "${_disassembly}" "${_output}${_error}")
file(APPEND "${_summary}" "Disassembler: ${ANO_CRYPTO_OBJDUMP}\n")

if(NOT _result EQUAL 0)
    file(APPEND "${_summary}"
        "Result: FAIL\n"
        "Reason: disassembler returned ${_result}\n"
        "Manual review required: yes\n")
    message(FATAL_ERROR "CMAC compiler-output disassembly failed")
endif()

set(_required_symbols
    ano_crypto_internal_cmac_init
    ano_crypto_internal_cmac_update
    ano_crypto_internal_cmac_final
    ano_crypto_internal_cmac_tags_equal
    ano_crypto_cmac_verify_with_key)
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

string(REGEX MATCHALL "[\n\r][ \t]*[0-9a-fA-F]+:[^\n\r]*[ \t](j[a-z]+|b\.[a-z]+)[ \t]" _branch_matches "${_output}")
list(LENGTH _branch_matches _branch_count)
string(REGEX MATCHALL "[\n\r][ \t]*[0-9a-fA-F]+:[^\n\r]*[ \t](call|bl)[q]?[ \t]" _call_matches "${_output}")
list(LENGTH _call_matches _call_count)

file(APPEND "${_summary}"
    "Required CMAC symbol markers found: ${_found_count}/5\n"
    "Whole-library conditional-branch mnemonic count: ${_branch_count}\n"
    "Whole-library call mnemonic count: ${_call_count}\n"
    "Automated interpretation: none\n"
    "Manual review required: yes\n"
    "Review scope TODO: inspect CMAC subkey doubling, pending-block selection, AES calls, cleanup paths, and tag comparison for target/compiler-specific behavior.\n"
    "Result: CAPTURED\n"
    "Sensitive runtime values captured: none\n")

message(STATUS "CMAC compiler-output review captured at ${ANO_CRYPTO_CMAC_COMPILER_REVIEW_DIR}")
