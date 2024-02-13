set(TSL_LINUX_SOURCES
  src/subprocess.cpp
  src/file.cpp
)

target_sources(tsl
  PRIVATE ${TSL_LINUX_SOURCES}
)

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

# if (TSL_TEST)
#   add_subdirectory(tests)
# endif ()

# vim: et ts=8 sts=2 sw=2
