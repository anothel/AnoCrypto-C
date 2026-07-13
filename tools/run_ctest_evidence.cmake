# Capture the complete configured CTest result as a standalone build-target step.
# Keeping CTest in its own process prevents a following evidence program from
# inheriting test-runner state inside the same CMake script process.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    message(FATAL_ERROR "ANO_CRYPTO_BUILD_DIR is required")
endif()
if(NOT DEFINED ANO_CRYPTO_CTEST_EVIDENCE_DIR)
    set(ANO_CRYPTO_CTEST_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/ctest-evidence")
endif()

get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_CTEST_EVIDENCE_DIR "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}" ABSOLUTE)

find_program(_ctest_exe NAMES ctest)
if(NOT _ctest_exe)
    message(FATAL_ERROR "CTest executable is unavailable")
endif()
if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}/CTestTestfile.cmake")
    message(FATAL_ERROR "configured CTest tree is unavailable: ${ANO_CRYPTO_BUILD_DIR}")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
set(_ctest_log "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest.log")
set(_ctest_command
    "${_ctest_exe}" --output-on-failure --timeout 180)
if(DEFINED ANO_CRYPTO_BUILD_CONFIG AND
   NOT ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
    list(APPEND _ctest_command --build-config "${ANO_CRYPTO_BUILD_CONFIG}")
else()
    set(ANO_CRYPTO_BUILD_CONFIG "default")
endif()

# Stream stdout/stderr directly to the evidence file instead of capturing them
# in CMake pipe variables.  Some nested build/test programs may leave inherited
# pipe descriptors open briefly; file redirection prevents that from stalling
# the evidence wrapper after CTest itself has completed.
execute_process(
    COMMAND ${_ctest_command}
    WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
    RESULT_VARIABLE _ctest_result
    OUTPUT_FILE "${_ctest_log}"
    ERROR_FILE "${_ctest_log}"
    TIMEOUT 900)
file(WRITE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/CTEST_EVIDENCE_SUMMARY.txt"
    "CTest result: ${_ctest_result}\n"
    "Build configuration: ${ANO_CRYPTO_BUILD_CONFIG}\n"
    "Sensitive cryptographic values logged: no\n")
if(NOT "${_ctest_result}" STREQUAL "0")
    message(FATAL_ERROR "CTest evidence run failed (${_ctest_result}); see ${_ctest_log}")
endif()
message(STATUS "AnoCrypto-C CTest evidence capture passed: ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
