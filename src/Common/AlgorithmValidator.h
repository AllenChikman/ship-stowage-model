//
// Created by orbar on 17/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H
#define SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H

#include "Container.h"
#include "Ship.h"
#include <vector>
#include <memory>
#include "ErrorHandling.h"

class AlgorithmValidator
{
public:
    ErrorHandle errorHandle;
    bool userIsSimulator ; //i.e simulator = 1,  or algorithm = 0

    int getErrorBits() { return errorHandle.getErrorBits(); }

    void clear() { errorHandle.clear(); }

    AlgorithmValidator(bool userIsSimulator) : errorHandle(ErrorHandle()), userIsSimulator(userIsSimulator) {}

    bool validateShipHeightInput(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors);

    bool validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y);

    bool validateShipPlanFloorsFormat(const std::vector<std::string> &shipFloors);

    bool validateShipPlanFirstLine(std::vector<std::string> &firstFloor);

    bool validateReadingShipPlanFileAltogether(const std::string &shipPlanFilePath);

    bool validateSamePortInstancesConsecutively(const std::vector<std::string> &routeVec);

    bool validatePortFormat(const std::string &port);

    bool validateOpenReadShipRouteFileAltogether(const std::string &shipRouteFilePath);

    bool validateAmountOfValidPorts(const std::vector<std::string> &routeVec);

    bool validateDuplicateIDOnPort(const std::vector<Container> &containersAtPort);

    bool validateDuplicateIDOnPort(const std::vector<std::vector<std::string>> &containersAtPort,
                                   const std::vector<std::string> &containerLine, unsigned pos);

    bool validateContainerFromFile(const std::vector<string> &line,
                                   const std::vector<SeaPortCode> &travelRouteStack,
                                   bool containerToLoad);

    bool validateDuplicateIDOnShip(const std::string &containerID, const std::shared_ptr<ShipPlan> &shipPlan);

    bool validateContainerID(const std::string &id);

    bool validateContainerWeight(const std::string &weight);

    bool validateContainerDestPort(const string &destPort, const vector<SeaPortCode> &travelRouteStack,
                                   bool containerToLoad);

    bool validateReadingContainingFileAltogether(const std::string &containersAtPortFile);

    bool validateContainerAtLastPort(const std::string &travelPath);

    bool validateShipFull(const std::shared_ptr<ShipPlan> &shipPlan);

    bool validateDuplicateXYCordsWithDifferentData(const std::vector<std::vector<std::string>> &vecLines,
                                                        std::vector<std::vector<std::string>> &validVecLines);

    void filterInvalidVecLines(const vector<vector<string>> &vecLines, vector<vector<string>> &validVecLines);
};


#endif //SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H
