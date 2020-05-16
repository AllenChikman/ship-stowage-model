//
// Created by orbar on 15/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ERRORHANDLING_H
#define SHIP_STOWAGE_MODEL_ERRORHANDLING_H
#include "Utils.h"
#include "Container.h"


enum Errors{
    floorsExceedMaxHeight = 1<<0,
    posExceedsXYLimits = 1<<1,
    badLineFormatAfterFirstLine = 1<<2,
    BadFirstLineOrfileCannotBeRead = 1<<3,  //fatal error
    reserved = 1<<4,
    portAppearsMoreThanOnceConsecutively = 1<<5,
    wrongSeaPortCode = 1<<6,
    emptyFileOrFileCannotBeRead = 1<<7,
    atMostOneValidPort = 1<<8,
    reserved2 = 1<<9,
    duplicateIDOnPort = 1<<10,


};
class ErrorHandle {
private:
    unsigned long errorBits;
public:
    explicit ErrorHandle(unsigned long errorBits) : errorBits(errorBits){}
    void reportError(Errors error){errorBits |= error;}

    void validateShipDims(unsigned maximalHeight, unsigned x, unsigned y, unsigned numOfFloors);
    void validateShipXYCords(unsigned width, unsigned length, unsigned x, unsigned y);
    void validateShipPlanFloorsFormat(const vector<vector<string>> &shipFloors);
    void validateShipPlanFirstLine(vector<string> &firstFloor);
    void validateReadingShipPlanFileAltogether(const string &shipPlanFilePath);
    void validateSamePortInstancesConsecutively(const vector<SeaPortCode> &routeVec);
    void validatePortFormat(const SeaPortCode &port);
    void validateOpenReadShipRouteFileAltogether(const string &shipRouteFilePath);
    void validateAmountOfValidPorts(const vector<SeaPortCode> &routeVec);
    void validateDuplicateIDOnPort(const vector<Container> &containersAtPort);
};


#endif //SHIP_STOWAGE_MODEL_ERRORHANDLING_H
