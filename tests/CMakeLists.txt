enable_testing ()

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})

add_executable (test_rename_strings
  test_rename_strings.cc
  ${CMAKE_SOURCE_DIR}/src/string_renamer.cc
  ${CMAKE_SOURCE_DIR}/src/string_renamer.h)
target_link_libraries (test_rename_strings ${DEPS_LIBRARIES})
add_test (test_rename_strings ${CMAKE_CURRENT_BINARY_DIR}/test_rename_strings)
