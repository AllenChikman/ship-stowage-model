#include <iostream>
#include "Simulation.h"
#include "Utils.h"


int main()
{
    Simulation sim(R"(../resources/simulationFolder)");
    sim.runAlgorithm();
    return EXIT_SUCCESS;
}


