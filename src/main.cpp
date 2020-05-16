#include <vector>
#include <iostream>
#include "Simulation.h"
#include "Utils.h"
#include "NaiveAlgorithm.h"


void test()
{
    compareOutput((R"(../resources/simulationFolder/travel3)"));
    compareOutput((R"(../resources/simulationFolder/travel4)"));
}


int main()
{
    const string simRootPath = (R"(../resources/simulationFolder)");
    Simulation sim(simRootPath);

    //sim.runAlgorithm();

    // new flow
    NaiveAlgorithm algorithm;
    sim.runAlgorithmOnTravels(R"(../resources/simulationFolder)", algorithm, "");

    bool debug = true;
    if (debug) { test(); }

    return EXIT_SUCCESS;
}
