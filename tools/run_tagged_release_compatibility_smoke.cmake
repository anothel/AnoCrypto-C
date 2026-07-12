# Build a clean installed P1-30 package, then compile and run current-header and
# retained-P1-29-header C99 consumers against that package.
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR)
    set(ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR
        "${ANO_CRYPTO_BUILD_DIR}/tagged-release-compatibility")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
if(NOT CMAKE_CTEST_COMMAND)
    find_program(CMAKE_CTEST_COMMAND NAMES ctest REQUIRED)
endif()
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR
    "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")

set(_baseline_include
    "${ANO_CRYPTO_SOURCE_DIR}/abi/baselines/${ANO_CRYPTO_COMPAT_BASELINE_VERSION}/include")
set(_compat_source "${ANO_CRYPTO_SOURCE_DIR}/tests/compat")
set(_exact_version_source "${ANO_CRYPTO_SOURCE_DIR}/tests/package_version/exact")
set(_incompatible_version_source "${ANO_CRYPTO_SOURCE_DIR}/tests/package_version/incompatible")
set(_release_build "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/release-build")
set(_install_prefix "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/install-prefix")
set(_consumer_build "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/consumer-build")
set(_exact_version_build "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/package-version-exact")
set(_incompatible_version_build "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/package-version-incompatible")

foreach(_required_path IN ITEMS
        "${_baseline_include}/anocrypto/anocrypto.h"
        "${_compat_source}/CMakeLists.txt"
        "${_compat_source}/p1_29_compatibility_probe.c"
        "${_exact_version_source}/CMakeLists.txt"
        "${_incompatible_version_source}/CMakeLists.txt")
    if(NOT EXISTS "${_required_path}")
        message(FATAL_ERROR "Release compatibility input is unavailable: ${_required_path}")
    endif()
endforeach()

file(REMOVE_RECURSE "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}")

function(_ano_run _log_name _description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr)
    file(WRITE "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/${_log_name}"
        "# ${_description}\n# Result: ${_result}\n\n${_stdout}${_stderr}")
    if(NOT _result EQUAL 0)
        message(FATAL_ERROR
            "${_description} failed; see ${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/${_log_name}")
    endif()
endfunction()

function(_ano_append_generator _list_var)
    set(_command "${${_list_var}}")
    if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR AND
       NOT ANO_CRYPTO_CMAKE_GENERATOR STREQUAL "")
        list(APPEND _command -G "${ANO_CRYPTO_CMAKE_GENERATOR}")
    endif()
    if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM AND
       NOT ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM STREQUAL "")
        list(APPEND _command -A "${ANO_CRYPTO_CMAKE_GENERATOR_PLATFORM}")
    endif()
    if(DEFINED ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET AND
       NOT ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET STREQUAL "")
        list(APPEND _command -T "${ANO_CRYPTO_CMAKE_GENERATOR_TOOLSET}")
    endif()
    set(${_list_var} "${_command}" PARENT_SCOPE)
endfunction()

set(_release_configure
    "${CMAKE_COMMAND}" -S "${ANO_CRYPTO_SOURCE_DIR}" -B "${_release_build}"
    -DANO_CRYPTO_BUILD_TESTS=OFF
    -DANO_CRYPTO_BUILD_EXAMPLES=OFF
    -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON
    -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF
    -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF
    -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF
    -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
    -DBUILD_SHARED_LIBS=ON
    -DANO_CRYPTO_BUILD_GIT_COMMIT=compatibility-smoke
    -DANO_CRYPTO_BUILD_FINGERPRINT=${ANO_CRYPTO_RELEASE_FINGERPRINT}-compatibility-smoke
    -DCMAKE_BUILD_TYPE=Release)
_ano_append_generator(_release_configure)
_ano_run(RELEASE_CONFIGURE.log "Configure clean P1-30 Release package"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    ${_release_configure})
_ano_run(RELEASE_BUILD.log "Build clean P1-30 Release package"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_release_build}" --config Release --parallel)
_ano_run(RELEASE_POLICY.log "Check P1-30 release compatibility policy"
    "${CMAKE_COMMAND}" --build "${_release_build}" --config Release
    --target ano_crypto_release_compatibility_policy_check)
_ano_run(RELEASE_INSTALL.log "Install clean P1-30 Release package"
    "${CMAKE_COMMAND}" --install "${_release_build}" --config Release
    --prefix "${_install_prefix}")

set(_consumer_configure
    "${CMAKE_COMMAND}" -S "${_compat_source}" -B "${_consumer_build}"
    -DCMAKE_PREFIX_PATH=${_install_prefix}
    -DANO_CRYPTO_P129_BASELINE_INCLUDE_DIR=${_baseline_include}
    -DCMAKE_BUILD_TYPE=Release)
_ano_append_generator(_consumer_configure)
_ano_run(CONSUMER_CONFIGURE.log "Configure current and retained-header consumers"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    ${_consumer_configure})
_ano_run(CONSUMER_BUILD.log "Build current and retained-header consumers"
    "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_COMMAND}" --build "${_consumer_build}" --config Release --parallel)

