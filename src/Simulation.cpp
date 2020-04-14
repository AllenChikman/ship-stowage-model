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

// File input validators

bool validateShipPlanEntry(unsigned width, unsigned length, unsigned maximalHeight,
                           unsigned x, unsigned y, unsigned numOfFloors)
{

    std::ostringstream msg;
    bool valid = true;

    if (x > width || y > length)
    {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    else if (numOfFloors >= maximalHeight)
    {
        msg << "Number of floors is not smaller then the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid) { log(msg.str(), MessageSeverity::WARNING); }
    return valid;
}

bool validateShipRouteFile(const std::vector<std::string> &vec)
{
    for (const auto &portSymbol : vec)
    {
        if (!SeaPortCode::isValidCode(portSymbol))
        {
            return false;
        }
    }
    return true;
}


// Simulation private class method implementation

std::pair<std::string, std::string> Simulation::getPortFilePaths
        (const std::string &curPortFileName, const SeaPortCode &port, int numOfVisits)
{

    std::string fileName = port.toStr() + "_" + std::to_string(numOfVisits);
    std::string inputPath = curTravelFolder + '/' + fileName + ".cargo_data";
    std::string outputPath = curTravelFolder + "/output/" + fileName + ".out_cargo_data";
    return std::make_pair(inputPath, outputPath);

}

void Simulation::createOutputDirectory()
{
    createDirIfNotExists(curTravelFolder + "/output");
}

bool Simulation::initTravel(const std::string &travelDir)
{
    logStartingDecorator();
    log("Initializing travel...");

    visitedPorts = {};
    curTravelFolder = travelDir;
    log("Travel Root Folder is: " + curTravelFolder);
    createOutputDirectory();
    log("Output Folder is: " + curTravelFolder + "/output");

    bool isSuccessful = true;
    isSuccessful &= readShipPlan(getShipPlanFilePath());
    isSuccessful &= readShipRoute(getRouteFilePath());
    return isSuccessful;
}

// Simulation public class method implementation

bool Simulation::readShipPlan(const std::string &path)
{
    try
    {
        std::vector<std::vector<std::string>> vecLines;
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
        UIntMat startingHeightsMat(width, std::vector<unsigned>(length, 0));

        for (const auto &vecLine : vecLines)
        {
            x = stringToUInt(vecLine[0]);
            y = stringToUInt(vecLine[1]);
            numOfFloors = stringToUInt(vecLine[2]);
            if (!validateShipPlanEntry(width, length, maximalHeight, x, y, numOfFloors))
            {
                continue; //invalid input entry is ignored
            }
            startingHeightsMat[x][y] = maximalHeight - numOfFloors;
        }

        shipPlan = new ShipPlan(width, length, maximalHeight, startingHeightsMat);
        return true;
    }

    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship plan", MessageSeverity::ERROR);
        return false;
    }
}

bool Simulation::readShipRoute(const std::string &path)
{
    try
    {
        std::vector<std::string> vec;
        if (!readToVec(path, vec)) { throw std::runtime_error(""); }

        std::vector<SeaPortCode> routeVec;
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
        return true;
    }
    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship route", MessageSeverity::ERROR);
        return false;
    }
}

bool Simulation::startTravel(const std::string &travelDir)
{
    if (!initTravel(travelDir))
    {
        log("Failed to init the travel - Ignoring this travel simulation " , MessageSeverity::ERROR);
        return false;
    }

    std::string currPortFileName;
    std::string currInputPath;
    std::string currOutputPath;
    std::string portStr;
    int numOfVisits;

    for (SeaPortCode port : shipRoute)
    {
        portStr = port.toStr();
        numOfVisits = (visitedPorts.find(port.toStr()) == visitedPorts.end()) ? 0 : visitedPorts[portStr];
        visitedPorts[portStr] = ++numOfVisits;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(currPortFileName, port, numOfVisits);
        try
        {
            getInstructionsForCargo(currInputPath, currOutputPath, shipPlan, &port);
        }
        catch (const std::exception &e)
        {
            log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
        }
    }
    return true;
}

void Simulation::runAlgorithm()
{
    std::vector<std::string> travelDirPaths;
    putDirListToVec(rootFolder, travelDirPaths);
    for (const auto &travelFolder :travelDirPaths)
    {
        if(startTravel(travelFolder)){
            log("Travel Finished Successfully!!!");
        }
    }

}
