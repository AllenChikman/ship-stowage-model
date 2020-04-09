#ifndef SHIP_STOWAGE_MODEL_STOWAGEALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGEALGORITHM_H

#include <fstream>
#include "Ship.h"
#include "Port.h"

enum Operation{
    U, L, M, R
};

class Algorithm{
public:
    ShipPlan shipPlan;
    Port port;
    void operateOnShip(const Container &container, ofstream &instructions, Operation op = Operation::U,
            unsigned x = 0, unsigned y = 0, unsigned z = 0, unsigned unloadContainersToMove = 0);
    void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath);
};


#endif //SHIP_STOWAGE_MODEL_STOWAGEALGORITHM_H
