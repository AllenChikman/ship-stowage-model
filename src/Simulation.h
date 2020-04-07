#ifndef SHIP_STOWAGE_MODEL_SIMULTAION_H
#define SHIP_STOWAGE_MODEL_SIMULTAION_H


#include <string>
#include "Ship.h"

void splitByWhiteSpace();

class Simulation {
    std::string rootFolder;
    std::string routeFilePath;
    std::string shipPlanPath;
    std::vector<std::string> portFiles;

public:
    void readShipPlan(const std::string &path);

    void readShipRoute(const std::string &path);

    void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath);

};


#endif //SHIP_STOWAGE_MODEL_SIMULTAION_H
