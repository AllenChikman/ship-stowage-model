#include "Ship.h"

bool ShipPlan::isContainerEmpty(unsigned x, unsigned y, unsigned z)
{
return cargo[x][y][z].getDestinationPort().toStr().empty() ;
}