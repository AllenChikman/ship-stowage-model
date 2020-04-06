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
public:
    ShipPlan shipPlan1;
    ShipWeightBalanceCalculator(int balanceThreshold, ShipPlan shipPlan1);
    balanceStatus tryOperation(char loadUnload, int kg, int X, int Y);
};





#endif //SHIP_STOWAGE_MODEL_WEIGHTBALANCECALCULATOR_H
