cmake_minimum_required(VERSION 3.10.0)
project(SimpleShell VERSION 0.1.0 LANGUAGES C CXX)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC_FILES
    src/main.cpp
)
set (COMMAND_PATH "${CMAKE_SOURCE_DIR}/bin/commands")                                 
configure_file(
    "${CMAKE_SOURCE_DIR}/include/build_in.h.in"
    "${CMAKE_SOURCE_DIR}/include/build_in.h"
)      

add_library(build_in_basic STATIC 
    src/build_in.cpp
)

add_subdirectory(commands)
add_executable(SimpleShell_main ${SRC_FILES})

target_link_libraries(SimpleShell_main readline)
target_link_libraries(SimpleShell_main build_in_basic)

set(INSTALL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)


install(TARGETS SimpleShell_main DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS 
    ls 
    test_cmd
    cat
    xargs
    touch
    mkdir
    cp
    rm
    mv
    less
    ln
    DESTINATION ${INSTALL_BIN_DIR}/commands
)

