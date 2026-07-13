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
get_filename_component(_report_dir "${ANO_CRYPTO_SYMBOL_REPORT}" DIRECTORY)
file(MAKE_DIRECTORY "${_report_dir}")
set(_build_dir "${ANO_CRYPTO_SYMBOL_WORK_DIR}/clean-release")
set(_configure_log "${ANO_CRYPTO_SYMBOL_WORK_DIR}/configure.log")
set(_build_log "${ANO_CRYPTO_SYMBOL_WORK_DIR}/build-and-symbol-check.log")

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

message(STATUS "Configuring independent clean Release symbol build")
execute_process(
    COMMAND ${_configure_command}
    RESULT_VARIABLE _configure_result
    OUTPUT_FILE "${_configure_log}"
    ERROR_FILE "${_configure_log}"
    TIMEOUT 180)
if(NOT "${_configure_result}" STREQUAL "0")
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nStage: configure\nExit: ${_configure_result}\nLog: ${_configure_log}\n")
    message(FATAL_ERROR "Clean Release symbol configure failed; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

message(STATUS "Building independent clean Release library and checking 69-symbol baseline")
execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_build_dir}" --config Release --target ano_crypto_exported_symbol_baseline_check --parallel
    RESULT_VARIABLE _build_result
    OUTPUT_FILE "${_build_log}"
    ERROR_FILE "${_build_log}"
    TIMEOUT 600)
if(NOT "${_build_result}" STREQUAL "0")
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nStage: build/check\nExit: ${_build_result}\nLog: ${_build_log}\n")
    message(FATAL_ERROR "Clean Release symbol baseline check failed; see ${ANO_CRYPTO_SYMBOL_REPORT}")
endif()

set(_nested_report "${_build_dir}/policy-check/EXPORTED_SYMBOL_BASELINE_CHECK.txt")
if(NOT EXISTS "${_nested_report}")
    file(WRITE "${ANO_CRYPTO_SYMBOL_REPORT}"
        "Result: FAIL\nExpected nested report was not generated: ${_nested_report}\n")
    message(FATAL_ERROR "Clean Release symbol report missing")
endif()
configure_file("${_nested_report}" "${ANO_CRYPTO_SYMBOL_REPORT}" COPYONLY)
file(STRINGS "${_nested_report}" _library_lines REGEX "^Library: ")
list(LENGTH _library_lines _library_line_count)
if(NOT _library_line_count EQUAL 1)
    message(FATAL_ERROR "Clean Release symbol report does not identify one library")
endif()
list(GET _library_lines 0 _library_line)
string(REGEX REPLACE "^Library: " "" _library_file "${_library_line}")

if(ANO_CRYPTO_DEPENDENCY_STYLE STREQUAL "MSVC")
    set(_dependency_command "${ANO_CRYPTO_DEPENDENCY_TOOL}" /nologo /dependents "${_library_file}")
elseif(ANO_CRYPTO_DEPENDENCY_STYLE STREQUAL "APPLE")
    find_program(_dependency_tool NAMES otool)
    if(NOT _dependency_tool)
        message(FATAL_ERROR "otool is required for clean Release dependency inspection")
    endif()
    set(_dependency_command "${_dependency_tool}" -L "${_library_file}")
else()
    find_program(_dependency_tool NAMES ldd)
    if(NOT _dependency_tool)
        message(FATAL_ERROR "ldd is required for clean Release dependency inspection")
    endif()
    set(_dependency_command "${_dependency_tool}" "${_library_file}")
endif()
execute_process(
    COMMAND ${_dependency_command}
    RESULT_VARIABLE _dependency_result
    OUTPUT_VARIABLE _dependency_stdout
    ERROR_VARIABLE _dependency_stderr
    TIMEOUT 60)
if(NOT "${_dependency_result}" STREQUAL "0")
    message(FATAL_ERROR "Clean Release runtime-dependency inspection failed: ${_dependency_stderr}")
endif()
string(TOLOWER "${_dependency_stdout}${_dependency_stderr}" _dependency_output_lower)
if(_dependency_output_lower MATCHES "libcrypto|libssl|crypto-[0-9]|ssl-[0-9]")
    message(FATAL_ERROR "Clean Release product library has an OpenSSL runtime dependency")
endif()
file(APPEND "${ANO_CRYPTO_SYMBOL_REPORT}"
    "OpenSSL runtime dependency: none\n"
    "Dependency inspection result: PASS\n")
message(STATUS "AnoCrypto-C clean Release exported-symbol baseline check passed")
