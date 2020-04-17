#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <ostream>
#include <fstream>

namespace Crane
{

char getCraneCmdChar(Command cmd)
{
    switch (cmd)
    {
        case Command::UNLOAD:
            return 'U';
        case Command::LOAD:
            return 'L';
        case Command::REJECT:
            return 'R';
        case Command::MOVE:
            return 'M';
        default:
            //unreachable code
            return 'X';
    }

}

void dumpInstruction(std::ofstream &outputStream, Command cmd, const Container &container, XYCord cord)
{
    auto id = container.getID();
    char op = getCraneCmdChar(cmd);

    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << cord.x << CSV_DELIM
                 << cord.y << std::endl;
}


void performUnload(ShipPlan *shipPlan, XYCord xyCord)
{
    auto &cargoMat = shipPlan->getCargo();
    auto &upperCellsMat = shipPlan->getUpperCellsMat();

    unsigned availableUpperCell = upperCellsMat[xyCord];

    cargoMat[xyCord][availableUpperCell - 1] = std::nullopt;
    upperCellsMat[xyCord]--;
}

void performNaiveLoad(ShipPlan *shipPlan, const Container &container)
{
    const auto shipXYCords = shipPlan->getShipXYCordsVec();
    CargoMat &cargoMat = shipPlan->getCargo();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

    unsigned heightToLoad;
    unsigned numOfFloors;

    for (XYCord xyCord: shipXYCords)
    {
        numOfFloors = shipPlan->getNumOfFloors(xyCord);
        if (upperCellsMat[xyCord] < numOfFloors)
        {
            heightToLoad = upperCellsMat[xyCord];
            cargoMat[xyCord][heightToLoad] = container;  // TODO: needs to be moved to Crane
            upperCellsMat[xyCord]++;
            break;
        }
    }
}


void updateShipPlan(std::ofstream &outputFile, ShipPlan *shipPlan, const Container &container,
                    Command cmd, XYCord xyCord)
{
    switch (cmd)
    {
        case Command::UNLOAD:
            performUnload(shipPlan, xyCord);
            break;
        case Command::LOAD:
            performNaiveLoad(shipPlan, container);
            break;
        case Command::REJECT:
            break;
        default:
            log("For HW2");
    }
    dumpInstruction(outputFile, Command::REJECT, container, xyCord);
}

}