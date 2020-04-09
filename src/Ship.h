#include <utility>
#include <string>
#include <vector>

using namespace std;
typedef vector<vector<unsigned>> UIntMat;

class Container {
private:
    unsigned weight;
    string destinationPort;
    string id;

public:
    Container() = default;

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

    const string& toStr() const{ return seaPortCode;}

    static bool isSeaportCode(const std::string &portSymbol);

};

struct CargoData {
    string id;
    unsigned weight;
    string destPort;
};

class ShipPlan {

private:
    const unsigned width;      //x
    const unsigned length;     //y
    const unsigned height;     //z
    const UIntMat startingHeight;

    CargoMat cargo;

public:
    ShipPlan(unsigned width, unsigned length, unsigned maxHeight, UIntMat startingHeight);

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    UIntMat getStartingHeight() { return startingHeight; }

    CargoMat getCargo() { return cargo; }



};

class WeightBalance {
    int currentWeight;
    int threshold;
};

class Ship {

private:
    vector<SeaPortCode> shipRoute;
    ShipPlan shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan);

    Ship(const WeightBalance &balanceCalculator);

    ShipPlan getShipPlan();


};


