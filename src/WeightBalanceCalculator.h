#ifndef SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H
#define SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H




class ShipPlan;

enum balanceStatus{
    APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
};

class ShipWeightBalanceCalculator{
private:
    balanceStatus status;
public:
    ShipPlan *shipPlan1;
    ShipWeightBalanceCalculator(ShipPlan *shipPlan1);
    bool validateTryOperationsArguments(char loadUnload, int kg, int X, int Y);
    balanceStatus checkBalance(int x, int y, unsigned int z, int kg);
    balanceStatus tryOperation(char loadUnload, int kg, int X, int Y);

};






#endif //SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H