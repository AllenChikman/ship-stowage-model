
#include <iostream>
#include "WeightBalanceCalculator.h"
#include "Ship.h"
#include "Utils.h"

bool ShipWeightBalanceCalculator::validateTryOperationsArguments(ShipPlan *shipPlan, char loadUnload, unsigned kg, unsigned X, unsigned Y)
{
    std::string reason;
    bool valid = true;
    if (loadUnload != 'U' && loadUnload != 'L')
    {
        reason = "Illegal operation.";
        valid = false;
    }
    else if (X > shipPlan->getWidth() || Y > shipPlan->getLength())
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
    //TODO: implement (for exercise 2)
    return APPROVED;
}

balanceStatus ShipWeightBalanceCalculator::tryOperation(ShipPlan *shipPlan, char loadUnload, unsigned kg, unsigned X, unsigned Y)
{
    if (!validateTryOperationsArguments(shipPlan, loadUnload, kg, X, Y))
    {
        return status;
    }
    unsigned Z = 0;
    unsigned startingHeight = shipPlan->getStartingHeight()[X][Y];
    if (loadUnload == 'U')
    {
        Z = static_cast<unsigned int>();
    }

    if (loadUnload == 'L')
    {
        Z = static_cast<unsigned int>(shipPlan->getFreeCells()[X][Y]);
    }
    return checkBalance(X, Y, Z, kg, loadUnload);
}

