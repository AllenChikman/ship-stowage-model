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
#include "../Common/Ship.h"
#include "../Interfaces/AbstractAlgorithm.h"
#include "../Common/AlgorithmValidator.h"

class Simulation
{

private:

    // saving the current travel folder
    std::string curTravelFolder;

    // initialized in initTravel Function
    std::shared_ptr<ShipPlan> shipPlan; // read ship plan
    std::vector<SeaPortCode> shipRoute; // read ship route

    // mapping between a port and number of times it has been visited so far.
    // Used for getting the updated cargo path (and also detecting if we're in the last port)
    std::unordered_map<std::string, unsigned> visitedPorts = {};

    // A map between each port and how many time it appears in the DIRECTORY (regardless of the run)
    // used for the last port check. TODO: might be redundant
    std::unordered_map<std::string, unsigned> routeMap = {};

    // keeping track of all cargo files in the cargo directory
    // usage of see if the file we are expecting to get (port_name + numOfVisits) appears in directory and
    // also we check this set at the end of each run to see what files we haven't used
    std::unordered_set<std::string> cargoFilesSet = {};

    // saving the results (number of operations) of each pair
    std::vector<std::string> allTravelsNames;

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

    std::pair<std::string, std::string> getPortFilePaths
    (const std::string &outputDir, const SeaPortCode &port, int numOfVisits);

    std::string getShipPlanFilePath() { return curTravelFolder + "/shipPlan.csv"; } //TODO: any suffix will work here

    std::string getRouteFilePath() { return curTravelFolder + "/routeFile.csv"; } //TODO: any suffix will work here

    void updateVisitedPortsMap(const SeaPortCode &port);

    void updateRouteMap();

    void updateRouteFileSet();

    bool popRouteFileSet(const std::string &currInputPath);

    void validateAllCargoFilesWereUsed();

    bool initTravel(const std::string &travelName);

    int performAndValidateAlgorithmInstructions(const std::string &portFilePath , const std::string &instructionsFilePath, const SeaPortCode &curPort);

    void getSortedResultVec(std::vector<std::tuple<std::string,int,int>>& algoScore);

    void writeSimulationOutput(const std::string &outputFilePath);

    bool validateUnload(const std::string &id, XYCord xyCord, const SeaPortCode &curPort);

    bool validateLoad(const std::string &id, XYCord xyCord, const vector<string> &portContainerLine);

    bool validateReject(const string &id, const std::vector<std::string> &portContainerLine);

    bool validateMove(const std::string &id);

    bool validateInstructionLine(const std::vector <std::string> &instructionLine, const std::vector <std::string> &portContainerLine, const SeaPortCode &curPort);
    void handleCargoFileExistence(const std::string &currInputPath , bool cargoFileExists , bool lastPortVisit);

    Errors validateInstructionLine(const std::vector<std::string> &instructionLine);

    void updateResults(const std::string &algoName, const std::string &travelName, int numOfOperations);

    void loadAlgorithms(const std::string &dirPath);

public:

    bool readShipPlan(const std::string &path);

    bool readShipRoute(const std::string &path);

    void runAlgorithmOnTravels(const std::string &travelsRootDir,
                                           AbstractAlgorithm &algorithm, const std::string &outputDirPath = "./");

    void runAlgorithmTravelPair(const std::string &travelDirPath, AbstractAlgorithm &algorithm,
                                const std::string &outputDirPath);

};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H