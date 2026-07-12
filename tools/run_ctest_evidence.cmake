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

find_program(_ctest_exe ctest)
if(NOT _ctest_exe)
    message(FATAL_ERROR "ctest executable not found")
endif()
if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}/CTestTestfile.cmake")
    message(FATAL_ERROR "configured CTest tree is unavailable: ${ANO_CRYPTO_BUILD_DIR}")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
execute_process(
    COMMAND "${_ctest_exe}" --output-on-failure
    WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
    RESULT_VARIABLE _ctest_result
    OUTPUT_VARIABLE _ctest_stdout
    ERROR_VARIABLE _ctest_stderr)
file(WRITE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest.log"
    "${_ctest_stdout}${_ctest_stderr}")
file(WRITE "${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/CTEST_EVIDENCE_SUMMARY.txt"
    "CTest result: ${_ctest_result}\n"
    "Sensitive cryptographic values logged: no\n")
if(NOT _ctest_result EQUAL 0)
    message(FATAL_ERROR "CTest evidence run failed; see ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}/ctest.log")
endif()
message(STATUS "AnoCrypto-C CTest evidence capture passed: ${ANO_CRYPTO_CTEST_EVIDENCE_DIR}")
