#include "AlgorithmRegistration.h"

#include <utility>
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(std::move(algorithmFactory));
}