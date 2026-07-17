cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_DRA_SMOKE_DIR)
  set(ANO_CRYPTO_DRA_SMOKE_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/downstream-requirement-analysis-smoke")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
  -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
  -DANO_CRYPTO_DRA_OUTPUT_DIR=${ANO_CRYPTO_DRA_SMOKE_DIR}
  -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_downstream_requirement_analysis_package.cmake"
  RESULT_VARIABLE _create_result)
if(NOT _create_result EQUAL 0)
  message(FATAL_ERROR "downstream requirement analysis package creation failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
  -DANO_CRYPTO_DRA_DIR=${ANO_CRYPTO_DRA_SMOKE_DIR}
  -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_downstream_requirement_analysis_package.cmake"
  RESULT_VARIABLE _check_result)
if(NOT _check_result EQUAL 0)
  message(FATAL_ERROR "downstream requirement analysis package check failed")
endif()
message(STATUS "P3-01 downstream requirement analysis package smoke passed")
