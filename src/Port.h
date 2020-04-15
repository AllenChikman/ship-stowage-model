#ifndef SHIP_STOWAGE_MODEL_PORT_H
#define SHIP_STOWAGE_MODEL_PORT_H


#include "Container.h"
#include "Ship.h"

enum CraneCommand
{
    LOAD, UNLOAD, REJECT, MOVE
};

class ShipPlan;

void updateShipPlan(const Container &container, std::ofstream &outputFile,
                    ShipPlan *shipPlan, CraneCommand op = UNLOAD, XYCord xyUpdateCord = {0, 0});

#endif //SHIP_STOWAGE_MODEL_PORT_H