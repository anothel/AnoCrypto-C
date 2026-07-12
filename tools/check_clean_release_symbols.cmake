cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_SYMBOL_WORK_DIR)
    set(ANO_CRYPTO_SYMBOL_WORK_DIR "${ANO_CRYPTO_SOURCE_DIR}/build-symbol-baseline")
endif()
if(NOT DEFINED ANO_CRYPTO_SYMBOL_REPORT)
    set(ANO_CRYPTO_SYMBOL_REPORT "${ANO_CRYPTO_SYMBOL_WORK_DIR}/EXPORTED_SYMBOL_BASELINE_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SYMBOL_WORK_DIR "${ANO_CRYPTO_SYMBOL_WORK_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SYMBOL_REPORT "${ANO_CRYPTO_SYMBOL_REPORT}" ABSOLUTE)

file(REMOVE_RECURSE "${ANO_CRYPTO_SYMBOL_WORK_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_SYMBOL_WORK_DIR}")
set(_build_dir "${ANO_CRYPTO_SYMBOL_WORK_DIR}/clean-release")

set(_configure_command
    "${CMAKE_COMMAND}" -S "${ANO_CRYPTO_SOURCE_DIR}" -B "${_build_dir}"
    -DANO_CRYPTO_BUILD_TESTS=OFF
    -DANO_CRYPTO_BUILD_EXAMPLES=OFF
    -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF
    -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF
    -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
    -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON
    -DBUILD_SHARED_LIBS=ON
    -DCMAKE_BUILD_TYPE=Release)
if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR AND NOT ANO_CRYPTO_CMAKE_GENERATOR STREQUAL "")
    list(APPEND _configure_command -G "${ANO_CRYPTO_CMAKE_GENERATOR}")
endif()
if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM AND NOT ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND _configure_command -A "${ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM}")
endif()
if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET AND NOT ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND _configure_command -T "${ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET}")
endif()

execute_process(
    COMMAND ${_configure_command}
    RESULT_VARIABLE _configure_result
    OUTPUT_VARIABLE _configure_output
    ERROR_VARIABLE _configure_error)
if(NOT _configure_result EQUAL 0)
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nStage: configure\n${_configure_output}${_configure_error}\n")
    message(FATAL_ERROR "Clean Release symbol configure failed; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_build_dir}" --config Release --target ano_crypto_exported_symbol_baseline_check --parallel
    RESULT_VARIABLE _build_result
    OUTPUT_VARIABLE _build_output
    ERROR_VARIABLE _build_error)
if(NOT _build_result EQUAL 0)
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nStage: build/check\n${_build_output}${_build_error}\n")
    message(FATAL_ERROR "Clean Release symbol baseline check failed; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

set(_nested_report "${_build_dir}/policy-check/EXPORTED_SYMBOL_BASELINE_CHECK.txt")
if(NOT EXISTS "${_nested_report}")
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nExpected nested report was not generated: ${_nested_report}\n")
    message(FATAL_ERROR "Clean Release symbol report missing")
endif()
configure_file("${_nested_report}" "${ANO_CRYPTO_SYMBOL_REPORT}" COPYONLY)
message(STATUS "AnoCrypto-C clean Release exported-symbol baseline check passed")
