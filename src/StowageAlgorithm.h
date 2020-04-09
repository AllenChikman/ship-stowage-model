#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <fstream>
#include "Container.h"
#include "Port.h"

enum Operation{
    U, L, M, R
};

//void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath){}
class ShipPlan;

void operateOnShip(const Container &container, std::ofstream &instructions, ShipPlan *shipPlan, Operation op = Operation::U,
                   unsigned x = 0, unsigned y = 0, unsigned z = 0, unsigned unloadContainersToMove = 0);

void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan, SeaPortCode *curSeaPortCode);



#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
