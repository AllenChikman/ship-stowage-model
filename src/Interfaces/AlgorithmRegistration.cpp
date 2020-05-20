#include "AlgorithmRegistration.h"

#include <utility>
#include "../Simulator/AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(std::move(algorithmFactory));
}