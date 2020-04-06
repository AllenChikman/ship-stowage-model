//
// Created by orbar on 06/04/2020.
//
#include <iostream>
#include "WeightBalanceCalculator.h"
ShipWeightBalanceCalculator::ShipWeightBalanceCalculator(int balanceThreshold, ShipPlan shipPlan1): balanceThreshold(balanceThreshold), shipPlan1(shipPlan1)
{
    status = APPROVED; // for now
}

bool ShipWeightBalanceCalculator::validateArguments(char loadUnload, int kg, int X, int Y, ShipPlan shipPlan1)
{
    if(loadUnload != 'U' && loadUnload != 'L')
    {
        std :: cerr << "Illegal operation.";
        return false;
    }
    if(X>shipPlan1.getWidth() || Y>shipPlan1.getLength())
    {
        std :: cerr << "Illegal container location on ship.";
        return false;
    }
    if(kg <= 0)
    {
        std :: cerr << "Illegal container weight.";
        return false;
    }
    return true;
}

}
balanceStatus ShipWeightBalanceCalculator::tryOperationRec(char loadUnload, int kg, int X, int Y, unsigned curHeight) {
    if (!validateArguments(loadUnload, kg, X, Y, shipPlan1)) {
        return status;
    }
    unsigned startingHeight = shipPlan1.getStartingHeight()[X][Y];
    unsigned z;
    int curWeight;
    if (loadUnload == 'U') {
        z = curHeight;
        while (startingHeight <= z) {
            curWeight = shipPlan1.getCargo()[X][Y][z].getWeight();
            if (curWeight == kg) // ie. the wanted container
            {
                //There should be an algorithm that calculates the balance itself
                return APPROVED;
            } else if (curWeight > 0) {
                balanceStatus moveUnload = tryOperationRec('U', curWeight, X, Y, z);
                balanceStatus moveLoad = tryOperationRec('L', curWeight, X, Y, z);
                if (moveUnload == APPROVED && moveLoad == APPROVED) {
                    z--;
                } else if (moveUnload == X_IMBALANCED && moveLoad == X_IMBALANCED) {
                    return X_IMBALANCED;
                } else if (moveUnload == Y_IMBALANCED && moveLoad == Y_IMBALANCED) {
                    return Y_IMBALANCED;
                } else {
                    return X_Y_IMBALANCED;
                }
            }
            z--;
        }
    }
    if (loadUnload == 'L')
    {
        for (z = startingHeight; z <= shipPlan1.getHeight(); z++)
        {
            if (shipPlan1.getCargo()[X][Y][z].getWeight() == 0) // ie. no container
            {
                //There should be an algorithm that calculates the balance itself
                return APPROVED;
            }
        }
    }
}
balanceStatus ShipWeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y)
{
    return tryOperationRec(loadUnload, kg, X, Y, shipPlan1.getHeight());
}
