#include <string>
#include <unordered_map>
#include "Ship.h"
#include "StowageAlgorithm.h"

enum class MessageSeverity {
    INFO,
    WARNING,
    ERROR
};

class Simulation {
    std::string rootFolder;

    ShipPlan shipPlan;
    vector<SeaPortCode> shipRoute;
    std::unordered_map<SeaPortCode, int> visitedPorts;

public:
    void readShipPlan(const std::string &path);

    void readShipRoute(const std::string &path);

    void initSimulation(const std::string &shipPlanPath, const std::string &routePath) {
        readShipPlan(shipPlanPath);
        readShipRoute(routePath);
    }

    void startTravel();


    std::pair<std::string, std::string> getPortFilePaths
            (const string &curPortFileName, const SeaPortCode &port, int numOfVisits);

    std::string getshipPlanFilePath() { return rootFolder + "/shipPlan.txt"; }

    std::string getRouteFilePath() { return rootFolder + "/routeFile.txt"; }

};

unsigned stringToUInt(const std::string &str);

bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines);

