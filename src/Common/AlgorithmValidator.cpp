//
// Created by orbar on 17/05/2020.
//

#include <iostream>
#include "vector"
#include <sstream>
#include <fstream>
#include <unordered_map>

#include "AlgorithmValidator.h"
#include "Utils.h"


bool checkShipPlanLineFormat(const std::vector<std::string> &line)
{
    bool valid = true;
    if (line.size() != 3)
    {
        valid = false;
    }
    for (auto &word : line)
    {
        try
        {
            std::stoul(word);
        }
        catch (std::invalid_argument const &e)
        {
            valid = false;
        }
    }
    return valid;
}

void AlgorithmValidator::filterInvalidVecLines(const vector<vector<string>> &vecLines, vector<vector<string>> &validVecLines)
{
    for(auto vecLine : vecLines)
    {
        if (!validateShipPlanFloorsFormat(vecLine)) { continue; }
        validVecLines.push_back(vecLine);
    }
}

bool AlgorithmValidator::validateShipHeightInput(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors)
{
    std::ostringstream msg;
    bool valid = true;

    if (numOfFloors >= maximalHeight)
    {
        msg << "Number of floors is not smaller than the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid)
    {
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::WARNING);
        }
        errorHandle.reportError(Errors::floorsExceedMaxHeight);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y)
{
    std::ostringstream msg;
    bool valid = true;
    if (x >= width || y >= length)
    {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    if (!valid)
    {
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::WARNING);
        }
        errorHandle.reportError(Errors::posExceedsXYLimits);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipPlanFloorsFormat(const std::vector<string> &shipFloor)
{
    std::ostringstream msg;
    bool valid = checkShipPlanLineFormat(shipFloor);

    if (!valid)
    {
        errorHandle.reportError(Errors::badLineFormatAfterFirstLine);
        msg << "Bad line format after first line";
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::WARNING);
        }
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateShipPlanFirstLine(std::vector<string> &firstFloor)
{
    std::ostringstream msg;
    bool valid = true;
    valid = checkShipPlanLineFormat(firstFloor);
    if (!valid)
    {
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        msg << "Fatal Error: Bad first line format";
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::ERROR);
        }
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateReadingShipPlanFileAltogether(const string &shipPlanFilePath)
{
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(shipPlanFilePath.c_str());

    // Check if object is valid
    if (!in)
    {
        msg << "Cannot open the File: " + shipPlanFilePath;
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::ERROR);
        }
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateDuplicateXYCordsWithDifferentData(const vector<vector<string>> &vecLines, vector<vector<string>> &validVecLines)
{
    std::ostringstream msg;
    unsigned x_i, x_j, y_i, y_j, numOfFloors_i, numOfFloors_j;
    filterInvalidVecLines(vecLines, validVecLines);
    for(unsigned i=0; i<vecLines.size()-1; i++)
    {
        x_i= stringToUInt(vecLines[i][0]);
        y_i = stringToUInt(vecLines[i][1]);
        numOfFloors_i = stringToUInt(vecLines[i][2]);
        for(unsigned j=i+1; j<vecLines.size(); j++)
        {
            x_j = stringToUInt(vecLines[j][0]);
            y_j = stringToUInt(vecLines[j][1]);
            numOfFloors_j = stringToUInt(vecLines[j][2]);
            if(x_i == x_j && y_i == y_j && numOfFloors_i != numOfFloors_j)
            {
                msg << " Ship plan: travel error - duplicate x,y appearance with different data.";
                if(userIsSimulator)
                {
                    log(msg.str(), MessageSeverity::ERROR);
                    errorHandle.reportError(Errors::duplicatedXYWithDifferentData);
                    return false;
                }
            }
        }
    }
    return true;
}

bool AlgorithmValidator::validateSamePortInstancesConsecutively(const std::vector<string> &routeVec)
{
    std::ostringstream msg;

    for (size_t i = 1; i < routeVec.size(); i++)
    {
        auto prevPort = routeVec[i - 1];
        auto port = routeVec[i];

        if (port == prevPort)
        {
            msg << port + " appears twice or more consecutively.";
            if(userIsSimulator)
            {
                log(msg.str(), MessageSeverity::WARNING);
            }
            errorHandle.reportError(Errors::portAppearsMoreThanOnceConsecutively);
            return false;
        }

    }
    return true;
}

bool AlgorithmValidator::validatePortFormat(const string &port)
{
    std::ostringstream msg;
    bool valid = SeaPortCode::isValidCode(port);
    if (!valid)
    {
        msg << "Bad port symbol format.";
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::WARNING);
        }
        errorHandle.reportError(Errors::wrongSeaPortCode);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateOpenReadShipRouteFileAltogether(const string &shipRouteFilePath)
{
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
    while (std::getline(in, line))
    {
        // Line contains string of length > 0 then save it in std::vector
        if (!line.empty() && line[0] != '#')
        {
            strCleanWhitespaces(line);
            vec.push_back(line);
        }
    }
    valid = vec.empty();
    in.close();

    if (!valid)
    {
        msg << "Cannot open the File: " + shipRouteFilePath;
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::ERROR);
        }
        errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
        return false;
    }

    return true;
}

