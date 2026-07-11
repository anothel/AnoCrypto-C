cmake_minimum_required(VERSION 3.16)

project(AnoCryptoC
    VERSION 0.1.0
    DESCRIPTION "AnoCrypto-C C99 module skeleton"
    LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(ANO_CRYPTO_BUILD_TESTS "Build AnoCrypto-C unit and smoke tests" ON)
option(ANO_CRYPTO_BUILD_EXAMPLES "Build AnoCrypto-C example programs" ON)
option(ANO_CRYPTO_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS "Enable internal entropy failure-injection hooks for dedicated test builds" OFF)
option(ANO_CRYPTO_ENABLE_THREAD_SAFETY "Enable internal locks for module/self-test/CSP global state" ON)
option(ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER "Enable reviewed internal platform entropy provider where supported" ON)

if(NOT DEFINED ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS)
    if(ANO_CRYPTO_BUILD_TESTS)
        set(ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS OFF CACHE BOOL "Hide internal symbols in shared-library builds where supported")
    else()
        set(ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS ON CACHE BOOL "Hide internal symbols in shared-library builds where supported")
    endif()
endif()

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

if(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS AND NOT ANO_CRYPTO_BUILD_TESTS)
    message(FATAL_ERROR "ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS requires ANO_CRYPTO_BUILD_TESTS=ON and must not be enabled for release/PoC builds")
endif()

if(ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS AND ANO_CRYPTO_BUILD_TESTS)
    message(WARNING "ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS is ON while internal tests are enabled; internal-unit tests that link through the shared library may fail on ELF toolchains")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ANO_CRYPTO_BUILD_GIT_COMMIT "unknown" CACHE STRING "Git commit or source revision for build identity")
set(ANO_CRYPTO_BUILD_FINGERPRINT "AnoCrypto-C-p1-18-entropy-operational-policy" CACHE STRING "Build fingerprint string")

set(ANO_CRYPTO_PUBLIC_HEADERS
    include/anocrypto/anocrypto.h
    include/anocrypto/ac_status.h
    include/anocrypto/ac_version.h
    include/anocrypto/ac_build.h
    include/anocrypto/ac_module.h
    include/anocrypto/ac_selftest.h
    include/anocrypto/ac_hash.h
    include/anocrypto/ac_hmac.h
    include/anocrypto/ac_drbg.h
    include/anocrypto/ac_random.h
    include/anocrypto/ac_csp.h
    include/anocrypto/ac_key.h)

add_library(anocrypto_c
    src/core/status.c
    src/core/module_state.c
    src/core/build_identity.c
    src/core/thread_lock.c
    src/selftest/selftest.c
    src/selftest/kat_manifest.c
    src/hash/hash.c
    src/hash/sha256.c
    src/mac/hmac.c
    src/drbg/drbg.c
    src/drbg/random.c
    src/drbg/entropy_adapter.c
    src/drbg/entropy_health.c
    src/drbg/process_guard.c
    src/csp/csp_handle.c
    src/csp/key.c
    src/csp/zeroize.c)

add_library(AnoCryptoC::AnoCryptoC ALIAS anocrypto_c)

set_target_properties(anocrypto_c PROPERTIES
    OUTPUT_NAME anocrypto_c
    EXPORT_NAME AnoCryptoC
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    PUBLIC_HEADER "${ANO_CRYPTO_PUBLIC_HEADERS}")

