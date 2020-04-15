#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <utility>
#include <string>
#include <vector>
#include "Container.h"
#include "WeightBalanceCalculator.h"

typedef std::vector<std::vector<unsigned>> UIntMat;

class ShipPlan
{

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;    //z

    UIntMat startingHeight;
    UIntMat firstCellAvailable;
    CargoMat cargo;
    ShipWeightBalanceCalculator balanceCalculator;

public:
    ShipPlan(unsigned width, unsigned length, unsigned height, UIntMat startingHeight1,
             ShipWeightBalanceCalculator balanceCalculator)
            : width(width),
              length(length),
              height(height), balanceCalculator(balanceCalculator)
    {

        cargo = CargoMat(width, std::vector<std::vector<std::optional<Container>>>
                (length, std::vector<std::optional<Container>>(height, std::nullopt)));

        startingHeight = std::move(startingHeight1);
        firstCellAvailable = startingHeight;
    }

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    UIntMat &getFirstAvailableCellMat() { return firstCellAvailable; }

    CargoMat &getCargo() { return cargo; }

    ShipWeightBalanceCalculator getBalanceCalculator() { return balanceCalculator; }

};

#endif //SHIP_STOWAGE_MODEL_SHIP_H