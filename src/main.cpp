#include <vector>
#include <iostream>
#include "Simulation.h"
#include "Utils.h"


void test()
{
    compareOutput((R"(../resources/simulationFolder/travel1)"));
    compareOutput((R"(../resources/simulationFolder/travel2)"));
    compareOutput((R"(../resources/simulationFolder/travel3)"));
    compareOutput((R"(../resources/simulationFolder/travel4)"));
}


int main()
{
    const string simRootPath = (R"(../resources/simulationFolder)");
    Simulation sim(simRootPath);
    sim.runAlgorithm();


    bool debug = false;
    if (debug) { test(); }

    return EXIT_SUCCESS;
}
