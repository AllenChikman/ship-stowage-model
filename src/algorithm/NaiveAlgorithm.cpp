#include <vector>
#include <string>
#include <sstream>
#include "filesystem"

#include "NaiveAlgorithm.h"
#include "../common/Utils.h"


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
    WeightBalanceCalculator::BalanceStatus status = shipPlan->getBalanceCalculator().tryOperation(op,
                                                                                                  container.getWeight(),
                                                                                                  cord.x, cord.y);
    return status == WeightBalanceCalculator::BalanceStatus::APPROVED;
}

/*
 * Write algorithm results to instruction file
 */

char getCraneCmdChar(Crane::Command cmd)
{
    switch (cmd)
    {
        case Crane::Command::UNLOAD:
            return 'U';
        case Crane::Command::LOAD:
            return 'L';
        case Crane::Command::REJECT:
            return 'R';
        case Crane::Command::MOVE:
            return 'M';
        default:
            //unreachable code
            return 'X';
    }

}

void dumpInstruction(std::ofstream &outputStream, const Container &container, const Crane::Command &cmd,
                     const XYCord &xyCord)
{
    auto id = container.getID();
    char op = getCraneCmdChar(cmd);
    int x = (cmd == Crane::Command::REJECT) ? -1 : (int) xyCord.x;
    int y = (cmd == Crane::Command::REJECT) ? -1 : (int) xyCord.y;
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
}

void dumpInstruction(std::ofstream &outputStream, const string &containerLine)
{
    //only rejected containers
    int x = -1;
    int y = -1;
    auto reject = " R ";
    outputStream << reject << CSV_DELIM
                 << containerLine << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
}

/* getInstructionsForCargo
 * first part:
 * unloading cargo
 */

//step1:
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

//step2:
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

//step3:
void NaiveAlgorithm::Unloading(vector<Container> &containersToUnload,
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
        if (shipUnbalanced)
        {
            cmd = Crane::Command::REJECT;
        }
        else
        {
            cmd = Crane::Command::UNLOAD;
            Crane::performUnload(shipPlan, xyCord);
        }
        dumpInstruction(outputFile, container, cmd, xyCord);
    }
}

/* getInstructionsForCargo
 * second part:
 * loading cargo
 */

//step1:
XYCord findFreeXYCordsOnShipToLoad(const std::shared_ptr<ShipPlan> &shipPlan)
{
    /*
    * this is the function finding the free cell of Algorithm1
    * */

    const auto shipXYCords = shipPlan->getShipXYCordsVec();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

    unsigned numOfFloors;

    for (XYCord xyCord: shipXYCords)
    {
        numOfFloors = shipPlan->getNumOfFloors(xyCord);
        if (upperCellsMat[xyCord] < numOfFloors)
        {
            return xyCord;
        }
    }

    //Unreachable code
    return XYCord{0, 0};
}


XYCord findLowestFreeXYCord(const std::shared_ptr<ShipPlan> &shipPlan)
{
    /*
     * this is the function finding the free cell of Algorithm2
     * */

    const auto shipXYCords = shipPlan->getShipXYCordsVec();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

    XYCord bestCord{0, 0};
    unsigned lowestHeight = shipPlan->getMaxHeight();
    unsigned availableFloor;

    for (XYCord xyCord: shipXYCords)
    {
        availableFloor = upperCellsMat[xyCord];
        if (availableFloor < lowestHeight)
        {
            lowestHeight = availableFloor;
            bestCord = xyCord;
        }
    }

    return bestCord;


}

XYCord NaiveAlgorithm::chooseXYCordByAlgorithmType(const std::shared_ptr<ShipPlan> &shipPlan)
{
    return (useSecondAlgorithm) ?
           findLowestFreeXYCord(shipPlan) :
           findFreeXYCordsOnShipToLoad(shipPlan);
}

//step2:
void NaiveAlgorithm::Loading(vector<Container> &containersToLoad,
                             std::ofstream &outputFile)
{
    for (const auto &curContainerToLoad : containersToLoad)
    {
        bool shipUnbalanced;
        Crane::Command cmd;
        XYCord xyCord = {0, 0};
        shipUnbalanced = !isBalanced(shipPlan, 'L', curContainerToLoad); //optional
        if (validator.validateShipFull(shipPlan) || shipUnbalanced)
        {
            cmd = Crane::Command::REJECT;
        }
        else
        {
            cmd = Crane::Command::LOAD;
            xyCord = chooseXYCordByAlgorithmType(shipPlan);
            Crane::performLoad(shipPlan, curContainerToLoad, xyCord);
        }
        dumpInstruction(outputFile, curContainerToLoad, cmd, xyCord);
    }
}


