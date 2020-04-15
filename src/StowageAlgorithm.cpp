
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
            if (shipPlan->getFirstAvailableCellMat()[x][y] < maxHeight)
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

bool containerHasIllegalDestPort(const std::vector<SeaPortCode> &shipRoute, const Container &container)
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

bool isBalanced(ShipPlan *shipPlan, char op, const Container &container, unsigned x, unsigned y)
{
    balanceStatus status = shipPlan->getBalanceCalculator().tryOperation(shipPlan, op, container.getWeight(), x, y);
    return status == balanceStatus::APPROVED;
}

bool isLastPort(ShipPlan *shipPlan, const SeaPortCode &curSeaPortCode, const std::vector<SeaPortCode> &shipRoute)
{
    //#TODO: implement
    return false;
}

bool rejectContainer(ShipPlan *shipPlan, char op, const Container &container, const std::vector<SeaPortCode> &shipRoute)
{
    bool correctContainer = !container.hasWrongID();
    bool legalDestPort = !containerHasIllegalDestPort(shipRoute, container);
    bool legalOp = correctContainer && legalDestPort;
    if (op == 'L')
    {
        legalOp &= !isShipFull(shipPlan);
    }
    return !legalOp;
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

unsigned findMinContainerPosToUnload(UIntMat startingHeightMat, CargoMat cargoMat, const SeaPortCode &curSeaPortCode,
                                     const unsigned height, unsigned x, unsigned y) {



    unsigned z = startingHeightMat[x][y];
    unsigned startingIdx = height;

    while (z < height && cargoMat[x][y][z]) {
        const Container &curContainer = *cargoMat[x][y][z];
        if (curContainer.isBelongToPort(curSeaPortCode)) {
            startingIdx = z;
            break;
        }
        z++;
    }
    return startingIdx;
}

std::vector<Container> collectingPotentialContainersToLoad(bool lastPort, std::vector<Container> &containersToLoad, CargoMat cargoMat,
                                                           const std::string &seaPortCodeStr, const unsigned height, unsigned x, unsigned y, unsigned z)
{
    std::vector<Container> containersToUnload;
    while (z < height && cargoMat[x][y][z]) {
        Container curContainer = *cargoMat[x][y][z];
        if (seaPortCodeStr != curContainer.getDestinationPort().toStr() && !lastPort) {
            containersToLoad.push_back(curContainer);
        }
        containersToUnload.insert(containersToUnload.begin(), *cargoMat[x][y][z]);
        z++;
    }
    return containersToUnload;
}

void checkIfUnloadPossible(ShipPlan *shipPlan, std::vector<Container> &containersToUnload, const std::vector<SeaPortCode> &shipRoute,
                           unsigned x, unsigned y, std::ofstream *outputFile)
{
    bool shipUnbalanced = false;
    CraneCommand cmd;

    for (const auto &container: containersToUnload) {
        if (!isBalanced(shipPlan, 'U', container, x, y)) {
            shipUnbalanced = true;
        }
// Checking if the operation should be rejected
        if (rejectContainer(shipPlan, 'U', container, shipRoute) || shipUnbalanced) {
            cmd = CraneCommand::REJECT;
        } else {
            cmd = CraneCommand::UNLOAD;
        }
        updateShipPlan(container, *outputFile, shipPlan, cmd, XYCord{x, y});
    }
}

void checkIfLoadPossible(ShipPlan *shipPlan, std::vector<Container> &containersToLoad,
                         std::ofstream *outputFile, const std::vector<SeaPortCode> &shipRoute)
{
    for (const auto &curContainerToLoad : containersToLoad) {
        bool shipUnbalanced = false;
        CraneCommand cmd;
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad, 0, 0); //optional
        if (rejectContainer(shipPlan, 'L', curContainerToLoad, shipRoute) || shipUnbalanced) {
            cmd = CraneCommand::REJECT;
        } else {
            cmd = CraneCommand::LOAD;
        }
        updateShipPlan(curContainerToLoad, *outputFile, shipPlan, cmd);
    }
}

bool getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                                     const SeaPortCode &curSeaPortCode, const std::vector<SeaPortCode> &shipRoute) {

            try {
                std::vector<Container> containerVec;
                if (!parseInputToContainersVec(containerVec, inputPath)) { return false; };

                std::ofstream outputFile;
                outputFile.open(outputPath, std::ios::out);
                std::vector<Container> containersToUnload, containersToLoad;

                const auto startingHeightMat = shipPlan->getStartingHeight();
                auto availableCells = shipPlan->getFirstAvailableCellMat();
                auto cargoMat = shipPlan->getCargo();

                const unsigned length = shipPlan->getLength();
                const unsigned width = shipPlan->getWidth();
                const unsigned height = shipPlan->getHeight();
                const auto &seaPortCodeStr = curSeaPortCode.toStr();

                unsigned z;
                bool lastPort = isLastPort(shipPlan, curSeaPortCode, shipRoute);
                for (unsigned x = 0; x < length; x++) {
                    for (unsigned y = 0; y < width; y++) {
                        //1st step: Finding minimum container position on ship that needs to be unloaded
                        if (lastPort) {
                            z = startingHeightMat[x][y];
                        } else {
                            z = findMinContainerPosToUnload(startingHeightMat, cargoMat, curSeaPortCode, height, x, y);
                        }
                        // 2nd step: Preparing all containers above to be unloaded and marking the ones to be reloaded
                        containersToUnload = collectingPotentialContainersToLoad(lastPort, containersToLoad, cargoMat,
                                                                                 seaPortCodeStr, height, x, y, z);

                        // 3rd step: Checking whether the unload operations can be executed, and if so - executing them
                        checkIfUnloadPossible(shipPlan, containersToUnload, shipRoute, x, y, &outputFile);
                        containersToUnload.clear();
                    }
                }
                if (lastPort && !containerVec.empty()) {
                    log("Last port. Cargo won't be loaded to Ship.", MessageSeverity::WARNING); //#TODO:change severity
                    return true;
                }
                for (const auto &container : containerVec) {
                    containersToLoad.push_back(container);
                }
                checkIfLoadPossible(shipPlan, containersToLoad, &outputFile, shipRoute);
                return true;
            }

            catch (const std::exception &e) {
                return false;
            }
        }
