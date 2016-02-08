cmake_minimum_required(VERSION 3.2)
project ("prefixsuffix")

set(PACKAGE_VERSION "0.6.9")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED gtkmm-3.0)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/data/prefixsuffix.glade
        ${CMAKE_CURRENT_BINARY_DIR}/data/prefixsuffix.glade
        COPYONLY)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src
        ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)


set (resource_dir ${CMAKE_CURRENT_SOURCE_DIR}/data)
set (resource_file ${resource_dir}/prefixsuffix.gresource.xml)

add_custom_command(
    OUTPUT prefixsuffix-resources.c
    MAIN_DEPENDENCY ${resource_file}
    DEPENDS ${resource_dir}/prefixsuffix.glade

    COMMAND glib-compile-resources --internal --target=prefixsuffix-resources.c --sourcedir=${CMAKE_CURRENT_SOURCE_DIR} --generate-source ${resource_file}
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
    src/file_renamer.cc
    src/file_renamer.h
    src/main.cc
    src/mainwindow.cc
    src/mainwindow.h
    src/string_renamer.cc
    src/string_renamer.h
    src/constants.h
    prefixsuffix-resources.c)

add_executable(prefixsuffix ${SOURCE_FILES})

target_link_libraries(prefixsuffix ${DEPS_LIBRARIES})

