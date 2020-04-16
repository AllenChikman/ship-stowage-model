#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <ostream>
#include <fstream>


void dumpInstruction(std::ofstream &outputStream, char op, const Container &container, XYCord cord)
{
    auto id = container.getID();
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << cord.x << CSV_DELIM
                 << cord.y << std::endl;
}


void updateShipPlan(const Container &container, std::ofstream &outputFile, ShipPlan *shipPlan,
                    CraneCommand op, XYCord xyUpdateCord)
{
    unsigned z = 0;
    auto cargoMat = shipPlan->getCargo();
    const auto shipXYCords = shipPlan->getShipXYCordsVec();
    unsigned availableCell;

    switch (op)
    {
        case CraneCommand::UNLOAD:
            availableCell = shipPlan->getFirstAvailableCellMat()[xyUpdateCord];
            cargoMat[xyUpdateCord.x][xyUpdateCord.y][availableCell -1] = std::nullopt;
            shipPlan->getFirstAvailableCellMat()[xyUpdateCord]--;
            dumpInstruction(outputFile, 'U', container, xyUpdateCord);
            break;
        case CraneCommand::LOAD:

            // choosing a free cell in a naive way
            for (XYCord cord: shipXYCords)
            {
                if (shipPlan->getFirstAvailableCellMat()[cord] < shipPlan->getHeight())
                {
                    xyUpdateCord = {cord.x , cord.y};
                    z = shipPlan->getFirstAvailableCellMat()[cord];
                    //updating free cell matrix
                    shipPlan->getFirstAvailableCellMat()[cord]++;
                    break;
                }
            }
            dumpInstruction(outputFile, 'L', container, xyUpdateCord);
            shipPlan->getCargo()[xyUpdateCord.x][xyUpdateCord.y][z] = container;  //needs to be moved to Crane
            break;
        case CraneCommand::REJECT:
            dumpInstruction(outputFile, 'R', container, xyUpdateCord);
            break;
        default:
            log("For HW2");
    }
}