#include "Port.h"
#include "Ship.h"
#include "Utils.h"
#include <ostream>
#include <fstream>


void dumpInstruction(std::ofstream &outputStream, char op, Container const &container,
                     unsigned x, unsigned y, unsigned z)
{
    auto id = container.getID();
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << z << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
}


void updateShipPlan(const Container &container, std::ofstream &outputFile, ShipPlan *shipPlan,
                    CraneCommand op, unsigned x, unsigned y, unsigned z)
{

    unsigned newFreeCell;
    // auto &containerMat = shipPlan->getCargo(); //TODO: Or- this line has no use
    bool freeCellFound = false;

    switch (op)
    {
        case CraneCommand::UNLOAD:
            //#TODO: delete container from ShipPlan - for Crane
            newFreeCell = shipPlan->getFreeCells()[x][y];
            shipPlan->getFreeCells()[x][y] = std::max(shipPlan->getStartingHeight()[x][y], std::min(newFreeCell, z));
            dumpInstruction(outputFile, 'U', shipPlan->getCargo()[x][y][z], x, y, z);

            break;
        case CraneCommand::LOAD:
            // choosing a free cell in a naive way
            for (unsigned i = 0; i < shipPlan->getLength(); i++)
            {
                for (unsigned j = 0; j < shipPlan->getWidth(); j++)
                {
                    if (shipPlan->getFreeCells()[i][j] < shipPlan->getHeight())
                    {
                        x = i;
                        y = j;
                        z = shipPlan->getFreeCells()[i][j];
                        //updating free cell matrix
                        shipPlan->getFreeCells()[i][j]++;
                        freeCellFound = true;
                        break;
                    }
                }
                if (freeCellFound)
                {
                    break;
                }
            }
            dumpInstruction(outputFile, 'L', container, x, y, z);
            shipPlan->getCargo()[x][y][z] = container;  //needs to be moved to Crane
            break;
        default:
            log("For HW2");
    }
}