set(_runtime_path "$ENV{PATH}")
if(WIN32)
    set(_runtime_path "${_install_prefix}/bin;${_runtime_path}")
endif()
string(REPLACE ";" "\\;" _runtime_path_escaped "${_runtime_path}")
set(_library_path "${_install_prefix}/lib:$ENV{LD_LIBRARY_PATH}")
set(_dyld_path "${_install_prefix}/lib:$ENV{DYLD_LIBRARY_PATH}")
_ano_run(CONSUMER_CTEST.log "Run current and retained-header consumers"
    "${CMAKE_COMMAND}" -E env
        "PATH=${_runtime_path_escaped}"
        "LD_LIBRARY_PATH=${_library_path}"
        "DYLD_LIBRARY_PATH=${_dyld_path}"
        --unset=MAKEFLAGS --unset=MFLAGS --unset=MAKELEVEL
    "${CMAKE_CTEST_COMMAND}" --test-dir "${_consumer_build}" -C Release
        --verbose --output-on-failure)

set(_exact_configure
    "${CMAKE_COMMAND}" -S "${_exact_version_source}" -B "${_exact_version_build}"
    -DCMAKE_PREFIX_PATH=${_install_prefix} -DCMAKE_BUILD_TYPE=Release)
_ano_append_generator(_exact_configure)
_ano_run(PACKAGE_VERSION_EXACT_CONFIGURE.log "Require exact package version 0.1.0"
    ${_exact_configure})
_ano_run(PACKAGE_VERSION_EXACT_BUILD.log "Build exact package-version probe"
    "${CMAKE_COMMAND}" --build "${_exact_version_build}" --config Release --parallel)
_ano_run(PACKAGE_VERSION_EXACT_CTEST.log "Run exact package-version probe"
    "${CMAKE_COMMAND}" -E env
        "PATH=${_runtime_path_escaped}"
        "LD_LIBRARY_PATH=${_library_path}"
        "DYLD_LIBRARY_PATH=${_dyld_path}"
    "${CMAKE_CTEST_COMMAND}" --test-dir "${_exact_version_build}" -C Release
        --verbose --output-on-failure)

set(_incompatible_configure
    "${CMAKE_COMMAND}" -S "${_incompatible_version_source}"
    -B "${_incompatible_version_build}"
    -DCMAKE_PREFIX_PATH=${_install_prefix} -DCMAKE_BUILD_TYPE=Release)
_ano_append_generator(_incompatible_configure)
_ano_run(PACKAGE_VERSION_INCOMPATIBLE_CONFIGURE.log
    "Confirm the 0.1.x package does not satisfy a 0.2.0 request"
    ${_incompatible_configure})

file(GLOB_RECURSE _installed_files
    LIST_DIRECTORIES false RELATIVE "${_install_prefix}" "${_install_prefix}/*")
set(_snapshot_leaks "")
set(_installed_public_headers "")
foreach(_installed_file IN LISTS _installed_files)
    if(_installed_file MATCHES "(^|/)abi/baselines(/|$)" OR
       _installed_file MATCHES "(^|/)tests/compat(/|$)" OR
       _installed_file MATCHES "p1_29_compatibility_probe" OR
       _installed_file MATCHES "HOSTED_ARTIFACT")
        list(APPEND _snapshot_leaks "${_installed_file}")
    endif()
    if(_installed_file MATCHES "^include/anocrypto/[^/]+\\.h$")
        list(APPEND _installed_public_headers "${_installed_file}")
    endif()
endforeach()
if(_snapshot_leaks)
    message(FATAL_ERROR "Compatibility or hosted-review material leaked into the installed SDK: ${_snapshot_leaks}")
endif()
list(LENGTH _installed_public_headers _installed_header_count)
if(NOT "${_installed_header_count}" STREQUAL "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}")
    message(FATAL_ERROR "Installed public-header count is not ${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}")
endif()

file(WRITE "${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}/TAGGED_COMPAT_SUMMARY.txt"
    "AnoCrypto-C P1-30 tagged-release compatibility smoke\n"
    "Current version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Baseline: ${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\n"
    "Package compatibility policy: ${ANO_CRYPTO_RELEASE_PACKAGE_COMPATIBILITY}\n"
    "Current-header/current-library consumer: PASS\n"
    "P1-29-header/current-library consumer: PASS\n"
    "Exact 0.1.0 package request: PASS\n"
    "Incompatible 0.2.0 package request: REJECTED\n"
    "P1-29 public symbol references: ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "Installed public headers: ${_installed_header_count}\n"
    "Retained header snapshot installed: no\n"
    "Sensitive cryptographic values logged: no\n"
    "Limit: tag existence and commit identity are checked separately in a tagged Git build.\n"
    "Limit: untested compiler ABIs, loaders, and complete semantic compatibility are not established.\n"
    "Result: PASS\n")
message(STATUS
    "AnoCrypto-C tagged-release compatibility smoke passed: ${ANO_CRYPTO_TAGGED_COMPAT_SMOKE_DIR}")
