#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "filesystem"
#include <map>

#include "Simulation.h"
#include "AlgorithmRegistrar.h"
#include "../Common/Utils.h"
#include "../Common/Port.h"

#ifndef LINUX_ENV

#include "../Algorithms/NaiveAlgorithm.h"

#endif


// Simulation private class method implementation

std::pair<string, string> Simulation::getPortFilePaths(const string &outputDir,
                                                       const SeaPortCode &port, int numOfVisits)
{
    const string &inputFileName = port.toStr(true);
    const string &outputFileName = port.toStr();

    const string visitsStr = "_" + std::to_string(numOfVisits);

    string inputPath = curTravelFolder + '/' + inputFileName + visitsStr + ".cargo_data";
    string outputPath = outputDir + '/' + outputFileName + visitsStr + ".crane_instructions";

    return std::make_pair(inputPath, outputPath);

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
        // TODO: write to error file
        // TODO: Check if the files correspond to a stop in the route file or not
        log("File: " + file + " was not used and ignored (not in route or too many files for port)",
            MessageSeverity::WARNING);
    }
}

void Simulation::initSimulationTravelState(const string &travelDir)
{
    logStartingDecorator();
    log("Initializing travel...");
    log("Travel Root Folder is: " + travelDir);
    log("Output Folder is: " + travelDir + "/output");

    // clear
    curTravelFolder = travelDir;
    shipRoute.clear();
    visitedPorts.clear();
    cargoFilesSet.clear();
    routeMap.clear();
    simValidator.clear();

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

int Simulation::readShipPlan(const string &path)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(path, vecLines))
    {
        simValidator.errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return simValidator.getErrorBits();
    }
    auto shipPlanData = vecLines[0];

    if (!simValidator.validateShipPlanFirstLine(shipPlanData))
    {
        return simValidator.getErrorBits();
    }

    unsigned maximalHeight = stringToUInt(shipPlanData[0]);
    unsigned width = stringToUInt(shipPlanData[1]);
    unsigned length = stringToUInt(shipPlanData[2]);

    unsigned x;
    unsigned y;
    unsigned numOfFloors;

    vecLines.erase(vecLines.begin());
    shipPlan = std::make_shared<ShipPlan>(width, length, maximalHeight, WeightBalanceCalculator());
    CargoMat &cargoMat = shipPlan->getCargo();

    vector<vector<string>> validVecLines;

    if(!simValidator.validateDuplicateXYCordsWithDifferentData(vecLines, validVecLines))
    {
        return simValidator.getErrorBits();
    }

    for (const auto &vecLine : validVecLines)
    {
//        if (!simValidator.validateShipPlanFloorsFormat(vecLine)) { continue; }
        x = stringToUInt(vecLine[0]);
        y = stringToUInt(vecLine[1]);
        numOfFloors = stringToUInt(vecLine[2]);

        if (simValidator.validateShipHeightInput(maximalHeight, x, y, numOfFloors) &&
            simValidator.validateShipXYCords(width, length, x, y))
        {
            cargoMat[x][y].resize(numOfFloors);
        }
    }

    return simValidator.getErrorBits();

}

int Simulation::readShipRoute(const string &path)
{
    vector<string> vec;
    if (!readToVec(path, vec))
    {
        simValidator.errorHandle.reportError(Errors::emptyFileOrRouteFileCannotBeRead);
        return simValidator.getErrorBits();
    }

    if (!simValidator.validateAmountOfValidPorts(vec))
    {
        return simValidator.getErrorBits();
    }
    if (!simValidator.validateSamePortInstancesConsecutively(vec))
    {
        clearDuplicatedConsecutiveStrings(vec);
    }
    for (const auto &portSymbol : vec)
    {
        if (simValidator.validatePortFormat(portSymbol))
            shipRoute.emplace_back(portSymbol);
    }
    updateRouteMap();
    updateRouteFileSet();

    return 0;
}



/////// EX2 Part


void writeToErrorFile(const string &errorDirectoryPath, const string &errorFilePath, const string &text)
{
    //if there were errors create the error directory
    createDirIfNotExists(errorDirectoryPath);
    std::ofstream errorFile(errorFilePath, std::ios_base::app);
    errorFile << text << std::endl;
    errorFile.close();

}


