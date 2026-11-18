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
if(NOT DEFINED ANO_CRYPTO_BUILD_CONFIG OR ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
    set(ANO_CRYPTO_BUILD_CONFIG "Release")
endif()

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
        ERROR_VARIABLE _stderr
        TIMEOUT 900)
    file(WRITE "${log_file}" "# ${description}\n")
    file(APPEND "${log_file}" "# Command:")
    foreach(_arg IN LISTS ARGN)
        file(APPEND "${log_file}" " ${_arg}")
    endforeach()
    file(APPEND "${log_file}" "\n# Result: ${_result}\n\n")
    file(APPEND "${log_file}" "${_stdout}${_stderr}")
    if(NOT "${_result}" STREQUAL "0")
        message("Native result: ${_result}\n${_stdout}${_stderr}")
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
        -DANO_CRYPTO_POC_BUILD_CONFIG=${ANO_CRYPTO_BUILD_CONFIG}
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_poc_package.cmake")

if(NOT EXISTS "${_example_source}/CMakeLists.txt")
    message(FATAL_ERROR "Packaged core PoC example is missing: ${_example_source}")
endif()

set(_required_package_records
    "PACKAGE_MANIFEST.md"
    "PACKAGE_REVIEW_MANIFEST.md"
    "SBOM_STATUS.txt"
    "SIGNING_STATUS.txt"
    "RELEASE_PROVENANCE_PACKAGE_NOTES.txt"
    "THREAD_SAFETY_PACKAGE_NOTES.txt"
    "CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt"
    "SHA512_FAMILY_PACKAGE_NOTES.txt"
    "DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt"
    "HKDF_SHA256_PACKAGE_NOTES.txt"
    "AES256_GCM_PACKAGE_NOTES.txt"
    "AES_CBC_PACKAGE_NOTES.txt"
    "AES_HARDENING_PACKAGE_NOTES.txt"
    "AES_CMAC_PACKAGE_NOTES.txt"
    "RANDOM_SERVICE_PACKAGE_NOTES.txt"
    "RANDOM_LIFECYCLE_PACKAGE_NOTES.txt"
    "ENTROPY_HEALTH_PACKAGE_NOTES.txt"
    "ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt"
    "CLEAN_BUILD_CONFIGURE.log"
    "CLEAN_BUILD.log"
    "CLEAN_RELEASE_SYMBOL_BASELINE.log"
    "RELEASE_COMPATIBILITY_CHECK.log"
    "RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt"
    "RELEASE_PROVENANCE_POLICY_CHECK.log"
    "INSTALL_PACKAGE.log")
foreach(_record IN LISTS _required_package_records)
    if(NOT EXISTS "${_package_dir}/${_record}")
        message(FATAL_ERROR "PoC package record is missing: ${_package_dir}/${_record}")
    endif()
endforeach()

file(GLOB_RECURSE _poc_package_files LIST_DIRECTORIES false "${_package_dir}/*")
foreach(_poc_file IN LISTS _poc_package_files)
    string(TOLOWER "${_poc_file}" _poc_file_lower)
    if(_poc_file_lower MATCHES "(^|[/\\])(libcrypto|libssl|openssl)([./\\]|$)" OR
       _poc_file_lower MATCHES "(sha2_hmac|hkdf_sha256|aes_cbc|aes_cmac|aes256_gcm)_openssl_differential_tests\.c$" OR
       _poc_file_lower MATCHES "sha512_portability_review\.c$" OR
       _poc_file_lower MATCHES "aes_(cbc|cmac)_(hardening_review|lifecycle_stress_tests)\.c$" OR
       _poc_file_lower MATCHES "(aes|cmac)_compiler_output_(review|disassembly)\.txt$" OR
       _poc_file_lower MATCHES "(capture_cmac_compiler_review|check_cmac_hardening_policy)\.cmake$" OR
       _poc_file_lower MATCHES "ac_(aes|cipher|cmac)_internal\.h$" OR
       _poc_file_lower MATCHES "cmac(_kat_tests|_api_negative_tests|_openssl_differential_tests)?\.c$" OR
       _poc_file_lower MATCHES "p1_29_compatibility_probe\.c$" OR
       _poc_file_lower MATCHES "hosted_artifact_manifest" OR
       _poc_file_lower MATCHES "(^|[/\\])(source_sbom\.spdx\.json|source_files\.sha256|release_provenance\.txt|release_artifacts\.sha256|signing_request\.txt|release_authorization\.md|anocrypto-c-source\.tar\.gz)$" OR
       _poc_file_lower MATCHES "(^|[/\\])abi[/\\]baselines([/\\]|$)")
        message(FATAL_ERROR "PoC package contains test-only differential/provider artifact: ${_poc_file}")
    endif()
endforeach()

if(NOT EXISTS "${_sdk_prefix}/include/anocrypto/ac_cipher.h")
    message(FATAL_ERROR "PoC SDK is missing public AES/CBC header: ${_sdk_prefix}/include/anocrypto/ac_cipher.h")
endif()

if(NOT EXISTS "${_sdk_prefix}/include/anocrypto/ac_cmac.h")
    message(FATAL_ERROR "PoC SDK is missing public AES-CMAC header: ${_sdk_prefix}/include/anocrypto/ac_cmac.h")
endif()

