#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H


#include <utility>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
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

    typedef std::unordered_map<std::string, std::map<std::string, int>> ResultsPairMap;
    ResultsPairMap algorithmTravelResults = {};

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

    void validateAllCargoFilesWereUsed();

    bool initTravel(const std::string &travelName);

    bool performAndValidateAlgorithmInstructions(const std::string &outputDirPath);


    void getSortedResultVec(std::vector<std::tuple<std::string,int,int>>& algoScore);

    void writeSimulationOutput(const std::string &outputFilePath);

    void runAlgorithmTravelPair(const std::string &travelDirPath, AbstractAlgorithm &algorithm,
                                const std::string &outputDirPath);

public:
    explicit Simulation(std::string rootFolder) :
            rootFolder(std::move(rootFolder)) {}

    bool readShipPlan(const std::string &path);

    bool readShipRoute(const std::string &path);

    bool startTravel(const std::string &travelName);

    void runAlgorithm();

};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H