cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_CCR_SMOKE_DIR)
  set(ANO_CRYPTO_CCR_SMOKE_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/consultation-contact-readiness-smoke")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_CCR_OUTPUT_DIR=${ANO_CRYPTO_CCR_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_consultation_contact_readiness_package.cmake"
  RESULT_VARIABLE _create_result)
if(NOT _create_result EQUAL 0)
  message(FATAL_ERROR "consultation contact-readiness package creation failed")
endif()
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DANO_CRYPTO_CCR_DIR=${ANO_CRYPTO_CCR_SMOKE_DIR}
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_consultation_contact_readiness_package.cmake"
  RESULT_VARIABLE _check_result)
if(NOT _check_result EQUAL 0)
  message(FATAL_ERROR "consultation contact-readiness package check failed")
endif()
message(STATUS "P2-12 consultation contact-readiness package smoke passed")
