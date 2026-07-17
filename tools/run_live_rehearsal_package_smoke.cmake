cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_LIVE_REHEARSAL_SMOKE_DIR)
  set(ANO_CRYPTO_LIVE_REHEARSAL_SMOKE_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/live-rehearsal-smoke")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_LIVE_REHEARSAL_OUTPUT_DIR=${ANO_CRYPTO_LIVE_REHEARSAL_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_live_rehearsal_package.cmake"
  RESULT_VARIABLE _create_result)
if(NOT _create_result EQUAL 0)
  message(FATAL_ERROR "assisted rehearsal package creation failed")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_LIVE_REHEARSAL_DIR=${ANO_CRYPTO_LIVE_REHEARSAL_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_live_rehearsal_package.cmake"
  RESULT_VARIABLE _check_result)
if(NOT _check_result EQUAL 0)
  message(FATAL_ERROR "assisted rehearsal package check failed")
endif()
message(STATUS "P2-10 assisted rehearsal package smoke passed")
