// AlgorithmRegistration.h

#pragma once

#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );

class AlgorithmRegistration {
public:
    AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>);
};

