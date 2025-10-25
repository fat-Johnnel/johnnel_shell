cmake_minimum_required(VERSION 3.10.0)
project(SimpleShell VERSION 0.1.0 LANGUAGES C CXX)


include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC_FILES
    src/main.cpp
    src/build_in.cpp
    src/external_wrapper.cpp
)
set (COMMAND_PATH "${CMAKE_SOURCE_DIR}/bin/commands")                                 
configure_file(
    "${CMAKE_SOURCE_DIR}/include/build_in.h.in"
    "${CMAKE_SOURCE_DIR}/include/build_in.h"

)      

add_subdirectory(commands)
add_executable(SimpleShell_main ${SRC_FILES})
set(INSTALL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)


install(TARGETS SimpleShell_main DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS 
    ls 
    DESTINATION ${INSTALL_BIN_DIR}/commands
)

