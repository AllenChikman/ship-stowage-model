//
// Created by Allen on 21/03/2020.
//

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
    Container(int weight, string destinationPort, string id);

    //TODO: implement getters
};

class SeaPortCode {
    string seaPortCode;
};

typedef vector<vector<Container>> CargoMat;

class FloorPlan {

private:
    int width;
    int length;
    //TODO: cargo matrix


public:
    CargoMat cargo;
    FloorPlan(int width, int length);

    ~FloorPlan();

};

class WeightBalance {
    int currentWeight;
    int threshold;
};

class Ship {

private:
    vector<SeaPortCode> shipRoute;
    vector<FloorPlan> shipPlan;
    WeightBalance balanceCalculator;

public:
    Ship(const vector<SeaPortCode> &shipRoute, const vector<FloorPlan> &shipPlan);

    Ship(const WeightBalance &balanceCalculator);


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
    void performLoadingOperations(vector<FloorPlan> &shipPlan, const vector<CraneOperation> &craneOperations);

}

class Port {
private:
    SeaPortCode id;
    vector<Container> containerData;
public:
    Port(string id, const vector<Container> &containerData);


};


#endif //SHIP_STOWAGE_MODEL_SHIP_H
