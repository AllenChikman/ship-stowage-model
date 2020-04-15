#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <fstream>
#include "Container.h"
#include "Port.h"

class Algorithm
{

};

bool getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                             const SeaPortCode &curSeaPortCode, const std::vector<SeaPortCode> &shipRoute,
                             bool isLastPortVisit);


#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
