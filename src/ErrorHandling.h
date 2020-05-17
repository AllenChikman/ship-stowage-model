//
// Created by orbar on 15/05/2020.
//

#ifndef SHIP_STOWAGE_MODEL_ERRORHANDLING_H
#define SHIP_STOWAGE_MODEL_ERRORHANDLING_H


enum Errors{
    floorsExceedMaxHeight = 1<<0,
    posExceedsXYLimits = 1<<1,
    badLineFormatAfterFirstLine = 1<<2,
    BadFirstLineOrShipPlanFileCannotBeRead = 1 << 3,  //fatal error
    reserved = 1<<4,
    portAppearsMoreThanOnceConsecutively = 1<<5,
    wrongSeaPortCode = 1<<6,
    emptyFileOrRouteFileCannotBeRead = 1 << 7,
    atMostOneValidPort = 1<<8,
    reserved2 = 1<<9,
    duplicateIDOnPort = 1<<10,
    duplicateIDOnShip = 1<<11,
    badContainerWeight = 1 << 12,
    badContainerDestPort = 1<<13,
    ContainerIDCannotBeRead = 1<<14,
    badContainerID = 1<<15,
    containerFileCannotBeRead = 1<<16,
    lastPortHasContainersWaiting = 1<<17,
    containersExceedShipCapacity = 1<<18


};
class ErrorHandle {
private:
    unsigned long errorBits;
public:
    explicit ErrorHandle(unsigned long errorBits) : errorBits(errorBits){}
    void reportError(Errors error){errorBits |= error;}

};


#endif //SHIP_STOWAGE_MODEL_ERRORHANDLING_H
