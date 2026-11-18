# Run an installed-package consumer smoke outside the release-evidence process.
# This separation prevents nested build tools from retaining an outer build
# runner's jobserver state while evidence files are being assembled.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR)
    set(ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR "${ANO_CRYPTO_BUILD_DIR}/install-package-smoke")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
if(NOT CMAKE_CTEST_COMMAND)
    find_program(CMAKE_CTEST_COMMAND NAMES ctest)
    if(NOT CMAKE_CTEST_COMMAND)
        message(FATAL_ERROR "CTest executable is unavailable")
    endif()
endif()
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}" ABSOLUTE)
if(NOT DEFINED ANO_CRYPTO_BUILD_CONFIG OR ANO_CRYPTO_BUILD_CONFIG STREQUAL "")
    set(ANO_CRYPTO_BUILD_CONFIG "Release")
endif()

set(_install_prefix "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/install-prefix")
set(_consumer_source "${ANO_CRYPTO_SOURCE_DIR}/tests/install_smoke")
set(_consumer_build "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/consumer-build")
set(_compat_consumer_source "${ANO_CRYPTO_SOURCE_DIR}/tests/compat")
set(_compat_consumer_build "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/p1-29-compat-consumer-build")
set(_compat_baseline_include
    "${ANO_CRYPTO_SOURCE_DIR}/abi/baselines/0.1.0-p1-29-cmac-hardening/include")

if(NOT EXISTS "${_consumer_source}/CMakeLists.txt")
    message(FATAL_ERROR "install smoke source is unavailable: ${_consumer_source}")
endif()
if(NOT EXISTS "${_compat_consumer_source}/CMakeLists.txt" OR
   NOT EXISTS "${_compat_baseline_include}/anocrypto/anocrypto.h")
    message(FATAL_ERROR "P1-29 compatibility consumer or preserved header baseline is unavailable")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")

function(_ano_crypto_run_install_smoke_step _log_name _description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr
        TIMEOUT 600)
    file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/${_log_name}"
        "# ${_description}\n# Result: ${_result}\n\n${_stdout}${_stderr}")
    if(NOT "${_result}" STREQUAL "0")
        message("Native result: ${_result}\n${_stdout}${_stderr}")
        message(FATAL_ERROR "${_description} failed; see ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/${_log_name}")
    endif()
endfunction()

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_INSTALL.log
    "Install AnoCrypto-C package prefix"
    "${CMAKE_COMMAND}" --install "${ANO_CRYPTO_BUILD_DIR}"
        --config "${ANO_CRYPTO_BUILD_CONFIG}" --prefix "${_install_prefix}")

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_CONFIGURE.log
    "Configure installed-package smoke consumer"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" -S "${_consumer_source}" -B "${_consumer_build}"
        -DCMAKE_PREFIX_PATH=${_install_prefix}
        -DCMAKE_BUILD_TYPE=${ANO_CRYPTO_BUILD_CONFIG})

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_BUILD.log
    "Build installed-package smoke consumer"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_consumer_build}"
        --config "${ANO_CRYPTO_BUILD_CONFIG}" --parallel)

set(_smoke_candidates
    "${_consumer_build}/anocrypto_c_install_smoke"
    "${_consumer_build}/anocrypto_c_install_smoke.exe"
    "${_consumer_build}/${ANO_CRYPTO_BUILD_CONFIG}/anocrypto_c_install_smoke"
    "${_consumer_build}/${ANO_CRYPTO_BUILD_CONFIG}/anocrypto_c_install_smoke.exe")
set(_smoke_exe "")
foreach(_candidate IN LISTS _smoke_candidates)
    if(EXISTS "${_candidate}" AND NOT IS_DIRECTORY "${_candidate}")
        set(_smoke_exe "${_candidate}")
        break()
    endif()
endforeach()
if(NOT _smoke_exe)
    message(FATAL_ERROR "installed-package smoke executable was not found under ${_consumer_build}")
endif()

set(_runtime_path "$ENV{PATH}")
if(WIN32)
    set(_runtime_path "${_install_prefix}/bin;${_runtime_path}")
endif()
string(REPLACE ";" "\\;" _runtime_path_escaped "${_runtime_path}")
set(_library_path "${_install_prefix}/lib:$ENV{LD_LIBRARY_PATH}")
set(_dyld_path "${_install_prefix}/lib:$ENV{DYLD_LIBRARY_PATH}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "PATH=${_runtime_path_escaped}"
        "LD_LIBRARY_PATH=${_library_path}"
        "DYLD_LIBRARY_PATH=${_dyld_path}"
        "${_smoke_exe}"
    WORKING_DIRECTORY "${_consumer_build}"
    RESULT_VARIABLE _run_result
    OUTPUT_VARIABLE _run_stdout
    ERROR_VARIABLE _run_stderr
    TIMEOUT 120)
file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_RUN.log"
    "# Run installed-package smoke consumer\n# Result: ${_run_result}\n\n${_run_stdout}${_run_stderr}")
if(NOT "${_run_result}" STREQUAL "0")
    message("Native result: ${_run_result}\n${_run_stdout}${_run_stderr}")
    message(FATAL_ERROR "installed-package smoke consumer failed; see ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_RUN.log")
endif()

_ano_crypto_run_install_smoke_step(
    P1_29_COMPAT_CONFIGURE.log
    "Configure current-header and P1-29-header compatibility consumers"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" -S "${_compat_consumer_source}" -B "${_compat_consumer_build}"
    -DCMAKE_PREFIX_PATH=${_install_prefix}
    -DCMAKE_BUILD_TYPE=${ANO_CRYPTO_BUILD_CONFIG}
    -DANO_CRYPTO_P129_BASELINE_INCLUDE_DIR=${_compat_baseline_include})

_ano_crypto_run_install_smoke_step(
    P1_29_COMPAT_BUILD.log
    "Build current-header and P1-29-header compatibility consumers"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_compat_consumer_build}"
        --config "${ANO_CRYPTO_BUILD_CONFIG}" --parallel)

_ano_crypto_run_install_smoke_step(
    P1_29_COMPAT_CTEST.log
    "Run current-header and P1-29-header compatibility consumers"
    "${CMAKE_COMMAND}" -E env
        "PATH=${_runtime_path_escaped}"
        "LD_LIBRARY_PATH=${_library_path}"
        "DYLD_LIBRARY_PATH=${_dyld_path}"
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_CTEST_COMMAND}" --test-dir "${_compat_consumer_build}"
        --build-config "${ANO_CRYPTO_BUILD_CONFIG}" --verbose --output-on-failure)
set(_compat_run_result 0)

file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_SUMMARY.txt"
    "Install prefix: ${_install_prefix}\n"
    "Install smoke source: ${_consumer_source}\n"
    "Install smoke build: ${_consumer_build}\n"
    "Install smoke executable: ${_smoke_exe}\n"
    "Install smoke result: ${_run_result}\n"
    "P1-29 compatibility baseline: ${_compat_baseline_include}\n"
    "Current/P1-29 header compatibility CTest result: ${_compat_run_result}\n"
    "Sensitive cryptographic values logged: no\n")

message(STATUS "AnoCrypto-C installed-package smoke passed: ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")
