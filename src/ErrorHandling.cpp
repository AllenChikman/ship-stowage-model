//
// Created by orbar on 15/05/2020.
//
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "ErrorHandling.h"

bool checkShipPlanLineFormat(const vector<string> &line)
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

void ErrorHandle::validateShipDims(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors)
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
       reportError(Errors::floorsExceedMaxHeight);
   }
}

void ErrorHandle::validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y)
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
        reportError(Errors::posExceedsXYLimits);
    }

}

void ErrorHandle::validateShipPlanFloorsFormat(const vector<vector<string>> &shipFloors)
{
    std::ostringstream msg;
    bool valid = true;
    for (auto &line : shipFloors)
    {
        valid = checkShipPlanLineFormat(line);
    }
    if(!valid){
        reportError(Errors::badLineFormatAfterFirstLine);
        msg << "Bad line format after first line";
        log(msg.str(), MessageSeverity::WARNING);
    }
}

void ErrorHandle::validateShipPlanFirstLine(vector<string> &firstFloor)
{
    std::ostringstream msg;
    bool valid = true;
    valid = checkShipPlanLineFormat(firstFloor);
    if(!valid){
        reportError(Errors::BadFirstLineOrfileCannotBeRead);
        msg << "Fatal Error: Bad first line format";
        log(msg.str(), MessageSeverity::ERROR);
    }
}

void ErrorHandle::validateReadingShipPlanFileAltogether(const string &shipPlanFilePath)
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
        reportError(Errors::BadFirstLineOrfileCannotBeRead);
    }

}

void ErrorHandle::validateSamePortInstancesConsecutively(const vector<SeaPortCode> &routeVec)
{
    std::ostringstream msg;

   auto prevPort = routeVec[0];
   for(auto &port : routeVec)
   {
       if (port.toStr() == prevPort.toStr())
       {
           msg << "Port " + port.toStr() + " appears twice or more consecutively.";
           log(msg.str(), MessageSeverity::WARNING);
           reportError(Errors::portAppearsMoreThanOnceConsecutively);
           break;
       }
   }
}

void ErrorHandle::validatePortFormat(const SeaPortCode &port)
{
    std::ostringstream msg;
    bool valid = SeaPortCode::isValidCode(port.toStr());
    if(!valid)
    {
        msg << "Bad port symbol format.";
        log(msg.str(), MessageSeverity::WARNING);
        reportError(Errors::wrongSeaPortCode);
    }
}

void ErrorHandle::validateOpenReadShipRouteFileAltogether(const string &shipRouteFilePath)
{
    std::ostringstream msg;
    bool valid = true;

    // Open the File
    std::ifstream in;
    in.open(shipRouteFilePath.c_str());

    // Check if object is valid
    valid = !!in;

    string line;
    vector<string> vec;

    // Read the next line from File until it reaches the end.
    while (std::getline(in, line))
    {
        // Line contains string of length > 0 then save it in vector
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
        reportError(Errors::emptyFileOrFileCannotBeRead);
    }
    in.close();
}

void ErrorHandle::validateAmountOfValidPorts(const vector<SeaPortCode> &routeVec)
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
           reportError(Errors::atMostOneValidPort);
    }
}

void ErrorHandle::validateDuplicateIDOnPort(const vector<Container> &containersAtPort)
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
            reportError(Errors::duplicateIDOnPort);
            //#TODO: reject container in output file
        }
    }
}

