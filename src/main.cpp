#include <iostream>
#include "Simulation.h"

int main()
{
    //Simulation sim("C:\\Users\\orbar\\CLionProjects\\ship-stowage-model\\resources\\simulationFolder");
    Simulation sim(R"(C:\Users\Allen\CLionProjects\ship-stowage-model\resources\simulationFolder)");
    sim.startTravel("travel2");
    return 0;
}


