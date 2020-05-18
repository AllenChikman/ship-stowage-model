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

    int getErrorBits() { return errorHandle.getErrorBits(); }

    explicit AlgorithmValidator() : errorHandle(ErrorHandle()) {}

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

    bool validateContainerFromFile(const std::vector<std::string> &line);

    bool validateDuplicateIDOnShip(const Container &container, const std::shared_ptr<ShipPlan> &shipPlan);

    bool validateContainerID(const std::string &id);

    bool validateContainerWeight(const std::string &weight);

    bool validateContainerDestPort(const std::string &destPort);

    bool validateReadingContainingFileAltogether(const std::string &containersAtPortFile);

    bool validateContainerAtLastPort(const std::string &travelPath);

    bool validateShipFull(const std::vector<Container> &container);

};


#endif //SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H
