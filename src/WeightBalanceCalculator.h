//
// Created by orbar on 06/04/2020.
//
#include "Ship.h"

#ifndef SHIP_STOWAGE_MODEL_WEIGHTBALANCECALCULATOR_H
#define SHIP_STOWAGE_MODEL_WEIGHTBALANCECALCULATOR_H

enum balanceStatus{
    APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
};

class ShipWeightBalanceCalculator{
private:
    balanceStatus state;
    int balanceThreshold;
    std :: vector<FloorPlan> shipPlan;
public:
    ShipWeightBalanceCalculator(int balanceThreshold, std :: vector<FloorPlan> shipPlan);
    balanceStatus tryOperation(char loadUnload, int kg, int X, int Y);
    std :: vector<FloorPlan> getShipPlan();
};





#endif //SHIP_STOWAGE_MODEL_WEIGHTBALANCECALCULATOR_H
