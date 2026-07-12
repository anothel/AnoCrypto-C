cmake_minimum_required(VERSION 3.16)
project(AnoCryptoCExactPackageVersionProbe LANGUAGES C)
find_package(AnoCryptoC 0.1.0 EXACT CONFIG REQUIRED)
if(NOT AnoCryptoC_VERSION STREQUAL "0.1.0")
    message(FATAL_ERROR "Unexpected AnoCryptoC package version: ${AnoCryptoC_VERSION}")
endif()
add_executable(anocrypto_c_exact_package_version_probe main.c)
target_link_libraries(anocrypto_c_exact_package_version_probe PRIVATE AnoCryptoC::AnoCryptoC)

enable_testing()
add_test(NAME exact_package_version_runtime
    COMMAND anocrypto_c_exact_package_version_probe)
