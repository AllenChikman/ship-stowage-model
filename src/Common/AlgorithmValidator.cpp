//
// Created by orbar on 17/05/2020.
//

#include "AlgorithmValidator.h"
#include <iostream>
#include <string>
#include "vector"
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "Utils.h"


bool checkShipPlanLineFormat(const std::vector<std::string> &line) {
    bool valid = true;
    if (line.size() != 3) {
        valid = false;
    }
    for (auto &word : line) {
        try {
            std::stoul(word);
        }
        catch (std::invalid_argument const &e) {
            valid = false;
        }
    }
    return valid;
}

bool AlgorithmValidator::validateShipHeightInput(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors) {
    std::ostringstream msg;
    bool valid = true;

    if (numOfFloors >= maximalHeight) {
        msg << "Number of floors is not smaller than the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid) {
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::floorsExceedMaxHeight);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y) {
    std::ostringstream msg;
    bool valid = true;
    if (x >= width || y >= length) {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    if (!valid) {
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::posExceedsXYLimits);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipPlanFloorsFormat(const std::vector<string> &shipFloor) {
    std::ostringstream msg;
    bool valid = checkShipPlanLineFormat(shipFloor);

    if (!valid) {
        errorHandle.reportError(Errors::badLineFormatAfterFirstLine);
        msg << "Bad line format after first line";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipPlanFirstLine(std::vector<string> &firstFloor) {
    std::ostringstream msg;
    bool valid = true;
    valid = checkShipPlanLineFormat(firstFloor);
    if (!valid) {
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        msg << "Fatal Error: Bad first line format";
        log(msg.str(), MessageSeverity::ERROR);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateReadingShipPlanFileAltogether(const string &shipPlanFilePath) {
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(shipPlanFilePath.c_str());

    // Check if object is valid
    if (!in) {
        msg << "Cannot open the File: " + shipPlanFilePath;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateSamePortInstancesConsecutively(const std::vector<string> &routeVec) {
    std::ostringstream msg;


    for (size_t i = 1; i < routeVec.size(); i++) {
        auto prevPort = routeVec[i - 1];
        auto port = routeVec[i];

        if (port == prevPort) {
            msg << port + " appears twice or more consecutively.";
            log(msg.str(), MessageSeverity::WARNING);
            errorHandle.reportError(Errors::portAppearsMoreThanOnceConsecutively);
            return false;
        }

    }
    return true;
}

bool AlgorithmValidator::validatePortFormat(const string &port) {
    std::ostringstream msg;
    bool valid = SeaPortCode::isValidCode(port);
    if (!valid) {
        msg << "Bad port symbol format.";
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::wrongSeaPortCode);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateOpenReadShipRouteFileAltogether(const string &shipRouteFilePath) {
    std::ostringstream msg;
    bool valid = true;

    // Open the File
    std::ifstream in;
    in.open(shipRouteFilePath.c_str());

    // Check if object is valid
    valid = !!in;

    string line;
    std::vector<string> vec;

    // Read the next line from File until it reaches the end.
    while (std::getline(in, line)) {
        // Line contains string of length > 0 then save it in std::vector
        if (!line.empty() && line[0] != '#') {
            strCleanWhitespaces(line);
            vec.push_back(line);
        }
    }
    valid = vec.empty();
    in.close();

    if (!valid) {
        msg << "Cannot open the File: " + shipRouteFilePath;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
        return false;
    }

    return true;
}

bool AlgorithmValidator::validateAmountOfValidPorts(const vector<string> &routeVec) {
    std::ostringstream msg;
    int validPorts = 0;
    for (auto &port : routeVec) {
        if (SeaPortCode::isValidCode(port)) {
            validPorts++;
        }
    }
    if (validPorts <= 1) {
        msg << "File with only a single valid port.";
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::atMostOneValidPort);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateDuplicateIDOnPort(const std::vector<Container> &containersAtPort) {
    std::ostringstream msg;
    std::unordered_map<string, unsigned> idMap = {};
    for (auto &container : containersAtPort) {
        if (idMap.find(container.getID()) == idMap.end()) {
            idMap.insert(make_pair(container.getID(), 1));
        } else {
            idMap[container.getID()]++;
        }
    }
    for (auto &temp_id : idMap) {
        auto id = temp_id.first;
        if (idMap[id] > 1) {
            msg << "Duplicated ID on port.";
            log(msg.str(), MessageSeverity::WARNING);
            errorHandle.reportError(Errors::duplicateIDOnPort);
            //#TODO: reject container in output file
            return false;
        }
    }

    return true;
}

bool AlgorithmValidator::validateDuplicateIDOnShip(const string &containerID, const std::shared_ptr<ShipPlan> &shipPlan) {
    std::ostringstream msg;
    auto xyCords = shipPlan->getShipXYCordsVec();
    for (auto xyCord : xyCords)
    {
        auto maxFloor = shipPlan->getUpperCellsMat()[xyCord];
        for (unsigned floor = 0; floor < maxFloor; floor++)
        {
            auto shipContainer = shipPlan->getCargo()[xyCord][floor];
            if (shipContainer->getID() == containerID)
            {
                msg << "Containers at port: ID already on ship ";
                log(msg.str(), MessageSeverity::Reject);
                errorHandle.reportError(Errors::duplicateIDOnShip);
                return false;
            }
        }
    }
    return true;
}

bool AlgorithmValidator::validateContainerID(const string &id) {
    // id doesn't exist
    std::ostringstream msg;
    size_t properIDLen = 11;
    if (id.size() != properIDLen) {
        msg << "Containers at port: bad line format, ID cannot be read.";
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::ContainerIDCannotBeRead);
        return false;
    }

    // id exists but has wrong format
    Container tmpContainer(id, 0, SeaPortCode("null"));  //TODO :consult with Allen
    if (!tmpContainer.isValidID()) {
        msg << "Containers at port: illegal ID check ISO 6346";
        log(msg.str(), MessageSeverity::Reject);
        errorHandle.reportError(Errors::badContainerID);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateContainerWeight(const string &weight) {
    std::ostringstream msg;
    for (char i : weight) {
        if (!isdigit(i)) {
            msg << "Containers at port: bad line format, missing or bad weight.";
            log(msg.str(), MessageSeverity::Reject);
            errorHandle.reportError(Errors::badContainerWeight);
            return false;
        }
    }
    return true;
}

bool AlgorithmValidator::validateContainerDestPort(const string &destPort, const vector<SeaPortCode> &travelRouteStack) {
    std::ostringstream msg;
    auto curSeaPortCode = travelRouteStack.back();
    auto destPortCode = SeaPortCode(destPort);
    if (SeaPortCode::isValidCode(destPortCode.toStr())) {
        for (auto &port : travelRouteStack) {
            if (port.toStr() == destPortCode.toStr() && (port.toStr() != curSeaPortCode.toStr())) { return true; }
        }

    }
    msg << "Containers at port: bad line format, missing or bad port dest.";
    log(msg.str(), MessageSeverity::Reject);
    errorHandle.reportError(Errors::badContainerDestPort);
    return false;
}

bool AlgorithmValidator::validateContainerFromFile(const std::vector<string> &line,
                                                   const std::vector<SeaPortCode> &travelRouteStack) {
    /* valid line should be at length 3 where
 * line[0] - container Id (string)
 * line[1] - numOf.. (unsigned int)
 * line[2] - destination port (string)
 * */

    auto lineSize = line.size();
    string id, weight, destPort;
    switch (lineSize) {
        case 0:
            //unreachable code
            return false;
        case 1:
            id = line[0];
            validateContainerID(id);
            errorHandle.reportError(Errors::badContainerWeight);
            errorHandle.reportError(Errors::badContainerDestPort);
            return false;
        case 2:
            id = line[0];
            weight = line[1];
            validateContainerID(id);
            validateContainerWeight(weight);
            errorHandle.reportError(Errors::badContainerDestPort);
            return false;
        default:
            id = line[0];
            weight = line[1];
            destPort = line[2];
            return validateContainerID(id) &&
                   validateContainerWeight(weight) &&
                   validateContainerDestPort(destPort, travelRouteStack);
    }

}

bool AlgorithmValidator::validateReadingContainingFileAltogether(const string &containersAtPortFile) {
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(containersAtPortFile.c_str());

    // Check if object is valid
    if (!in) {
        msg << "Cannot open the File: " << containersAtPortFile;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::containerFileCannotBeRead);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateContainerAtLastPort(const std::string &shipRouteFilePath) {
    (void) shipRouteFilePath;
    return false;
}

bool AlgorithmValidator::validateShipFull(const std::shared_ptr<ShipPlan> &shipPlan) {
    std::ostringstream msg;
    const auto upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (XYCord cord: shipXYCordVec) {
        unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
        if (upperCellsMat[cord] < numOfFloors) { return false; }
    }
    msg << "Containers at port: total containers amount exceeds ship capacity.";
    log(msg.str(), MessageSeverity::Reject);
    errorHandle.reportError(Errors::containersExceedShipCapacity);
    return true;
}

