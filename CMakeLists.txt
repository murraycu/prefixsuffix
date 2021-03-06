cmake_minimum_required(VERSION 3.2)
project ("prefixsuffix")

set(PACKAGE_VERSION "0.6.9")

set (CMAKE_CXX_STANDARD 14)


enable_testing()
add_subdirectory (tests)


include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED gtkmm-4.0)

# Let the C++ compiler find the #include files for our own source code:
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src
        ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})

# Generate config.h from config.h.in:
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)


# Check that we have the glib-compile-resources utiltity executable:
find_program (glib_compile_resources_executable glib-compile-resources)
if (glib_compile_resources_executable)
        # TODO: Find a way to write if(), without else(), so we only mention failure.
	message (STATUS "glib-compile-resources executable found.")
else ()
	message (FATAL_ERROR "glib-compile-resources executable not found. This should be provided by glib.")
endif ()

set (resource_dir ${CMAKE_CURRENT_SOURCE_DIR}/data)
set (resource_file ${resource_dir}/prefixsuffix.gresource.xml)
set (resource_file_generated_source prefixsuffix-resources.c)

add_custom_command(
    OUTPUT prefixsuffix-resources.c
    MAIN_DEPENDENCY ${resource_file}
    DEPENDS ${resource_dir}/prefixsuffix.glade

    COMMAND ${glib_compile_resources_executable} --internal --target=${resource_file_generated_source} --sourcedir=${CMAKE_CURRENT_SOURCE_DIR} --generate-source ${resource_file}
)


set(SOURCE_FILES
    src/bakery/Configuration/Association.cc
    src/bakery/Configuration/Association.h
    src/bakery/Configuration/AssociationBase.cc
    src/bakery/Configuration/AssociationBase.h
    src/bakery/Configuration/Client.cc
    src/bakery/Configuration/Client.h
    src/application.cc
    src/application.h
    src/constants.h
    src/file_renamer.cc
    src/file_renamer.h
    src/main.cc
    src/mainwindow.cc
    src/mainwindow.h
    src/string_renamer.cc
    src/string_renamer.h
    src/constants.h
    ${resource_file_generated_source})

add_executable(prefixsuffix ${SOURCE_FILES})

target_link_libraries(prefixsuffix ${DEPS_LIBRARIES})