bool AlgorithmValidator::validateAmountOfValidPorts(const vector<string> &routeVec)
{
    int validPorts = 0;
    for (auto &port : routeVec)
    {
        if (SeaPortCode::isValidCode(port))
        {
            validPorts++;
        }
    }

    switch (validPorts)
    {
        case 0:
            if(userIsSimulator)
            {
                log("File with no valid ports.", MessageSeverity::ERROR);
            }
            errorHandle.reportError(Errors::atMostOneValidPort);
            return false;

        case 1:
            if(userIsSimulator)
            {
                log("File with only a single valid port.", MessageSeverity::ERROR);
            }
            errorHandle.reportError(Errors::atMostOneValidPort);
            return false;

        default:
            return true;
    }
}

bool AlgorithmValidator::validateDuplicateIDOnPort(const std::vector<Container> &containersAtPort)
{
    std::ostringstream msg;
    std::unordered_map<string, unsigned> idMap = {};
    for (auto &container : containersAtPort)
    {
        if (idMap.find(container.getID()) == idMap.end())
        {
            idMap.insert(make_pair(container.getID(), 1));
        }
        else
        {
            idMap[container.getID()]++;
        }
    }
    for (auto &temp_id : idMap)
    {
        auto id = temp_id.first;
        if (idMap[id] > 1)
        {
            msg << "Duplicated ID on port.";
            if(userIsSimulator)
            {
                log(msg.str(), MessageSeverity::WARNING);
            }
            errorHandle.reportError(Errors::duplicateIDOnPort);
            return false;
        }
    }

    return true;
}

bool AlgorithmValidator::validateDuplicateIDOnShip(const string &containerID, const std::shared_ptr<ShipPlan> &shipPlan)
{
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
                if(userIsSimulator)
                {
                    log(msg.str(), MessageSeverity::Reject);
                }
                errorHandle.reportError(Errors::duplicateIDOnShip);
                return false;
            }
        }
    }
    return true;
}

bool AlgorithmValidator::validateContainerID(const string &id)
{
    // id doesn't exist
    std::ostringstream msg;
    size_t properIDLen = 11;
    if (id.size() != properIDLen)
    {
        msg << "Containers at port: bad line format, ID cannot be read.";
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::WARNING);
        }
        errorHandle.reportError(Errors::ContainerIDCannotBeRead);
        return false;
    }

    // id exists but has wrong format
    Container tmpContainer(id, 0, SeaPortCode("null"));  //TODO :consult with Allen
    if (!tmpContainer.isValidID())
    {
        msg << "Containers at port: illegal ID check ISO 6346";
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::Reject);
        }
        errorHandle.reportError(Errors::badContainerID);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateContainerWeight(const string &weight)
{
    std::ostringstream msg;
    for (char i : weight)
    {
        if (!isdigit(i))
        {
            msg << "Containers at port: bad line format, missing or bad weight.";
            if(userIsSimulator)
            {
                log(msg.str(), MessageSeverity::Reject);
            }
            errorHandle.reportError(Errors::badContainerWeight);
            return false;
        }
    }
    return true;
}

bool AlgorithmValidator::validateContainerDestPort(const string &destPort, const vector<SeaPortCode> &travelRouteStack)
{
    std::ostringstream msg;
    auto curSeaPortCode = travelRouteStack.back();
    auto destPortCode = SeaPortCode(destPort);
    if (SeaPortCode::isValidCode(destPortCode.toStr()))
    {
        for (auto &port : travelRouteStack)
        {
            if (port.toStr() == destPortCode.toStr() && (port.toStr() != curSeaPortCode.toStr())) { return true; }
        }

    }
    msg << "Containers at port: bad line format, missing or bad port dest.";
    if(userIsSimulator)
    {
        log(msg.str(), MessageSeverity::Reject);
    }
    errorHandle.reportError(Errors::badContainerDestPort);
    return false;
}

bool AlgorithmValidator::validateContainerFromFile(const std::vector<string> &line,
                                                   const std::vector<SeaPortCode> &travelRouteStack)
{
    /* valid line should be at length 3 where
 * line[0] - container Id (string)
 * line[1] - numOf.. (unsigned int)
 * line[2] - destination port (string)
 * */

    auto lineSize = line.size();
    string id, weight, destPort;
    switch (lineSize)
    {
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

bool AlgorithmValidator::validateReadingContainingFileAltogether(const string &containersAtPortFile)
{
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(containersAtPortFile.c_str());

    // Check if object is valid
    if (!in)
    {
        msg << "Cannot open the File: " << containersAtPortFile;
        if(userIsSimulator)
        {
            log(msg.str(), MessageSeverity::ERROR);
        }
        errorHandle.reportError(Errors::containerFileCannotBeRead);
        return false;
    }
    return true;
}

bool AlgorithmValidator::validateContainerAtLastPort(const std::string &shipRouteFilePath)
{
    (void) shipRouteFilePath;
    return false;
}

bool AlgorithmValidator::validateShipFull(const std::shared_ptr<ShipPlan> &shipPlan)
{
    std::ostringstream msg;
    const auto upperCellsMat = shipPlan->getUpperCellsMat();
    const auto shipXYCordVec = shipPlan->getShipXYCordsVec();

    for (XYCord cord: shipXYCordVec)
    {
        unsigned numOfFloors = shipPlan->getNumOfFloors(cord);
        if (upperCellsMat[cord] < numOfFloors) { return false; }
    }
    msg << "Containers at port: total containers amount exceeds ship capacity.";
    if(userIsSimulator)
    {
        log(msg.str(), MessageSeverity::Reject);
    }
    errorHandle.reportError(Errors::containersExceedShipCapacity);
    return true;
}

