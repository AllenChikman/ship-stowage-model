#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
//#include <dlfcn.h>

#include "Simulation.h"
#include "../Common/Utils.h"
#include "../Algorithms/NaiveAlgorithm.h"

#include "filesystem"
#include "AlgorithmRegistrar.h"


// Ex1 functions for reference

/*
bool Simulation::startTravel(const string &travelDir)
{
    if (!initTravel(travelDir))
    {
        log("Failed to init the travel - Ignoring this travel simulation ", MessageSeverity::ERROR);
        return false;
    }

    NaiveAlgorithm algorithm;
    algorithm.readShipPlan(travelDir + "/shipPlan.csv");
    algorithm.readShipRoute(travelDir + "/routeFile.csv");

    string currInputPath;
    string currOutputPath;
    string portStr;
    unsigned numOfVisits;
    bool lastPortVisit;

    vector<SeaPortCode> routeTravelStack(shipRoute.rbegin(), shipRoute.rend());

    for (const SeaPortCode &port : shipRoute)
    {
        portStr = port.toStr();
        numOfVisits = (visitedPorts.find(port.toStr()) == visitedPorts.end()) ? 0 : visitedPorts[portStr];
        visitedPorts[portStr] = ++numOfVisits;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(port, numOfVisits);
        lastPortVisit = isLastPortVisit(port);
        const bool cargoFileExists = popRouteFileSet(currInputPath);
        if (cargoFileExists && lastPortVisit)
        {
            log("Last visited port should not have a file for it", MessageSeverity::WARNING);
        }

        if (!cargoFileExists && !lastPortVisit)
        {
            log("This port visit has no file for it (expected: " + currInputPath + "). Unloading Only",
                MessageSeverity::WARNING);
        }

        if (!algorithm.getInstructionsForCargo(currInputPath, currOutputPath))
        {
            log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
        }

        routeTravelStack.pop_back();
    }

    validateAllCargoFilesWereUsed();

    return true;
}
*/

/*void Simulation::runAlgorithm()
{
    vector<string> travelDirPaths;
    putDirFileListToVec(rootFolder, travelDirPaths);
    for (const auto &travelFolder :travelDirPaths)
    {
        if (startTravel(travelFolder))
        {
            log("Travel Finished Successfully!!!");
        }
    }

}*/


// Simulation private class method implementation

std::pair<string, string> Simulation::getPortFilePaths(const SeaPortCode &port, int numOfVisits)
{
    const string &inputFileName = port.toStr(true);
    const string &outputFileName = port.toStr();

    const string middlePart = "_" + std::to_string(numOfVisits);

    string inputPath = curTravelFolder + '/' + inputFileName + middlePart + ".cargo_data";
    string outputPath = curTravelFolder + "/output/" + outputFileName + middlePart + ".out_cargo_data";

    return std::make_pair(inputPath, outputPath);

}

void Simulation::createOutputDirectory()
{
    createDirIfNotExists(curTravelFolder + "/output");
}

void Simulation::updateRouteMap()
{
    for (const auto &port : shipRoute)
    {
        const string &portStr = port.toStr();
        routeMap[portStr] = (routeMap.find(portStr) == routeMap.end()) ? 1 : routeMap[portStr] + 1;
    }
}

void Simulation::updateRouteFileSet()
{
    vector<string> cargoFilesVec;
    putDirFileListToVec(curTravelFolder, cargoFilesVec, ".cargo_data");
    for (const auto &path : cargoFilesVec)
    {
        cargoFilesSet.insert(path);
    }
}

bool Simulation::popRouteFileSet(const string &currInputPath)
{
    string pathToPop;
    std::filesystem::path p2 = currInputPath;
    for (const auto &path : cargoFilesSet)
    {
        std::filesystem::path p1 = path;
        if (std::filesystem::equivalent(p1, p2))
        {
            pathToPop = path;
            break;
        }
    }
    if (!pathToPop.empty())
    {
        cargoFilesSet.erase(pathToPop);
        return true;
    }
    return false;
}

void Simulation::validateAllCargoFilesWereUsed()
{
    for (const auto &file : cargoFilesSet)
    {
        log("File: " + file + " was not used and ignored (not in route or too many files for port)",
            MessageSeverity::WARNING);
    }
}

