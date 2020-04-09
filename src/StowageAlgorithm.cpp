#include "StowageAlgorithm.h"
#include <ostream>



void organizeCargoDataToList(std::vector<CargoData> &cargoData, const std::string &inputPath)
{
    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(inputPath, vecLines);
    for (const auto &lineVec : vecLines) {
        cargoData.push_back({lineVec[0], stringToUInt(lineVec[1]), lineVec[2]});
    }

}

void writeInstruction(ofstream &instructions, char op, Container const &container, unsigned x, unsigned y, unsigned z)
{
   std::string data[5] = {(const char*)(op), container.getID(), (const char*) x, (const char*)y, (const char*)z};
   for(int i = 0; i<5; i++)
   {
       if(i == 4) { instructions << "<" << data[i] << ">"; }
       else {  instructions << "<" << data[i] << ">" << ","; }
   }
}

void Algorithm::operateOnShip(const Container &container,ofstream &instructions,
        Operation op, unsigned x , unsigned y , unsigned z, unsigned unloadContainersToMove)
{
    switch(op)
    {
        unsigned int cur;
        case Operation ::U :
            cur = shipPlan.getHeight() - 1;
                do
                {
                    //#TODO: delete container from ShipPlan
                    writeInstruction(instructions, 'U', container, x, y, z);
                    // updating list of vacant cells
                    ShipCell newCell = {x, y ,z};
                    shipPlan.getVacantCells().push_back(newCell);
                    if(unloadContainersToMove == 0)
                    {
                        break;
                    }
                    cur--;
                }while(cur >= z);
            break;
        case Operation ::L:
            // ie, x, y and z are negative
            x = shipPlan.getVacantCells()[0].x;
            y = shipPlan.getVacantCells()[0].x;
            z = shipPlan.getVacantCells()[0].z;
            shipPlan.getCargo()[x][y][z] = container;
            // updating list of vacant cells
            shipPlan.getVacantCells().erase(shipPlan.getVacantCells().begin());
            writeInstruction(instructions, 'U', container, x, y, z);
            break;
    }
}

void Algorithm::getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath) {
    std::vector<CargoData> cargoDataVec;
    organizeCargoDataToList(cargoDataVec, inputPath);

    ofstream instructionsForCargo;
    instructionsForCargo.open("InstructionsForCargo.txt", ios::out);

    vector<Container> potentialContainersToMove, containersToLoad;
    unsigned unloadContainersToMove = 0;

    for(unsigned x = 0; x<shipPlan.getLength(); x++)
    {
        for(unsigned y = 0; y<shipPlan.getWidth(); y++)
        {
            for(unsigned z = shipPlan.getHeight() -1; z>=shipPlan.getStartingHeight()[x][y]; z--)
            {
                if(port.getPortID() == shipPlan.getCargo()[x][y][z].getDestinationPort())
                {
                    if(!potentialContainersToMove.empty()) {unloadContainersToMove = 1;}
                    operateOnShip(shipPlan.getCargo()[x][y][z], instructionsForCargo, Operation::U,
                            x, y, z, unloadContainersToMove);
                    for(auto c : potentialContainersToMove)
                    {
                        containersToLoad.push_back(c);
                    }
                    potentialContainersToMove.clear();
                }
                else
                {
                    potentialContainersToMove.push_back(shipPlan.getCargo()[x][y][z]);
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
        operateOnShip(c, instructionsForCargo, Operation::L);
    }
}

