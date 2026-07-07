cmake_minimum_required(VERSION 3.16)
project(AnoCryptoCInstallSmoke C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(AnoCryptoC CONFIG REQUIRED)

add_executable(anocrypto_c_install_smoke main.c)
target_link_libraries(anocrypto_c_install_smoke PRIVATE AnoCryptoC::AnoCryptoC)
