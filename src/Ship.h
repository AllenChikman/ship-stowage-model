#include <utility>
#include <string>
#include <vector>
#include "Container.h"

typedef std::vector<std::vector<unsigned>> UIntMat;


    Container(unsigned weight, const string &destinationPort, const string &id);

    int getWeight() { return weight; }

    string getDestinationPort() { return destinationPort; }

    string getID() { return id; }
};

typedef vector<vector<vector<Container>>> CargoMat;

class SeaPortCode {
    string seaPortCode;


public:
    SeaPortCode(string str) : seaPortCode(std::move(str)) {
        transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
    }

    static bool isSeaportCode(const std::string &portSymbol);

};

struct ShipCell{
    unsigned x;
    unsigned y;
    unsigned z;
};

class ShipPlan {

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;     //z
    const UIntMat startingHeight;

    vector<ShipCell> vacantCells;
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

    vector<ShipCell> getVacantCells() {return vacantCells; }

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
    Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, WeightBalance &balanceCalculator);

    vector<SeaPortCode> shipRoute;
    std::vector<SeaPortCode> shipRoute;
    ShipPlan shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const std::vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, const WeightBalance &balanceCalculator);

    ShipPlan getShipPlan(){return shipPlan};


};
*/


