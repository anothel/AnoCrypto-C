cmake_minimum_required(VERSION 3.16)

project(AnoCryptoC_CorePoC
    VERSION 0.1.0
    DESCRIPTION "AnoCrypto-C customer PoC integration example"
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT TARGET AnoCryptoC::AnoCryptoC)
    find_package(AnoCryptoC CONFIG REQUIRED)
endif()

add_executable(anocrypto_c_core_poc main.c)
target_link_libraries(anocrypto_c_core_poc PRIVATE AnoCryptoC::AnoCryptoC)

if(WIN32 AND TARGET anocrypto_c)
    add_custom_command(TARGET anocrypto_c_core_poc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:anocrypto_c>
            $<TARGET_FILE_DIR:anocrypto_c_core_poc>
        VERBATIM)
endif()
