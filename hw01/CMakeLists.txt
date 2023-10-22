cmake_minimum_required(VERSION 3.20)
project(sem11_llvm_hw01)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

add_subdirectory(third-party/SDL2)
include_directories(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/third-party/SDL2/include")

add_library(sim STATIC sim.cpp sim.h)
target_link_libraries(sim SDL2::SDL2-static)

add_executable(game main.c sim.h)
target_link_libraries(game sim)
