#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H


#include <utility>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Ship.h"
#include "AbstractAlgorithm.h"

class Simulation
{

private:
    const std::string rootFolder;
    std::string curTravelFolder;

    std::shared_ptr<ShipPlan> shipPlan;
    std::vector<SeaPortCode> shipRoute;
    std::unordered_map<std::string, unsigned> visitedPorts = {};
    std::unordered_map<std::string, unsigned> routeMap = {};
    std::unordered_set<std::string> cargoFilesSet = {};

    std::unordered_map<std::string, unsigned> algorithmPathIdxMap = {};
    std::vector<std::vector<int>> instructionCountMat;

    bool isLastPortVisit(const SeaPortCode &port)
    {
        const std::string& portStr = port.toStr();
        return routeMap[portStr] == visitedPorts[portStr] && portStr == shipRoute.back().toStr();
    }

    std::pair<std::string, std::string> getPortFilePaths(const SeaPortCode &port, int numOfVisits);

    std::string getShipPlanFilePath() { return curTravelFolder + "/shipPlan.csv"; }

    std::string getRouteFilePath() { return curTravelFolder + "/routeFile.csv"; }

    void createOutputDirectory();

    void updateVisitedPortsMap(const SeaPortCode &port);

    void updateRouteMap();

    void updateRouteFileSet();

    bool popRouteFileSet(const std::string &currInputPath);

    void ValidateAllCargoFilesWereUsed();

    bool initTravel(const std::string &travelName);

    bool performAndValidateAlgorithmInstructions(const std::string &outputDirPath);

    void runAlgorithmTravelPair(const std::string &travelDirPath, AbstractAlgorithm &algorithm,
                                const std::string &outputDirPath);

public:
    explicit Simulation(std::string rootFolder) :
            instructionCountMat(std::vector<std::vector<int>>()),
            rootFolder(std::move(rootFolder)) {}

    bool readShipPlan(const std::string &path);

    bool readShipRoute(const std::string &path);

    bool startTravel(const std::string &travelName);

    void runAlgorithm();

};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H