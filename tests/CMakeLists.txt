add_executable(main main.cpp)
target_link_libraries(main PRIVATE tsl)

# # Enable warnings.
# if (TSL_MASTER_PROJECT)
#   if (MSVC)
#     target_compile_options(tsl INTERFACE /W4)
#   else()
#     target_compile_options(tsl PRIVATE -Wall -Wextra -pedantic)
#   endif()
# endif()
