#ifndef SHIP_STOWAGE_MODEL_PORT_H
#define SHIP_STOWAGE_MODEL_PORT_H


#include "Container.h"
#include "Ship.h"


class ShipPlan;

namespace Crane
{

enum Command
{
    LOAD, UNLOAD, REJECT, MOVE
};

char getCraneCmdChar(Command cmd);

void updateShipPlan(std::ofstream &outputFile, ShipPlan *shipPlan, const Container &container, Command cmd,
                    XYCord xyCord = {0, 0});

}


#endif //SHIP_STOWAGE_MODEL_PORT_H