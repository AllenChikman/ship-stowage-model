
#include <vector>
#include <string>
#include "StowageAlgorithm.h"
#include "Utils.h"
#include "Ship.h"
#include "WeightBalanceCalculator.h"


bool isShipFull(ShipPlan *shipPlan)
{
    const auto upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (XYCord cord: shipXYCordVec)
    {
        unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
        if (upperCellsMat[cord] < numOfFloors) { return false; }
    }
    return true;

}

bool isContainerDestPortInRoute(const vector<SeaPortCode> &shipRoute, const Container &container)
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

bool rejectContainer(ShipPlan *shipPlan, char op, const Container &container, const vector<SeaPortCode> &shipRoute)
{
    const bool validID = container.isValidID();
    const bool legalDestPort = isContainerDestPortInRoute(shipRoute, container);
    const bool legalLoading = !((op == 'L') && isShipFull(shipPlan));

    if (!validID) { log("Invalid container ID", MessageSeverity::WARNING); }
    if (!legalDestPort) { log("Container destination port is not in route", MessageSeverity::WARNING); }
    if (!legalLoading) { log("Ship full - unable to load container", MessageSeverity::WARNING); }

    const bool isInvalid = !validID || !legalDestPort || !legalLoading;

    if (isInvalid) {{ log("Container " + container.getID() + " Rejected!", MessageSeverity::WARNING); }}
    return isInvalid;
}

bool parseInputToContainersVec(vector<Container> &ContainersVec, const string &inputPath)
{
    try
    {
        vector<vector<string>> vecLines;
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

unsigned findMinContainerPosToUnload(const CargoMat &cargoMat, const SeaPortCode &curSeaPortCode,
                                     unsigned ShipMaxHeight, XYCord xyCord)
{

    unsigned z = 0;
    while (z < ShipMaxHeight && cargoMat[xyCord][z])
    {
        const Container &curContainer = *cargoMat[xyCord][z];
        if (curContainer.isBelongToPort(curSeaPortCode)) { return z; }
        z++;
    }
    return ShipMaxHeight;
}

void fillVecsToLoadUnload(bool lastPort,vector<Container> &containersToUnload,
                          vector<Container> &containersToLoad,
                          CargoMat &cargoMat,
                          const string &seaPortCodeStr,
                          const unsigned ShipMaxHeight,XYCord xyCord, unsigned z)
{
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
}

void Unloading(ShipPlan *shipPlan, vector<Container> &containersToUnload,
               const vector<SeaPortCode> &shipRoute,
               XYCord xyCord, std::ofstream &outputFile)
{
    bool shipUnbalanced = false;
    Crane::Command cmd;

    for (const auto &container: containersToUnload)
    {
        if (!isBalanced(shipPlan, 'U', container, xyCord))
        {
            shipUnbalanced = true;
        }
// Checking if the operation should be rejected
        if (rejectContainer(shipPlan, 'U', container, shipRoute) || shipUnbalanced)
        {
            cmd = Crane::Command::REJECT;
        }
        else
        {
            cmd = Crane::Command::UNLOAD;
        }
        Crane::updateShipPlan(outputFile, shipPlan, container, cmd, xyCord);
    }
}

void Loading(ShipPlan *shipPlan, vector<Container> &containersToLoad,
             std::ofstream &outputFile, const vector<SeaPortCode> &shipRoute)
{
    for (const auto &curContainerToLoad : containersToLoad)
    {
        bool shipUnbalanced;
        Crane::Command cmd;
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad); //optional
        if (rejectContainer(shipPlan, 'L', curContainerToLoad, shipRoute) || shipUnbalanced)
        {
            cmd = Crane::Command::REJECT;
        }
        else
        {
            cmd = Crane::Command::LOAD;
        }
        Crane::updateShipPlan(outputFile, shipPlan, curContainerToLoad, cmd);
    }
}

bool getInstructionsForCargo(const string &inputPath, const string &outputPath, ShipPlan *shipPlan,
                             const SeaPortCode &curSeaPortCode, const vector<SeaPortCode> &shipRoute,
                             bool isLastPortVisit)
{

    try
    {
        vector<Container> portContainers;
        if (!isLastPortVisit)
        {
            if (!parseInputToContainersVec(portContainers, inputPath)) { return false; };

        }


        std::ofstream outputFile;
        outputFile.open(outputPath, std::ios::out);
        vector<Container> containersToLoad;
        vector<Container> containersToUnload;

        UIntMat &availableCells = shipPlan->getUpperCellsMat();
        CargoMat &cargoMat = shipPlan->getCargo();

        const unsigned height = shipPlan->getMaxHeight();
        const auto &seaPortCodeStr = curSeaPortCode.toStr();
        const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

        unsigned z = 0;
        for (const XYCord cord : shipXYCordVec)
        {
            //1st step: Finding minimum container position on ship that needs to be unloaded
            if (!isLastPortVisit)
            {
                z = findMinContainerPosToUnload(cargoMat, curSeaPortCode, height, cord);
            }
            // 2nd step: Preparing all containers above to be unloaded and marking the ones to be reloaded
            fillVecsToLoadUnload(isLastPortVisit, containersToUnload, containersToLoad, cargoMat,
                                 seaPortCodeStr, height, cord, z);

            // 3rd step: Checking whether the unload operations can be executed, and if so - executing them
            Unloading(shipPlan, containersToUnload, shipRoute, cord, outputFile);
            containersToUnload.clear();
        }

        for (const auto &container : portContainers)
        {
            containersToLoad.push_back(container);
        }
        Loading(shipPlan, containersToLoad, outputFile, shipRoute);
        return true;
    }

    catch (const std::exception &e)
    {
        return false;
    }
}
