cmake_minimum_required(VERSION 3.16)

foreach(_required IN ITEMS
    ANO_CRYPTO_LIBRARY_FILE
    ANO_CRYPTO_SECURE_CODING_REVIEW_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

file(MAKE_DIRECTORY "${ANO_CRYPTO_SECURE_CODING_REVIEW_DIR}")
set(_summary "${ANO_CRYPTO_SECURE_CODING_REVIEW_DIR}/SECURE_CODING_COMPILER_CPU_REVIEW.txt")
set(_elf "${ANO_CRYPTO_SECURE_CODING_REVIEW_DIR}/ELF_HARDENING_REVIEW.txt")
set(_violations "")

file(WRITE "${_summary}"
    "AnoCrypto-C P2-06 compiler/CPU and build-hardening capture\n"
    "Policy ID: SCP-P2-06-R1\n"
    "Status: LOCAL_ENGINEERING_ONLY; exact OE-01 and named human review remain NOT_RUN.\n"
    "Compiler ID: ${ANO_CRYPTO_COMPILER_ID}\n"
    "Compiler version: ${ANO_CRYPTO_COMPILER_VERSION}\n"
    "Build type: ${ANO_CRYPTO_BUILD_TYPE}\n"
    "Target system: ${ANO_CRYPTO_SYSTEM_NAME}\n"
    "Target processor: ${ANO_CRYPTO_SYSTEM_PROCESSOR}\n"
    "Library: ${ANO_CRYPTO_LIBRARY_FILE}\n")

if(NOT EXISTS "${ANO_CRYPTO_LIBRARY_FILE}")
    file(APPEND "${_summary}" "Result: FAIL\nReason: library missing\n")
    message(FATAL_ERROR "P2-06 compiler review library missing")
endif()
file(SHA256 "${ANO_CRYPTO_LIBRARY_FILE}" _library_sha256)
file(APPEND "${_summary}" "Library SHA-256: ${_library_sha256}\n")

if(NOT CMAKE_HOST_UNIX OR ANO_CRYPTO_SYSTEM_NAME STREQUAL "Darwin")
    file(WRITE "${_elf}"
        "ELF hardening review is not applicable to this local host/target.\n"
        "Selected OE review status: NOT_RUN\n")
    file(APPEND "${_summary}"
        "ELF review: NOT_APPLICABLE_LOCAL\n"
        "Result: CAPTURED_WITH_OPEN_SELECTED_OE_REVIEW\n"
        "Sensitive runtime values captured: none\n")
    return()
endif()

foreach(_tool IN ITEMS ANO_CRYPTO_READELF ANO_CRYPTO_NM)
    if(NOT DEFINED ${_tool} OR "${${_tool}}" STREQUAL "" OR
       NOT EXISTS "${${_tool}}")
        list(APPEND _violations "required tool unavailable: ${_tool}")
    endif()
endforeach()

if(NOT _violations)
    execute_process(
        COMMAND "${ANO_CRYPTO_READELF}" -W -l -d -n "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _readelf_result
        OUTPUT_VARIABLE _readelf_output
        ERROR_VARIABLE _readelf_error
        TIMEOUT 120)
    execute_process(
        COMMAND "${ANO_CRYPTO_NM}" -D "${ANO_CRYPTO_LIBRARY_FILE}"
        RESULT_VARIABLE _nm_result
        OUTPUT_VARIABLE _nm_output
        ERROR_VARIABLE _nm_error
        TIMEOUT 120)
    file(WRITE "${_elf}"
        "# readelf -W -l -d -n\n${_readelf_output}${_readelf_error}\n"
        "# nm -D\n${_nm_output}${_nm_error}\n")
    if(NOT _readelf_result EQUAL 0)
        list(APPEND _violations "readelf returned ${_readelf_result}")
    endif()
    if(NOT _nm_result EQUAL 0)
        list(APPEND _violations "nm returned ${_nm_result}")
    endif()

    if(NOT _readelf_output MATCHES "GNU_STACK[^\\n]*RW([^E\\n]|$)")
        list(APPEND _violations "non-executable GNU_STACK was not confirmed")
    endif()
    if(NOT _readelf_output MATCHES "GNU_RELRO")
        list(APPEND _violations "GNU_RELRO was not confirmed")
    endif()
    if(NOT _readelf_output MATCHES "BIND_NOW" AND
       NOT _readelf_output MATCHES "FLAGS[^\\n]*NOW")
        list(APPEND _violations "BIND_NOW/full RELRO was not confirmed")
    endif()
    if(_readelf_output MATCHES "TEXTREL")
        list(APPEND _violations "TEXTREL was present")
    endif()
    if(_readelf_output MATCHES "LOAD[^\\n]*RWE")
        list(APPEND _violations "RWX LOAD segment was present")
    endif()
    if(NOT _nm_output MATCHES "__stack_chk_fail")
        list(APPEND _violations "stack-protector reference was not confirmed")
    endif()
endif()

if(_violations)
    file(APPEND "${_summary}" "Result: FAIL\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${_summary}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "P2-06 compiler/build-hardening review failed")
endif()

file(APPEND "${_summary}"
    "ELF executable stack: no\n"
    "GNU RELRO: present\n"
    "BIND_NOW: present\n"
    "RWX LOAD segment: absent\n"
    "TEXTREL: absent\n"
    "Stack protector reference: present\n"
    "Automated side-channel proof: no\n"
    "Selected OE-01 compiler/CPU review: NOT_RUN\n"
    "Named human disposition: TODO\n"
    "Result: PASS_LOCAL_ENGINEERING\n"
    "Sensitive runtime values captured: none\n")
message(STATUS "P2-06 compiler/build-hardening capture passed")
