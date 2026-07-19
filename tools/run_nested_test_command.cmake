# Run one configured CMake-script test without exposing nested build processes to
# the outer generator's jobserver or CTest output-capture descriptors.
cmake_minimum_required(VERSION 3.16)

foreach(_required IN ITEMS
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_NESTED_TEST_WORK_DIR
        ANO_CRYPTO_NESTED_TEST_SCRIPT
        ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG
        ANO_CRYPTO_NESTED_TEST_NAME)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR
    "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_NESTED_TEST_WORK_DIR
    "${ANO_CRYPTO_NESTED_TEST_WORK_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_NESTED_TEST_SCRIPT
    "${ANO_CRYPTO_NESTED_TEST_SCRIPT}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG
    "${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}" ABSOLUTE)

if(NOT EXISTS "${ANO_CRYPTO_NESTED_TEST_SCRIPT}" OR
   IS_DIRECTORY "${ANO_CRYPTO_NESTED_TEST_SCRIPT}" OR
   IS_SYMLINK "${ANO_CRYPTO_NESTED_TEST_SCRIPT}")
    message(FATAL_ERROR
        "Nested configured-test script is unavailable or unsafe: ${ANO_CRYPTO_NESTED_TEST_SCRIPT}")
endif()
if(NOT DEFINED ANO_CRYPTO_NESTED_TEST_TIMEOUT OR
   ANO_CRYPTO_NESTED_TEST_TIMEOUT STREQUAL "")
    set(ANO_CRYPTO_NESTED_TEST_TIMEOUT 900)
endif()

get_filename_component(_output_dir
    "${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
set(_stdout_log "${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}.stdout")
set(_stderr_log "${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}.stderr")
file(REMOVE "${_stdout_log}" "${_stderr_log}")

# OUTPUT_FILE/ERROR_FILE are deliberate. They prevent nested descendants from
# retaining a CMake or CTest capture pipe after the configured command exits.
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=MAKEFLAGS
        --unset=MFLAGS
        --unset=MAKELEVEL
        --unset=GNUMAKEFLAGS
        "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_TEST_WORK_DIR=${ANO_CRYPTO_NESTED_TEST_WORK_DIR}
        -P "${ANO_CRYPTO_NESTED_TEST_SCRIPT}"
    RESULT_VARIABLE _result
    OUTPUT_FILE "${_stdout_log}"
    ERROR_FILE "${_stderr_log}"
    TIMEOUT "${ANO_CRYPTO_NESTED_TEST_TIMEOUT}")

set(_stdout "")
set(_stderr "")
if(EXISTS "${_stdout_log}")
    file(READ "${_stdout_log}" _stdout)
endif()
if(EXISTS "${_stderr_log}")
    file(READ "${_stderr_log}" _stderr)
endif()
file(WRITE "${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}"
    "Configured test: ${ANO_CRYPTO_NESTED_TEST_NAME}\n"
    "Script: ${ANO_CRYPTO_NESTED_TEST_SCRIPT}\n"
    "Result: ${_result}\n\n"
    "===== stdout =====\n${_stdout}\n"
    "===== stderr =====\n${_stderr}\n")
file(REMOVE "${_stdout_log}" "${_stderr_log}")

if(NOT "${_result}" STREQUAL "0")
    message(FATAL_ERROR
        "Configured test ${ANO_CRYPTO_NESTED_TEST_NAME} failed; see ${ANO_CRYPTO_NESTED_TEST_OUTPUT_LOG}")
endif()
message(STATUS
    "Configured nested-process test passed: ${ANO_CRYPTO_NESTED_TEST_NAME}")
