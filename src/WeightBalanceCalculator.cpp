//
// Created by orbar on 06/04/2020.
//
#include <iostream>
#include "WeightBalanceCalculator.h"
ShipWeightBalanceCalculator::ShipWeightBalanceCalculator(int balanceThreshold, std :: vector<FloorPlan> shipPlan): balanceThreshold(balanceThreshold), shipPlan(shipPlan)
{
    state = APPROVED; // for now

}

std::vector<FloorPlan> ShipWeightBalanceCalculator:: getShipPlan()
{
    return this->shipPlan;
}

balanceStatus ShipWeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y)
{
    if(loadUnload != 'U' && loadUnload != 'L')
    {
        std :: cerr << "Illegal operation argument";
        return APPROVED; //?- needs to be fixed
    }

    if(loadUnload == 'U') // assuming the shipPlan field is arranged from lowest floor to highest
    {
        std :: vector<FloorPlan> theShipPlan = getShipPlan();
        int numOfFloors = (int) theShipPlan.size();
        for( int i = numOfFloors - 1; i <= 0 ; i++)
        {
            if(theShipPlan[i].cargo[X][Y].getWeight == 0)
        }
    }

}
