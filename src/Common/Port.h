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

    void performUnload(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord);

    void performLoad(const std::shared_ptr<ShipPlan> &shipPlan, const Container &container, const XYCord &xyCord);

    void preformMove();

    void performValidLineCommand(const std::vector<std::string> &vecLine);

}


#endif //SHIP_STOWAGE_MODEL_PORT_H