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

#include "../common/Ship.h"
#include "../interfaces/AbstractAlgorithm.h"
#include "../common/AlgorithmValidator.h"
#include "../common/EnviormentConfig.h"

typedef std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmFactory;
typedef std::unordered_map<std::string, std::map<std::string, int>> ResultsPairMap;

struct RunResults
{
    std::string travelName;
    std::vector<std::tuple<std::string, std::string, int>> scoreResults;
};


class SimulationWrapper
{
    void getSortedResultVec(std::vector<std::tuple<std::string, int, int>> &algoScore);
public:
    std::vector<std::pair<AlgorithmFactory, std::string>> loadedAlgorithmFactories;
    std::set<std::string> allTravelsNames;
    ResultsPairMap algorithmTravelResults = {};

    void loadAlgorithms(const string &algorithmsRootDit, const std::string &outputDir);

    void run(const string &travelsRootDir, const string &outputDirPath, unsigned numOfThreads);

    void writeSimulationOutput(const std::string &outputFilePath);

    void updateResults(const std::string &algoName, const std::string &travelName, int numOfOperations);

};

class SimulationRun
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

    //ResultsPairMap algorithmTravelResults = {};

    AlgorithmValidator simValidator = AlgorithmValidator(true);

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

    int performAndValidateAlgorithmInstructions(const std::string &portFilePath, const std::string &instructionsFilePath,
                                                    const SeaPortCode &curPort, const vector<SeaPortCode> &travelRouteStack);


    bool validateUnload(const std::string &id, XYCord xyCord, const SeaPortCode &curPort,
                        std::vector<std::vector<std::string>> &moveContainers);

    bool validateLoad(const std::string &id, XYCord xyCord, const vector<string> &portContainerLine);

    bool validateReject(const string &id, const std::vector<std::string> &portContainerLine);

    bool validateMove(const string &id, const XYCord &xyCord_U, const SeaPortCode &curPort, const XYCord &xyCord_L);

    bool validateInstructionLine(const vector<string> &instructionLine,
                                 const std::unordered_map<string, vector<vector<string>>> &idLinesMap,
                                 const SeaPortCode &curPort, std::vector<std::vector<std::string>> &moveContainers);

    void handleCargoFileExistence(const std::string &currInputPath, bool cargoFileExists, bool lastPortVisit);


public:

    int readShipPlan(const std::string &path);

    int readShipRoute(const std::string &path);

    void runAlgorithmTravelPair(const string &travelDirPath,
                                const std::pair<AlgorithmFactory, string> &algoFactoryNamePair,
                                const string &outputDirPath, RunResults &pairResult);

    bool allContainersUnloadedAtPort(const SeaPortCode &code);


};

#endif //SHIP_STOWAGE_MODEL_SIMULATION_H