# Internal-unit tests link to src/internal seams through the shared test build.
# Windows auto-export is enabled only for test builds; clean Release/PoC builds
# keep tests off and export only ANO_CRYPTO_API public symbols.
if(WIN32 AND ANO_CRYPTO_BUILD_TESTS)
    set_target_properties(anocrypto_c PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

target_include_directories(anocrypto_c
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(anocrypto_c PUBLIC ANO_CRYPTO_BUILD_SHARED)
endif()

target_compile_definitions(anocrypto_c
    PRIVATE
        ANO_CRYPTO_BUILDING_LIBRARY
        ANO_CRYPTO_BUILD_GIT_COMMIT="${ANO_CRYPTO_BUILD_GIT_COMMIT}"
        ANO_CRYPTO_BUILD_TYPE="$<IF:$<CONFIG:>,unknown,$<CONFIG>>"
        ANO_CRYPTO_BUILD_COMPILER="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"
        ANO_CRYPTO_BUILD_TARGET="${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
        ANO_CRYPTO_BUILD_FINGERPRINT="${ANO_CRYPTO_BUILD_FINGERPRINT}")

if(ANO_CRYPTO_ENABLE_THREAD_SAFETY)
    find_package(Threads QUIET)
    if(Threads_FOUND)
        target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_ENABLE_THREAD_SAFETY)
        target_link_libraries(anocrypto_c PRIVATE Threads::Threads)
    else()
        message(WARNING "ANO_CRYPTO_ENABLE_THREAD_SAFETY is ON, but CMake Threads was not found; using unsupported no-op fallback")
    endif()
endif()

if(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
    target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
endif()

if(ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER)
    target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_USE_LINUX_GETRANDOM _GNU_SOURCE)
    elseif(WIN32)
        target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_USE_WINDOWS_BCRYPTGENRANDOM)
        target_link_libraries(anocrypto_c PRIVATE bcrypt)
    elseif(APPLE)
        target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_USE_APPLE_SECRANDOMCOPYBYTES)
        target_link_libraries(anocrypto_c PRIVATE "-framework Security")
    else()
        message(STATUS "ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER is ON, but no reviewed provider is configured for ${CMAKE_SYSTEM_NAME}")
    endif()
endif()

