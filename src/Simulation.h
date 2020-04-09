#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H



#include <utility>
#include <string>
#include <unordered_map>
#include "Ship.h"

class Simulation {

private:
    const std::string rootFolder;
    std::string curTravelFolder;

    ShipPlan *shipPlan = nullptr;
    std::vector<SeaPortCode> shipRoute;
    std::unordered_map<std::string, int> visitedPorts = {};


    std::pair<std::string, std::string> getPortFilePaths
            (const std::string &curPortFileName, const SeaPortCode &port, int numOfVisits);

    std::string getShipPlanFilePath() { return curTravelFolder + "/shipPlan.txt"; }

    std::string getRouteFilePath() { return curTravelFolder + "/routeFile.txt"; }

    void initTravel(const std::string &travelName) {
        curTravelFolder = rootFolder + '/' + travelName;
        readShipPlan(getShipPlanFilePath());
        readShipRoute(getRouteFilePath());
    }


public:
    explicit Simulation(std::string rootFolder) :
            rootFolder(std::move(rootFolder)) {}


    ~Simulation() { free(shipPlan); }


    void readShipPlan(const std::string &path);

    void readShipRoute(const std::string &path);

    void startTravel(const std::string &travelName);


};



#endif //SHIP_STOWAGE_MODEL_SIMULATION_H