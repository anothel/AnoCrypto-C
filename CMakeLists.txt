cmake_minimum_required(VERSION 3.16)

project(AnoCryptoC
    VERSION 0.1.0
    DESCRIPTION "AnoCrypto-C C99 module skeleton"
    LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(ANO_CRYPTO_BUILD_TESTS "Build AnoCrypto-C unit and smoke tests" ON)
option(ANO_CRYPTO_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(ANO_CRYPTO_ENABLE_TEST_HOOKS "Enable internal test-only hooks" ${ANO_CRYPTO_BUILD_TESTS})
option(ANO_CRYPTO_ENABLE_THREAD_SAFETY "Enable internal locks for module/self-test/CSP global state" ON)

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ANO_CRYPTO_BUILD_GIT_COMMIT "unknown" CACHE STRING "Git commit or source revision for build identity")
set(ANO_CRYPTO_BUILD_FINGERPRINT "anocrypto-c-p1-6-thread-safety" CACHE STRING "Build fingerprint string")

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
    src/drbg/entropy_adapter.c
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

if(ANO_CRYPTO_ENABLE_TEST_HOOKS)
    target_compile_definitions(anocrypto_c PRIVATE ANO_CRYPTO_ENABLE_TEST_HOOKS)
endif()

if(ANO_CRYPTO_ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(anocrypto_c PRIVATE /W4)
    else()
        target_compile_options(anocrypto_c PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

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
    target_compile_definitions(ano_crypto_module_state_tests PRIVATE ANO_CRYPTO_ENABLE_TEST_HOOKS)

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

    add_executable(ano_crypto_hmac_key_handle_kat_tests
        tests/kat/hmac_key_handle_kat_tests.c)
    target_link_libraries(ano_crypto_hmac_key_handle_kat_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hmac_key_handle_kat_tests COMMAND ano_crypto_hmac_key_handle_kat_tests)

    add_executable(ano_crypto_hash_hmac_api_negative_tests
        tests/negative/hash_hmac_api_negative_tests.c)
    target_link_libraries(ano_crypto_hash_hmac_api_negative_tests PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_hash_hmac_api_negative_tests COMMAND ano_crypto_hash_hmac_api_negative_tests)

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

    add_executable(ano_crypto_thread_lock_tests
        tests/unit/thread_lock_tests.c)
    target_link_libraries(ano_crypto_thread_lock_tests PRIVATE anocrypto_c)
    target_include_directories(ano_crypto_thread_lock_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

    add_test(NAME ano_crypto_thread_lock_tests COMMAND ano_crypto_thread_lock_tests)

    if(CMAKE_USE_PTHREADS_INIT)
        add_executable(ano_crypto_csp_thread_safety_tests
            tests/unit/csp_thread_safety_tests.c)
        target_link_libraries(ano_crypto_csp_thread_safety_tests PRIVATE anocrypto_c Threads::Threads)

        add_test(NAME ano_crypto_csp_thread_safety_tests COMMAND ano_crypto_csp_thread_safety_tests)
    endif()

    add_executable(ano_crypto_install_smoke_buildtree
        tests/install_smoke/main.c)
    target_link_libraries(ano_crypto_install_smoke_buildtree PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_install_smoke_buildtree COMMAND ano_crypto_install_smoke_buildtree)

    add_custom_target(ano_crypto_collect_release_evidence
        COMMAND ${CMAKE_COMMAND}
            -DANO_CRYPTO_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -DANO_CRYPTO_EVIDENCE_DIR=${CMAKE_CURRENT_BINARY_DIR}/evidence/p1-6
            -DANO_CRYPTO_RUN_CTEST=ON
            -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON
            -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/collect_release_evidence.cmake
        USES_TERMINAL
        COMMENT "Collecting AnoCrypto-C P1-6 release evidence draft")
    add_dependencies(ano_crypto_collect_release_evidence
        anocrypto_c
        ano_crypto_module_state_tests
        ano_crypto_service_gate_negative_tests
        ano_crypto_sha256_hmac_kat_tests
        ano_crypto_sha256_hmac_extended_kat_tests
        ano_crypto_hmac_drbg_kat_tests
        ano_crypto_hmac_key_handle_kat_tests
        ano_crypto_hash_hmac_api_negative_tests
        ano_crypto_drbg_api_negative_tests
        ano_crypto_csp_key_lifecycle_negative_tests
        ano_crypto_csp_handle_stale_exhaustion_tests
        ano_crypto_entropy_adapter_tests
        ano_crypto_thread_lock_tests
        ano_crypto_install_smoke_buildtree)

    if(TARGET ano_crypto_csp_thread_safety_tests)
        add_dependencies(ano_crypto_collect_release_evidence ano_crypto_csp_thread_safety_tests)
    endif()
endif()
