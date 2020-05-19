//
// Created by Allen on 18/05/2020.
//

//#include <iostream>
#include "AlgorithmRegistrar.h"

#ifdef LINUX_ENV
#include <dlfcn.h>
#endif

// Singleton Design Pattern

AlgorithmRegistrar AlgorithmRegistrar::instance;

AlgorithmRegistrar &AlgorithmRegistrar::getInstance() {
    return instance;
}


// Registration

void AlgorithmRegistrar::registerAlgorithm(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithm) {
    algorithmsFactory.push_back(algorithm);
}

int AlgorithmRegistrar::howManyAdded() {
    auto delta = static_cast<int>(algorithmsFactory.size() - size);
    size = algorithmsFactory.size();
    return delta;
}

std::function<std::unique_ptr<AbstractAlgorithm>()> &AlgorithmRegistrar::getLast() {
    return algorithmsFactory.back();
}

bool AlgorithmRegistrar::loadSharedObject(const std::string &path) {
#ifdef LINUX_ENV
    std::unique_ptr<void, DlCloser> handle(dlopen(path.c_str(), RTLD_LAZY));
    if (!handle) {
        return false;
    }

    handles.push_back(std::move(handle));
#endif
    (void)path;  // ignore unused parameter
    return true;
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    algorithmsFactory.clear();
#ifdef LINUX_ENV
    handles.clear();
#endif
}