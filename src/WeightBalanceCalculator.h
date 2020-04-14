#ifndef SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H
#define SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H


class ShipPlan;

enum balanceStatus {
    APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
};

class ShipWeightBalanceCalculator {
private:
    balanceStatus status;
public:
    ShipPlan *shipPlan1;

    explicit ShipWeightBalanceCalculator(ShipPlan *shipPlan1);

    bool validateTryOperationsArguments(char loadUnload, unsigned kg, unsigned X, unsigned Y);

    balanceStatus checkBalance(unsigned x, unsigned y, unsigned z, unsigned kg);

    balanceStatus tryOperation(char loadUnload, unsigned kg, unsigned X, unsigned Y);

};


#endif //SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H