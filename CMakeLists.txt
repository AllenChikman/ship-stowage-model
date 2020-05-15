cmake_minimum_required(VERSION 3.13)
project(ship_stowage_model)

set(CMAKE_CXX_STANDARD 17)

add_executable(ship_stowage_model
        src/main.cpp
        src/StowageAlgorithm.cpp
        src/StowageAlgorithm.h
        src/AbstractAlgorithm.h
        src/Ship.cpp
        src/Ship.h
        src/Simulation.cpp
        src/Simulation.h
        src/WeightBalanceCalculator.cpp
        src/WeightBalanceCalculator.h
        src/Port.cpp
        src/Port.h
        src/Utils.cpp
        src/Utils.h
        src/Container.cpp
        src/Container.h)
