cmake_minimum_required(VERSION 3.20)
project(pass-try)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED)

add_definitions(${LLVM_DEFINITIONS})
add_library(Pass MODULE Pass.cpp)

target_include_directories(Pass PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_directories(Pass PRIVATE ${LLVM_LIBRARY_DIRS})

add_subdirectory(hw01)
target_compile_options(game PRIVATE "-fpass-plugin=$<TARGET_FILE:Pass>" "-O2")
