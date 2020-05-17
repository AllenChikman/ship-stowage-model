//
// Created by orbar on 17/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H
#define SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H

//#include "Utils.h"
#include "Container.h"
#include "Ship.h"
#include <vector>
#include "ErrorHandling.h"
class AlgorithmValidator {
private:
    ErrorHandle errorHandle;
public:
    explicit AlgorithmValidator(ErrorHandle errorHandle):errorHandle(errorHandle){}

    void validateShipDims(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors);
    void validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y);
    void validateShipPlanFloorsFormat(const std::vector<std::vector<std::string>> &shipFloors);
    void validateShipPlanFirstLine(std::vector<std::string> &firstFloor);
    void validateReadingShipPlanFileAltogether(const std::string &shipPlanFilePath);
    void validateSamePortInstancesConsecutively(const std::vector<SeaPortCode> &routeVec);
    void validatePortFormat(const SeaPortCode &port);
    void validateOpenReadShipRouteFileAltogether(const std::string &shipRouteFilePath);
    void validateAmountOfValidPorts(const std::vector<SeaPortCode> &routeVec);
    void validateDuplicateIDOnPort(const std::vector<Container> &containersAtPort);
    void validateContainerFromFile(const std::vector<std::string> &line);
    void validateDuplicateIDOnShip(const Container &container, const std::shared_ptr<ShipPlan>& shipPlan);
    bool validateContainerID(const std::string &id);
    void validateContainerWeight(const std::string &weight);
    void validateContainerDestPort(const std::string &destPort);

    void validateReadingContainingFileAltogether(const std::string &containersAtPortFile);

};


#endif //SHIP_STOWAGE_MODEL_ALGORITHMVALIDATOR_H
