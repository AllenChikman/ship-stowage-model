#include "Ship.h"

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
    void performLoadingOperations(vector <ShipPlan> &shipPlan, const vector <CraneOperation> &craneOperations);

}

class Port {
private:
    SeaPortCode id;
    vector <Container> containerData;
public:
    Port(string id, const vector <Container> &containerData);


};
