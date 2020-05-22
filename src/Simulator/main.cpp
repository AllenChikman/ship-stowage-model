#include <vector>
#include <iostream>

#include "Simulation.h"
#include "../Common/Utils.h"
#include "../Algorithms/NaiveAlgorithm.h"

bool SetSimulatorCmdParams(char **argv, int argc,
                           std::string &travel_path,
                           std::string &algorithm_path,
                           std::string &output_path)
{
    std::vector<std::string> args = std::vector<std::string>(static_cast<unsigned long long int>(argc - 1));
    for (size_t idx = 0; idx < static_cast<size_t>(argc) - 1; idx++) args[idx] = std::string(argv[idx + 1]);
    size_t idx = 0;

    bool showHelp = false;
    bool travelPathFound = false;

    while (idx < args.size())
    {
        if ((args[idx] == "--help") ||
            (args[idx] == "-h"))
        {
            showHelp = true;
            break;
        }
        else if (args[idx] == "-travel_path")
        {
            travel_path = args[++idx];
            travelPathFound = true;
        }
        else if (args[idx] == "-algorithm_path")
        {
            algorithm_path = args[++idx];
        }
        else if (args[idx] == "-output")
        {
            output_path = args[++idx];
        }
        else
        {
            std::cout << "ERROR: Unknown argument:" << args[idx] << std::endl;
            showHelp = true;
            break;
        }
        idx++;
    }


    if (!travelPathFound)
    {
        std::cout << "ERROR: travel_path mandatory parameter is not passed"
                  << std::endl;
    }

    if (showHelp || !travelPathFound)
    {
        std::cout << "Usage: simulator [-travel_path <path>] [-algorithm_path <algorithm path>] [-output <output path>]"
                  << std::endl;
        return false;
    }

    return true;
}


int main()
{

    /*For tests pass to the cmd arguments the next line:*/
    // -travel_path ../tests/test_only_ooe_port -algorithm_path ../tests/so_algorithms -output ../tests/_output
    string travelDir;
    string algorithmsDir(R"(./)");
    string outputDir((R"(./)"));

    if (!SetSimulatorCmdParams(__argv, __argc, travelDir, algorithmsDir, outputDir))
    {
        return EXIT_FAILURE;
    }

    Simulation sim;

#ifdef LINUX_ENV
    sim.loadAlgorithms(algorithmsDir);
#endif

    sim.runAlgorithmsOnTravels(travelDir, outputDir);

    return EXIT_SUCCESS;
}
