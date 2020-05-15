#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#include "Utils.h"
#include "Simulation.h"
#include "StowageAlgorithm.h"
#include "filesystem"
#include "AbstractAlgorithm.h"


// Simulation private class method implementation

std::pair<string, string> Simulation::getPortFilePaths(const SeaPortCode &port, int numOfVisits)
{
    const string &inputFileName = port.toStr(true);
    const string &outputFileName = port.toStr();

    const string middlePart = "_" + std::to_string(numOfVisits);

    string inputPath = curTravelFolder + '/' + inputFileName + middlePart + ".cargo_data";
    string outputPath = curTravelFolder + "/output/" + outputFileName + middlePart + ".out_cargo_data";

    return std::make_pair(inputPath, outputPath);

}

void Simulation::createOutputDirectory()
{
    createDirIfNotExists(curTravelFolder + "/output");
}

void Simulation::updateRouteMap()
{
    for (const auto &port : shipRoute)
    {
        const string &portStr = port.toStr();
        routeMap[portStr] = (routeMap.find(portStr) == routeMap.end()) ? 1 : routeMap[portStr] + 1;
    }
}

void Simulation::updateRouteFileSet()
{
    vector<string> cargoFilesVec;
    putDirFileListToVec(curTravelFolder, cargoFilesVec, ".cargo_data");
    for (const auto &path : cargoFilesVec)
    {
        cargoFilesSet.insert(path);
    }
}

bool Simulation::popRouteFileSet(const string &currInputPath)
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

void Simulation::ValidateAllCargoFilesWereUsed()
{
    for (const auto &file : cargoFilesSet)
    {
        log("File: " + file + " was not used and ignored (not in route or too many files for port)",
            MessageSeverity::WARNING);
    }
}

bool Simulation::initTravel(const string &travelDir)
{
    logStartingDecorator();
    log("Initializing travel...");

    visitedPorts.clear();
    cargoFilesSet.clear();
    routeMap.clear();

    curTravelFolder = travelDir;
    log("Travel Root Folder is: " + curTravelFolder);
    createOutputDirectory();
    log("Output Folder is: " + curTravelFolder + "/output");

    bool isSuccessful = true;
    isSuccessful &= readShipPlan(getShipPlanFilePath());
    isSuccessful &= readShipRoute(getRouteFilePath());
    return isSuccessful;
}


// File input validators

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


// Simulation public class method implementation

bool Simulation::readShipPlan(const string &path)
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

bool Simulation::readShipRoute(const string &path)
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
        shipRoute = routeVec;
        updateRouteMap();
        updateRouteFileSet();

        return true;
    }
    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship route", MessageSeverity::ERROR);
        return false;
    }
}


// TODO: this function is for reference
bool Simulation::startTravel(const string &travelDir)
{
    if (!initTravel(travelDir))
    {
        log("Failed to init the travel - Ignoring this travel simulation ", MessageSeverity::ERROR);
        return false;
    }

    string currInputPath;
    string currOutputPath;
    string portStr;
    unsigned numOfVisits;
    bool lastPortVisit;

    vector<SeaPortCode> routeTravelStack(shipRoute.rbegin(), shipRoute.rend());

    for (const SeaPortCode &port : shipRoute)
    {
        portStr = port.toStr();
        numOfVisits = (visitedPorts.find(port.toStr()) == visitedPorts.end()) ? 0 : visitedPorts[portStr];
        visitedPorts[portStr] = ++numOfVisits;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(port, numOfVisits);
        lastPortVisit = isLastPortVisit(port);
        const bool cargoFileExists = popRouteFileSet(currInputPath);
        if (cargoFileExists && lastPortVisit)
        {
            log("Last visited port should not have a file for it", MessageSeverity::WARNING);
        }

        if (!cargoFileExists && !lastPortVisit)
        {
            log("This port visit has no file for it (expected: " + currInputPath + "). Unloading Only",
                MessageSeverity::WARNING);
        }

        if (!getInstructionsForCargo(currInputPath, currOutputPath, shipPlan, port, routeTravelStack, cargoFileExists))
        {
            log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
        }

        routeTravelStack.pop_back();
    }

    ValidateAllCargoFilesWereUsed();

    return true;
}

