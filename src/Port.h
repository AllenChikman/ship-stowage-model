#ifndef SHIP_STOWAGE_MODEL_PORT_H
#define SHIP_STOWAGE_MODEL_PORT_H


#include "Container.h"

enum CraneCommand
{
    LOAD, UNLOAD, REJECT
};

class CraneOperation
{
private:
    CraneCommand cmd;
    int containerId;
    int floorIdx;
    int rowIdx;
    int columnIdx;

};


#endif //SHIP_STOWAGE_MODEL_PORT_H