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
    const SeaPortCode &curSeaPortCode;
    std::vector<SeaPortCode> &travelRouteStack;
    bool ignoreInputFile;
public:
    NaiveAlgorithm( std::shared_ptr<ShipPlan> shipPlan,
    const SeaPortCode &curSeaPortCode,
    std::vector<SeaPortCode> &shipRoute) : shipPlan(std::move(shipPlan)),curSeaPortCode(curSeaPortCode), travelRouteStack(shipRoute) {
        ignoreInputFile = false;
    }
    void updateIgnoreInputFile(bool flag) {ignoreInputFile = flag;}

    void updateTravelRouteStack(const std::vector<SeaPortCode> &updatedTravelRouteStack)
    {
        travelRouteStack = updatedTravelRouteStack;
    }
    int readShipPlan(const std::string &full_path_and_file_name) override = 0;

    int readShipRoute(const std::string &full_path_and_file_name) override = 0;

    int setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override = 0;

    int getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                const std::string &output_full_path_and_file_name) override = 0;

};

#endif //SHIP_STOWAGE_MODEL_STOWAGE_ALGORITHM_H