if(NOT EXISTS "${_sdk_prefix}/include/anocrypto/ac_kdf.h")
    message(FATAL_ERROR "PoC SDK is missing public HKDF header: ${_sdk_prefix}/include/anocrypto/ac_kdf.h")
endif()

if(NOT EXISTS "${_sdk_prefix}/include/anocrypto/ac_aead.h")
    message(FATAL_ERROR "PoC SDK is missing public AES-256-GCM header: ${_sdk_prefix}/include/anocrypto/ac_aead.h")
endif()

file(GLOB_RECURSE _poc_public_headers LIST_DIRECTORIES false "${_sdk_prefix}/include/*.h")
foreach(_poc_header IN LISTS _poc_public_headers)
    file(READ "${_poc_header}" _poc_header_content)
    if(_poc_header_content MATCHES "openssl/|OpenSSL::Crypto|EVP_Q_digest|EVP_MAC_fetch")
        message(FATAL_ERROR "PoC public header exposes test-only provider detail: ${_poc_header}")
    endif()
endforeach()

ano_crypto_poc_run_logged(
    "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_CONFIGURE.log"
    "Configure packaged core PoC example"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" -S "${_example_source}" -B "${_example_build}"
        -DCMAKE_PREFIX_PATH=${_sdk_prefix}
        -DCMAKE_BUILD_TYPE=${ANO_CRYPTO_BUILD_CONFIG})

ano_crypto_poc_run_logged(
    "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_BUILD.log"
    "Build packaged core PoC example"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_example_build}"
        --config "${ANO_CRYPTO_BUILD_CONFIG}" --parallel)

set(_example_exe "${_example_build}/anocrypto_c_core_poc")
if(NOT EXISTS "${_example_exe}")
    set(_example_exe
        "${_example_build}/${ANO_CRYPTO_BUILD_CONFIG}/anocrypto_c_core_poc.exe")
endif()
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

set(_runtime_path "$ENV{PATH}")
if(WIN32)
    set(_runtime_path "${_sdk_prefix}/bin;${_runtime_path}")
endif()
string(REPLACE ";" "\\;" _runtime_path_escaped "${_runtime_path}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "PATH=${_runtime_path_escaped}"
        "LD_LIBRARY_PATH=${_sdk_prefix}/lib:$ENV{LD_LIBRARY_PATH}"
        "DYLD_LIBRARY_PATH=${_sdk_prefix}/lib:$ENV{DYLD_LIBRARY_PATH}"
        "${_example_exe}"
    WORKING_DIRECTORY "${_example_build}"
    RESULT_VARIABLE _run_result
    OUTPUT_VARIABLE _run_stdout
    ERROR_VARIABLE _run_stderr
    TIMEOUT 120)
file(WRITE "${ANO_CRYPTO_POC_SMOKE_DIR}/POC_EXAMPLE_RUN.log"
    "# Run packaged core PoC example\n"
    "# Executable: ${_example_exe}\n"
    "# Result: ${_run_result}\n\n"
    "${_run_stdout}${_run_stderr}")
if(NOT "${_run_result}" STREQUAL "0")
    message("Native result: ${_run_result}\n${_run_stdout}${_run_stderr}")
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
    "Release provenance boundary: ${_package_dir}/RELEASE_PROVENANCE_PACKAGE_NOTES.txt\n"
    "Thread-safety notes: ${_package_dir}/THREAD_SAFETY_PACKAGE_NOTES.txt\n"
    "Concurrency/sanitizer notes: ${_package_dir}/CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt\n"
    "SHA-512-family notes: ${_package_dir}/SHA512_FAMILY_PACKAGE_NOTES.txt\n"
    "Differential/portability notes: ${_package_dir}/DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt\n"
    "HKDF notes: ${_package_dir}/HKDF_SHA256_PACKAGE_NOTES.txt\n"
    "AES-256-GCM notes: ${_package_dir}/AES256_GCM_PACKAGE_NOTES.txt\n"
    "AES/CBC notes: ${_package_dir}/AES_CBC_PACKAGE_NOTES.txt\n"
    "AES/CBC hardening notes: ${_package_dir}/AES_HARDENING_PACKAGE_NOTES.txt\n"
    "AES-CMAC notes: ${_package_dir}/AES_CMAC_PACKAGE_NOTES.txt\n"
    "Random service notes: ${_package_dir}/RANDOM_SERVICE_PACKAGE_NOTES.txt\n"
    "Random lifecycle notes: ${_package_dir}/RANDOM_LIFECYCLE_PACKAGE_NOTES.txt\n"
    "Clean Release symbol baseline: ${_package_dir}/CLEAN_RELEASE_SYMBOL_BASELINE.log\n"
    "Release compatibility check: ${_package_dir}/RELEASE_COMPATIBILITY_CHECK.log\n"
    "Release compatibility notes: ${_package_dir}/RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt\n"
    "Release provenance policy check: ${_package_dir}/RELEASE_PROVENANCE_POLICY_CHECK.log\n"
    "Entropy health boundary notes: ${_package_dir}/ENTROPY_HEALTH_PACKAGE_NOTES.txt\n"
    "Entropy operational policy notes: ${_package_dir}/ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt\n"
    "PoC package smoke result: ${_run_result}\n")

message(STATUS "AnoCrypto-C PoC package smoke passed: ${ANO_CRYPTO_POC_SMOKE_DIR}")
