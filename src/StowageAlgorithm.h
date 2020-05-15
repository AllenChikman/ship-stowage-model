#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include "Container.h"
#include "Port.h"


//std::unordered_map<CraneCommand, unsigned> numberOfOperations;
bool
getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, std::shared_ptr<ShipPlan> shipPlan,
                        const SeaPortCode &curSeaPortCode, const std::vector<SeaPortCode> &shipRoute,
                        bool isLastPortVisit);


#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
