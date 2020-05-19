#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <ostream>
#include <fstream>

namespace Crane {

    void performUnload(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord) {
        auto &cargoMat = shipPlan->getCargo();
        auto &upperCellsMat = shipPlan->getUpperCellsMat();

        unsigned availableUpperCell = upperCellsMat[xyCord];

        cargoMat[xyCord][availableUpperCell - 1] = std::nullopt;
        upperCellsMat[xyCord]--;
    }


    void performLoad(const std::shared_ptr<ShipPlan> &shipPlan, const Container &container, const XYCord &xyCord) {
        CargoMat &cargoMat = shipPlan->getCargo();
        UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

        unsigned heightToLoad;
        heightToLoad = upperCellsMat[xyCord];
        cargoMat[xyCord][heightToLoad] = container;
        upperCellsMat[xyCord]++;

    }

    void preformMove()
    {
        //TODO: for ex 3
    }


}