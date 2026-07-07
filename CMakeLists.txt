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

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ANO_CRYPTO_BUILD_GIT_COMMIT "unknown" CACHE STRING "Git commit or source revision for build identity")
set(ANO_CRYPTO_BUILD_FINGERPRINT "anocrypto-c-p1-foundation" CACHE STRING "Build fingerprint string")

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
    src/selftest/selftest.c
    src/selftest/kat_manifest.c
    src/hash/hash.c
    src/mac/hmac.c
    src/drbg/drbg.c
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

    add_executable(ano_crypto_install_smoke_buildtree
        tests/install_smoke/main.c)
    target_link_libraries(ano_crypto_install_smoke_buildtree PRIVATE anocrypto_c)

    add_test(NAME ano_crypto_install_smoke_buildtree COMMAND ano_crypto_install_smoke_buildtree)
endif()
