#include <vector>
#include <iostream>

#include "Simulation.h"
#include "../common/Utils.h"

bool SetSimulatorCmdParams(char **argv, int argc,
                           std::string &travel_path,
                           std::string &algorithm_path,
                           std::string &output_path,
                           unsigned &numOfThreads)
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
        else if (args[idx] == "-num_threads")
        {
            try
            {
                int tempNumOfThreads = std::stoi(args[++idx]);
                if (tempNumOfThreads <= 0) { throw std::invalid_argument(""); }

                numOfThreads = static_cast<unsigned int>(tempNumOfThreads);
            }
            catch (std::invalid_argument const &e)
            {
                std::cout << "ERROR: -num_threads value should be a non-zero positive integer" << std::endl;
                showHelp = true;
                break;
            }
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
        std::cout
                << "Usage: simulator [-travel_path <path>] [-algorithm_path <algorithm path>] [-output <output path>] [-num_threads <num>]"
                << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char *argv[])
{

    /*For tests pass to the cmd arguments the next line:*/
    // -travel_path ../tests/test_only_ooe_port -algorithm_path ../tests/so_algorithms -output ../tests/_output
    string travelDir;
    string algorithmsDir(R"(./)");
    string outputDir((R"(./)"));
    unsigned numOfThreads = 1;

    if (!SetSimulatorCmdParams(argv, argc, travelDir, algorithmsDir, outputDir, numOfThreads))
    {
        return EXIT_FAILURE;
    }

    Simulation sim;

#ifdef LINUX_ENV
    sim.loadAlgorithms(algorithmsDir, outputDir);
#endif

    sim.runAlgorithmsOnTravels(travelDir, outputDir, numOfThreads);

    return EXIT_SUCCESS;
}
