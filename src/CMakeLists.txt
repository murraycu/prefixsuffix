cmake_minimum_required(VERSION 3.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(SOURCE_FILES
    bakery/Configuration/Association.cc
    bakery/Configuration/Association.h
    bakery/Configuration/AssociationBase.cc
    bakery/Configuration/AssociationBase.h
    bakery/Configuration/Client.cc
    bakery/Configuration/Client.h
    application.cc
    application.h
    file_renamer.cc
    file_renamer.h
    main.cc
    mainwindow.cc
    mainwindow.h
    string_renamer.cc
    string_renamer.h constants.h)

add_executable(prefixsuffix ${SOURCE_FILES})

target_link_libraries(prefixsuffix ${DEPS_LIBRARIES})
