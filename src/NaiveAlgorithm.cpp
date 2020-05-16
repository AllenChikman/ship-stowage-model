
#include <vector>
#include <string>
#include "NaiveAlgorithm.h"
#include "Utils.h"
#include "Ship.h"
#include "WeightBalanceCalculator.h"


void sortPortContainersByShipRoute(vector<Container> &portContainers, const vector<SeaPortCode> &travelRouteStack,
                                   vector<Container> &containersToLoad)
{
    unsigned cur;
    size_t routeSize = travelRouteStack.size();
    for (size_t portPos = routeSize; portPos > 0; portPos--)
    {
        cur = 0;
        auto &port = travelRouteStack[portPos-1];
        size_t containerSize = portContainers.size();
        for(size_t i = 0; i<containerSize && !portContainers.empty(); i++)
        {
            if(portContainers[cur].getDestinationPort().toStr() == port.toStr())
            {
                containersToLoad.push_back(portContainers[cur]);
                portContainers.erase(portContainers.begin() + cur);
            } else{
                cur++;
            }
        }

    }
}
bool isShipFull(const std::shared_ptr<ShipPlan>& shipPlan)
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

bool isContainerDestPortInRoute(const vector<SeaPortCode> &travelRouteStack, const Container &container, const SeaPortCode &curSeaPortCode)
{
    for (auto &port : travelRouteStack)
    {
        if (container.isBelongToPort(port) && (port.toStr() != curSeaPortCode.toStr())) { return true; }
    }
    return false;
}

bool isBalanced(const std::shared_ptr<ShipPlan>& shipPlan, char op, const Container &container, XYCord cord = {0, 0})
{
    balanceStatus status = shipPlan->getBalanceCalculator().tryOperation(shipPlan, op, container.getWeight(), cord);
    return status == balanceStatus::APPROVED;
}

bool rejectContainer(const std::shared_ptr<ShipPlan>& shipPlan, char op, const Container &container,
        const vector<SeaPortCode> &travelRouteStack, const SeaPortCode &curSeaPortCode)
{
    const bool validID = container.isValidID();
    const bool legalDestPort = (op != 'L') || isContainerDestPortInRoute(travelRouteStack, container, curSeaPortCode);
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
                                     unsigned numOfFloors, XYCord xyCord)
{

    unsigned z = 0;
    while (z < numOfFloors && cargoMat[xyCord][z])
    {
        const Container &curContainer = *cargoMat[xyCord][z];
        if (curContainer.isBelongToPort(curSeaPortCode)) { return z; }
        z++;
    }
    return numOfFloors;
}

void fillVecsToLoadReload(vector<Container> &containersToUnload,
                          vector<Container> &containersToReload,
                          CargoMat &cargoMat,
                          const SeaPortCode &port,
                          const unsigned numOfFloors,XYCord xyCord, unsigned z)
{
    while (z < numOfFloors && cargoMat[xyCord][z])
    {
        Container curContainer = *cargoMat[xyCord][z];
        if(!curContainer.isBelongToPort(port))
        {
            containersToReload.push_back(curContainer);
        }
        containersToUnload.insert(containersToUnload.begin(), *cargoMat[xyCord][z]);
        z++;
    }
}

void Unloading(const std::shared_ptr<ShipPlan> &shipPlan, vector<Container> &containersToUnload,
               const vector<SeaPortCode> &travelRouteStack,
               XYCord xyCord, std::ofstream &outputFile, const SeaPortCode &curSeaPortCode)
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
        if (rejectContainer(shipPlan, 'U', container, travelRouteStack, curSeaPortCode) || shipUnbalanced)
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

void Loading(const std::shared_ptr<ShipPlan> &shipPlan, vector<Container> &containersToLoad,
             std::ofstream &outputFile, const vector<SeaPortCode> &travelRouteStack, const SeaPortCode &curSeaPortCode)
{
    for (const auto &curContainerToLoad : containersToLoad)
    {
        bool shipUnbalanced;
        Crane::Command cmd;
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad); //optional
        if (rejectContainer(shipPlan, 'L', curContainerToLoad, travelRouteStack, curSeaPortCode) || shipUnbalanced)
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

int NaiveAlgorithm::getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                            const std::string &output_full_path_and_file_name)
{
   bool useInputFile;
    try
    {
        vector<Container> portContainers;
        if (useInputFile)
        {
            if (!parseInputToContainersVec(portContainers, input_full_path_and_file_name)) { return -1; };
        }


        std::ofstream outputFile;
        outputFile.open(output_full_path_and_file_name, std::ios::out);
        vector<Container> containersToLoad;
        vector<Container> containersToUnload;

        CargoMat &cargoMat = shipPlan->getCargo();

        const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

        unsigned z = 0;
        for (const XYCord cord : shipXYCordVec)
        {
            //1st step: Finding minimum container position on ship that needs to be unloaded
            unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
            z = findMinContainerPosToUnload(cargoMat, curSeaPortCode, numOfFloors, cord);
            // 2nd step: Preparing all containers above to be unloaded and marking the ones to be reloaded
            fillVecsToLoadReload(containersToUnload, containersToLoad, cargoMat,
                                 curSeaPortCode, numOfFloors, cord, z);

            // 3rd step: Checking whether the unload operations can be executed, and if so - executing them
            Unloading(shipPlan, containersToUnload, travelRouteStack, cord, outputFile, curSeaPortCode);
            containersToUnload.clear();
        }

        sortPortContainersByShipRoute(portContainers, travelRouteStack, containersToLoad);
        if(!portContainers.empty())
        {
            for(auto &portContainer : portContainers)
            {
                containersToLoad.push_back(portContainer);
            }
        }
        Loading(shipPlan, containersToLoad, outputFile, travelRouteStack, curSeaPortCode);
        return 0;
    }

    catch (const std::exception &e)
    {
        return -1;
    }
}

int NaiveAlgorithm::readShipPlan(const std::string &full_path_and_file_name) {
    return 0;
}

