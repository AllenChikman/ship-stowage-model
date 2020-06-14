#include <vector>
#include <string>
#include <sstream>
#include "filesystem"

#include "GoodAlgorithm.h"
#include "../common/Utils.h"

// Dumping to instruction File

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

void dumpMoveInstruction(std::ofstream &outputStream, const Container &container,
                         const XYCord &fromXYCord, int fromFloorIdx,
                         const XYCord &toXYCord, int toFloorIdx)
{
    auto id = container.getID();
    char op = getCraneCmdChar(Crane::Command::MOVE);
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << fromFloorIdx << CSV_DELIM
                 << fromXYCord.x << CSV_DELIM
                 << fromXYCord.y << CSV_DELIM
                 << toFloorIdx << CSV_DELIM
                 << toXYCord.x << CSV_DELIM
                 << toXYCord.y << std::endl;
}

void dumpInstruction(std::ofstream &outputStream, const Container &container, const Crane::Command &cmd,
                     const XYCord &xyCord, int floorIdx)
{
    auto id = container.getID();
    char op = getCraneCmdChar(cmd);
    int x = (cmd == Crane::Command::REJECT) ? -1 : (int) xyCord.x;
    int y = (cmd == Crane::Command::REJECT) ? -1 : (int) xyCord.y;
    outputStream << op << CSV_DELIM
                 << id << CSV_DELIM
                 << floorIdx << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
}

void dumpRejectInstruction(std::ofstream &outputStream, const string &containerId)
{
    //only rejected containers

    int x = -1;
    int y = -1;
    int floorIdx = -1;
    auto reject = "R";
    outputStream << reject << CSV_DELIM
                 << containerId << CSV_DELIM
                 << floorIdx << CSV_DELIM
                 << x << CSV_DELIM
                 << y << std::endl;
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
                dumpInstruction(outputFile, portContainers[j], Crane::Command::REJECT, XYCord{0, 0}, -1);
                portContainers.erase(portContainers.begin() + j);
                continue;
            }
            j++;
        }
    }
}

void clearDuplicatedContainers(vector<Container> &containers, std::ofstream &outputFile , std::shared_ptr<ShipPlan> &shipPlan)
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
                    dumpInstruction(outputFile, containers[pos], Crane::Command::REJECT, XYCord{0, 0}, -1);
                    containers.erase(containers.begin() + pos);
                    continue;
                }
                pos++;
            }
        }
    }
}


//// Ex3

// helper functions

int getPortDistance(const vector<SeaPortCode> &travelRouteStack, const SeaPortCode &destPort)
{
    int i = 0;

    const vector<SeaPortCode> &routeVec = vector<SeaPortCode>(travelRouteStack.rbegin(), travelRouteStack.rend());
    for (auto const &port : routeVec)
    {
        if (port.toStr() == destPort.toStr())
        {
            return i;
        }
        i++;
    }

    return -1; //unreachable code
}


unsigned getNumberOfShipFreeCells(const std::shared_ptr<ShipPlan> &shipPlan)
{
    unsigned sum = 0;
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (const XYCord cord : shipXYCordVec)
    {
        sum += shipPlan->getNumOfFloors(cord) - upperCellsMat[cord];
    }

    return sum;
}


unsigned getNumberOfContainersToBeUnloaded(const std::shared_ptr<ShipPlan> &shipPlan, const SeaPortCode &destPort)
{
    unsigned res = 0;
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();
    const CargoMat &cargoMat = shipPlan->getCargo();

    for (const XYCord cord : shipXYCordVec)
    {
        for (unsigned floorIdx = 0; floorIdx < upperCellsMat[cord]; floorIdx++)
        {
            const Container &curContainer = *cargoMat[cord][floorIdx];
            if (curContainer.getDestinationPort().toStr() == destPort.toStr())
            {
                res++;
            }
        }
    }

    return res;
}


void sortContainersVec(const vector<SeaPortCode> &travelRouteStack,
                       vector<Container> &portContainers, bool reverse = false)
{
    // sort the vector
    std::sort(portContainers.begin(), portContainers.end(),
              [&travelRouteStack](Container &left, Container &right)
              {
                  return getPortDistance(travelRouteStack, left.getDestinationPort()) <
                         getPortDistance(travelRouteStack, right.getDestinationPort());
              });

    if (reverse)
    {
        std::reverse(portContainers.begin(), portContainers.end());
    }

}


