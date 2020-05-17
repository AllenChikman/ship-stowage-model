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



bool checkShipPlanLineFormat(const std::vector<std::string> &line)
{
    bool valid = true;
    if (line.size() != 3)
    {
        valid = false;
    }
    for(auto &word : line)
    {
        try{
            std::stoul(word);
        }
        catch(std::invalid_argument const &e)
        {
            valid = false;
        }
    }
    return valid;
}

void AlgorithmValidator::validateShipDims(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors)
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
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::floorsExceedMaxHeight);
    }
}

void AlgorithmValidator::validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y)
{
    std::ostringstream msg;
    bool valid = true;
    if (x >= width || y >= length)
    {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    if (!valid) {
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::posExceedsXYLimits);
    }

}

void AlgorithmValidator::validateShipPlanFloorsFormat(const std::vector<std::vector<string>> &shipFloors)
{
    std::ostringstream msg;
    bool valid = true;
    for (auto &line : shipFloors)
    {
        valid = checkShipPlanLineFormat(line);
    }
    if(!valid){
        errorHandle.reportError(Errors::badLineFormatAfterFirstLine);
        msg << "Bad line format after first line";
        log(msg.str(), MessageSeverity::WARNING);
    }
}

void AlgorithmValidator::validateShipPlanFirstLine(std::vector<string> &firstFloor)
{
    std::ostringstream msg;
    bool valid = true;
    valid = checkShipPlanLineFormat(firstFloor);
    if(!valid){
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        msg << "Fatal Error: Bad first line format";
        log(msg.str(), MessageSeverity::ERROR);
    }
}

void AlgorithmValidator::validateReadingShipPlanFileAltogether(const string &shipPlanFilePath)
{
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(shipPlanFilePath.c_str());

    // Check if object is valid
    if(!in)
    {
        msg << "Cannot open the File: " + shipPlanFilePath;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
    }

}

void AlgorithmValidator::validateSamePortInstancesConsecutively(const std::vector<SeaPortCode> &routeVec)
{
    std::ostringstream msg;

    auto prevPort = routeVec[0];
    for(auto &port : routeVec)
    {
        if (port.toStr() == prevPort.toStr())
        {
            msg << "Port " + port.toStr() + " appears twice or more consecutively.";
            log(msg.str(), MessageSeverity::WARNING);
            errorHandle.reportError(Errors::portAppearsMoreThanOnceConsecutively);
            break;
        }
    }
}

void AlgorithmValidator::validatePortFormat(const SeaPortCode &port)
{
    std::ostringstream msg;
    bool valid = SeaPortCode::isValidCode(port.toStr());
    if(!valid)
    {
        msg << "Bad port symbol format.";
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::wrongSeaPortCode);
    }
}

void AlgorithmValidator::validateOpenReadShipRouteFileAltogether(const string &shipRouteFilePath)
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

    if(!valid)
    {
        msg << "Cannot open the File: " + shipRouteFilePath;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
    }
    in.close();
}

void AlgorithmValidator::validateAmountOfValidPorts(const std::vector<SeaPortCode> &routeVec)
{
    std::ostringstream msg;
    int validPorts = 0;
    for(auto &port : routeVec)
    {
        if(SeaPortCode::isValidCode(port.toStr()))
        {
            validPorts ++;
        }
    }
    if(validPorts <= 1)
    {
        msg << "File with only a single valid port.";
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::atMostOneValidPort);
    }
}

void AlgorithmValidator::validateDuplicateIDOnPort(const std::vector<Container> &containersAtPort)
{
    std::ostringstream msg;
    std::unordered_map<string, unsigned> idMap = {};
    for(auto &container : containersAtPort)
    {
        if (idMap.find(container.getID()) == idMap.end())
        {
            idMap.insert(make_pair(container.getID(), 1));
        }
        else
        {
            idMap[container.getID()] ++;
        }
    }
    for(auto &temp_id : idMap)
    {
        auto id = temp_id.first;
        if(idMap[id] > 1)
        {
            msg << "Duplicated ID on port.";
            log(msg.str(), MessageSeverity::WARNING);
            errorHandle.reportError(Errors::duplicateIDOnPort);
            //#TODO: reject container in output file
        }
    }
}

