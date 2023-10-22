cmake_minimum_required(VERSION 3.20)
project(pass-try)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_library(Pass MODULE Pass.cpp)
