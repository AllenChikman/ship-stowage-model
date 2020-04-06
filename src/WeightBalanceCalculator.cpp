//
// Created by orbar on 06/04/2020.
//
#include <iostream>
#include "WeightBalanceCalculator.h"
ShipWeightBalanceCalculator::ShipWeightBalanceCalculator(int balanceThreshold, ShipPlan shipPlan1): balanceThreshold(balanceThreshold), shipPlan1(shipPlan1)
{
    state = APPROVED; // for now
}



balanceStatus ShipWeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y)
{
    if(loadUnload != 'U' && loadUnload != 'L')
    {
        std :: cerr << "Illegal operation argument";
        return APPROVED;
    }
    if(X>shipPlan1.getWidth() || Y>shipPlan1.getLength())
    {
        return APPROVED; // needs to be fixed
    }
    unsigned startingHeight = shipPlan1.getStartingHeight()[X][Y];

    if(loadUnload == 'U')
    {
        int z = shipPlan1.getHeight();
        while (z >= startingHeight)
        {
            int curWeight = shipPlan1.getCargo()[X][Y][z].getWeight();
                if (curWeight== kg) // ie. the wanted container
                {
                    //There should be an algorithm that calculates the balance itself
                    return APPROVED;
                }
               /* else if(curWeight != 0) // ie. not the one we wanted but not empty
                {
                    if(tryOperation('U',curWeight,X,Y))
                }*/
        }
    }
    if(loadUnload == 'L')
    {
        for(unsigned z = startingHeight; z <= shipPlan1.getHeight(); z++)
        {
            if(shipPlan1.getCargo()[X][Y][z].getWeight() == 0) // ie. no container
            {
                //There should be an algorithm that calculates the balance itself
                return APPROVED;
            }
    }

}