bool Simulation::initTravel(const string &travelDir)
{
    logStartingDecorator();
    log("Initializing travel...");
    log("Travel Root Folder is: " + travelDir);
    log("Output Folder is: " + travelDir + "/output");

    // clear
    curTravelFolder = travelDir;
    visitedPorts.clear();
    cargoFilesSet.clear();
    routeMap.clear();

    createOutputDirectory();

    bool isSuccessful = true;
    isSuccessful &= readShipPlan(getShipPlanFilePath());
    isSuccessful &= readShipRoute(getRouteFilePath());
    return isSuccessful;
}


// File input validators

bool algoValidateShipPlanEntry(unsigned width, unsigned length, unsigned maximalHeight,
                               unsigned x, unsigned y, unsigned numOfFloors)
{

    std::ostringstream msg;
    bool valid = true;

    if (x >= width || y >= length)
    {
        msg << "[" << x << "][" << y << "]" << " coordinate is out of bounds (ship plan size is:" <<
            "[" << width << "][" << length << "]";
        valid = false;
    }
    else if (numOfFloors >= maximalHeight)
    {
        msg << "Number of floors is not smaller than the maximal height "
            << maximalHeight << " in [" << x << "][" << y << "]";
        valid = false;
    }

    if (!valid) { log(msg.str(), MessageSeverity::WARNING); }
    return valid;
}

bool algoValidateShipRouteFile(const vector<string> &vec)
{
    string prevSymbol;
    for (const auto &portSymbol : vec)
    {
        if (!SeaPortCode::isValidCode(portSymbol))
        {
            log("SeaPortCode: " + portSymbol + " is invalid!", MessageSeverity::ERROR);
            return false;
        }
        if (portSymbol == prevSymbol)
        {
            log("SeaPortCode: " + portSymbol + " appears twice in a row!", MessageSeverity::ERROR);
            return false;
        }
        prevSymbol = portSymbol;
    }
    return true;
}


// Simulation public class method implementation

bool Simulation::readShipPlan(const string &path)
{
    try
    {
        vector<vector<string>> vecLines;
        if (!readToVecLine(path, vecLines))
        {
            throw std::runtime_error("");
        }
        auto shipPlanData = vecLines[0];

        unsigned maximalHeight = stringToUInt(shipPlanData[0]);
        unsigned width = stringToUInt(shipPlanData[1]);
        unsigned length = stringToUInt(shipPlanData[2]);

        unsigned x;
        unsigned y;
        unsigned numOfFloors;

        vecLines.erase(vecLines.begin());
        shipPlan = std::make_shared<ShipPlan>(width, length, maximalHeight, WeightBalanceCalculator());
        CargoMat &cargoMat = shipPlan->getCargo();

        for (const auto &vecLine : vecLines)
        {
            x = stringToUInt(vecLine[0]);
            y = stringToUInt(vecLine[1]);
            numOfFloors = stringToUInt(vecLine[2]);
            if (algoValidateShipPlanEntry(width, length, maximalHeight, x, y, numOfFloors))
            {
                cargoMat[x][y].resize(numOfFloors);
            }
            // else: invalid input entry is ignored
        }

        return true;
    }

    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship plan", MessageSeverity::ERROR);
        return false;
    }
}

bool Simulation::readShipRoute(const string &path)
{
    try
    {
        vector<string> vec;
        if (!readToVec(path, vec)) { throw std::runtime_error(""); }

        vector<SeaPortCode> routeVec;
        if (algoValidateShipRouteFile(vec))
        {
            for (const auto &portSymbol : vec)
            {
                routeVec.emplace_back(portSymbol);
            }
        }
        else
        {
            throw std::runtime_error("");
        }
        shipRoute = routeVec;
        updateRouteMap();
        updateRouteFileSet();

        return true;
    }
    catch (const std::runtime_error &e)
    {
        log("Failed to read the ship route", MessageSeverity::ERROR);
        return false;
    }
}



/////// EX2 Part


