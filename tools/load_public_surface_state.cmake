# Load and validate the current additive public-surface state.
#
# Historical P1/P2/P3-01 policy files retain their frozen 14-header,
# 69-symbol ANOCRYPTO_C_0 snapshot. P3-02 retains ac_kdf.h and one symbol on
# ANOCRYPTO_C_1. Current checks validate both retained subsets while allowing
# the reviewed P3-03 one-shot symbols on ANOCRYPTO_C_2 and the P3-04
# AES-256-GCM streaming symbols on ANOCRYPTO_C_3.

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

function(_ano_crypto_load_symbol_manifest _path _out)
    file(STRINGS "${_path}" _lines)
    set(_symbols "")
    foreach(_line IN LISTS _lines)
        string(STRIP "${_line}" _line)
        if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
            list(APPEND _symbols "${_line}")
        endif()
    endforeach()
    set(${_out} "${_symbols}" PARENT_SCOPE)
endfunction()

file(GLOB ANO_CRYPTO_CURRENT_PUBLIC_HEADERS LIST_DIRECTORIES false
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(LENGTH ANO_CRYPTO_CURRENT_PUBLIC_HEADERS ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT)

set(ANO_CRYPTO_RETAINED_PUBLIC_HEADERS "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
list(REMOVE_ITEM ANO_CRYPTO_RETAINED_PUBLIC_HEADERS
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_kdf.h"
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_aead.h")
list(LENGTH ANO_CRYPTO_RETAINED_PUBLIC_HEADERS ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT)

set(ANO_CRYPTO_P3_02_PUBLIC_HEADERS "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
list(REMOVE_ITEM ANO_CRYPTO_P3_02_PUBLIC_HEADERS
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_aead.h")
list(LENGTH ANO_CRYPTO_P3_02_PUBLIC_HEADERS ANO_CRYPTO_P3_02_PUBLIC_HEADER_COUNT)

_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI0_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI1_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI2_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_ABI3_SYMBOL_COUNT)
_ano_crypto_count_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt"
    ANO_CRYPTO_CURRENT_SYMBOL_COUNT)

if(NOT ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT EQUAL 14)
    message(FATAL_ERROR
        "Retained public-header baseline changed: ${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
endif()
if(NOT ANO_CRYPTO_P3_02_PUBLIC_HEADER_COUNT EQUAL 15)
    message(FATAL_ERROR
        "P3-02 retained public-header baseline changed: ${ANO_CRYPTO_P3_02_PUBLIC_HEADER_COUNT}")
endif()
if(NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69)
    message(FATAL_ERROR
        "ANOCRYPTO_C_0 retained symbol baseline changed: ${ANO_CRYPTO_ABI0_SYMBOL_COUNT}")
endif()
if(NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1)
    message(FATAL_ERROR
        "ANOCRYPTO_C_1 retained symbol baseline changed: ${ANO_CRYPTO_ABI1_SYMBOL_COUNT}")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 16 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 79)
    message(FATAL_ERROR
        "P3-04 additive public surface is inconsistent: headers=${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}, ABI2=${ANO_CRYPTO_ABI2_SYMBOL_COUNT}, ABI3=${ANO_CRYPTO_ABI3_SYMBOL_COUNT}, total=${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}")
endif()
if(NOT ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT EQUAL ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT OR
   NOT ANO_CRYPTO_RELEASE_SYMBOL_COUNT EQUAL ANO_CRYPTO_CURRENT_SYMBOL_COUNT OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_3")
    message(FATAL_ERROR
        "Release identity does not match the P3-04 additive public surface")
endif()

_ano_crypto_load_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt"
    _ano_crypto_abi1_symbols)
if(NOT _ano_crypto_abi1_symbols STREQUAL "ano_crypto_hkdf_sha256")
    message(FATAL_ERROR "Unexpected ANOCRYPTO_C_1 symbol set: ${_ano_crypto_abi1_symbols}")
endif()

_ano_crypto_load_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt"
    _ano_crypto_abi2_symbols)
set(_ano_crypto_expected_abi2_symbols
    ano_crypto_aes256_gcm_decrypt
    ano_crypto_aes256_gcm_encrypt)
if(NOT _ano_crypto_abi2_symbols STREQUAL _ano_crypto_expected_abi2_symbols)
    message(FATAL_ERROR "Unexpected ANOCRYPTO_C_2 symbol set: ${_ano_crypto_abi2_symbols}")
endif()

_ano_crypto_load_symbol_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt"
    _ano_crypto_abi3_symbols)
set(_ano_crypto_expected_abi3_symbols
    ano_crypto_aes256_gcm_stream_aad_update
    ano_crypto_aes256_gcm_stream_context_size
    ano_crypto_aes256_gcm_stream_decrypt_final
    ano_crypto_aes256_gcm_stream_encrypt_final
    ano_crypto_aes256_gcm_stream_init
    ano_crypto_aes256_gcm_stream_uninit
    ano_crypto_aes256_gcm_stream_update)
if(NOT _ano_crypto_abi3_symbols STREQUAL _ano_crypto_expected_abi3_symbols)
    message(FATAL_ERROR "Unexpected ANOCRYPTO_C_3 symbol set: ${_ano_crypto_abi3_symbols}")
endif()
