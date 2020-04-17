#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H


#include <utility>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Ship.h"

class Simulation
{

private:
    const std::string rootFolder;
    std::string curTravelFolder;

    ShipPlan *shipPlan = nullptr;
    std::vector<SeaPortCode> shipRoute;
    std::unordered_map<std::string, unsigned> visitedPorts = {};
    std::unordered_map<std::string, unsigned> routeMap = {};
    std::unordered_set<std::string> cargoFilesSet = {};

    bool isLastPortVisit(const std::string &portStr)
    {
        return routeMap[portStr] == visitedPorts[portStr]
               && portStr == shipRoute.back().toStr();
    }

    std::pair<std::string, std::string> getPortFilePaths(const SeaPortCode &port, int numOfVisits);

    std::string getShipPlanFilePath() { return curTravelFolder + "/shipPlan.csv"; }

    std::string getRouteFilePath() { return curTravelFolder + "/routeFile.csv"; }

    void createOutputDirectory();

    void updateRouteMap();

    void updateRouteFileSet();

    bool popRouteFileSet(const string &currInputPath);

    void WarnOnUnusedCargoFiles();

    bool initTravel(const std::string &travelName);

public:
    explicit Simulation(std::string rootFolder) :
            rootFolder(std::move(rootFolder)) {}

    ~Simulation() { free(shipPlan); }

    bool readShipPlan(const std::string &path);

    bool readShipRoute(const std::string &path);

    bool startTravel(const std::string &travelName);

    void runAlgorithm();

};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H