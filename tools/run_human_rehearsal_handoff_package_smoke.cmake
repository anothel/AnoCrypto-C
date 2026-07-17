cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_HRH_SMOKE_DIR)
  set(ANO_CRYPTO_HRH_SMOKE_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/human-rehearsal-handoff-smoke")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HRH_OUTPUT_DIR=${ANO_CRYPTO_HRH_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_human_rehearsal_handoff_package.cmake"
  RESULT_VARIABLE _create_result)
if(NOT _create_result EQUAL 0)
  message(FATAL_ERROR "human rehearsal handoff package creation failed")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_HRH_DIR=${ANO_CRYPTO_HRH_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_human_rehearsal_handoff_package.cmake"
  RESULT_VARIABLE _check_result)
if(NOT _check_result EQUAL 0)
  message(FATAL_ERROR "human rehearsal handoff package check failed")
endif()
message(STATUS "P2-11 human rehearsal handoff package smoke passed")
