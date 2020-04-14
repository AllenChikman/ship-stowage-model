
#include <iostream>
#include "WeightBalanceCalculator.h"
#include "Ship.h"


ShipWeightBalanceCalculator::ShipWeightBalanceCalculator(ShipPlan *shipPlan1) : shipPlan1(shipPlan1)
{
    status = APPROVED; // for now
}

bool ShipWeightBalanceCalculator::validateTryOperationsArguments(char loadUnload, unsigned kg, unsigned X, unsigned Y)
{
    std::string reason;
    bool valid = true;
    if (loadUnload != 'U' && loadUnload != 'L')
    {
        reason = "Illegal operation.";
        valid = false;
    }
    else if (X > shipPlan1->getWidth() || Y > shipPlan1->getLength())
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
        std::cerr << reason;
    }
    return valid;
}

balanceStatus ShipWeightBalanceCalculator::tryOperation(char loadUnload, unsigned kg, unsigned X, unsigned Y)
{
    if (!validateTryOperationsArguments(loadUnload, kg, X, Y))
    {
        return status;
    }
    unsigned Z = 0;
    unsigned weight = kg;
    unsigned startingHeight = shipPlan1->getStartingHeight()[X][Y];
    if (loadUnload == 'U')
    {
        Z = static_cast<unsigned int>(shipPlan1->getHeight() - shipPlan1->getCargo()[X][Y].size());
        // weight = -weight; //TODO: Or - what did you mean by this line
    }

    if (loadUnload == 'L')
    {
        Z = static_cast<unsigned int>(startingHeight + shipPlan1->getCargo()[X][Y].size());
    }
    return checkBalance(X, Y, Z, weight);
}

balanceStatus ShipWeightBalanceCalculator::checkBalance(unsigned x, unsigned y, unsigned z, unsigned kg)
{

    (void) x;
    (void) y;
    (void) z;
    (void) kg;
    //TODO: implement (for exercise 2)
    return APPROVED;
}

