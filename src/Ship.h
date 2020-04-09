#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <string>
#include <vector>

using namespace std;
typedef vector<vector<unsigned>> UIntMat;

class SeaPortCode {
    string seaPortCode;

public:
    SeaPortCode(string str) : seaPortCode(std::move(str)) {
        transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
    }
    static bool isSeaportCode(const std::string &portSymbol);

};

class Container {
private:
    unsigned weight;
    SeaPortCode destinationPort;
    string id;

public:
    Container() = default;

    Container(unsigned weight, const SeaPortCode &destinationPort, const string &id);

    int getWeight() { return weight; }

    SeaPortCode getDestinationPort() { return destinationPort; }

    string getID() const { return id; }
};

typedef vector<vector<vector<Container>>> CargoMat;


struct CargoData {
    string id;
    unsigned weight;
    SeaPortCode destPort;
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
    ShipPlan(unsigned width, unsigned length, unsigned maxHeight, UIntMat startingHeight);

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    vector<ShipCell> getVacantCells() {return vacantCells; }

    CargoMat getCargo() { return cargo; }

    ~ShipPlan() = default;

};

class WeightBalance {
    int currentWeight;
    int threshold;
};

class Ship {

private:
    Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, WeightBalance &balanceCalculator);

    vector<SeaPortCode> shipRoute;
    ShipPlan shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan, const WeightBalance &balanceCalculator);

    ShipPlan getShipPlan(){return shipPlan};


};


#endif //SHIP_STOWAGE_MODEL_SHIP_H
