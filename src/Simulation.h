#ifndef SHIP_STOWAGE_MODEL_SIMULTAION_H
#define SHIP_STOWAGE_MODEL_SIMULTAION_H


#include <string>
#include "Ship.h"

enum class MessageSeverity {
    INFO,
    WARNING,
    ERROR
};

class Simulation {
    std::string rootFolder;
    std::string routeFilePath;
    std::string shipPlanPath;
    std::vector<std::string> portFiles;

public:
    void readShipPlan(const std::string &path);

    void readShipRoute(const std::string &path);


};
unsigned stringToUInt(const std::string &str);
bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines);


#endif //SHIP_STOWAGE_MODEL_SIMULTAION_H
