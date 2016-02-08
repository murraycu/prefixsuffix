cmake_minimum_required(VERSION 3.2)
project(prefixsuffix)

set(PACKAGE_VERSION "0.6.9")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/data/prefixsuffix.glade
        ${CMAKE_CURRENT_BINARY_DIR}/data/prefixsuffix.glade
        COPYONLY)


include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED gtkmm-3.0)


# Include custom functions
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/gresource.cmake")

find_package (gresources REQUIRED)

gresource (
	"${CMAKE_CURRENT_SOURCE_DIR}/data/" # resource dir
	"prefixsuffix.gresource.xml" # resource file
	"${CMAKE_CURRENT_BINARY_DIR}/src" # output dir
	generated_resource
)

set (generated_resources ${generated_resource})


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

find_program (gresources_executable glib-compile-resources)

if (gresources_executable)
  set (gresources_FOUND true)
else ()
  set (gresources_FOUND false)
endif ()

if (gresources_FIND_REQUIRED)
  message (FATAL_ERROR "glib-compile-resources not found")
endif ()

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
    src/string_renamer.h constants.h
    ${generated_resources})

add_executable(prefixsuffix ${SOURCE_FILES})

target_link_libraries(prefixsuffix ${DEPS_LIBRARIES})

