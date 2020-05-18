#include <iostream>
#include "WeightBalanceCalculator.h"
#include "Ship.h"
#include "Utils.h"
#include <memory>


bool validateTryOperationsArgs(const std::shared_ptr<ShipPlan> &shipPlan, char loadUnload, unsigned kg,
                                                            XYCord cord)
{
    std::string reason;
    bool valid = true;
    if (loadUnload != 'U' && loadUnload != 'L')
    {
        reason = "Illegal operation.";
        valid = false;
    }
    else if (cord.x > shipPlan->getLength() || cord.y > shipPlan->getWidth())
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
        log(reason, MessageSeverity::ERROR);
    }
    return valid;
}

WeightBalanceCalculator::BalanceStatus checkBalance(unsigned x, unsigned y, unsigned z, unsigned kg, char loadUnload)
{

    (void) x;
    (void) y;
    (void) z;
    (void) kg;
    (void) loadUnload;
    //TODO: implement (for exercise 2)
    return WeightBalanceCalculator::BalanceStatus::APPROVED;
}

/*
WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int x, int y);
{
    auto &upperCellsMat = shipPlan->getUpperCellsMat();

    if (!validateTryOperationsArgs(shipPlan, loadUnload, kg, cord))
    {
        return status;
    }
    unsigned z = 0;
    if (loadUnload == 'U')
    {
        // assuming we always call this func when we have something to unload
        z = upperCellsMat[cord] - 1;
    }

    if (loadUnload == 'L')
    {
        z = upperCellsMat[cord];
    }
    return checkBalance(cord.x, cord.y, z, kg, loadUnload);
}

*/
int WeightBalanceCalculator::readShipPlan(const std::string &full_path_and_file_name)
{
    return 0;
}

WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int x, int y)
{
    return APPROVED;
}
