cmake_minimum_required(VERSION 3.10.0)
project(SimpleShell VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC_FILES
    src/main.cpp
    src/build_in.cpp
)

add_subdirectory(commands)
add_executable(SimpleShell ${SRC_FILES})
set(INSTALL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS SimpleShell DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS 
    ls_cmd 
DESTINATION ${INSTALL_BIN_DIR}/commands)

