
#include <vector>
#include <ostream>
#include "StowageAlgorithm.h"
#include "Utils.h"
#include "Ship.h"


void organizeCargoDataToList(std::vector<CargoData> &cargoData, const std::string &inputPath)
{
    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(inputPath, vecLines);
    for (const auto &lineVec : vecLines) {
        //cargoData.push_back({lineVec[0], stringToUInt(lineVec[1]), lineVec[2]});
    }

}

void writeInstruction(std::ofstream &instructions, char op, Container const &container, unsigned x, unsigned y, unsigned z)
{
    instructions << "<" <<  op << ">" << ",";
    instructions << "<" << container.getID() << ">" << ",";
    instructions << "<" << z << ">" << ",";
    instructions << "<" << x << ">" << ",";
    instructions << "<" << y << ">" << "," << std::endl;
}

void operateOnShip(const Container &container,std::ofstream &instructions, ShipPlan *shipPlan,
        Operation op, unsigned x , unsigned y , unsigned z, unsigned unloadContainersToMove)
{
    switch(op)
    {
        unsigned int cur;
        case Operation ::U :
            cur = shipPlan->getHeight() - 1;
                do
                {
                    //#TODO: delete container from ShipPlan
                    writeInstruction(instructions, 'U', container, x, y, z);
                    // updating list of vacant cells
                    ShipCell newCell = {x, y ,z};
                    shipPlan->getVacantCells().push_back(newCell);
                    if(unloadContainersToMove == 0)
                    {
                        break;
                    }
                    cur--;
                }while(cur >= z);
            break;
        case Operation ::L:
            // ie, x, y and z are negative
            x = shipPlan->getVacantCells()[0].x;
            y = shipPlan->getVacantCells()[0].x;
            z = shipPlan->getVacantCells()[0].z;
            shipPlan->getCargo()[x][y][z] = container;
            // updating list of vacant cells
            shipPlan->getVacantCells().erase(shipPlan->getVacantCells().begin());
            writeInstruction(instructions, 'U', container, x, y, z);
            break;
        default:
           log("For HW2");
    }
}

void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan, SeaPortCode *curSeaPortCode) {
    std::vector<CargoData> cargoDataVec;
    organizeCargoDataToList(cargoDataVec, inputPath);

    std::ofstream instructionsForCargo;
    instructionsForCargo.open(outputPath, std::ios::out);

    std::vector<Container> potentialContainersToMove, containersToLoad;
    unsigned unloadContainersToMove = 0;

    for(unsigned x = 0; x<shipPlan->getLength(); x++)
    {
        for(unsigned y = 0; y<shipPlan->getWidth(); y++)
        {
            for(unsigned z = shipPlan->getHeight() -1; z>=shipPlan->getStartingHeight()[x][y]; z--)
            {
                if(curSeaPortCode->toStr() == shipPlan->getCargo()[x][y][z].getDestinationPort().toStr())
                {
                    if(!potentialContainersToMove.empty()) {unloadContainersToMove = 1;}
                    operateOnShip(shipPlan->getCargo()[x][y][z], instructionsForCargo, shipPlan, Operation::U,
                            x, y, z, unloadContainersToMove);
                    for(auto const &c : potentialContainersToMove)
                    {
                        containersToLoad.push_back(c);
                    }
                    potentialContainersToMove.clear();
                }
                else
                {
                    potentialContainersToMove.push_back(shipPlan->getCargo()[x][y][z]);
                }
            }
        }
    }
    for(const auto& newCargo : cargoDataVec)
    {
        Container newContainer(newCargo.weight, newCargo.destPort, newCargo.id);
        containersToLoad.push_back(newContainer);
    }
    for(auto c : containersToLoad)
    {
        operateOnShip(c, instructionsForCargo, shipPlan, Operation::L);
    }
}


