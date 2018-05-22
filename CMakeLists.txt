cmake_minimum_required(VERSION 3.0)

project(ProxyBench
  VERSION 0.1.0
  LANGUAGES CXX)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

############################################################
# Build Specification

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release
    CACHE STRING "Build Type")
endif()

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

include_directories(${PROJECT_BINARY_DIR})
include_directories(src)

############################################################
# Build Output Specification

set(ProxyBench_OUTPUT_DIR "${PROJECT_BINARY_DIR}/proxy-bench-${PROJECT_VERSION}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ProxyBench_OUTPUT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ProxyBench_OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ProxyBench_OUTPUT_DIR}/lib")

############################################################
# Dependency

find_package(Boost
  REQUIRED COMPONENTS
  program_options
  )

add_definitions(-DBOOST_ALL_DYN_LINK)

############################################################
# Config file

configure_file(
  ${PROJECT_SOURCE_DIR}/src/config.h.cmake
  ${PROJECT_BINARY_DIR}/config.h
  @ONLY
  )

############################################################
# Subdirectories

add_subdirectory(src)