void AlgorithmValidator::validateDuplicateIDOnShip(const Container &container, const std::shared_ptr<ShipPlan>& shipPlan)
{
    std::ostringstream msg;
    auto xyCords = shipPlan->getShipXYCordsVec();
    for(auto &xyCord : xyCords)
    {
        auto maxFloor = shipPlan->getNumOfFloors(xyCord);
        for(int floor = 0; floor < maxFloor; floor++)
        {
            if(shipPlan->getCargo()[xyCord.x][xyCord.y][floor]->getID() == container.getID())
            {
                msg << "Containers at port: ID already on ship";
                log(msg.str(), MessageSeverity::Reject);
                errorHandle.reportError(Errors::duplicateIDOnShip);
            }
        }
    }
}

bool AlgorithmValidator::validateContainerID(const string &id)
{
    // id doesn't exist
    std::ostringstream msg;
    auto properIDLen = 11;
    if(id.size() != properIDLen)
    {
        msg << "Containers at port: bad line format, ID cannot be read.";
        log(msg.str(), MessageSeverity::WARNING);
        errorHandle.reportError(Errors::ContainerIDCannotBeRead);
        return false;
    }

    // id exists but has wrong format
    Container tmpContainer(id, 0, SeaPortCode("null"));  //TODO :consult with Allen
    if(!tmpContainer.isValidID())
    {
        msg << "Containers at port: illegal ID check ISO 6346";
        log(msg.str(), MessageSeverity::Reject);
        errorHandle.reportError(Errors::badContainerID);
    }
    return true;
}

void AlgorithmValidator::validateContainerWeight(const string &weight)
{
    std::ostringstream msg;
    for(char i : weight)
    {
        if(!isdigit(i))
        {
            msg << "Containers at port: bad line format, missing or bad weight.";
            log(msg.str(), MessageSeverity::Reject);
            errorHandle.reportError(Errors::badContainerWeight);
        }
    }
}

void AlgorithmValidator::validateContainerDestPort(const string &destPort)
{
    std::ostringstream msg;
    if(!SeaPortCode::isValidCode(destPort))
    {
        msg << "Containers at port: bad line format, missing or bad port dest.";
        log(msg.str(), MessageSeverity::Reject);
        errorHandle.reportError(Errors::badContainerDestPort);
    }
}

void AlgorithmValidator::validateContainerFromFile(const std::vector<string> &line) {
    /* valid line should be at length 3 where
 * line[0] - container Id (string)
 * line[1] - numOf.. (unsigned int)
 * line[2] - destination port (string)
 * */

    if (!line.empty())
    {
        string id, weight, destPort;
        bool idExists;

        //check if ID exists
        id = line[0];
        idExists = validateContainerID(id);
        if (idExists)
        {
            if (line.size() > 1)
            {
                weight = line[1];
                validateContainerWeight(weight);
                if (line.size() > 2)
                {
                    destPort = line[2];
                }
                else{
                    destPort = line[1];
                }
                validateContainerDestPort(destPort);
            }
            else{
                errorHandle.reportError(Errors::badContainerDestPort);
                errorHandle.reportError(Errors::badContainerWeight);
            }
        }
    }

}

void AlgorithmValidator::validateReadingContainingFileAltogether(const string &containersAtPortFile)
{
    std::ostringstream msg;
    // Open the File
    std::ifstream in;
    in.open(containersAtPortFile.c_str());

    // Check if object is valid
    if(!in)
    {
        msg << "Cannot open the File: " + containersAtPortFile;
        log(msg.str(), MessageSeverity::ERROR);
        errorHandle.reportError(Errors::containerFileCannotBeRead);
    }

}

