#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Simulation.h"
#include <algorithm>
#include <string>

// Static functions for parsing purposes

unsigned stringToUInt(const std::string &str) {
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
    // Read the next line from File untill it reaches the end.
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

// Simulation class method implementation


void Simulation::readShipPlan(const std::string &path) {

    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(path, vecLines);
    auto shipPlanData = vecLines[0];


    unsigned maximalHeight = stringToUInt(shipPlanData[0]);
    unsigned width = stringToUInt(shipPlanData[1]);
    unsigned length = stringToUInt(shipPlanData[2]);

    unsigned y;
    unsigned x;
    unsigned startingHeight;

    vecLines.erase(vecLines.begin());
    UIntMat startingHeightsMat(width, vector<unsigned>(length, 0));

    for (const auto &vecLine : vecLines) {
        x = stringToUInt(vecLine[0]);
        y = stringToUInt(vecLine[1]);
        startingHeight = maximalHeight - stringToUInt(vecLine[2]);
        startingHeightsMat[x][y] = startingHeight;
    }

    ShipPlan shipPlan(width, length, maximalHeight , startingHeightsMat);

}

void Simulation::readShipRoute(const std::string &path) {

    std::vector<std::string> vec;
    readToVec(path, vec);
}

void Simulation::getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath) {
    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(inputPath, vecLines);
    std::vector<CargoData> cargoData;

    for (const auto &lineVec : vecLines) {
        cargoData.push_back({lineVec[0], stringToUInt(lineVec[1]), lineVec[2]});
    }

}

