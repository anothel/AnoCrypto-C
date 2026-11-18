cmake_minimum_required(VERSION 3.16)
foreach(stage configure build symbols)
    set(root "${ANO_CRYPTO_TEST_WORK_DIR}/${stage}")
    file(REMOVE_RECURSE "${root}")
    file(MAKE_DIRECTORY "${root}/source/tools" "${root}/source/release" "${root}/build")
    configure_file("${ANO_CRYPTO_SOURCE_DIR}/tools/create_poc_package.cmake"
        "${root}/source/tools/create_poc_package.cmake" COPYONLY)
    configure_file("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake"
        "${root}/source/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake" COPYONLY)
    set(marker "SMOKE_REGRESSION_${stage}_FAILURE")
    file(WRITE "${root}/source/fail.cmake" "message(FATAL_ERROR \"${marker}\")\n")
    set(project "cmake_minimum_required(VERSION 3.16)\nproject(SmokeFailure NONE)\n")
    if(stage STREQUAL "configure")
        string(APPEND project "message(STATUS \"SMOKE_REGRESSION_STDOUT\")\nmessage(FATAL_ERROR \"${marker}\")\n")
    elseif(stage STREQUAL "build")
        string(APPEND project "add_custom_target(fail ALL COMMAND \"${CMAKE_COMMAND}\" -P \"${root}/source/fail.cmake\")\n")
    else()
        string(APPEND project "add_custom_target(ano_crypto_exported_symbol_baseline_check COMMAND \"${CMAKE_COMMAND}\" -P \"${root}/source/fail.cmake\")\n")
    endif()
    file(WRITE "${root}/source/CMakeLists.txt" "${project}")
    execute_process(COMMAND "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${root}/source"
        "-DANO_CRYPTO_BUILD_DIR=${root}/build"
        "-DANO_CRYPTO_POC_SMOKE_DIR=${root}/smoke"
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake"
        RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 120)
    if("${result}" STREQUAL "0")
        message(FATAL_ERROR "${stage} failure was swallowed")
    endif()
    if(NOT "${out}${err}" MATCHES "${marker}")
        message(FATAL_ERROR "${stage}: deepest child diagnostic missing from parent output: ${out}${err}")
    endif()
    if(stage STREQUAL "configure" AND NOT "${out}${err}" MATCHES "SMOKE_REGRESSION_STDOUT")
        message(FATAL_ERROR "Child stdout was lost")
    endif()
    if(NOT EXISTS "${root}/smoke/POC_PACKAGE_CREATE.log")
        message(FATAL_ERROR "Persistent package log missing")
    endif()
    message(STATUS "${stage}: nested diagnostic visible and failure preserved")
endforeach()

# Installation errors must survive the installed-package wrapper as well.
set(root "${ANO_CRYPTO_TEST_WORK_DIR}/install")
file(REMOVE_RECURSE "${root}")
file(MAKE_DIRECTORY "${root}/source/tests/install_smoke" "${root}/source/tests/compat"
    "${root}/source/abi/baselines/0.1.0-p1-29-cmac-hardening/include/anocrypto" "${root}/build")
file(WRITE "${root}/source/tests/install_smoke/CMakeLists.txt" "")
file(WRITE "${root}/source/tests/compat/CMakeLists.txt" "")
file(WRITE "${root}/source/abi/baselines/0.1.0-p1-29-cmac-hardening/include/anocrypto/anocrypto.h" "")
execute_process(COMMAND "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${root}/source"
    "-DANO_CRYPTO_BUILD_DIR=${root}/build"
    "-DANO_CRYPTO_INSTALL_PACKAGE_SMOKE_DIR=${root}/smoke"
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/run_install_package_smoke.cmake"
    RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 120)
if("${result}" STREQUAL "0" OR NOT "${out}${err}" MATCHES "cmake_install.cmake")
    message(FATAL_ERROR "Install child failure or diagnostic was lost: ${out}${err}")
endif()
message(STATUS "install: child diagnostic visible and failure preserved")
