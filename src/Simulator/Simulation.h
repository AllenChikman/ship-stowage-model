#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SIMULATION_H
#define SHIP_STOWAGE_MODEL_SIMULATION_H

#include <functional>
#include <utility>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>

#include "../Common/Ship.h"
#include "../Interfaces/AbstractAlgorithm.h"
#include "../Common/AlgorithmValidator.h"
#include "../Common/EnviormentConfig.h"

class Simulation
{

private:

    // saving the current travel folder
    std::string curTravelFolder;

    // initialized in initSimulationTravelState Function
    std::shared_ptr<ShipPlan> shipPlan; // read ship plan
    std::vector<SeaPortCode> shipRoute; // read ship route

    // mapping between a port and number of times it has been visited so far.
    // Used for getting the updated cargo path (and also detecting if we're in the last port)
    std::unordered_map<std::string, unsigned> visitedPorts = {};

    // A map between each port and how many time it appears in the DIRECTORY (regardless of the run)
    std::unordered_map<std::string, unsigned> routeMap = {};

    // keeping track of all cargo files in the cargo directory
    // usage of see if the file we are expecting to get (port_name + numOfVisits) appears in directory and
    // also we check this set at the end of each run to see what files we haven't used
    std::unordered_set<std::string> cargoFilesSet = {};

    // saving the results (number of operations) of each pair
    std::set<std::string> allTravelsNames;

    typedef std::unordered_map<std::string, std::map<std::string, int>> ResultsPairMap;
    ResultsPairMap algorithmTravelResults = {};

    AlgorithmValidator simValidator = AlgorithmValidator(true);

    typedef std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmFactory;
    std::vector<std::pair<AlgorithmFactory, std::string>> loadedAlgorithmFactories;

    bool isLastPortVisit(const SeaPortCode &port)
    {
        const std::string &portStr = port.toStr();
        return routeMap[portStr] == visitedPorts[portStr] && portStr == shipRoute.back().toStr();
    }

    std::pair<std::string, std::string> getPortFilePaths
            (const std::string &outputDir, const SeaPortCode &port, int numOfVisits);

    const std::string getShipPlanFilePath(int &status);

    const std::string getRouteFilePath(int &status);

    void updateVisitedPortsMap(const SeaPortCode &port);

    void updateRouteMap();

    void updateRouteFileSet();

    bool popRouteFileSet(const std::string &currInputPath);

    void validateAllCargoFilesWereUsed();

    void initSimulationTravelState(const std::string &travelDir);

    int
    performAndValidateAlgorithmInstructions(const std::string &portFilePath, const std::string &instructionsFilePath,
                                            const SeaPortCode &curPort);

    void getSortedResultVec(std::vector<std::tuple<std::string, int, int>> &algoScore);

    void writeSimulationOutput(const std::string &outputFilePath);

    bool validateUnload(const std::string &id, XYCord xyCord, const SeaPortCode &curPort,
                        std::vector<std::vector<std::string>> &moveContainers);

    bool validateLoad(const std::string &id, XYCord xyCord, const vector<string> &portContainerLine);

    bool validateReject(const string &id, const std::vector<std::string> &portContainerLine);

    bool validateMove(const string &id, const XYCord &xyCord_U, const SeaPortCode &curPort, const XYCord &xyCord_L);

    bool validateInstructionLine(const vector<string> &instructionLine,
                                 const std::unordered_map<string, vector<vector<string>>> &idLinesMap,
                                 const SeaPortCode &curPort, std::vector<std::vector<std::string>> &moveContainers);

    void handleCargoFileExistence(const std::string &currInputPath, bool cargoFileExists, bool lastPortVisit);


    void updateResults(const std::string &algoName, const std::string &travelName, int numOfOperations);

public:

    int readShipPlan(const std::string &path);

    int readShipRoute(const std::string &path);

    void loadAlgorithms(const string &algorithmsRootDit, const std::string &outputDir);

    void runAlgorithmsOnTravels(const string &travelsRootDir, const string &outputDirPath);

    void runAlgorithmTravelPair(const std::string &travelDirPath,
                                std::pair<AlgorithmFactory, string> &algoFactoryNamePair,
                                const std::string &outputDirPath);

    bool allContainersUnloadedAtPort(const SeaPortCode &code);


};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H