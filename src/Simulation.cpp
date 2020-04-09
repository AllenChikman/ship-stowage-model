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
                           unsigned x, unsigned y, unsigned numOfFloors) {

    std::ostringstream msg;
    bool valid = true;

    if (x > width || y > length) {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    } else if (numOfFloors >= maximalHeight) {
        msg << "Number of floors is not smaller then the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid) { log(msg.str() , MessageSeverity:: WARNING); }
    return valid;
}

bool validateShipRouteFile(const std::vector<std::string> &vec) {
    for (const auto &portSymbol : vec) {
        if (!SeaPortCode::isSeaportCode(portSymbol)) {
            return false;
        }
    }
    return true;
}

// Simulation class method implementation

void Simulation::readShipPlan(const std::string &path) {

    try {

        std::vector<std::vector<std::string>> vecLines;
        readToVecLine(path, vecLines);
        auto shipPlanData = vecLines[0];

        unsigned maximalHeight = stringToUInt(shipPlanData[0]);
        unsigned width = stringToUInt(shipPlanData[1]);
        unsigned length = stringToUInt(shipPlanData[2]);

        unsigned x;
        unsigned y;
        unsigned numOfFloors;

        vecLines.erase(vecLines.begin());
        UIntMat startingHeightsMat(width, std::vector<unsigned>(length, 0));

        for (const auto &vecLine : vecLines) {
            x = stringToUInt(vecLine[0]);
            y = stringToUInt(vecLine[1]);
            numOfFloors = stringToUInt(vecLine[2]);
            if (!validateShipPlanEntry(width, length, maximalHeight, x, y, numOfFloors)) {
                continue; //invalid input entry is ignored
            }
            startingHeightsMat[x][y] = maximalHeight - numOfFloors;
        }

        shipPlan = new ShipPlan(width, length, maximalHeight, startingHeightsMat);
    }

    catch (const std::exception &e) {
        log("Failed to read the file: " + path, MessageSeverity::ERROR);
    }
}

void Simulation::readShipRoute(const std::string &path) {

    std::vector<std::string> vec;
    readToVec(path, vec);

    std::vector<SeaPortCode> routeVec;
    if (validateShipRouteFile(vec)) {
        for (const auto &portSymbol : vec) {
            routeVec.emplace_back(portSymbol);
        }
    }

    shipRoute = routeVec;
}

void Simulation::startTravel(const std::string &travelName) {

    initTravel(travelName);

    std::string currPortFileName;
    std::string currInputPath;
    std::string currOutputPath;
    std::string portStr;
    int numOfVisits;

    for (const SeaPortCode &port : shipRoute) {
        portStr = port.toStr();
        numOfVisits = (visitedPorts.find(port.toStr()) == visitedPorts.end()) ? 0 : visitedPorts[portStr];
        visitedPorts[portStr] = ++numOfVisits;
        std::tie(currInputPath , currOutputPath) = getPortFilePaths(currPortFileName, port, numOfVisits);
        getInstructionsForCargo(currInputPath , currOutputPath);
        // performInstructionsForCargo(currOutputPath);
    }

}

std::pair<std::string, std::string> Simulation::getPortFilePaths
        (const std::string &curPortFileName, const SeaPortCode &port, int numOfVisits) {

    std::string fileName = port.toStr() + "_" + std::to_string(numOfVisits);
    std::string inputPath = curTravelFolder + '/' + fileName + ".cargo_data";
    std::string outputPath = curTravelFolder + '/' + fileName + ".out_cargo_data";
    return std::make_pair(inputPath, outputPath);

}