unsigned findLowestContainerToUnload(const CargoMat &cargoMat, const SeaPortCode &curSeaPortCode,
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


XYCord findLowestFreeXYCord(const std::shared_ptr<ShipPlan> &shipPlan)
{
    /* this is the function finding the free cell of Algorithm2  */

    const auto shipXYCords = shipPlan->getShipXYCordsVec();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();

    XYCord bestCord{0, 0};
    unsigned bestFreeFloors = 0;

    for (XYCord xyCord: shipXYCords)
    {
        const unsigned cordFreeFloors = shipPlan->getNumOfFloors(xyCord) - upperCellsMat[xyCord];
        if (cordFreeFloors > bestFreeFloors)
        {
            bestFreeFloors = cordFreeFloors;
            bestCord = xyCord;
        }
    }

    return bestCord;


}

// private header functions

int GoodAlgorithm::parseInputToContainersVec(vector<Container> &ContainersVec, const string &inputPath,
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
        else if ((validator.getErrorBits() & Errors::ContainerIDCannotBeRead) == 0)
        {
            dumpRejectInstruction(outputStream, lineVec[0]);
        }
    }

    return validator.getErrorBits();
}

bool GoodAlgorithm::popRouteFileSet(const string &currInputPath)
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

void GoodAlgorithm::updateRouteMap()
{
    for (const auto &port : travelRouteStack)
    {
        const string &portStr = port.toStr();
        routeMap[portStr] = (routeMap.find(portStr) == routeMap.end()) ? 1 : routeMap[portStr] + 1;
    }
}

void GoodAlgorithm::updateRouteFileSet(const string &curTravelFolder)
{
    vector<string> cargoFilesVec;
    putDirFileListToVec(curTravelFolder, cargoFilesVec, ".cargo_data");
    for (const auto &path : cargoFilesVec)
    {
        cargoFilesSet.insert(path);
    }
}

void GoodAlgorithm::cleanAndRejectFarContainers(std::ofstream &outputFile, vector<Container> &portContainers)
{
    const int numOfFreeCells = static_cast<int>(getNumberOfShipFreeCells(shipPlan));
    const int numberOfContainersToUnload = static_cast<int>(getNumberOfContainersToBeUnloaded(shipPlan,
                                                                                              travelRouteStack.back()));
    const int numberOfContainersToLoad = static_cast<int>(portContainers.size());
    const int numOfElementsToRemove = numberOfContainersToLoad - (numOfFreeCells + numberOfContainersToUnload);

    if (numOfElementsToRemove <= 0) { return; }

    sortContainersVec(travelRouteStack, portContainers);
    for (int i = 0; i < numOfElementsToRemove; i++)
    {
        auto curContainer = portContainers.back();
        dumpRejectInstruction(outputFile, curContainer.getID());
        portContainers.pop_back();
    }
}


bool GoodAlgorithm::getBestCordForLoading(XYCord &currCord, bool excludeCurCord)
{
    SeaPortCode curSeaPortCode = travelRouteStack.back();
    CargoMat &cargoMat = shipPlan->getCargo();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    XYCord bestCord{};
    int bestMinimalDistance = -1;

    for (const XYCord cord : shipXYCordVec)
    {
        if (excludeCurCord && cord.x == currCord.x && cord.y == currCord.y) { continue; } // not the original cord

        if (upperCellsMat[cord] == shipPlan->getNumOfFloors(cord)) { continue; } // if there is place in this cord

        int cordMinimalDistance = static_cast<int>(travelRouteStack.size() + 1);
        for (unsigned int floorIdx = 0; floorIdx < upperCellsMat[cord]; floorIdx++)
        {
            Container &curContainer = *cargoMat[cord][floorIdx];
            int containerDistance = getPortDistance(travelRouteStack, curContainer.getDestinationPort());
            if (containerDistance < cordMinimalDistance)
            {
                cordMinimalDistance = containerDistance;
            }

        }

        if (cordMinimalDistance > bestMinimalDistance)
        {
            bestCord = cord;
            bestMinimalDistance = cordMinimalDistance;
        }

    }

    currCord = bestCord;
    return bestMinimalDistance != -1;
}


bool GoodAlgorithm::performInstructionsValidations(const std::string &inputFilePath,
                                                   std::ofstream &outputFile, vector<Container> &portContainers)
{
    // if cargo files exists we:
    // 1) validate if it the last port and not empty
    // 2) parse the containers to vector

    const bool cargoFileExists = popRouteFileSet(inputFilePath);
    if (cargoFileExists)
    {
        validator.validateContainerAtLastPort(inputFilePath, travelRouteStack);
        const auto errorCode = parseInputToContainersVec(portContainers, inputFilePath, outputFile);
        const auto isFatal = (errorCode & Errors::containerFileCannotBeRead) != 0;
        if (isFatal) { return false; }
    }

    // validate if there are duplicate container Ids on port, and if so clear the redundant containers and reject them
    if (!validator.validateDuplicateIDOnPort(portContainers))
    {
        clearDuplicatedContainers(portContainers, outputFile);
    }

    // validate if there are duplicate Ids on the ship, and if so clear the redundant containers and reject them
    for (auto &container : portContainers)
    {
        string containerID = container.getID();
        if (!validator.validateDuplicateIDOnShip(containerID, shipPlan))
        {
            clearDuplicatedContainers(portContainers, outputFile, shipPlan);
            break;
        }
    }


    // validate ship full (based on number of free cells vs containersVec size)
    unsigned numberOfFreeCells = getNumberOfShipFreeCells(shipPlan);
    const auto curDestPort = travelRouteStack.back();
    unsigned numberOfContainersToBeUnloaded = getNumberOfContainersToBeUnloaded(shipPlan, curDestPort);


    validator.validateShipFull(numberOfFreeCells, numberOfContainersToBeUnloaded, portContainers);
    cleanAndRejectFarContainers(outputFile, portContainers);

    return true;
}


