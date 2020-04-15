#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <utility>
#include <string>
#include <vector>
#include "Container.h"
//#include "WeightBalanceCalculator.h"

typedef std::vector<std::vector<unsigned>> UIntMat;

class ShipPlan
{

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;    //z

    UIntMat startingHeight;
    UIntMat freeCells;
    CargoMat cargo;
    //ShipWeightBalanceCalculator balanceCalculator;

public:
    ShipPlan(unsigned width, unsigned length, unsigned height, UIntMat startingHeight1)
            : width(width),
              length(length),
              height(height)
    {

        cargo = CargoMat(width, std::vector<std::vector<Container>>(length,
                                                                    std::vector<Container>(height,
                                                                                           Container(
                                                                                                   "", 0,
                                                                                                   SeaPortCode("")))));
        startingHeight = std::move(startingHeight1);
        freeCells = startingHeight;


    }

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    UIntMat &getFreeCells() { return freeCells; }

    CargoMat &getCargo() { return cargo; }

    bool isContainerEmpty(unsigned x, unsigned y, unsigned z);

};

#endif //SHIP_STOWAGE_MODEL_SHIP_H