//// Ex2



void clearDuplicatedContainers(vector<Container> &portContainers, std::ofstream &outputFile)
{
    for (unsigned i = 0; i < portContainers.size() - 1; i++)
    {
        auto j = i + 1;
        while (j < portContainers.size())
        {
            if (portContainers[i].getID() == portContainers[j].getID())
            {
                dumpInstruction(outputFile, portContainers[j], Crane::Command::REJECT, XYCord{0,0});
                portContainers.erase(portContainers.begin() + j);
                continue;
            }
            j++;
        }
    }
}

void clearDuplicatedContainers(vector<Container> &containers, std::shared_ptr<ShipPlan> &shipPlan)
{
    auto xyCords = shipPlan->getShipXYCordsVec();
    unsigned pos = 0;
    while (pos < containers.size())
    {
        for (auto &xyCord : xyCords)
        {
            auto maxFloor = shipPlan->getUpperCellsMat()[xyCord];
            for (unsigned floor = 0; floor < maxFloor; floor++)
            {
                if (shipPlan->getCargo()[xyCord][floor]->getID() == containers[pos].getID())
                {
                    containers.erase(containers.begin() + pos);
                    continue;
                }
                pos++;
            }
        }
    }
}

// private header functions

int NaiveAlgorithm::parseInputToContainersVec(vector<Container> &ContainersVec, const string &inputPath,
                                              std::ofstream &outputStream)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(inputPath, vecLines))
    {
        validator.errorHandle.reportError(Errors::containerFileCannotBeRead);
        return validator.getErrorBits();
    }
    for (const auto &lineVec : vecLines)
    {
        if (validator.validateContainerFromFile(lineVec, travelRouteStack, true))
        {
            ContainersVec.emplace_back(Container(lineVec));
        }
        else
        {
            string containerLine = " ";
            for (auto &word : lineVec)
            {
                containerLine += word;
            }
            dumpInstruction(outputStream, containerLine);
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

    if(vecLines.empty())
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
    shipPlan = std::make_shared<ShipPlan>(width, length, maximalHeight, WeightBalanceCalculator());
    CargoMat &cargoMat = shipPlan->getCargo();

    if(!validator.validateDuplicateXYCordsWithDifferentData(vecLines))
    {
        return validator.getErrorBits();
    }

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
    validator.clear();
    vector<string> vec;
    if (!readToVec(path, vec))
    {
        validator.errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
        return validator.getErrorBits();
    }

    vector<SeaPortCode> routeVec;
    if (!validator.validateAmountOfValidPorts(vec))
    {
        return validator.getErrorBits();
    }
    if (!validator.validateSamePortInstancesConsecutively(vec))
    {
        clearDuplicatedConsecutiveStrings(vec);
    }
    for (const auto &portSymbol : vec)
    {
        if (validator.validatePortFormat(portSymbol))
            routeVec.emplace_back(portSymbol);
    }
    travelRouteStack = vector<SeaPortCode>(routeVec.rbegin(), routeVec.rend());
    updateRouteMap();
    updateRouteFileSet(getDirectoryOfPath(path));

    return validator.getErrorBits();
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator)
{
    (void) calculator;
    return 0;
}

int NaiveAlgorithm::getInstructionsForCargo(const std::string &inputFilePath,
                                            const std::string &outputFilePath)
{
    validator.clear();
    std::ofstream outputFile(outputFilePath);

    const bool cargoFileExists = popRouteFileSet(inputFilePath);
    vector<Container> portContainers;
    if (cargoFileExists)
    {
        validator.validateContainerAtLastPort(inputFilePath, travelRouteStack);
        const auto errorCode = parseInputToContainersVec(portContainers, inputFilePath, outputFile);
        const auto isFatal = (errorCode & Errors::containerFileCannotBeRead) != 0;
        if (isFatal) { validator.getErrorBits(); }
    }

    if (!validator.validateDuplicateIDOnPort(portContainers))
    {
        clearDuplicatedContainers(portContainers, outputFile);
    }
    for (auto &container : portContainers)
    {
        string containerID = container.getID();
        if (!validator.validateDuplicateIDOnShip(containerID, shipPlan))
        {
            clearDuplicatedContainers(portContainers, shipPlan);
            break;
        }
    }

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
        Unloading(containersToUnload, cord, outputFile);
        containersToUnload.clear();
    }

    sortPortContainersByShipRoute(portContainers, travelRouteStack, containersToLoad);
    for (auto &portContainer : portContainers)
    {
        containersToLoad.push_back(portContainer);
    }
    Loading(containersToLoad, outputFile);
    travelRouteStack.pop_back();

    outputFile.close();
    return validator.getErrorBits();

}