void Simulation::getSortedResultVec(std::vector<std::tuple<string, int, int>> &algoScoreVec)
{

    // fill algoScoreVec vector with result data
    for (const auto &algoTravelPair : algorithmTravelResults)
    {
        int operationsSum = 0;
        int errorsSum = 0;
        string travelName;

        for (const auto &travelResultPair : algoTravelPair.second)
        {
            int travelOperations;
            travelName = travelResultPair.first;
            travelOperations = travelResultPair.second;
            (travelOperations < 0) ? errorsSum++ : operationsSum += travelOperations;
        }

        algoScoreVec.emplace_back(travelName, operationsSum, errorsSum);
    }

    // sort the vector
    std::sort(algoScoreVec.begin(), algoScoreVec.end(),
              [](auto &left, auto &right)
              {
                  int leftNumOfErrors, leftNumOfOperations;
                  int rightNumOfErrors, rightNumOfOperations;

                  std::tie(std::ignore, leftNumOfErrors, leftNumOfOperations) = left;
                  std::tie(std::ignore, rightNumOfErrors, rightNumOfOperations) = right;

                  if (leftNumOfErrors != rightNumOfErrors)
                      return leftNumOfErrors < rightNumOfErrors;

                  return leftNumOfOperations < rightNumOfOperations;
              });

}


void Simulation::writeSimulationOutput(const string &outputDirPath)
{

    //sort first by error, than by num of operations
    std::vector<std::tuple<string, int, int>> algoScoreVec;
    getSortedResultVec(algoScoreVec);

    const std::string outputFileName = "simulation.results";
    const std::string outputFileFullPath = outputDirPath + "/" + outputFileName;

    //algorithmPathIdxMap

    std::ofstream outputFile;
    outputFile.open(outputFileFullPath);

    outputFile << "RERULTS" << CSV_DELIM;
    for (const auto &travelPath : allTravels)
    {
        outputFile << travelPath << CSV_DELIM;
    }
    outputFile << "Sum, Num Errors\n";

    for (const auto &algoScore : algoScoreVec)
    {
        (void) algoScore;
        const std::string algoName;
        int sumOfOperations, sumOfErrors;
        std::tie(algoName, sumOfOperations, sumOfErrors);

        outputFile << algoName << CSV_DELIM;


        for (const auto &travelName: allTravels)
        {
            const bool travelInAlgo = algorithmTravelResults[algoName].find(travelName) !=
                                      algorithmTravelResults[algoName].end();

            int numOfOperations = (travelInAlgo) ? algorithmTravelResults[algoName][travelName] : -1;
            outputFile << CSV_DELIM << numOfOperations << CSV_DELIM;
        }

        outputFile << sumOfOperations << CSV_DELIM << sumOfErrors << "\n";

    }


    outputFile.close();


}


void validateIfAlgorithmSucceeded(int algorithmReturnFlag, const string &currInputPath)
{
    if (algorithmReturnFlag)
    {
        log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
    }
}

void validateLastPort(const string &currInputPath, bool cargoFileExists, bool lastPortVisit)
{

    if (cargoFileExists && lastPortVisit)
    {
        log("Last visited port should not have a file for it", MessageSeverity::WARNING);
    }

    if (!cargoFileExists && !lastPortVisit)
    {
        log("This port visit has no file for it (expected: " + currInputPath + "). Unloading Only",
            MessageSeverity::WARNING);
    }

}


void Simulation::updateVisitedPortsMap(const SeaPortCode &port)
{
    const string &portStr = port.toStr();
    if (visitedPorts.find(port.toStr()) != visitedPorts.end())
    {
        visitedPorts[portStr]++;
    }
    else
    {
        visitedPorts[portStr] = 1;
    }
}

bool SetSimulatorCmdParams(char **argv, int argc,
                           std::string &travel_path,
                           std::string &algorithm_path)
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
            algorithm_path = args[++idx];
        }
        else
        {
            std::cout << "ERROR: Unknown argument:" << args[idx] << std::endl;
            showHelp = true;
            break;
        }
        idx++;
    }


    if (showHelp || !travelPathFound)
    {
        std::cout << "Usage: simulator [-travel_path <path>] [-algorithm_path <algorithm path>] [-output <output path>]"
                  << std::endl;
        return false;
    }

    return true;
}



