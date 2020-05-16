#ifndef SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H
#define SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H


#include <memory>

class ShipPlan;
struct XYCord;

enum balanceStatus {
    APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
};

class ShipWeightBalanceCalculator {
private:
    balanceStatus status;
public:

    ShipWeightBalanceCalculator() = default;

    explicit ShipWeightBalanceCalculator(balanceStatus status1):status(status1){}

    balanceStatus tryOperation(std::shared_ptr<ShipPlan> shipPlan, char loadUnload, unsigned kg, XYCord cord);

    balanceStatus getStatus (){return status;}

    void setStatus(balanceStatus newStatus){ status = newStatus;}

};


#endif //SHIP_STOWAGE_MODEL_BALANCE_CALCULATOR_H