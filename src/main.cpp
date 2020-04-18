#include <vector>
#include <iostream>
#include "Simulation.h"
#include "Utils.h"


int main()
{
    const string simRootPath = (R"(../resources/simulationFolder)");
    Simulation sim(simRootPath);
    sim.runAlgorithm();
    return EXIT_SUCCESS;
}