void Simulation::getSortedResultVec(std::vector<std::tuple<string, int, int>> &algoScoreVec)
{

    // fill algoScoreVec vector with result data
    for (const auto &algoTravelPair : algorithmTravelResults)
    {
        int operationsSum = 0;
        int errorsSum = 0;
        string algoName = algoTravelPair.first;
        string travelName;

        for (const auto &travelResultPair : algoTravelPair.second)
        {
            int travelOperations;
            travelName = travelResultPair.first;
            travelOperations = travelResultPair.second;
            (travelOperations < 0) ? errorsSum++ : operationsSum += travelOperations;
        }

        algoScoreVec.emplace_back(algoName, operationsSum, errorsSum);
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
    for (const auto &travelPath : allTravelsNames)
    {
        outputFile << travelPath << CSV_DELIM;
    }
    outputFile << "Sum, Num Errors\n";

    for (const auto &algoScore : algoScoreVec)
    {
        std::string algoName;
        int sumOfOperations, sumOfErrors;
        std::tie(algoName, sumOfOperations, sumOfErrors) = algoScore;

        outputFile << algoName << CSV_DELIM;


        for (const auto &travelName: allTravelsNames)
        {
            const bool travelInAlgo = algorithmTravelResults[algoName].find(travelName) !=
                                      algorithmTravelResults[algoName].end();

            int numOfOperations = (travelInAlgo) ? algorithmTravelResults[algoName][travelName] : -1;
            outputFile << numOfOperations << CSV_DELIM;
        }

        outputFile << sumOfOperations << CSV_DELIM << sumOfErrors << "\n";

    }


    outputFile.close();


}


void Simulation::handleCargoFileExistence(const string &currInputPath, bool cargoFileExists, bool lastPortVisit)
{

    if (!cargoFileExists)
    {
        createEmptyFile(currInputPath);
        if (!lastPortVisit)
        {
            log("This port visit has no file for it (expected: " + currInputPath + "). Creating empty file",
                MessageSeverity::WARNING);
        }

    }

}


void handleAlgorithmReturnCode(int algorithmReturnFlag, const string &currInputPath)
{
    if (algorithmReturnFlag)
    {
        log("Failed to get instruction for cargo from file: " + currInputPath, MessageSeverity::WARNING);
    }
}


void Simulation::updateResults(const string &algoName, const string &travelName, int numOfOperations)
{
    const bool hadErrors = (algorithmTravelResults[algoName][travelName] == -1);
    const bool isError = (numOfOperations == -1);

    if (hadErrors || isError)
    {
        algorithmTravelResults[algoName][travelName] = -1;

    }
    else
    {
        algorithmTravelResults[algoName][travelName] += numOfOperations;
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


int Simulation::performAndValidateAlgorithmInstructions(const string &portFilePath, const string &instructionsFilePath,
                                                        const SeaPortCode &curPort)
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

    if (!readToVecLine(portFilePath, vecLinesPort))
    {
        simValidator.errorHandle.reportError(Errors::containerFileCannotBeRead);
        return false;
    }

    readToVecLine(instructionsFilePath, vecLinesInstructions);

    int instructionCounter = 0;

    for (const auto &vecLineIns : vecLinesInstructions)
    {
        for (const auto &portContainerLine : vecLinesPort)
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
    if(!allContainersUnloadedAtPort(curPort))
    {
        return -1;
    }
    return instructionCounter;
}


void Simulation::runAlgorithmTravelPair(const string &travelDirPath,
                                        std::pair<AlgorithmFactory, string> &algoFactoryNamePair,
                                        const string &outputDirPath)
{

#ifdef LINUX_ENV
    std::unique_ptr<AbstractAlgorithm> algoPtr = std::get<0>(algoFactoryNamePair)();
#endif

#ifndef LINUX_ENV
    NaiveAlgorithm temp_algo;
    auto algoPtr = &temp_algo;
#endif

    // get algorithm and travel names
    const string algoName = std::get<1>(algoFactoryNamePair);
    const string travelName = getPathFileName(travelDirPath);

    // init simulator travel state
    initSimulationTravelState(travelDirPath);

    // get shipPlan and shipRoute file paths
    const string shipPlanPath = getShipPlanFilePath();
    const string shipRoutePath = getRouteFilePath();


    // init simulator for this travel
    const auto shipPlanErrCode = readShipPlan(shipPlanPath);
    if (ErrorHandle::isFatalError(shipPlanErrCode)) { return; }

    const auto shipRouteErrCode = readShipRoute(shipRoutePath);
    if (ErrorHandle::isFatalError(shipRouteErrCode)) { return; }

    // Add the current travel to the travel names
    allTravelsNames.insert(travelName);

    // init algorithm for this travel
    const auto algoPlanErrCode  = algoPtr->readShipPlan(shipPlanPath); //TODO: report error in simulator
    if (ErrorHandle::isFatalError(algoPlanErrCode)) { return; }

    const auto algoRouteErrCode  = algoPtr->readShipRoute(shipRoutePath); //TODO: report error in simulator
    if (ErrorHandle::isFatalError(algoRouteErrCode)) { return; }

    // construct the new algorithm-travel pair output dir
    const string newOutputDir = outputDirPath + "/" + algoName + "_" + travelName + "__crane_instructions";
    createDirIfNotExists(newOutputDir);

    // get the error directory and file
    const string errorDirectory = outputDirPath + "/errors";
    const string errorFile = errorDirectory + "/" + algoName + "_" + travelName + ".errors";

    // traverse each port
    for (const SeaPortCode &port : shipRoute)
    {
        // update visited ports map
        updateVisitedPortsMap(port);

        // get the input and output for the algorithm "getInstructionsForCargo"
        string currInputPath, currOutputPath;
        std::tie(currInputPath, currOutputPath) = getPortFilePaths(newOutputDir, port, visitedPorts[port.toStr()]);

        // set boolean variables
        const bool cargoFileExists = popRouteFileSet(currInputPath);
        const bool lastPortVisit = isLastPortVisit(port);

        // if cargo file doesn't exist we create a new empty file
        handleCargoFileExistence(currInputPath, cargoFileExists, lastPortVisit);

        // call algorithms' get instruction for cargo
        const int algorithmReturnCode = algoPtr->getInstructionsForCargo(currInputPath, currOutputPath);

        // handle return code of the Algorithm
        handleAlgorithmReturnCode(algorithmReturnCode, currInputPath);

        // Go through the instruction output of the algorithm and approve every move
        const int numOfOperations = performAndValidateAlgorithmInstructions(currInputPath, currOutputPath, port);

        // update results
        updateResults(algoName, travelName, numOfOperations);
    }

    validateAllCargoFilesWereUsed();


}


void Simulation::runAlgorithmsOnTravels(const string &travelsRootDir, const string &outputDirPath)
{

    createDirIfNotExists(outputDirPath);

    vector<string> travelDirPaths;
    putSubDirectoriesToVec(travelsRootDir, travelDirPaths);

#ifndef LINUX_ENV
    AlgorithmFactory mock;
    loadedAlgorithmFactories.emplace_back(mock, "algo1");
    loadedAlgorithmFactories.emplace_back(mock, "algo2");

#endif

    for (auto &loadedAlgoFactoryNamePair :loadedAlgorithmFactories)
    {
        for (const auto &travelFolder :travelDirPaths)
        {
            runAlgorithmTravelPair(travelFolder, loadedAlgoFactoryNamePair, outputDirPath);
            log("Travel Finished Successfully!!!");
        }
    }

    writeSimulationOutput(outputDirPath);
}


void Simulation::loadAlgorithms(const string &algorithmsRootDit)
{

    logStartingDecorator();
    std::cout << "Loading Algorithms: " << std::endl;

    vector<string> algoFilesVec;
    putDirFileListToVec(algorithmsRootDit, algoFilesVec, ".so");

    auto &registrar = AlgorithmRegistrar::getInstance();

    for (auto &soFilePath: algoFilesVec)
    {

        if (!registrar.loadSharedObject(soFilePath))
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

        const string algoFileName = getPathFileName(soFilePath, true);
        const auto algoFactory = registrar.getLast();
        const auto factoryNamePair = std::make_pair(algoFactory, algoFileName);

        loadedAlgorithmFactories.push_back(factoryNamePair);
        std::cout << "Loaded algorithm: " << algoFileName << std::endl;

    }

    std::cout << "Algorithms loading is finished: " << std::endl;

}


//Or's functions:

bool Simulation::validateUnload(const string &id, XYCord xyCord, const SeaPortCode &curPort)
{
    std::ostringstream msg;
    // check if container can be unloaded from the ship with the given coordinates
    unsigned maxFloor = shipPlan->getUpperCellsMat()[xyCord] - 1;
    if (maxFloor == 0) //i.e there are no containers in that coordinate
    {
        msg << "No Containers to unload in that coordinate";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }
    auto containerOnShip = shipPlan->getCargo()[xyCord][maxFloor];
    if (containerOnShip->getID() != id)
    {
        msg << "Container isn't on the top floor. Cannot unload container.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if container's dest port is compatible with current port
    string containerPort = containerOnShip->getDestinationPort().toStr();
    if (containerPort != curPort.toStr())
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
    if (!simValidator.validateDuplicateIDOnShip(id, shipPlan))
    {
        msg << "Rejecting Container. It's id has a duplicate on ship.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is full
    if (simValidator.validateShipFull(shipPlan))
    {
        msg << "Ship is full. Cannot load container.";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is empty in given position
    unsigned freeSpot = shipPlan->getUpperCellsMat()[xyCord];
    unsigned maxHeight = shipPlan->getNumOfFloors(xyCord);
    if (freeSpot == maxHeight)
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
    (void) id;
    return true;
}

bool Simulation::validateReject(const string &id, const std::vector<std::string> &portContainerLine)
{
    std::ostringstream msg;
    //check if container is invalid
    bool validContainerProperties = simValidator.validateContainerFromFile(portContainerLine,
                                                                           shipRoute);   ///re-check with shipRoute

    // check if container has any duplicates on ship
    bool nonDuplicatesOnShip = simValidator.validateDuplicateIDOnShip(id, shipPlan);

//    // check if container id has illegal ISO 6346 format
//    bool illegalID = !simValidator.validateContainerID(id);

    //check if ship is full
    bool shipNotFull = !simValidator.validateShipFull(shipPlan);

    //check if there are no containers with further destports

    //container has passed all simulator validations and can be loaded!
    bool isAllValid = validContainerProperties && nonDuplicatesOnShip && shipNotFull;
    return !(isAllValid);
}

bool Simulation::validateInstructionLine(const vector<string> &instructionLine, const vector<string> &portContainerLine,
                                         const SeaPortCode &curPort)
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

    switch (cmd)
    {
        case 'U':
            return validateUnload(id, xyCord, curPort);
        case 'L':
            return validateLoad(id, xyCord, portContainerLine);
        case 'M':
            return validateMove(id);
        case 'R':
            return validateReject(id, portContainerLine);

        default:
            return false;
    }
}

const string Simulation::getShipPlanFilePath()
{
    vector<string> shipPlansVec;
    putDirFileListToVec(curTravelFolder, shipPlansVec, ".ship_plan");

    auto numOfShipPlanFiles = shipPlansVec.size();
    switch (numOfShipPlanFiles)
    {
        case 0:
            //TODO: Or - report proper error of no shipPlans Found
            return "";

        case 1:
            return shipPlansVec[0];

        default:
            //TODO: Or - report proper error of more than 1 shipPlans Found
            return "";
    }

}

const string Simulation::getRouteFilePath()
{
    vector<string> shipRoutesVec;
    putDirFileListToVec(curTravelFolder, shipRoutesVec, ".route");

    auto numOfRouteFiles = shipRoutesVec.size();
    switch (numOfRouteFiles)
    {
        case 0:
            //TODO: Or - report proper error of no route files Found
            return "";

        case 1:
            return shipRoutesVec[0];

        default:
            //TODO: Or - report proper error of more than 1 route file Found
            return "";
    }
}

bool Simulation::allContainersUnloadedAtPort(const SeaPortCode &curPort)
{
    std::ostringstream msg;
    CargoMat cargo = shipPlan->getCargo();
    vector<XYCord> xyCordVec = shipPlan->getShipXYCordsVec();
    for(auto xyCord : xyCordVec)
    {
        unsigned maxOccupiedFloor = shipPlan->getUpperCellsMat()[xyCord];
        for (unsigned i = 0; i < maxOccupiedFloor; i++)
        {
            if (cargo[xyCord][i]->getDestinationPort().toStr() == curPort.toStr())
            {
                msg << "Algorithm didn't unload all containers at their dest port.";
                log(msg.str(), MessageSeverity::ERROR);
                return false;
            }
        }
    }
    return true;
}
