cmake_minimum_required(VERSION 3.10.0)
project(SimpleShell VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC_FILES
    src/main.cpp
)


add_executable(SimpleShell ${SRC_FILES})