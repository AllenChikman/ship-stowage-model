cmake_minimum_required(VERSION 3.13)
project(ship_stowage_model)

set(CMAKE_CXX_STANDARD 17)

add_executable(ship_stowage_model
        src/main.cpp
        src/Algorithms/NaiveAlgorithm.cpp
        src/Algorithms/NaiveAlgorithm.h
        src/Interfaces/AbstractAlgorithm.h
        src/Common/Ship.h
        src/Simulator/Simulation.cpp
        src/Simulator/Simulation.h
        src/Interfaces/WeightBalanceCalculator.cpp
        src/Interfaces/WeightBalanceCalculator.h
        src/Common/Port.cpp
        src/Common/Port.h
        src/Common/Utils.cpp
        src/Common/Utils.h
        src/Common/Container.cpp
        src/Common/Container.h
        src/Common/ErrorHandling.cpp
        src/Common/ErrorHandling.h
        src/Common/AlgorithmValidator.cpp
        src/Common/AlgorithmValidator.h
        src/Interfaces/AlgorithmRegistration.h
        src/Interfaces/AlgorithmRegistration.cpp
        src/Simulator/AlgorithmRegistrar.cpp
        src/Simulator/AlgorithmRegistrar.h
        src/Algorithms/_317624278_a.cpp
        src/Algorithms/_317624278_a.h
        src/Algorithms/_317624278_b.cpp
        src/Algorithms/_317624278_b.h src/Common/EnviormentConfig.h)