if(ANO_CRYPTO_ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(anocrypto_c PRIVATE /W4)
    else()
        target_compile_options(anocrypto_c PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS AND NOT MSVC)
    target_compile_options(anocrypto_c PRIVATE -fvisibility=hidden)
endif()


add_custom_target(ano_crypto_public_surface_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_POLICY_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/PUBLIC_SURFACE_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_public_surface.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C public surface and repository policy")

add_custom_target(ano_crypto_thread_safety_policy_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/THREAD_SAFETY_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_thread_safety_policy.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C thread-safety policy and support matrix")


add_custom_target(ano_crypto_sha2_scope_policy_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_SHA2_SCOPE_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/SHA2_SCOPE_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_sha2_scope_policy.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C SHA-384/SHA-512 reserved scope policy")

add_custom_target(ano_crypto_random_boundary_policy_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_RANDOM_BOUNDARY_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/RANDOM_BOUNDARY_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_random_boundary_policy.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C random-service and DRBG/entropy boundary policy")

add_custom_target(ano_crypto_entropy_health_policy_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_ENTROPY_HEALTH_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/ENTROPY_HEALTH_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_entropy_health_policy.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C entropy failure-injection and health-test policy")

add_custom_target(ano_crypto_entropy_operational_policy_check
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_ENTROPY_OPERATIONAL_CHECK_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/policy-check/ENTROPY_OPERATIONAL_POLICY_CHECK.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_entropy_operational_policy.cmake
    USES_TERMINAL
    COMMENT "Checking AnoCrypto-C entropy source classification, provider ownership, and process-boundary policy")

if(ANO_CRYPTO_BUILD_EXAMPLES)
    add_subdirectory(examples/core_poc)
endif()

add_custom_target(ano_crypto_create_poc_package
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_POC_PACKAGE_DIR=${CMAKE_CURRENT_BINARY_DIR}/poc-package/AnoCrypto-C-PoC-v0
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/create_poc_package.cmake
    DEPENDS anocrypto_c
    USES_TERMINAL
    COMMENT "Creating AnoCrypto-C customer PoC package v0 draft")

add_custom_target(ano_crypto_poc_package_smoke
    COMMAND ${CMAKE_COMMAND}
        -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DANO_CRYPTO_POC_SMOKE_DIR=${CMAKE_CURRENT_BINARY_DIR}/poc-package-smoke
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/run_poc_package_smoke.cmake
    DEPENDS anocrypto_c
    USES_TERMINAL
    COMMENT "Running AnoCrypto-C customer PoC package smoke")

install(TARGETS anocrypto_c
    EXPORT AnoCryptoCTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/anocrypto)

install(EXPORT AnoCryptoCTargets
    NAMESPACE AnoCryptoC::
    FILE AnoCryptoCTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnoCryptoC)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/AnoCryptoCConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AnoCryptoCConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnoCryptoC)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/AnoCryptoCConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AnoCryptoCConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AnoCryptoCConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnoCryptoC)

if(ANO_CRYPTO_BUILD_TESTS)
    enable_testing()

    add_executable(ano_crypto_module_state_tests
        tests/unit/module_state_tests.c)
    target_link_libraries(ano_crypto_module_state_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_module_state_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_module_state_tests COMMAND ano_crypto_module_state_tests)

    add_executable(ano_crypto_service_gate_negative_tests
        tests/negative/crypto_service_gate_tests.c)
    target_link_libraries(ano_crypto_service_gate_negative_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_service_gate_negative_tests COMMAND ano_crypto_service_gate_negative_tests)

    add_executable(ano_crypto_sha256_hmac_kat_tests
        tests/kat/sha256_hmac_kat_tests.c)
    target_link_libraries(ano_crypto_sha256_hmac_kat_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_sha256_hmac_kat_tests COMMAND ano_crypto_sha256_hmac_kat_tests)

    add_executable(ano_crypto_sha256_hmac_extended_kat_tests
        tests/kat/sha256_hmac_extended_kat_tests.c)
    target_link_libraries(ano_crypto_sha256_hmac_extended_kat_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_sha256_hmac_extended_kat_tests COMMAND ano_crypto_sha256_hmac_extended_kat_tests)

    add_executable(ano_crypto_hmac_drbg_kat_tests
        tests/kat/hmac_drbg_kat_tests.c)
    target_link_libraries(ano_crypto_hmac_drbg_kat_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hmac_drbg_kat_tests COMMAND ano_crypto_hmac_drbg_kat_tests)

    add_executable(ano_crypto_hmac_drbg_boundary_tests
        tests/kat/hmac_drbg_boundary_tests.c)
    target_link_libraries(ano_crypto_hmac_drbg_boundary_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hmac_drbg_boundary_tests COMMAND ano_crypto_hmac_drbg_boundary_tests)

    add_executable(ano_crypto_hmac_key_handle_kat_tests
        tests/kat/hmac_key_handle_kat_tests.c)
    target_link_libraries(ano_crypto_hmac_key_handle_kat_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hmac_key_handle_kat_tests COMMAND ano_crypto_hmac_key_handle_kat_tests)

    add_executable(ano_crypto_hash_hmac_api_negative_tests
        tests/negative/hash_hmac_api_negative_tests.c)
    target_link_libraries(ano_crypto_hash_hmac_api_negative_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hash_hmac_api_negative_tests COMMAND ano_crypto_hash_hmac_api_negative_tests)

    add_executable(ano_crypto_sha2_reserved_api_tests
        tests/negative/sha2_reserved_api_tests.c)
    target_link_libraries(ano_crypto_sha2_reserved_api_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_sha2_reserved_api_tests COMMAND ano_crypto_sha2_reserved_api_tests)

    add_executable(ano_crypto_random_reserved_api_tests
        tests/negative/random_reserved_api_tests.c)
    target_link_libraries(ano_crypto_random_reserved_api_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_random_reserved_api_tests COMMAND ano_crypto_random_reserved_api_tests)

    add_executable(ano_crypto_drbg_api_negative_tests
        tests/negative/drbg_api_negative_tests.c)
    target_link_libraries(ano_crypto_drbg_api_negative_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_drbg_api_negative_tests COMMAND ano_crypto_drbg_api_negative_tests)

    add_executable(ano_crypto_csp_key_lifecycle_negative_tests
        tests/negative/csp_key_lifecycle_negative_tests.c)
    target_link_libraries(ano_crypto_csp_key_lifecycle_negative_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_csp_key_lifecycle_negative_tests COMMAND ano_crypto_csp_key_lifecycle_negative_tests)


    add_executable(ano_crypto_csp_handle_stale_exhaustion_tests
        tests/negative/csp_handle_stale_exhaustion_tests.c)
    target_link_libraries(ano_crypto_csp_handle_stale_exhaustion_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_csp_handle_stale_exhaustion_tests COMMAND ano_crypto_csp_handle_stale_exhaustion_tests)

    add_executable(ano_crypto_entropy_adapter_tests
        tests/unit/entropy_adapter_tests.c)
    target_link_libraries(ano_crypto_entropy_adapter_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_entropy_adapter_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_entropy_adapter_tests COMMAND ano_crypto_entropy_adapter_tests)

    add_executable(ano_crypto_platform_entropy_provider_tests
        tests/unit/platform_entropy_provider_tests.c)
    target_link_libraries(ano_crypto_platform_entropy_provider_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_platform_entropy_provider_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_platform_entropy_provider_tests COMMAND ano_crypto_platform_entropy_provider_tests)

    add_executable(ano_crypto_entropy_source_negative_tests
        tests/negative/entropy_source_negative_tests.c)
    target_link_libraries(ano_crypto_entropy_source_negative_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_entropy_source_negative_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_entropy_source_negative_tests COMMAND ano_crypto_entropy_source_negative_tests)

    add_executable(ano_crypto_entropy_health_tests
        tests/unit/entropy_health_tests.c)
    target_link_libraries(ano_crypto_entropy_health_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_entropy_health_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_entropy_health_tests COMMAND ano_crypto_entropy_health_tests)

    add_executable(ano_crypto_entropy_operational_policy_tests
        tests/unit/entropy_operational_policy_tests.c)
    target_link_libraries(ano_crypto_entropy_operational_policy_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_entropy_operational_policy_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_entropy_operational_policy_tests COMMAND ano_crypto_entropy_operational_policy_tests)

    add_executable(ano_crypto_process_guard_tests
        tests/unit/process_guard_tests.c)
    target_link_libraries(ano_crypto_process_guard_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_process_guard_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_process_guard_tests COMMAND ano_crypto_process_guard_tests)

    add_executable(ano_crypto_entropy_health_error_state_tests
        tests/negative/entropy_health_error_state_tests.c)
    target_link_libraries(ano_crypto_entropy_health_error_state_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_entropy_health_error_state_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_entropy_health_error_state_tests COMMAND ano_crypto_entropy_health_error_state_tests)

    if(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
        add_executable(ano_crypto_platform_entropy_failure_injection_tests
            tests/negative/platform_entropy_failure_injection_tests.c)
        target_link_libraries(ano_crypto_platform_entropy_failure_injection_tests PRIVATE anocrypto_c)
        target_include_directories(ano_crypto_platform_entropy_failure_injection_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)
        target_compile_definitions(ano_crypto_platform_entropy_failure_injection_tests PRIVATE ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)

        add_test(NAME ano_crypto_platform_entropy_failure_injection_tests COMMAND ano_crypto_platform_entropy_failure_injection_tests)
        set_tests_properties(ano_crypto_platform_entropy_failure_injection_tests PROPERTIES LABELS "entropy-failure-injection")
    endif()

    add_executable(ano_crypto_drbg_entropy_helper_tests
        tests/unit/drbg_entropy_helper_tests.c)
    target_link_libraries(ano_crypto_drbg_entropy_helper_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_drbg_entropy_helper_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_drbg_entropy_helper_tests COMMAND ano_crypto_drbg_entropy_helper_tests)

    add_executable(ano_crypto_thread_lock_tests
        tests/unit/thread_lock_tests.c)
    target_link_libraries(ano_crypto_thread_lock_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_thread_lock_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_thread_lock_tests COMMAND ano_crypto_thread_lock_tests)

    if(ANO_CRYPTO_ENABLE_THREAD_SAFETY AND CMAKE_USE_PTHREADS_INIT)
        add_executable(ano_crypto_csp_thread_safety_tests
            tests/unit/csp_thread_safety_tests.c)
        target_link_libraries(ano_crypto_csp_thread_safety_tests PRIVATE anocrypto_c Threads::Threads)

        add_test(NAME ano_crypto_csp_thread_safety_tests COMMAND ano_crypto_csp_thread_safety_tests)
    endif()

    add_executable(ano_crypto_install_smoke_buildtree
        tests/install_smoke/main.c)
    target_link_libraries(ano_crypto_install_smoke_buildtree PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_install_smoke_buildtree COMMAND ano_crypto_install_smoke_buildtree)

    if(TARGET anocrypto_c_core_poc)
        add_test(NAME ano_crypto_poc_example_buildtree COMMAND anocrypto_c_core_poc)
    endif()

    add_custom_target(ano_crypto_collect_release_evidence
        COMMAND ${CMAKE_COMMAND}
            -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -DANO_CRYPTO_EVIDENCE_DIR=${CMAKE_CURRENT_BINARY_DIR}/evidence/p1-18
            -DANO_CRYPTO_RUN_CTEST=ON
            -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON
            -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON
            -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON
            -DANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON
            -DANO_CRYPTO_RUN_SHA2_SCOPE_CHECK=ON
            -DANO_CRYPTO_RUN_RANDOM_BOUNDARY_CHECK=ON
            -DANO_CRYPTO_RUN_ENTROPY_HEALTH_CHECK=ON
            -DANO_CRYPTO_RUN_ENTROPY_OPERATIONAL_CHECK=ON
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/collect_release_evidence.cmake
        USES_TERMINAL
        COMMENT "Collecting AnoCrypto-C P1-18 release evidence draft")
    add_dependencies(ano_crypto_collect_release_evidence
        ano_crypto_public_surface_check
        ano_crypto_thread_safety_policy_check
        ano_crypto_sha2_scope_policy_check
        ano_crypto_random_boundary_policy_check
        ano_crypto_entropy_health_policy_check
        ano_crypto_entropy_operational_policy_check
        anocrypto_c
        ano_crypto_module_state_tests
        ano_crypto_service_gate_negative_tests
        ano_crypto_sha256_hmac_kat_tests
        ano_crypto_sha256_hmac_extended_kat_tests
        ano_crypto_hmac_drbg_kat_tests
        ano_crypto_hmac_drbg_boundary_tests
        ano_crypto_hmac_key_handle_kat_tests
        ano_crypto_hash_hmac_api_negative_tests
        ano_crypto_sha2_reserved_api_tests
        ano_crypto_random_reserved_api_tests
        ano_crypto_drbg_api_negative_tests
        ano_crypto_csp_key_lifecycle_negative_tests
        ano_crypto_csp_handle_stale_exhaustion_tests
        ano_crypto_entropy_adapter_tests
        ano_crypto_platform_entropy_provider_tests
        ano_crypto_entropy_source_negative_tests
        ano_crypto_entropy_health_tests
        ano_crypto_entropy_operational_policy_tests
        ano_crypto_process_guard_tests
        ano_crypto_entropy_health_error_state_tests
        ano_crypto_drbg_entropy_helper_tests
        ano_crypto_thread_lock_tests
        ano_crypto_install_smoke_buildtree)

    if(TARGET anocrypto_c_core_poc)
        add_dependencies(ano_crypto_collect_release_evidence anocrypto_c_core_poc)
    endif()

    if(TARGET ano_crypto_csp_thread_safety_tests)
        add_dependencies(ano_crypto_collect_release_evidence ano_crypto_csp_thread_safety_tests)
    endif()

    if(TARGET ano_crypto_platform_entropy_failure_injection_tests)
        add_dependencies(ano_crypto_collect_release_evidence ano_crypto_platform_entropy_failure_injection_tests)
    endif()
endif()
