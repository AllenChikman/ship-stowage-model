#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <string>
#include <vector>

using namespace std;

class Container {
private:
    int weight;
    string destinationPort;
    string id;

public:
    Container() = default;
    Container(int weight, string destinationPort, string id);
    int getWeight();
    string getDestinationPort();
    string getID();
};

class SeaPortCode {
    string seaPortCode;
};

typedef vector<vector<vector<Container>>> CargoMat;
typedef vector<vector<unsigned>> UIntMat;


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
    ShipPlan(unsigned width, unsigned length, unsigned maxHeight,UIntMat startingHeight);
    unsigned getWidth();
    unsigned getLength();
    unsigned  getHeight();
    UIntMat getStartingHeight();
    CargoMat getCargo();

    ~ShipPlan()= default;

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

enum CraneCommand {
    LOAD, UNLOAD, REJECT
};

class CraneOperation {
private:
    CraneCommand cmd;
    int containerId;
    int floorIdx;
    int rowIdx;
    int columnIdx;

};

namespace Crane {
    void performLoadingOperations(vector<ShipPlan> &shipPlan, const vector<CraneOperation> &craneOperations);

}

class Port {
private:
    SeaPortCode id;
    vector<Container> containerData;
public:
    Port(string id, const vector<Container> &containerData);


};


#endif //SHIP_STOWAGE_MODEL_SHIP_H
