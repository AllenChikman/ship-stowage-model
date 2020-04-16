
#include <iostream>
#include "WeightBalanceCalculator.h"
#include "Ship.h"
#include "Utils.h"

bool validateTryOperationsArgs(ShipPlan *shipPlan, char loadUnload, unsigned kg,
                                                            XYCord cord)
{
    std::string reason;
    bool valid = true;
    if (loadUnload != 'U' && loadUnload != 'L')
    {
        reason = "Illegal operation.";
        valid = false;
    }
    else if (cord.x > shipPlan->getLength() || cord.y > shipPlan->getWidth())
    {
        reason = "Illegal container location on ship.";
        valid = false;
    }
    else if (kg <= 0)
    {
        reason = "Illegal container weight.";
        valid = false;
    }
    if (!valid)
    {
        log(reason, MessageSeverity::ERROR);
    }
    return valid;
}

balanceStatus checkBalance(unsigned x, unsigned y, unsigned z, unsigned kg, char loadUnload)
{

    (void) x;
    (void) y;
    (void) z;
    (void) kg;
    (void) loadUnload;
    //TODO: implement (for exercise 2)
    return APPROVED;
}

balanceStatus ShipWeightBalanceCalculator::tryOperation(ShipPlan *shipPlan, char loadUnload, unsigned kg, XYCord cord)
{
    if (!validateTryOperationsArgs(shipPlan, loadUnload, kg, cord))
    {
        return status;
    }
    unsigned Z = 0;
    if (loadUnload == 'U')
    {
        // assuming we always call this func when we have something to unload
        Z = shipPlan->getFirstAvailableCellMat()[cord] - 1;
    }

    if (loadUnload == 'L')
    {
        Z = shipPlan->getFirstAvailableCellMat()[cord];
    }
    return checkBalance(cord.x, cord.y, Z, kg, loadUnload);
}

