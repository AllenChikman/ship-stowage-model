#ifndef SHIP_STOWAGE_MODEL_GOODALGORITHM_H
#define SHIP_STOWAGE_MODEL_GOODALGORITHM_H

#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <unordered_set>

#include "../common/Port.h"
#include "../interfaces/AbstractAlgorithm.h"
#include "../common/AlgorithmValidator.h"

class GoodAlgorithm : public AbstractAlgorithm
{
private:
    bool useSecondAlgorithm = false;
    std::shared_ptr<ShipPlan> shipPlan;
    std::vector<SeaPortCode> travelRouteStack;
    WeightBalanceCalculator weightBalanceCalculator{};
    AlgorithmValidator validator = AlgorithmValidator(false);

    // from simulation

    std::unordered_map<std::string, unsigned> routeMap = {};
    std::unordered_set<std::string> cargoFilesSet = {};

    XYCord chooseXYCordByAlgorithmType(const std::shared_ptr<ShipPlan> &shipPlan);

    bool popRouteFileSet(const std::string &currInputPath);

    void updateRouteMap();

    void updateRouteFileSet(const std::string &curTravelFolder);

    int parseInputToContainersVec(std::vector<Container> &ContainersVec, const std::string &inputPath,
                                  std::ofstream &outputPath);

    void Unloading(std::vector<Container> &containersToUnload,
                   XYCord xyCord, std::ofstream &outputFile);

    void Loading(std::vector<Container> &containersToLoad,
                 std::ofstream &outputFile);

public:

    explicit GoodAlgorithm() = default;

    void setSecondAlgorithm() { useSecondAlgorithm = true; }

    int readShipPlan(const std::string &full_path_and_file_name) override;

    int readShipRoute(const std::string &full_path_and_file_name) override;

    int setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override;

    int getInstructionsForCargo(const std::string &inputFilePath,
                                const std::string &outputFilePath) override;

};

#endif //SHIP_STOWAGE_MODEL_GOODALGORITHM_H
