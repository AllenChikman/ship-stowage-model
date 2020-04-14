#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <fstream>
#include "Container.h"
#include "Port.h"


bool getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                             SeaPortCode *curSeaPortCode);


#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
