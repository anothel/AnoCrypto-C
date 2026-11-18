# Independent package regression coverage; existing policy-gated CI stays intact.
cmake_minimum_required(VERSION 3.16)
set(src "${ANO_CRYPTO_SOURCE_DIR}")
set(build "${src}/build-hosted-smoke")
set(config "${ANO_CRYPTO_BUILD_CONFIG}")
if(NOT config MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "Choose Debug or Release for the hosted smoke regression")
endif()
function(run)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE result OUTPUT_VARIABLE out
        ERROR_VARIABLE err WORKING_DIRECTORY "${src}" TIMEOUT 900)
    message("Command: ${ARGN}\nNative result: ${result}\n${out}${err}")
    if(NOT "${result}" STREQUAL "0")
        file(GLOB_RECURSE logs "${build}/install-package-smoke/*.log"
            "${build}/poc-package-smoke/*.log")
        foreach(log IN LISTS logs)
            file(READ "${log}" content)
            message("Log: ${log}\n${content}")
        endforeach()
        message(FATAL_ERROR "Hosted package regression failed")
    endif()
endfunction()
run("${CMAKE_COMMAND}" -S "${src}" -B "${build}" "-DCMAKE_BUILD_TYPE=${config}"
    -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
    -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON
    -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON
    -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF
    -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF)
run("${CMAKE_COMMAND}" --build "${build}" --config "${config}" --parallel)
# Keep fault injection separate from the existing indexed evidence suite.
run("${CMAKE_COMMAND}" "-DANO_CRYPTO_SOURCE_DIR=${src}"
    "-DANO_CRYPTO_TEST_WORK_DIR=${build}/smoke-diagnostics-tests"
    -P "${src}/tests/cmake/hosted_smoke_diagnostics_tests.cmake")
# CTest --test-dir needs CMake 3.20; use chdir for the script's 3.16 floor.
run("${CMAKE_COMMAND}" -E chdir "${build}" "${CMAKE_CTEST_COMMAND}"
    --build-config "${config}" --output-on-failure)
run("${CMAKE_COMMAND}" "-DANO_CRYPTO_SOURCE_DIR=${src}" "-DANO_CRYPTO_BUILD_DIR=${build}"
    "-DANO_CRYPTO_BUILD_CONFIG=${config}" -P "${src}/tools/run_install_package_smoke.cmake")
if(WIN32)
    # The parent PATH is unchanged by the child script. Verify the loader defect
    # separately so a build-tree DLL or an accidental copy cannot hide it.
    set(exe "${build}/install-package-smoke/consumer-build/${config}/anocrypto_c_install_smoke.exe")
    execute_process(COMMAND "${exe}" RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 120)
    message("Without installed bin, native result: ${result}\n${out}${err}")
    if(NOT "${result}" MATCHES "(0xc0000135|-1073741515)")
        message(FATAL_ERROR "Expected STATUS_DLL_NOT_FOUND without installed SDK bin")
    endif()
else()
    run("${CMAKE_COMMAND}" --build "${build}" --config "${config}" --target ano_crypto_poc_package_smoke)
endif()
