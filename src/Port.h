#ifndef SHIP_STOWAGE_MODEL_PORT_H
#define SHIP_STOWAGE_MODEL_PORT_H


#include <memory>
#include "Container.h"


class ShipPlan;
class XYCord;

namespace Crane
{

enum Command
{
    LOAD, UNLOAD, REJECT, MOVE
};

char getCraneCmdChar(Command cmd);

void updateShipPlan(std::ofstream &outputFile, std::shared_ptr<ShipPlan> shipPlan, const Container &container, Command cmd,
                    XYCord xyCord);


void performOperation(std::shared_ptr<ShipPlan> shipPlan);

}


#endif //SHIP_STOWAGE_MODEL_PORT_H