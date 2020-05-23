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

    std::optional<Container> performUnload(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord);

    void performLoad(const std::shared_ptr<ShipPlan> &shipPlan, const std::optional<Container> &container, const XYCord &xyCord);

    void performMove(const std::shared_ptr<ShipPlan> &shipPlan, const XYCord &xyCord_U, const XYCord &xyCord_L);

}


#endif //SHIP_STOWAGE_MODEL_PORT_H