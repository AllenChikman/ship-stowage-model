
#include <vector>
#include <string>
#include <sstream>
#include "filesystem"
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
        auto &port = travelRouteStack[portPos - 1];
        size_t containerSize = portContainers.size();
        for (size_t i = 0; i < containerSize && !portContainers.empty(); i++)
        {
            if (portContainers[cur].getDestinationPort().toStr() == port.toStr())
            {
                containersToLoad.push_back(portContainers[cur]);
                portContainers.erase(portContainers.begin() + cur);
            }
            else
            {
                cur++;
            }
        }

    }
}

bool isBalanced(const std::shared_ptr<ShipPlan> &shipPlan, char op, const Container &container, XYCord cord = {0, 0})
{
    balanceStatus status = shipPlan->getBalanceCalculator().tryOperation(shipPlan, op, container.getWeight(), cord);
    return status == balanceStatus::APPROVED;
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

void fillVecToLoadReload(vector<Container> &containersToUnload,
                         vector<Container> &containersToReload,
                         CargoMat &cargoMat,
                         const SeaPortCode &port,
                         const unsigned numOfFloors, XYCord xyCord, unsigned z)
{
    while (z < numOfFloors && cargoMat[xyCord][z])
    {
        Container curContainer = *cargoMat[xyCord][z];
        if (!curContainer.isBelongToPort(port))
        {
            containersToReload.push_back(curContainer);
        }
        containersToUnload.insert(containersToUnload.begin(), *cargoMat[xyCord][z]);
        z++;
    }
}

void NaiveAlgorithm::Unloading(vector<Container> &containersToUnload,
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
        if (shipUnbalanced)
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

void NaiveAlgorithm::Loading(vector<Container> &containersToLoad,
             std::ofstream &outputFile, const SeaPortCode &curSeaPortCode)
{
    for (const auto &curContainerToLoad : containersToLoad)
    {
        bool shipUnbalanced;
        Crane::Command cmd;
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad); //optional
        if (validator.validateShipFull(shipPlan) || shipUnbalanced)
        {
            cmd = Crane::Command::REJECT;
        }
        else
        {
            cmd = Crane::Command::LOAD;
        }
        Crane::updateShipPlan(outputFile, shipPlan, curContainerToLoad, cmd, {0, 0});
    }
}


//// Ex2


void clearDuplicatedPorts(const vector<string> &vec)
{
    //TODO: Allen
}

void clearDuplicatedContainers(const vector<Container> &portContainers)
{
    //TODO: Allen
}

// private header functions
int NaiveAlgorithm::parseInputToContainersVec(vector<Container> &ContainersVec, const string &inputPath)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(inputPath, vecLines))
    {
        validator.errorHandle.reportError(Errors::containerFileCannotBeRead);
        return validator.getErrorBits();
    }
    for (const auto &lineVec : vecLines)
    {
        if (validator.validateContainerFromFile(lineVec, travelRouteStack))
        {
            ContainersVec.emplace_back(lineVec[0], stringToUInt(lineVec[1]), SeaPortCode(lineVec[2]));
        }
    }

    return validator.getErrorBits();
}

bool NaiveAlgorithm::popRouteFileSet(const string &currInputPath)
{
    string pathToPop;
    std::filesystem::path p2 = currInputPath;
    for (const auto &path : cargoFilesSet)
    {
        std::filesystem::path p1 = path;
        if (std::filesystem::equivalent(p1, p2))
        {
            pathToPop = path;
            break;
        }
    }
    if (!pathToPop.empty())
    {
        cargoFilesSet.erase(pathToPop);
        return true;
    }
    return false;
}

void NaiveAlgorithm::updateRouteMap()
{
    for (const auto &port : travelRouteStack)
    {
        const string &portStr = port.toStr();
        routeMap[portStr] = (routeMap.find(portStr) == routeMap.end()) ? 1 : routeMap[portStr] + 1;
    }
}

void NaiveAlgorithm::updateRouteFileSet(const string &curTravelFolder)
{
    vector<string> cargoFilesVec;
    putDirFileListToVec(curTravelFolder, cargoFilesVec, ".cargo_data");
    for (const auto &path : cargoFilesVec)
    {
        cargoFilesSet.insert(path);
    }
}


// API Functions


