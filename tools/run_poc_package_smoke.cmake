# AnoCrypto-C customer PoC package v0 smoke script.
# Installs the SDK into a local PoC package, configures the packaged example
# through find_package(AnoCryptoC), builds it, and runs it.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_POC_SMOKE_DIR)
    set(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_BUILD_DIR}/poc-package-smoke")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_POC_SMOKE_DIR}" ABSOLUTE)

set(_package_dir "${ANO_CRYPTO_POC_SMOKE_DIR}/AnoCrypto-C-PoC-v0")
set(_example_source "${_package_dir}/examples/core_poc")
set(_example_build "${ANO_CRYPTO_POC_SMOKE_DIR}/core-poc-build")
set(_sdk_prefix "${_package_dir}/sdk")

file(REMOVE_RECURSE "${ANO_CRYPTO_POC_SMOKE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_POC_SMOKE_DIR}")

function(ano_crypto_poc_run_logged log_file description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr)
    file(WRITE "${log_file}" "# ${description}\n")
    file(APPEND "${log_file}" "# Command:")
    foreach(_arg IN LISTS ARGN)
        file(APPEND "${log_file}" " ${_arg}")
    endforeach()
    file(APPEND "${log_file}" "\n# Result: ${_result}\n\n")
    file(APPEND "${log_file}" "${_stdout}${_stderr}")
    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "${description} failed; see ${log_file}")
    endif()
endfunction()

ano_crypto_poc_run_logged(
    "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_PACKAGE_CREATE.log"
    "Create customer PoC package v0"
    "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
        -DANO_CRYPTO_POC_PACKAGE_DIR=${_package_dir}
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_poc_package.cmake")

if(NOT EXISTS "${_example_source}/CMakeLists.txt")
    message(FATAL_ERROR "Packaged core PoC example is missing: ${_example_source}")
endif()

set(_required_package_records
    "PACKAGE_MANIFEST.md"
    "PACKAGE_REVIEW_MANIFEST.md"
    "SBOM_STATUS.txt"
    "SIGNING_STATUS.txt"
    "THREAD_SAFETY_PACKAGE_NOTES.txt"
    "SHA2_SCOPE_PACKAGE_NOTES.txt"
    "RANDOM_BOUNDARY_PACKAGE_NOTES.txt")
foreach(_record IN LISTS _required_package_records)
    if(NOT EXISTS "${_package_dir}/${_record}")
        message(FATAL_ERROR "PoC package record is missing: ${_package_dir}/${_record}")
    endif()
endforeach()

ano_crypto_poc_run_logged(
    "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_CONFIGURE.log"
    "Configure packaged core PoC example"
    "${CMAKE_COMMAND}" -S "${_example_source}" -B "${_example_build}" -DCMAKE_PREFIX_PATH=${_sdk_prefix})

ano_crypto_poc_run_logged(
    "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_BUILD.log"
    "Build packaged core PoC example"
    "${CMAKE_COMMAND}" --build "${_example_build}" --parallel)

set(_example_exe "${_example_build}/anocrypto_c_core_poc")
if(NOT EXISTS "${_example_exe}")
    set(_example_exe "${_example_build}/anocrypto_c_core_poc.exe")
endif()
if(NOT EXISTS "${_example_exe}")
    file(GLOB_RECURSE _example_candidates
        LIST_DIRECTORIES false
        "${_example_build}/anocrypto_c_core_poc*")
    foreach(_candidate IN LISTS _example_candidates)
        if(NOT _candidate MATCHES "CMakeFiles")
            set(_example_exe "${_candidate}")
            break()
        endif()
    endforeach()
endif()
if(NOT EXISTS "${_example_exe}")
    message(FATAL_ERROR "Packaged core PoC example executable not found under ${_example_build}")
endif()

execute_process(
    COMMAND "${_example_exe}"
    WORKING_DIRECTORY "${_example_build}"
    RESULT_VARIABLE _run_result
    OUTPUT_VARIABLE _run_stdout
    ERROR_VARIABLE _run_stderr)
file(WRITE "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_RUN.log"
    "# Run packaged core PoC example\n"
    "# Executable: ${_example_exe}\n"
    "# Result: ${_run_result}\n\n"
    "${_run_stdout}${_run_stderr}")
if(NOT _run_result EQUAL 0)
    message(FATAL_ERROR "Packaged core PoC example failed; see ${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_RUN.log")
endif()

file(WRITE "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_PACKAGE_SMOKE_SUMMARY.txt"
    "PoC package directory: ${_package_dir}\n"
    "SDK prefix: ${_sdk_prefix}\n"
    "Example source: ${_example_source}\n"
    "Example build: ${_example_build}\n"
    "Example executable: ${_example_exe}\n"
    "Package manifest: ${_package_dir}/PACKAGE_MANIFEST.md\n"
    "Package review manifest: ${_package_dir}/PACKAGE_REVIEW_MANIFEST.md\n"
    "SBOM status: ${_package_dir}/SBOM_STATUS.txt\n"
    "Signing status: ${_package_dir}/SIGNING_STATUS.txt\n"
    "Thread-safety notes: ${_package_dir}/THREAD_SAFETY_PACKAGE_NOTES.txt\n"
    "SHA-2 scope notes: ${_package_dir}/SHA2_SCOPE_PACKAGE_NOTES.txt\n"
    "Random boundary notes: ${_package_dir}/RANDOM_BOUNDARY_PACKAGE_NOTES.txt\n"
    "PoC package smoke result: ${_run_result}\n")

message(STATUS "AnoCrypto-C PoC package smoke passed: ${ANO_CRYPTO_POC_SMOKE_DIR}")