void Simulation::runAlgorithm()
{
    vector<string> travelDirPaths;
    putDirFileListToVec(rootFolder, travelDirPaths);
    for (const auto &travelFolder :travelDirPaths)
    {
        if (startTravel(travelFolder))
        {
            log("Travel Finished Successfully!!!");
        }
    }

}

/////// EX2 Part

void validateIfAlgorithmSucceeded(int algorithmReturnFlag, const string &currInputPath)
{
    if (!algorithmReturnFlag)
    {
        log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
    }
}

void validateLastPort(const string &currInputPath, bool cargoFileExists, bool lastPortVisit)
{

    if (cargoFileExists && lastPortVisit)
    {
        log("Last visited port should not have a file for it", MessageSeverity::WARNING);
    }

    if (!cargoFileExists && !lastPortVisit)
    {
        log("This port visit has no file for it (expected: " + currInputPath + "). Unloading Only",
            MessageSeverity::WARNING);
    }

}


void Simulation::updateVisitedPortsMap(const SeaPortCode &port)
{
    const string &portStr = port.toStr();
    if (visitedPorts.find(port.toStr()) != visitedPorts.end())
    {
        visitedPorts[portStr]++;
    }
    else
    {
        visitedPorts[portStr] = 1;
    }
}


bool Simulation::performAndValidateAlgorithmInstructions(const string &outputDirPath)
{
    /* The simulator will check the following
     * legal output format
     * x,y legal coordinates
     * UNLOAD - legal pull in UNLOAD(from the top) using the container Id
     * LOAD - legal insert (at the top of the coordinate) using the container Id
     * Valid Container Id
     *
     * TODO: or - please fill the function
     *
     * */

    vector<vector<string>> vecLines;
    readToVecLine(outputDirPath, vecLines);
    int instructionCounter = 0;

    for (const auto &vecLine : vecLines)
    {
        //if(validateLine)
            // instructionCounter++;
            //performOperation

    }





    return true;
}


void Simulation::runAlgorithmTravelPair(const string &travelDirPath,
                                        AbstractAlgorithm &algorithm, const string &outputDirPath)
{

    // init simulator for this travel
    initTravel(travelDirPath);

    // init algorithm for this travel
    algorithm.readShipPlan(travelDirPath);
    algorithm.readShipRoute(travelDirPath);

    // get a stack (for popping logic of routes) of the shipRoute
    vector<SeaPortCode> routeTravelStack(shipRoute.rbegin(), shipRoute.rend());

    // traverse each port
    for (const SeaPortCode &port : shipRoute)
    {
        // update visited ports map
        updateVisitedPortsMap(port);

        // get the input and output for the algorithm "getInstructionsForCargo"
        string currInputPath, currOutputPath;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(port, visitedPorts[port.toStr()]);

        // set boolean variables
        const bool lastPortVisit = isLastPortVisit(port);
        const bool cargoFileExists = popRouteFileSet(currInputPath);

        // call algorithms' get instruction for cargo
        /*getInstructionsForCargo(currInputPath, currOutputPath, shipPlan, port, routeTravelStack,cargoFileExists);*/
        const int algorithmReturnFlag = algorithm.getInstructionsForCargo(currInputPath, currOutputPath);

        // validations regarding the last port and existence of expected file
        validateLastPort(currInputPath, cargoFileExists, lastPortVisit);

        // validate return code of the Algorithm
        validateIfAlgorithmSucceeded(algorithmReturnFlag, currInputPath);

        // Go through the instruction output of the algorithm and approve every move
        performAndValidateAlgorithmInstructions(outputDirPath);

        // pop the port we've been in from the left routes stack
        routeTravelStack.pop_back();
    }

    ValidateAllCargoFilesWereUsed();


}