int NaiveAlgorithm::readShipPlan(const std::string &path)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(path, vecLines))
    {
        validator.errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return validator.getErrorBits();
    }
    auto shipPlanData = vecLines[0];

    if (!validator.validateShipPlanFirstLine(shipPlanData))
    {
        return validator.getErrorBits();
    }

    unsigned maximalHeight = stringToUInt(shipPlanData[0]);
    unsigned width = stringToUInt(shipPlanData[1]);
    unsigned length = stringToUInt(shipPlanData[2]);

    unsigned x;
    unsigned y;
    unsigned numOfFloors;

    vecLines.erase(vecLines.begin());
    shipPlan = std::make_shared<ShipPlan>(width, length, maximalHeight, ShipWeightBalanceCalculator(APPROVED));
    CargoMat &cargoMat = shipPlan->getCargo();

    for (const auto &vecLine : vecLines)
    {
        if (!validator.validateShipPlanFloorsFormat(vecLine)) { continue; }
        x = stringToUInt(vecLine[0]);
        y = stringToUInt(vecLine[1]);
        numOfFloors = stringToUInt(vecLine[2]);

        if (validator.validateShipHeightInput(maximalHeight, x, y, numOfFloors) &&
            validator.validateShipXYCords(width, length, x, y))
        {
            cargoMat[x][y].resize(numOfFloors);
        }
    }

    return validator.getErrorBits();

}

int NaiveAlgorithm::readShipRoute(const std::string &path)
{
    vector<string> vec;
    if (!readToVec(path, vec))
    {
        validator.errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
        return validator.getErrorBits();
    }

    vector<SeaPortCode> routeVec;
    if (validator.validateAmountOfValidPorts(vec))
    {
        for (const auto &portSymbol : vec)
        {
            if (validator.validatePortFormat(portSymbol))
                routeVec.emplace_back(portSymbol);
        }
    }
    if (validator.validateSamePortInstancesConsecutively(vec))
    {
        clearDuplicatedPorts(vec);
    }
    travelRouteStack = vector<SeaPortCode>(routeVec.rbegin(), routeVec.rend());
    updateRouteMap();
    updateRouteFileSet(getDirectoryOfPath(path));

    return validator.getErrorBits();
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator)
{
    return 0;
}

int NaiveAlgorithm::getInstructionsForCargo(const std::string &inputFilePath,
                                            const std::string &outputFilePath)
{
    const bool cargoFileExists = popRouteFileSet(inputFilePath);
    vector<Container> portContainers;
    if (cargoFileExists)
    {
        validator.validateContainerAtLastPort(inputFilePath);

        const auto errorCode = parseInputToContainersVec(portContainers, inputFilePath);
        const auto isFatal = (errorCode & Errors::containerFileCannotBeRead) != 0;
        if (isFatal) { validator.getErrorBits(); }
    }

    if(!validator.validateDuplicateIDOnPort(portContainers))
    {
        //remove duplicates
    }
/*   // TODO: fix validator.validateDuplicateIDOnShip();
    {

    }*/
    std::ofstream outputFile;
    outputFile.open(outputFilePath, std::ios::out);

    SeaPortCode curSeaPortCode = travelRouteStack.back();
    vector<Container> containersToLoad;
    vector<Container> containersToUnload;

    CargoMat &cargoMat = shipPlan->getCargo();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    unsigned z = 0;
    for (const XYCord cord : shipXYCordVec)
    {

        // 1st step: Finding minimum container position on ship that needs to be unloaded
        unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
        z = findMinContainerPosToUnload(cargoMat, curSeaPortCode, numOfFloors, cord);

        // 2nd step: Preparing all containers above to be unloaded and marking the ones to be reloaded
        fillVecToLoadReload(containersToUnload, containersToLoad, cargoMat,
                            curSeaPortCode, numOfFloors, cord, z);

        // 3rd step: Checking whether the unload operations can be executed, and if so - executing them
        Unloading(containersToUnload, cord, outputFile, curSeaPortCode);
        containersToUnload.clear();
    }

    sortPortContainersByShipRoute(portContainers, travelRouteStack, containersToLoad);
    if (!portContainers.empty())
    {
        for (auto &portContainer : portContainers)
        {
            containersToLoad.push_back(portContainer);
        }
    }
    Loading(containersToLoad, outputFile, curSeaPortCode);
    travelRouteStack.pop_back();

    return validator.getErrorBits();

}
