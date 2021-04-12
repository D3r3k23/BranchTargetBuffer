
cmake_minimum_required(VERSION 3.16)
project(BranchTargetBuffer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_BUILD_TYPE Debug)

add_executable(BranchTargetBuffer
    src/main.cpp
    src/BTB.cpp
)
