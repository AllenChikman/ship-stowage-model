#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <unordered_set>

#include "Container.h"
#include "Port.h"
#include "AbstractAlgorithm.h"
#include "WeightBalanceCalculator.h"
#include "AlgorithmValidator.h"

class NaiveAlgorithm : public AbstractAlgorithm{
private:
    std::shared_ptr<ShipPlan> shipPlan;
    std::vector<SeaPortCode> travelRouteStack;
    ShipWeightBalanceCalculator weightBalanceCalculator{};
    AlgorithmValidator validator;

    // from simulation

    std::unordered_map<std::string, unsigned> routeMap = {};
    std::unordered_set<std::string> cargoFilesSet = {};

    bool popRouteFileSet(const std::string &currInputPath);

    void updateRouteMap();

    void updateRouteFileSet(const std::string &curTravelFolder);

    int parseInputToContainersVec(std::vector<Container> &ContainersVec, const std::string &inputPath);

public:

    explicit NaiveAlgorithm() = default;

    int readShipPlan(const std::string &full_path_and_file_name) override;

    int readShipRoute(const std::string &full_path_and_file_name) override;

    int setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override;

    int getInstructionsForCargo(const std::string &inputFilePath,
                                const std::string &outputFilePath) override;

};

#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
