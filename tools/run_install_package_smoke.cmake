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
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}" ABSOLUTE)

set(_install_prefix "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/install-prefix")
set(_consumer_source "${ANO_CRYPTO_SOURCE_DIR}/tests/install_smoke")
set(_consumer_build "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/consumer-build")

if(NOT EXISTS "${_consumer_source}/CMakeLists.txt")
    message(FATAL_ERROR "install smoke source is unavailable: ${_consumer_source}")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")

function(_ano_crypto_run_install_smoke_step _log_name _description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr)
    file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/${_log_name}"
        "# ${_description}\n# Result: ${_result}\n\n${_stdout}${_stderr}")
    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed; see ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/${_log_name}")
    endif()
endfunction()

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_INSTALL.log
    "Install AnoCrypto-C package prefix"
    "${CMAKE_COMMAND}" --install "${ANO_CRYPTO_BUILD_DIR}" --prefix "${_install_prefix}")

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_CONFIGURE.log
    "Configure installed-package smoke consumer"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" -S "${_consumer_source}" -B "${_consumer_build}" -DCMAKE_PREFIX_PATH=${_install_prefix})

_ano_crypto_run_install_smoke_step(
    INSTALL_SMOKE_BUILD.log
    "Build installed-package smoke consumer"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_consumer_build}" --parallel)

set(_smoke_candidates
    "${_consumer_build}/anocrypto_c_install_smoke"
    "${_consumer_build}/anocrypto_c_install_smoke.exe"
    "${_consumer_build}/Debug/anocrypto_c_install_smoke"
    "${_consumer_build}/Debug/anocrypto_c_install_smoke.exe"
    "${_consumer_build}/Release/anocrypto_c_install_smoke"
    "${_consumer_build}/Release/anocrypto_c_install_smoke.exe"
    "${_consumer_build}/RelWithDebInfo/anocrypto_c_install_smoke"
    "${_consumer_build}/RelWithDebInfo/anocrypto_c_install_smoke.exe")
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

execute_process(
    COMMAND "${_smoke_exe}"
    WORKING_DIRECTORY "${_consumer_build}"
    RESULT_VARIABLE _run_result
    OUTPUT_VARIABLE _run_stdout
    ERROR_VARIABLE _run_stderr)
file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_RUN.log"
    "# Run installed-package smoke consumer\n# Result: ${_run_result}\n\n${_run_stdout}${_run_stderr}")
if(NOT _run_result EQUAL 0)
    message(FATAL_ERROR "installed-package smoke consumer failed; see ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_RUN.log")
endif()

file(WRITE "${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}/INSTALL_SMOKE_SUMMARY.txt"
    "Install prefix: ${_install_prefix}\n"
    "Install smoke source: ${_consumer_source}\n"
    "Install smoke build: ${_consumer_build}\n"
    "Install smoke executable: ${_smoke_exe}\n"
    "Install smoke result: ${_run_result}\n"
    "Sensitive cryptographic values logged: no\n")

message(STATUS "AnoCrypto-C installed-package smoke passed: ${ANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR}")
