cmake_minimum_required(VERSION 3.2)
project(prefixsuffix)

set(PACKAGE_VERSION "0.6.9")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/prefixsuffix.glade
        ${CMAKE_CURRENT_BINARY_DIR}/data/prefixsuffix.glade
        COPYONLY)


include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED gtkmm-3.0)

add_subdirectory(src)

