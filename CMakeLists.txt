cmake_minimum_required(VERSION 3.13.0)
# Determine if tsl is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED TSL_MASTER_PROJECT)
  set(TSL_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(TSL_MASTER_PROJECT ON)
  endif ()
endif ()

project(tsl)

option(TSL_TEST "Generate the test target." ${TSL_MASTER_PROJECT})

include(GNUInstallDirs)

if (TSL_MASTER_PROJECT)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()

set(TSL_SOURCES
  src/tsl/internal/abort.cpp
)

add_library(tsl ${TSL_SOURCES})

target_sources(tsl
  PUBLIC
  FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    FILES
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/tsl>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tsl>
)

target_compile_features(tsl PUBLIC cxx_std_20)

if (TSL_TEST)
  add_subdirectory(tests)
endif ()

# add_subdirectory(linux)

install(TARGETS tsl
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# vim: et ts=8 sts=2 sw=2
