
#include <vector>
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

    for (unsigned x = 0; x < length; x++)
    {
        for (unsigned y = 0; y < width; y++)
        {
            if (shipPlan->getFreeCells()[x][y] < maxHeight)
            {
                shipVacant = true;
                break;
            }

        }
        if (shipVacant)
        {
            break;
        }
    }
    return !shipVacant;

}

bool containerHasIllegalDestPort(std::vector<SeaPortCode> shipRoute, Container const &container)
{
    bool legalDestPort = false;
    for (auto &port : shipRoute)
    {
        if (container.getDestinationPort().toStr() == port.toStr())
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

bool parseInputToContainersVec(std::vector<Container> &ContainersVec, const std::string &inputPath)
{
    try
    {
        std::vector<std::vector<std::string>> vecLines;
        if (!readToVecLine(inputPath, vecLines)) { throw std::runtime_error(""); }
        for (const auto &lineVec : vecLines)
        {
            ContainersVec.emplace_back(lineVec[0], stringToUInt(lineVec[1]), SeaPortCode(lineVec[2]));
        }
        return true;
    }

    catch (const std::runtime_error &e)
    {
        return false;
    }

}


bool getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                             SeaPortCode *curSeaPortCode)
{

    try
    {
        std::vector<Container> containerVec;
        if (!parseInputToContainersVec(containerVec, inputPath)) { return false; };

        std::ofstream outputFile;
        outputFile.open(outputPath, std::ios::out);
        std::vector<Container> containersToLoad;

        const auto startingHeightMat = shipPlan->getStartingHeight();
        auto cargoMat = shipPlan->getCargo();

        const unsigned length = shipPlan->getLength();
        const unsigned width = shipPlan->getWidth();
        const unsigned height = shipPlan->getHeight();
        const auto &seaPortCodeStr = curSeaPortCode->toStr();


        for (unsigned x = 0; x < length; x++)
        {
            for (unsigned y = 0; y < width; y++)
            {
                unsigned z = startingHeightMat[x][y];
                unsigned startingIdx = height;

                // 1st step: finding a container to load to current port
                while (z < height && !cargoMat[x][y][z].getDestinationPort().toStr().empty()) //#TODO
                {
                    if (seaPortCodeStr == cargoMat[x][y][z].getDestinationPort().toStr())
                    {
                        startingIdx = z;
                    }
                    z++;
                }
                z = startingIdx;

                // 2nd step: unloading all containers above and marking the ones to be reloaded
                while (z < height && !cargoMat[x][y][z].getDestinationPort().toStr().empty())
                {
                    auto curContainer = cargoMat[x][y][z];
                    if (seaPortCodeStr != curContainer.getDestinationPort().toStr())
                    {
                        containersToLoad.push_back(cargoMat[x][y][z]);
                    }
                    updateShipPlan(cargoMat[x][y][z], outputFile, shipPlan, CraneCommand::UNLOAD, x, y, z);
                    z++;
                }
            }
        }
        for (const auto &container : containerVec)
        {
            containersToLoad.push_back(container);
        }
        for (const auto &curContainerToLoad : containersToLoad)
        {
            updateShipPlan(curContainerToLoad, outputFile, shipPlan, CraneCommand::LOAD);
        }

        return true;
    }

    catch (const std::exception &e)
    {
        return false;
    }

}