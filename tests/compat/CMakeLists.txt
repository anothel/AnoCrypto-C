cmake_minimum_required(VERSION 3.16)
project(AnoCryptoCReleaseCompatibilityConsumers LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT DEFINED ANO_CRYPTO_P129_BASELINE_INCLUDE_DIR OR
   ANO_CRYPTO_P129_BASELINE_INCLUDE_DIR STREQUAL "")
    message(FATAL_ERROR "ANO_CRYPTO_P129_BASELINE_INCLUDE_DIR is required")
endif()
if(NOT EXISTS "${ANO_CRYPTO_P129_BASELINE_INCLUDE_DIR}/anocrypto/anocrypto.h")
    message(FATAL_ERROR "The retained P1-29 public-header snapshot is unavailable")
endif()

find_package(AnoCryptoC 0.1.0 EXACT CONFIG REQUIRED)

enable_testing()

add_executable(anocrypto_c_current_header_compatibility_consumer
    p1_29_compatibility_probe.c)
target_compile_definitions(anocrypto_c_current_header_compatibility_consumer PRIVATE
    ANO_CRYPTO_COMPAT_HEADER_MODE="current-installed-p3-03"
    ANO_CRYPTO_EXPECT_CURRENT_HEADER=1
    ANO_CRYPTO_EXPECT_RUNTIME_VERSION="0.1.0-p3-03-aes256-gcm-oneshot")
target_link_libraries(anocrypto_c_current_header_compatibility_consumer PRIVATE
    AnoCryptoC::AnoCryptoC)
add_test(NAME current_header_compatibility
    COMMAND anocrypto_c_current_header_compatibility_consumer)

add_executable(anocrypto_c_p129_header_compatibility_consumer
    p1_29_compatibility_probe.c)
# Put the retained P1-29 headers before the installed P3-03 include directory.
target_include_directories(anocrypto_c_p129_header_compatibility_consumer BEFORE PRIVATE
    "${ANO_CRYPTO_P129_BASELINE_INCLUDE_DIR}")
target_compile_definitions(anocrypto_c_p129_header_compatibility_consumer PRIVATE
    ANO_CRYPTO_COMPAT_HEADER_MODE="retained-p1-29"
    ANO_CRYPTO_EXPECT_P1_29_HEADER_SNAPSHOT=1
    ANO_CRYPTO_EXPECT_RUNTIME_VERSION="0.1.0-p3-03-aes256-gcm-oneshot")
target_link_libraries(anocrypto_c_p129_header_compatibility_consumer PRIVATE
    AnoCryptoC::AnoCryptoC)
add_test(NAME p129_header_current_library_compatibility
    COMMAND anocrypto_c_p129_header_compatibility_consumer)