int Simulation::performAndValidateAlgorithmInstructions(const string &portFilePath, const string &instructionsFilePath, const SeaPortCode &curPort)
{
/* The simulator will check the following
 * legal output format
 * x,y legal coordinates
 * UNLOAD - legal pull in UNLOAD(from the top) using the container Id
 * LOAD - legal insert (at the top of the coordinate) using the container Id
 * Valid Container Id
 * */

    vector<vector<string>> vecLinesPort;
    vector<vector<string>> vecLinesInstructions;

//    if (!readToVecLine(portFilePath, vecLinesPort))
//    {
//        simValidator.errorHandle.reportError(Errors::containerFileCannotBeRead);
//        return false;
//    }

    readToVecLine(instructionsFilePath, vecLinesInstructions);

    int instructionCounter = 0;

    for (const auto &vecLineIns : vecLinesInstructions)
    {
        for(const auto &portContainerLine : vecLinesPort)
        {
            if (portContainerLine[0] == vecLineIns[0])
            {
                if (!validateInstructionLine(vecLineIns, portContainerLine, curPort))
                {
                    return -1;
                }
                break;
            }
        }

       instructionCounter++;
    }
    return instructionCounter;
}


void Simulation::runAlgorithmTravelPair(const string &travelDirPath,
                                        AbstractAlgorithm &algorithm, const string &outputDirPath)
{


    // TODO: turn a algorithm path to a algorithm

    const string algoPath = "algo1./file.txt";
    //std::unique_ptr <AbstractAlgorithm> loadedAlgoPtr = algorithmFactories[0]();
    // init simulator for this travel
    initTravel(travelDirPath);

    // init algorithm for this travel
    algorithm.readShipPlan(travelDirPath + "/shipPlan.csv");
    algorithm.readShipRoute(travelDirPath + "/routeFile.csv");

    // traverse each port
    for (const SeaPortCode &port : shipRoute)
    {
        // update visited ports map
        updateVisitedPortsMap(port);

        // get the input and output for the algorithm "getInstructionsForCargo"
        string currInputPath, currOutputPath;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(port, visitedPorts[port.toStr()]);

        // set boolean variables
        const bool lastPortVisit = isLastPortVisit(port);
        const bool cargoFileExists = popRouteFileSet(currInputPath);

        // call algorithms' get instruction for cargo
        const int algorithmReturnFlag = algorithm.getInstructionsForCargo(currInputPath, currOutputPath);

        // validations regarding the last port and existence of expected file
        validateLastPort(currInputPath, cargoFileExists, lastPortVisit);

        // validate return code of the Algorithm
        validateIfAlgorithmSucceeded(algorithmReturnFlag, currInputPath);

        // Go through the instruction output of the algorithm and approve every move
        const int numOfOperations = performAndValidateAlgorithmInstructions(currInputPath, currOutputPath, port);
        //const string algoName = getPathFileName(algoPath);
        const string algoName = getDirectoryOfPath(algoPath);
        const string travelName = getDirectoryOfPath(curTravelFolder);
        algorithmTravelResults[algoName][travelName] = numOfOperations;
    }

    validateAllCargoFilesWereUsed();


}


void Simulation::runAlgorithmOnTravels(const string &travelsRootDir,
                                       AbstractAlgorithm &algorithm, const string &outputDirPath)
{

    vector<string> travelDirPaths;
    putDirFileListToVec(travelsRootDir, travelDirPaths);
    for (const auto &travelFolder :travelDirPaths)
    {
        allTravels.push_back(travelFolder);
        runAlgorithmTravelPair(travelFolder, algorithm, outputDirPath);
        log("Travel Finished Successfully!!!");
    }

    (void)outputDirPath;
    //writeSimulationOutput(outputDirPath);
}


void Simulation::loadAlgorithms(const string &dirPath)
{

    vector<string> algoFilesVec;
    putDirFileListToVec(dirPath, algoFilesVec, ".so");

    auto &registrar = AlgorithmRegistrar::getInstance();

    for (auto &soFile: algoFilesVec)
    {

        if (!registrar.loadSharedObject(soFile))
        {
            //TODO: report loading error
            continue;
        }

        int addedAlgorithms = registrar.howManyAdded();  // how many algorithms were added

        if (addedAlgorithms != 1)
        {
            //TODO: report {addedAlgorithms} so files were registered, while expecting only 1 file.
            continue;
        }

        algorithmFactories.push_back(registrar.getLast());
        algorithmNames.push_back(soFile);

    }

}


//Or's functions:

