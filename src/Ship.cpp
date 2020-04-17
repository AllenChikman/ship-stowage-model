/*
#include "Ship.h"

unsigned ShipPlan::getUpperCell(XYCord xyCord)
{
    const unsigned numOfFloors = getNumOfFloors(xyCord);
    for (unsigned z = 0; z < numOfFloors; z++)
    {
        if (!cargo[xyCord][z])
        {
            return z;
        }
    }
    return numOfFloors;
}
*/
