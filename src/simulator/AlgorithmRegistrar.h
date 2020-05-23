//
// Created by Allen on 18/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ALGORITHMRESGISTRAR_H
#define SHIP_STOWAGE_MODEL_ALGORITHMRESGISTRAR_H

#include <vector>
#include <memory>
#include <functional>
#include "../interfaces/AbstractAlgorithm.h"
#include "../common/EnviormentConfig.h"

#ifdef LINUX_ENV
#include "iostream"
#include <dlfcn.h>
#endif

class AlgorithmRegistrar
{
    static AlgorithmRegistrar instance;
    std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> algorithmsFactory;
    size_t size = 0;

    AlgorithmRegistrar() = default;  // singleton class, private constructor

#ifdef LINUX_ENV
    struct DlCloser{
        void operator()(void *dlHandle) const noexcept {
            dlclose(dlHandle);
        }
    };

    std::vector<std::unique_ptr<void, DlCloser>> handles;  // We keep them here to call the destructor at the end
#endif


public:
    void registerAlgorithm(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory);

    static AlgorithmRegistrar &getInstance();

    /// Returns the delta of factory size since the last call (0 or greater, should be 1 in a successful call).
    int howManyAdded();

    /// Returns the last algorithm to be pushed.
    std::function<std::unique_ptr<AbstractAlgorithm>()> &getLast();

    /// Loads SO file.
    bool loadSharedObject(const std::string &path);

    virtual ~AlgorithmRegistrar();


};

#endif //SHIP_STOWAGE_MODEL_ALGORITHMRESGISTRAR_H
