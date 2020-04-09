#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <utility>
#include <string>
#include <vector>
#include "Container.h"

typedef std::vector<std::vector<unsigned>> UIntMat;

class ShipPlan {

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;     //z
    const UIntMat startingHeight;

    std::vector <ShipCell> vacantCells;
    CargoMat cargo;

public:
    ShipPlan(unsigned width, unsigned length, unsigned height, UIntMat startingHeight)
            : width(width),
              length(length),
              height(height),
              startingHeight(std::move(startingHeight)) {

        cargo = CargoMat(width, std::vector<std::vector<Container>>(length,
                                                                    std::vector<Container>(height,
                                                                                           Container(0, SeaPortCode(""), ""
                                                                                           ))));
    }

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    std::vector <ShipCell> getVacantCells() { return vacantCells; }

    CargoMat getCargo() { return cargo; }


};

/*
class Ship {

private:
    Ship(const std::vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, WeightBalance &balanceCalculator);

    std::vector<SeaPortCode> shipRoute;
    std::vector<SeaPortCode> shipRoute;
    ShipPlan shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const std::vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, const WeightBalance &balanceCalculator);

    ShipPlan getShipPlan(){return shipPlan;}


};*/



#endif //SHIP_STOWAGE_MODEL_SHIP_H