// API Functions


int GoodAlgorithm::readShipPlan(const std::string &path)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(path, vecLines))
    {
        validator.errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return validator.getErrorBits();
    }

    if (vecLines.empty())
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

    if (!validator.validateDuplicateXYCordsWithDifferentData(vecLines))
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

int GoodAlgorithm::readShipRoute(const std::string &path)
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

int GoodAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator)
{
    (void) calculator;
    return 0;
}


int GoodAlgorithm::getInstructionsForCargo(const std::string &inputFilePath,
                                           const std::string &outputFilePath)
{
    validator.clear();
    std::ofstream outputFile(outputFilePath);
    SeaPortCode curSeaPortCode = travelRouteStack.back();
    vector<Container> containersToLoad;

    // perform all instruction validations ahead
    bool isFatalError = !performInstructionsValidations(inputFilePath, outputFile, containersToLoad);
    if (isFatalError) { return validator.getErrorBits(); }

    // Unloading
    unloadAndMoveContainers(outputFile, containersToLoad);

    // Loading
    loadContainers(outputFile, containersToLoad);

    travelRouteStack.pop_back();
    outputFile.close();
    return validator.getErrorBits();

}


/// unloading

void GoodAlgorithm::unloadAndMoveContainers(std::ofstream &outputFile, vector<Container> &containerToLoad)
{
    SeaPortCode curSeaPortCode = travelRouteStack.back();
    CargoMat &cargoMat = shipPlan->getCargo();
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (const XYCord cord : shipXYCordVec)
    {
        unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
        unsigned minFloorToUnload = findLowestContainerToUnload(cargoMat, curSeaPortCode, numOfFloors, cord);
        unsigned upperContainerHeight = upperCellsMat[cord] - 1; // we assume we have at least 1 container

        if (minFloorToUnload == numOfFloors) { continue; } // No containers to unload from this cord

        while (minFloorToUnload <= upperContainerHeight)
        {

            Container &upperContainer = *cargoMat[cord][upperContainerHeight];
            int floorHeightOffset = shipPlan->getMaxHeight() - shipPlan->getNumOfFloors(cord);
            XYCord newMoveCord = cord;

            // if container belongs to this port we simply unload it
            if (upperContainer.getDestinationPort().toStr() == curSeaPortCode.toStr())
            {
                dumpInstruction(outputFile, upperContainer, Crane::UNLOAD, cord,
                                floorHeightOffset + upperContainerHeight);

                Crane::performUnload(shipPlan, cord);
            }

                // if we find a place to move the container we move it there
            else if (getBestCordForLoading(newMoveCord, true))
            {
                int newFloorHeightOffset = shipPlan->getMaxHeight() - shipPlan->getNumOfFloors(newMoveCord);

                dumpMoveInstruction(outputFile, upperContainer,
                                    cord, floorHeightOffset + upperContainerHeight,
                                    newMoveCord, newFloorHeightOffset + upperCellsMat[newMoveCord]);

                Crane::performMove(shipPlan, cord, newMoveCord);

            }

                // else, no place to move the container. we have to unload it, and remember to reload it later
            else
            {
                dumpInstruction(outputFile, upperContainer, Crane::UNLOAD, cord,
                                floorHeightOffset + upperContainerHeight);
                Crane::performUnload(shipPlan, cord);
                containerToLoad.push_back(upperContainer);

            }

            if (upperContainerHeight == 0)
            {
                break; // deal with negative unsigned number problems
            }
            upperContainerHeight--;
        }

    }

}


/// loading

void GoodAlgorithm::loadContainers(std::ofstream &outputFile, vector<Container> &containerToLoad)
{
    sortContainersVec(travelRouteStack, containerToLoad, true);
    UIntMat &upperCellsMat = shipPlan->getUpperCellsMat();
    XYCord cordToLoad{};

    for (auto const &container : containerToLoad)
    {
        if (useSecondAlgorithm)
        {
            cordToLoad = findLowestFreeXYCord(shipPlan);
        }
        else
        {
            getBestCordForLoading(cordToLoad);
        }
        int floorHeight = shipPlan->getMaxHeight() - shipPlan->getNumOfFloors(cordToLoad);
        dumpInstruction(outputFile, container, Crane::LOAD, cordToLoad, floorHeight + upperCellsMat[cordToLoad]);
        Crane::performLoad(shipPlan, container, cordToLoad);

    }
}
