
#include <Algorithm>
#include <vector>
#include <ostream>
#include <string>
#include "StowageAlgorithm.h"
#include "Utils.h"
#include "Ship.h"
#include "WeightBalanceCalculator.h"

bool isShipFull(ShipPlan *shipPlan)
{
    bool shipVacant = false;
    unsigned length = shipPlan->getLength();
    unsigned width = shipPlan->getWidth();
    unsigned maxHeight = shipPlan->getHeight();

    for(unsigned x = 0; x<length; x++)
    {
        for(unsigned y = 0; x<width; y++)
        {
            if(shipPlan->getFreeCells()[x][y] < maxHeight)
            {
                shipVacant = true;
                break;
            }

        }
        if(shipVacant)
        {
            break;
        }
    }
    return !shipVacant;

}

bool containerHasIllegalDestPort(std::vector<Port> shipRoute, Container const &container)
{
    bool legalDestPort = false;
    for(auto &port : shipRoute)
    {
        if(container.getDestinationPort().toStr() == port.getPortID().toStr())
        {
            legalDestPort = true;
            break;
        }
    }
    return !legalDestPort;
}

bool isBalanced()
{
//#TODO: implement
    return false;
}
void organizeCargoDataToList(std::vector<CargoData> &cargoData, const std::string &inputPath) {
    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(inputPath, vecLines);
    for (const auto &lineVec : vecLines) {
        cargoData.push_back({lineVec[0], stringToUInt(lineVec[1]), SeaPortCode(lineVec[2])});
    }

}

void dumpInstruction(std::ofstream &instructions, char op, Container const &container, unsigned x, unsigned y, unsigned z) {
    instructions << "<" << op << ">" << ",";
    instructions << "<" << container.getID() << ">" << ",";
    instructions << "<" << z << ">" << ",";
    instructions << "<" << x << ">" << ",";
    instructions << "<" << y << ">" << std::endl;
}

void updateShipPlan(const Container &container, std::ofstream &outputFile, ShipPlan *shipPlan,
                    Operation op, unsigned x, unsigned y, unsigned z)  {

    unsigned  newFreeCell;
    auto &containerMat = shipPlan->getCargo();
    bool freeCellFound = false;

    switch (op) {
        case Operation::U :
            //#TODO: delete container from ShipPlan - for Crane
            newFreeCell = shipPlan->getFreeCells()[x][y];
            shipPlan->getFreeCells()[x][y] = std::max(shipPlan->getStartingHeight()[x][y], std::min(newFreeCell, z));
            dumpInstruction(outputFile, 'U', shipPlan->getCargo()[x][y][z], x, y, z);

            break;
        case Operation::L:
            // choosing a free cell in a naive way
            for(int i=0;i<shipPlan->getLength();i++)
            {
                for(int j=0; j<shipPlan->getWidth();j++)
                {
                    if(shipPlan->getFreeCells()[i][j]<shipPlan->getHeight())
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
                if(freeCellFound)
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

void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                             SeaPortCode *curSeaPortCode) {
    std::vector<CargoData> cargoDataVec;
    organizeCargoDataToList(cargoDataVec, inputPath);

    std::ofstream outputFile;
    outputFile.open(outputPath, std::ios::out);
    std::vector<Container> containersToLoad;

    const auto startingHeightMat = shipPlan->getStartingHeight();
    auto cargoMat = shipPlan->getCargo();

    bool unloadContainersToMove = false;
    const unsigned length = shipPlan->getLength();
    const unsigned width = shipPlan->getWidth();
    const unsigned height = shipPlan->getHeight();
    const auto &seaPortCodeStr = curSeaPortCode->toStr();


    for (unsigned x = 0; x < length; x++) {
        for (unsigned y = 0; y < width; y++) {
            for (unsigned z = startingHeightMat[x][y]; z < height; z++) {
                if (seaPortCodeStr !=    cargoMat[x][y][z].getDestinationPort().toStr()) {
                    if(unloadContainersToMove && cargoMat[x][y][z].getDestinationPort().toStr() != "") {
                        updateShipPlan(cargoMat[x][y][z], outputFile, shipPlan, Operation::U, x, y, z);
                        containersToLoad.insert(containersToLoad.begin(), cargoMat[x][y][z]);
                    }
                    continue;
                }
                unloadContainersToMove = true;
                updateShipPlan(cargoMat[x][y][z], outputFile, shipPlan, Operation::U, x, y, z);
            }
            unloadContainersToMove = false;
        }
    }
    for (const auto &newCargo : cargoDataVec) {
        Container newContainer(newCargo.weight, newCargo.destPort, newCargo.id);
        containersToLoad.push_back(newContainer);
    }
    for (const auto &c : containersToLoad) {
        updateShipPlan(c, outputFile, shipPlan, Operation::L);
    }



}