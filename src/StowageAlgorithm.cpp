
#include <vector>
#include <string>
#include "StowageAlgorithm.h"
#include "Utils.h"
#include "Ship.h"
#include "WeightBalanceCalculator.h"


bool isShipFull(ShipPlan *shipPlan)
{
    const unsigned maxHeight = shipPlan->getHeight();
    const auto firstCellAvailableMat = shipPlan->getFirstAvailableCellMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (XYCord cord: shipXYCordVec)
    {
        if (firstCellAvailableMat[cord] < maxHeight)
        {
            return false;
        }
    }
    return true;

}

bool isContainerDestPortInRoute(const std::vector<SeaPortCode> &shipRoute, const Container &container)
{
    for (auto &port : shipRoute)
    {
        if (container.isBelongToPort(port)) { return true; }
    }
    return false;
}

bool isBalanced(ShipPlan *shipPlan, char op, const Container &container, XYCord cord = {0, 0})
{
    balanceStatus status = shipPlan->getBalanceCalculator().tryOperation(shipPlan, op, container.getWeight(), cord);
    return status == balanceStatus::APPROVED;
}

bool rejectContainer(ShipPlan *shipPlan, char op, const Container &container, const std::vector<SeaPortCode> &shipRoute)
{
    const bool validID = container.isValidID();
    const bool legalDestPort = !isContainerDestPortInRoute(shipRoute, container);
    const bool legalLoading = !((op == 'L') && isShipFull(shipPlan));

    if (!validID) { log("Invalid container ID", MessageSeverity::WARNING); }
    if (!legalDestPort) { log("Container destination port is not in route", MessageSeverity::WARNING); }
    if (!legalLoading) { log("Ship full - unable to load container", MessageSeverity::WARNING); }

    const bool res = validID && legalDestPort && legalLoading;

    if (!res) {{ log("Container " + container.getID() + " Rejected!", MessageSeverity::WARNING); }}
    return res;
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

unsigned findMinContainerPosToUnload(const UIntMat &startingHeightMat, const CargoMat &cargoMat,
                                     const SeaPortCode &curSeaPortCode, unsigned ShipMaxHeight, XYCord xyCord)
{

    unsigned z = startingHeightMat[xyCord];

    while (z < ShipMaxHeight && cargoMat[xyCord][z])
    {
        const Container &curContainer = *cargoMat[xyCord][z];
        if (curContainer.isBelongToPort(curSeaPortCode)) { return z; }
        z++;
    }
    return ShipMaxHeight;
}

std::vector<Container> collectingPotentialContainersToLoad(bool lastPort,
                                                           std::vector<Container> &containersToLoad,
                                                           CargoMat cargoMat, const std::string &seaPortCodeStr,
                                                           const unsigned ShipMaxHeight, XYCord xyCord, unsigned z)
{
    std::vector<Container> containersToUnload;
    while (z < ShipMaxHeight && cargoMat[xyCord][z])
    {
        Container curContainer = *cargoMat[xyCord][z];
        if (seaPortCodeStr != curContainer.getDestinationPort().toStr() && !lastPort)
        {
            containersToLoad.push_back(curContainer);
        }
        containersToUnload.insert(containersToUnload.begin(), *cargoMat[xyCord][z]);
        z++;
    }
    return containersToUnload;
}

void checkIfUnloadPossible(ShipPlan *shipPlan, std::vector<Container> &containersToUnload,
                           const std::vector<SeaPortCode> &shipRoute,
                           XYCord xyCord, std::ofstream *outputFile)
{
    bool shipUnbalanced = false;
    CraneCommand cmd;

    for (const auto &container: containersToUnload)
    {
        if (!isBalanced(shipPlan, 'U', container, xyCord))
        {
            shipUnbalanced = true;
        }
// Checking if the operation should be rejected
        if (rejectContainer(shipPlan, 'U', container, shipRoute) || shipUnbalanced)
        {
            cmd = CraneCommand::REJECT;
        }
        else
        {
            cmd = CraneCommand::UNLOAD;
        }
        updateShipPlan(container, *outputFile, shipPlan, cmd, xyCord);
    }
}

void checkIfLoadPossible(ShipPlan *shipPlan, std::vector<Container> &containersToLoad,
                         std::ofstream *outputFile, const std::vector<SeaPortCode> &shipRoute)
{
    for (const auto &curContainerToLoad : containersToLoad)
    {
        bool shipUnbalanced;
        CraneCommand cmd;
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad); //optional
        if (rejectContainer(shipPlan, 'L', curContainerToLoad, shipRoute) || shipUnbalanced)
        {
            cmd = CraneCommand::REJECT;
        }
        else
        {
            cmd = CraneCommand::LOAD;
        }
        updateShipPlan(curContainerToLoad, *outputFile, shipPlan, cmd);
    }
}

bool getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath, ShipPlan *shipPlan,
                             const SeaPortCode &curSeaPortCode, const std::vector<SeaPortCode> &shipRoute,
                             bool isLastPortVisit)
{

    try
    {
        std::vector<Container> containerVec;
        if (!parseInputToContainersVec(containerVec, inputPath)) { return false; };

        std::ofstream outputFile;
        outputFile.open(outputPath, std::ios::out);
        std::vector<Container> containersToUnload, containersToLoad;

        const auto startingHeightMat = shipPlan->getStartingHeight();
        auto availableCells = shipPlan->getFirstAvailableCellMat();
        auto cargoMat = shipPlan->getCargo();

        const unsigned height = shipPlan->getHeight();
        const auto &seaPortCodeStr = curSeaPortCode.toStr();
        const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

        unsigned z;
        for (XYCord cord : shipXYCordVec)
        {
            //1st step: Finding minimum container position on ship that needs to be unloaded
            if (isLastPortVisit)
            {
                z = (availableCells[cord] > startingHeightMat[cord]) ? availableCells[cord] - 1 : 0;
            }
            else
            {
                z = findMinContainerPosToUnload(startingHeightMat, cargoMat, curSeaPortCode, height, cord);
            }
            // 2nd step: Preparing all containers above to be unloaded and marking the ones to be reloaded
            containersToUnload = collectingPotentialContainersToLoad(isLastPortVisit, containersToLoad, cargoMat,
                                                                     seaPortCodeStr, height, cord, z);

            // 3rd step: Checking whether the unload operations can be executed, and if so - executing them
            checkIfUnloadPossible(shipPlan, containersToUnload, shipRoute, cord, &outputFile);
            containersToUnload.clear();
        }
        if (isLastPortVisit && !containerVec.empty())
        {
            log("Last port. Cargo won't be loaded to Ship.", MessageSeverity::WARNING); //#TODO:change severity
            return true;
        }
        for (const auto &container : containerVec)
        {
            containersToLoad.push_back(container);
        }
        checkIfLoadPossible(shipPlan, containersToLoad, &outputFile, shipRoute);
        return true;
    }

    catch (const std::exception &e)
    {
        return false;
    }
}
