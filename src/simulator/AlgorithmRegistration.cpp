#include "../interfaces/AlgorithmRegistration.h"

#include <utility>
#include "../simulator/AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(std::move(algorithmFactory));
}