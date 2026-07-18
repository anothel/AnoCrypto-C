# Load and validate the current additive public-surface state.
#
# Historical P1/P2/P3-01 policy files retain their frozen 14-header,
# 69-symbol ANOCRYPTO_C_0 snapshot. Current checks use the variables below to
# validate that snapshot as a retained subset while allowing the reviewed
# P3-02 additive header and symbol on ANOCRYPTO_C_1.

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR OR ANO_CRYPTO_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "ANO_CRYPTO_SOURCE_DIR is required")
endif()

function(_ano_crypto_count_symbol_manifest _path _out)
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}" OR IS_SYMLINK "${_path}")
        message(FATAL_ERROR "Required symbol manifest is missing or unsafe: ${_path}")
    endif()
    file(STRINGS "${_path}" _lines)
    set(_count 0)
    foreach(_line IN LISTS _lines)
        string(STRIP "${_line}" _line)
        if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
            math(EXPR _count "${_count} + 1")
        endif()
    endforeach()
    set(${_out} "${_count}" PARENT_SCOPE)
endfunction()

file(GLOB ANO_CRYPTO_CURRENT_PUBLIC_HEADERS LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(LENGTH ANO_CRYPTO_CURRENT_PUBLIC_HEADERS ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT)
set(ANO_CRYPTO_RETAINED_PUBLIC_HEADERS "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
list(REMOVE_ITEM ANO_CRYPTO_RETAINED_PUBLIC_HEADERS
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_kdf.h")
list(LENGTH ANO_CRYPTO_RETAINED_PUBLIC_HEADERS ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT)

_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI0_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI1_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_CURRENT_SYMBOL_COUNT)

if(NOT ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT EQUAL 14)
    message(FATAL_ERROR
        "Retained public-header baseline changed: ${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
endif()
if(NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69)
    message(FATAL_ERROR
        "ANOCRYPTO_C_0 retained symbol baseline changed: ${ANO_CRYPTO_ABI0_SYMBOL_COUNT}")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 15 OR
   NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 70)
    message(FATAL_ERROR
        "P3-02 additive public surface is inconsistent: headers=${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}, ABI1=${ANO_CRYPTO_ABI1_SYMBOL_COUNT}, total=${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}")
endif()
if(NOT ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT EQUAL ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT OR
   NOT ANO_CRYPTO_RELEASE_SYMBOL_COUNT EQUAL ANO_CRYPTO_CURRENT_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_1")
    message(FATAL_ERROR
        "Release identity does not match the P3-02 additive public surface")
endif()

file(STRINGS
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt"
    _ano_crypto_abi1_lines)
set(_ano_crypto_abi1_symbols "")
foreach(_line IN LISTS _ano_crypto_abi1_lines)
    string(STRIP "${_line}" _line)
    if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
        list(APPEND _ano_crypto_abi1_symbols "${_line}")
    endif()
endforeach()
if(NOT _ano_crypto_abi1_symbols STREQUAL "ano_crypto_hkdf_sha256")
    message(FATAL_ERROR "Unexpected ANOCRYPTO_C_1 symbol set: ${_ano_crypto_abi1_symbols}")
endif()
