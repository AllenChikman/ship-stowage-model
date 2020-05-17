
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

bool isShipFull(const std::shared_ptr<ShipPlan> &shipPlan)
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

bool isContainerDestPortInRoute(const vector<SeaPortCode> &travelRouteStack, const Container &container,
                                const SeaPortCode &curSeaPortCode)
{
    for (auto &port : travelRouteStack)
    {
        if (container.isBelongToPort(port) && (port.toStr() != curSeaPortCode.toStr())) { return true; }
    }
    return false;
}

bool isBalanced(const std::shared_ptr<ShipPlan> &shipPlan, char op, const Container &container, XYCord cord = {0, 0})
{
    balanceStatus status = shipPlan->getBalanceCalculator().tryOperation(shipPlan, op, container.getWeight(), cord);
    return status == balanceStatus::APPROVED;
}

bool rejectContainer(const std::shared_ptr<ShipPlan> &shipPlan, char op, const Container &container,
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
        Crane::updateShipPlan(outputFile, shipPlan, curContainerToLoad, cmd, {0, 0});
    }
}


//// Ex2


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

bool validateShipPlanEntry(unsigned width, unsigned length, unsigned maximalHeight,
                           unsigned x, unsigned y, unsigned numOfFloors)
{

    std::ostringstream msg;
    bool valid = true;

    if (x >= width || y >= length)
    {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    else if (numOfFloors >= maximalHeight)
    {
        msg << "Number of floors is not smaller than the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid) { log(msg.str(), MessageSeverity::WARNING); }
    return valid;
}

bool validateShipRouteFile(const vector<string> &vec)
{
    string prevSymbol;
    for (const auto &portSymbol : vec)
    {
        if (!SeaPortCode::isValidCode(portSymbol))
        {
            log("SeaPortCode: " + portSymbol + " is invalid!", MessageSeverity::ERROR);
            return false;
        }
        if (portSymbol == prevSymbol)
        {
            log("SeaPortCode: " + portSymbol + " appears twice in a row!", MessageSeverity::ERROR);
            return false;
        }
        prevSymbol = portSymbol;
    }
    return true;
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


int NaiveAlgorithm::getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                            const std::string &output_full_path_and_file_name)
{
    const bool cargoFileExists = popRouteFileSet(input_full_path_and_file_name);
    try
    {
        vector<Container> portContainers;
        if (cargoFileExists)
        {
            if (!parseInputToContainersVec(portContainers, input_full_path_and_file_name)) { return -1; };
        }


        SeaPortCode curSeaPortCode = travelRouteStack.back();

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
        if (!portContainers.empty())
        {
            for (auto &portContainer : portContainers)
            {
                containersToLoad.push_back(portContainer);
            }
        }
        Loading(shipPlan, containersToLoad, outputFile, travelRouteStack, curSeaPortCode);
        travelRouteStack.pop_back();
        return (int) true;
    }

    catch (const std::exception &e)
    {
        return (false);
    }
}

int NaiveAlgorithm::readShipPlan(const std::string &path)
{
    try
    {
        vector<vector<string>> vecLines;
        if (!readToVecLine(path, vecLines))
        {
            throw std::runtime_error("");
        }
        auto shipPlanData = vecLines[0];

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
            x = stringToUInt(vecLine[0]);
            y = stringToUInt(vecLine[1]);
            numOfFloors = stringToUInt(vecLine[2]);
            if (validateShipPlanEntry(width, length, maximalHeight, x, y, numOfFloors))
            {
                cargoMat[x][y].resize(numOfFloors);
            }
            // else: invalid input entry is ignored
        }

        return true;
    }

    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship plan", MessageSeverity::ERROR);
        return false;
    }
}

int NaiveAlgorithm::readShipRoute(const std::string &path)
{
    try
    {
        vector<string> vec;
        if (!readToVec(path, vec)) { throw std::runtime_error(""); }

        vector<SeaPortCode> routeVec;
        if (validateShipRouteFile(vec))
        {
            for (const auto &portSymbol : vec)
            {
                routeVec.emplace_back(portSymbol);
            }
        }
        else
        {
            throw std::runtime_error("");
        }
        travelRouteStack = vector<SeaPortCode>(routeVec.rbegin(), routeVec.rend());
        updateRouteMap();
        updateRouteFileSet(getDirectoryOfPath(path));

        return true;
    }
    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship route", MessageSeverity::ERROR);
        return false;
    }
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator)
{
    return 0;
}

