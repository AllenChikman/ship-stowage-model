#include "Container.h"

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

//namespace Crane {
//    void performLoadingOperations(std::vector <ShipPlan> &shipPlan, const std::vector <CraneOperation> &craneOperations);
//
//}

class Port {
private:
    SeaPortCode id;
    std::vector <Container> containerData;
public:
    Port(SeaPortCode id, const std::vector <Container> &containerData);
    SeaPortCode getPortID(){return id;}


};