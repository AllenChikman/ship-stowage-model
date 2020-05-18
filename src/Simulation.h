#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H

#include <functional>
#include <utility>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Ship.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmValidator.h"

class Simulation
{

private:

    // saving the current travel folder
    std::string curTravelFolder;

    // initialized in initTravel Function
    std::shared_ptr<ShipPlan> shipPlan; // read ship plan
    std::vector<SeaPortCode> shipRoute; // read ship route

    // mapping between a port and number of times it has been visited so far.
    // Used for getting the updated cargo path (and also detecting if werwe in the last port)
    std::unordered_map<std::string, unsigned> visitedPorts = {};

    // A map between each port and how many time it appears in the DIRECTORY (regardless of the run)
    // used for the last port check. TODO: might be redundant
    std::unordered_map<std::string, unsigned> routeMap = {};

    // keeping track of all cargo files in the cargo directory
    // usage of see if the file we are expecting to get (port_name + numOfVisits) appears in directory and
    // also we check this set at the end of each run to see what files we haven't used
    std::unordered_set<std::string> cargoFilesSet = {};

    // saving the results (number of operations) of each pair
    typedef std::unordered_map<std::string, std::map<std::string, int>> ResultsPairMap;
    ResultsPairMap algorithmTravelResults = {};

    AlgorithmValidator simValidator;

    std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    std::vector<std::string> algorithmNames;

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

    int  performAndValidateAlgorithmInstructions(const std::string &outputDirPath);

    void getSortedResultVec(std::vector<std::tuple<std::string,int,int>>& algoScore);

    void writeSimulationOutput(const std::string &outputFilePath);

    bool validateInstructionLine(const std::vector<std::string> &instructionLine);

    void loadAlgorithms(const std::string &dirPath);

public:
    bool readShipPlan(const std::string &path);

    bool readShipRoute(const std::string &path);

    void runAlgorithmOnTravels(const std::string &travelsRootDir,
                                           AbstractAlgorithm &algorithm, const std::string &outputDirPath);

    void runAlgorithmTravelPair(const std::string &travelDirPath, AbstractAlgorithm &algorithm,
                                const std::string &outputDirPath);


    // EX1 functions - for reference

/*    bool startTravel(const std::string &travelName);

    void runAlgorithm();*/

};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H