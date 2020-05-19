//
// Created by orbar on 15/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ERRORHANDLING_H
#define SHIP_STOWAGE_MODEL_ERRORHANDLING_H


enum Errors
{
    floorsExceedMaxHeight = 1u << 0u,
    posExceedsXYLimits = 1u << 1u,
    badLineFormatAfterFirstLine = 1u << 2u,
    BadFirstLineOrShipPlanFileCannotBeRead = 1u << 3u,
    reserved = 1u << 4u,
    portAppearsMoreThanOnceConsecutively = 1u << 5u,
    wrongSeaPortCode = 1u << 6u,
    emptyFileOrRouteFileCannotBeRead = 1u << 7u,
    atMostOneValidPort = 1u << 8u,
    reserved2 = 1u << 9u,
    duplicateIDOnPort = 1u << 10u,
    duplicateIDOnShip = 1u << 11u,
    badContainerWeight = 1u << 12u,
    badContainerDestPort = 1u << 13u,
    ContainerIDCannotBeRead = 1u << 14u,
    badContainerID = 1u << 15u,
    containerFileCannotBeRead = 1u << 16u,
    lastPortHasContainersWaiting = 1u << 17u,
    containersExceedShipCapacity = 1u << 18u


};

class ErrorHandle
{
private:
    int errorBits;
public:
    int getErrorBits() { return errorBits; }

    void reportError(Errors error) { errorBits |= error; }

};


#endif //SHIP_STOWAGE_MODEL_ERRORHANDLING_H
