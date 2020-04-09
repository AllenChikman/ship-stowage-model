#include <iostream>
#include "Ship.h"
#include "Simulation.h"

int main() {
    Container c(0, "", "");
    UIntMat mat(10, vector<unsigned >(10, 0));
    ShipPlan plan(10, 10, 100, mat);

    Simulation sim;
    std::string simPath = R"(C:\Users\Allen\CLionProjects\ship-stowage-model\resources\simulationFolder\travel1)";
    const std::string shipPlanPath = simPath + R"(\shipPlan.txt)";
    const std::string routeFilePath = simPath + R"(\routeFile.txt)";

    sim.readShipPlan(shipPlanPath);
    sim.readShipRoute(routeFilePath);

    return 0;
}