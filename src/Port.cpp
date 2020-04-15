#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <ostream>
#include <fstream>


void dumpInstruction(std::ofstream &outputStream, char op, const Container &container,
                     unsigned x, unsigned y)
{
    auto id = container.getID();
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
}


void updateShipPlan(const Container &container, std::ofstream &outputFile, ShipPlan *shipPlan,
                    CraneCommand op, unsigned x, unsigned y)
{

    bool freeCellFound = false;
    const auto cargoMat = shipPlan->getCargo();

    switch (op)
    {
        case CraneCommand::UNLOAD:
            //#TODO: delete container from ShipPlan - for Crane
            shipPlan->getFirstAvailableCellMat()[x][y]--;
            dumpInstruction(outputFile, 'U', container, x, y);

            break;
        case CraneCommand::LOAD:
            // choosing a free cell in a naive way
            for (unsigned i = 0; i < shipPlan->getLength(); i++)
            {
                for (unsigned j = 0; j < shipPlan->getWidth(); j++)
                {
                    if (shipPlan->getFirstAvailableCellMat()[i][j] < shipPlan->getHeight())
                    {
                        x = i;
                        y = j;
                       z = shipPlan->getFirstAvailableCellMat()[i][j];
                        //updating free cell matrix
                        shipPlan->getFirstAvailableCellMat()[i][j]++;
                        freeCellFound = true;
                        break;
                    }
                }
                if (freeCellFound)
                {
                    break;
                }
            }
            dumpInstruction(outputFile, 'L', container, x, y);
            shipPlan->getCargo()[x][y][z] = container;  //needs to be moved to Crane
            break;
        case CraneCommand::REJECT:
            dumpInstruction(outputFile, 'R', container, x, y);
            break;
        default:
            log("For HW2");
    }
}