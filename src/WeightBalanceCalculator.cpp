
#include <iostream>
#include "WeightBalanceCalculator.h"
#include "Ship.h"


ShipWeightBalanceCalculator::ShipWeightBalanceCalculator(ShipPlan *shipPlan1) : shipPlan1(shipPlan1)
{
    status = APPROVED; // for now
}

bool ShipWeightBalanceCalculator::validateTryOperationsArguments(char loadUnload, int kg, int X, int Y)
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

balanceStatus ShipWeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y)
{
    if (!validateTryOperationsArguments(loadUnload, kg, X, Y))
    {
        return status;
    }
    unsigned Z = 0;
    int weight = kg;
    unsigned startingHeight = shipPlan1->getStartingHeight()[X][Y];
    if (loadUnload == 'U')
    {
        Z = shipPlan1->getHeight() - shipPlan1->getCargo()[X][Y].size();
        weight = -weight;
    }

    if (loadUnload == 'L')
    {
        Z = startingHeight + shipPlan1->getCargo()[X][Y].size();
    }
    return checkBalance(X, Y, Z, weight);
}

balanceStatus ShipWeightBalanceCalculator::checkBalance(int x, int y, unsigned int z, int kg)
{
    //TODO: implement (for exercise 2)
    return APPROVED;
}

