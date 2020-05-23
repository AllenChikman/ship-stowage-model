//
// Created by Allen on 19/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_317624278_B_H
#define SHIP_STOWAGE_MODEL_317624278_B_H

#include "NaiveAlgorithm.h"
#include "../common/EnviormentConfig.h"
#include "../interfaces/AlgorithmRegistration.h"

class _317624278_b : public NaiveAlgorithm
{
public:
    _317624278_b() : NaiveAlgorithm()
    {
        setSecondAlgorithm();
    }

};


#endif //SHIP_STOWAGE_MODEL_317624278_B_H
