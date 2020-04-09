#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Simulation.h"
#include <algorithm>
#include <string>

// prints

void log(const std::string &message, MessageSeverity severity = MessageSeverity::INFO,
         std::ostream &outputStream = std::cout) {
    std::string severityStr;
    switch (severity) {
        case MessageSeverity::WARNING :
            severityStr = "Warning";
            break;
        case MessageSeverity::ERROR :
            severityStr = "Error";
            break;
        default:
            severityStr = "Info";
    }

    outputStream << severityStr << ": " << message << std::endl;

}

// Static functions for parsing purposes

unsigned stringToUInt(const std::string &str) {
    // TODO: check validity
    return static_cast<unsigned int>(std::stoi(str));
}

void strCleanWhitespaces(std::string &str) {

    str.erase(std::remove_if(str.begin(),
                             str.end(),
                             [](unsigned char x) { return std::isspace(x); }),
              str.end());
}

std::vector<std::string> splitByDelimiter(const std::string &line, char delimiter) {
    std::vector<std::string> res;

    std::stringstream ss(line);

    while (ss.good()) {
        std::string substr;
        getline(ss, substr, delimiter);
        res.push_back(substr);
    }

    return res;
}

std::vector<std::string> splitByWhiteSpace(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(tokens));
    return tokens;
}

CargoData lineToCargoData(const std::string &line) {


}

// Reader functions as to guided format

bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines) {

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in) {
        std::cerr << "Cannot open the File : " << path << std::endl;
        return false;
    }

    std::string line;
    // Read the next line from File untill it reaches the end.
    while (std::getline(in, line)) {
        // Line contains string of length > 0 then save it in vector
        if (!line.empty() && line[0] != '#') {
            strCleanWhitespaces(line);
            vecLines.push_back(splitByDelimiter(line, ','));
        }
    }
    in.close();
    return true;
}

bool readToVec(const std::string &path, std::vector<std::string> &vec) {

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in) {
        std::cerr << "Cannot open the File : " << path << std::endl;
        return false;
    }

    std::string line;
    // Read the next line from File until it reaches the end.
    while (std::getline(in, line)) {
        // Line contains string of length > 0 then save it in vector
        if (!line.empty() && line[0] != '#') {
            strCleanWhitespaces(line);
            vec.push_back(line);
        }
    }
    in.close();
    return true;
}

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

    if (!valid) { log(msg.str()); }
    return valid;
}

bool validateShipRouteFile(const std::vector<std::string> &vec) {
    for (const auto &portSymbol : vec){
        if(!SeaPortCode::isSeaportCode(portSymbol)){
            return false;
        }
    }
    return true;
}

bool validateInstructionsForCargo() {

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
        UIntMat startingHeightsMat(width, vector<unsigned>(length, 0));

        for (const auto &vecLine : vecLines) {
            x = stringToUInt(vecLine[0]);
            y = stringToUInt(vecLine[1]);
            numOfFloors = stringToUInt(vecLine[2]);
            if (!validateShipPlanEntry(width, length, maximalHeight, x, y, numOfFloors)) {
                continue; //invalid input entry is ignored
            }
            startingHeightsMat[x][y] = maximalHeight - numOfFloors;
        }

        ShipPlan shipPlan(width, length, maximalHeight, startingHeightsMat);
    }

    catch (const std::exception &e) {
        log("Failed to read the file: " + path);
    }
}

void Simulation::readShipRoute(const std::string &path) {

    std::vector<std::string> vec;
    readToVec(path, vec);
    validateShipRouteFile(vec);

    vector<SeaPortCode> shipRoute;
    for(const auto &portSymbol : vec){
        shipRoute.emplace_back(portSymbol);
    }

}



