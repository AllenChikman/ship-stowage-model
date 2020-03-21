cmake_minimum_required(VERSION 3.13)
project(ship_stowage_model)

set(CMAKE_CXX_STANDARD 14)

add_executable(ship_stowage_model
        src/main.cpp
        src/StowageAlgorithm.cpp
        src/StowageAlgorithm.h)