bool Simulation::validateUnload(const string &id, XYCord xyCord, const SeaPortCode &curPort)
{
    std::ostringstream msg;
    // check if container can be unloaded from the ship with the given coordinates
    unsigned maxFloor = shipPlan->getUpperCellsMat()[xyCord] -1;
    if(maxFloor == -1) //i.e there are no containers in that coordinate
    {
        msg << "No Containers to unload in that coordinate";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }
    auto containerOnShip =  shipPlan->getCargo()[xyCord][maxFloor];
    if(containerOnShip->getID() != id)
    {
        msg << "Container isn't on the top floor. Cannot unload container.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if container's dest port is compatible with current port
    string containerPort = containerOnShip->getDestinationPort().toStr();
    if(containerPort != curPort.toStr())
    {
        msg << "Container's dest port differs from current port. Cannot unload container.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //container has passed all simulator validations and can be unloaded!
    msg << "Container passed!.";
    log(msg.str(), MessageSeverity::INFO);
    Crane::performUnload(shipPlan, xyCord);
    return true;
}

bool Simulation::validateLoad(const string &id, XYCord xyCord, const vector<string> &portContainerLine)
{
    std::ostringstream msg;
    //check container is valid
    if (!simValidator.validateContainerFromFile(portContainerLine, shipRoute))   ///re-check with shipRoute
        {
            msg << "Rejecting Container. It has illegal format.";
            log(msg.str(), MessageSeverity::WARNING);
            return false;
        }

    Container newContainer = Container(portContainerLine);

    // check if container has any duplicates on ship
    if(!simValidator.validateDuplicateIDOnShip(id, shipPlan))
    {
        msg << "Rejecting Container. It's id has a duplicate on ship.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is full
    if(simValidator.validateShipFull(shipPlan))
    {
        msg << "Ship is full. Cannot load container.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is empty in given position
    unsigned freeSpot = shipPlan->getUpperCellsMat()[xyCord];
    unsigned maxHeight = shipPlan->getNumOfFloors(xyCord);
    if(freeSpot == maxHeight)
    {
        msg << "Position is full. Cannot load container to that position.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //container has passed all simulator validations and can be loaded!
    msg << "Container passed!.";
    log(msg.str(), MessageSeverity::INFO);
    Crane::performLoad(shipPlan, newContainer, xyCord);
    return true;
}

bool Simulation::validateMove(const string &id)
{
    //TODO: implement
    return true;
}

bool Simulation::validateReject(const string &id, const std::vector<std::string> &portContainerLine)
{
    std::ostringstream msg;
    //check if container is invalid
    bool validContainerProperties = simValidator.validateContainerFromFile(portContainerLine, shipRoute);   ///re-check with shipRoute

    // check if container has any duplicates on ship
    bool nonDuplicatesOnShip = simValidator.validateDuplicateIDOnShip(id, shipPlan);

//    // check if container id has illegal ISO 6346 format
//    bool illegalID = !simValidator.validateContainerID(id);

    //check if ship is full
    bool shipNotFull = !simValidator.validateShipFull(shipPlan);

    //check if there are no containers with further destports
    //container has passed all simulator validations and can be loaded!
    msg << "Container passed!. It really is rejected.";
    log(msg.str(), MessageSeverity::INFO);
    bool isAllValid = validContainerProperties && nonDuplicatesOnShip && shipNotFull;
    return !(isAllValid);
}

bool Simulation::validateInstructionLine(const vector <string> &instructionLine, const vector <string> &portContainerLine, const SeaPortCode &curPort)

{
    /* instructionLine.size() = 4
     * [0] = U/L/M/R
     * [1] = id
     * [2] = x cord
     * [3] = y cord
     * */
    char cmd = instructionLine[0][0];
    string id = instructionLine[1];
    unsigned xCord = std::stoul(instructionLine[2]);
    unsigned yCord = std::stoul(instructionLine[3]);
    XYCord xyCord = {xCord, yCord};
    if (cmd == 'U') {
        return validateUnload(id, xyCord, curPort);
    }
    if (cmd == 'L') {
        return validateLoad(id, xyCord, portContainerLine);
    }
    if (cmd == 'M')
    {
        return validateMove(id);
    }
    if(cmd == 'R')
    {
        return validateReject(id, portContainerLine);
    }
    return true;
}
