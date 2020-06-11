cmake_minimum_required(VERSION 3.13)
project(ship_stowage_model)

set(CMAKE_CXX_STANDARD 17)

add_executable(ship_stowage_model
        src/simulator/main.cpp
        src/algorithm/NaiveAlgorithm.cpp
        src/algorithm/NaiveAlgorithm.h
        src/interfaces/AbstractAlgorithm.h
        src/common/Ship.h
        src/simulator/Simulation.cpp
        src/simulator/Simulation.h
        src/interfaces/WeightBalanceCalculator.cpp
        src/interfaces/WeightBalanceCalculator.h
        src/common/Port.cpp
        src/common/Port.h
        src/common/Utils.cpp
        src/common/Utils.h
        src/common/Container.cpp
        src/common/Container.h
        src/common/ErrorHandling.h
        src/common/AlgorithmValidator.cpp
        src/common/AlgorithmValidator.h
        src/common/ctpl_stl.h
        src/interfaces/AlgorithmRegistration.h
        src/interfaces/AlgorithmRegistration.cpp
        src/simulator/AlgorithmRegistrar.cpp
        src/simulator/AlgorithmRegistrar.h
        src/algorithm/_317624278_a.cpp
        src/algorithm/_317624278_a.h
        src/algorithm/_317624278_b.cpp
        src/algorithm/_317624278_b.h src/common/EnviormentConfig.h)
