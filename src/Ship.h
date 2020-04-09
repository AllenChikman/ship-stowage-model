#include <utility>
#include <string>
#include <vector>
#include "Container.h"

typedef std::vector<std::vector<unsigned>> UIntMat;



class SeaPortCode {
    std::string seaPortCode;


public:
    explicit SeaPortCode(std::string str) : seaPortCode(std::move(str)) {
        //std::transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
    }

    const std::string &toStr() const { return seaPortCode; }

    static bool isSeaportCode(const std::string &portSymbol);

};



class ShipPlan {

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;     //z
    const UIntMat startingHeight;
    CargoMat cargo;

public:
    ShipPlan(unsigned width, unsigned length, unsigned height, UIntMat startingHeight)
            : width(width),
              length(length),
              height(height),
              startingHeight(std::move(startingHeight)) {

        cargo = CargoMat(width, std::vector<std::vector<Container>>(length, std::vector<Container>(height, Container(0,"",""))));
    }

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    CargoMat getCargo() { return cargo; }


};

/*
class WeightBalance {
    int currentWeight;
    int threshold;
};
*/

/*
class Ship {

private:
    std::vector<SeaPortCode> shipRoute;
    ShipPlan shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const std::vector<SeaPortCode> &shipRoute, ShipPlan shipPlan);

    Ship(const WeightBalance &balanceCalculator);

    ShipPlan getShipPlan();


};
*/


