#ifndef SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
#define SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H


#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <utility>
#include "Container.h"
#include "Port.h"
#include "AbstractAlgorithm.h"

class NaiveAlgorithm : public AbstractAlgorithm{
private:
    std::shared_ptr<ShipPlan> shipPlan;
    std::vector<SeaPortCode> travelRouteStack;
    ShipWeightBalanceCalculator weightBalanceCalculator;
public:
    int readShipPlan(const std::string &full_path_and_file_name) override;

    int readShipRoute(const std::string &full_path_and_file_name) override;

    int setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override;

    int getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                const std::string &output_full_path_and_file_name) override;

};

#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
