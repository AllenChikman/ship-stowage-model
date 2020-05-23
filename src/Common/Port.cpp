#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <fstream>


namespace Crane
{

    std::optional<Container> performUnload(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord)
{
    auto &cargoMat = shipPlan->getCargo();
    auto &upperCellsMat = shipPlan->getUpperCellsMat();

    unsigned availableUpperCell = upperCellsMat[xyCord];

    std::optional<Container> container = cargoMat[xyCord][availableUpperCell - 1];
    cargoMat[xyCord][availableUpperCell - 1] = std::nullopt;
    upperCellsMat[xyCord]--;
    return container;
}


void performLoad(const std::shared_ptr<ShipPlan> &shipPlan, const std::optional<Container> &container, const XYCord &xyCord)
{
    CargoMat &cargoMat = shipPlan->getCargo();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

    unsigned heightToLoad;
    heightToLoad = upperCellsMat[xyCord];
    cargoMat[xyCord][heightToLoad] = container;
    upperCellsMat[xyCord]++;

}

void performMove(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord_U, const XYCord &xyCord_L)
{
    auto container = performUnload(shipPlan, xyCord_U);
    performLoad(shipPlan, container, xyCord_L);

}



}