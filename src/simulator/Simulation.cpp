#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "filesystem"
#include <map>
#include <bitset>

#include "Simulation.h"
#include "AlgorithmRegistrar.h"
#include "../common/Utils.h"
#include "../common/Port.h"

#ifndef LINUX_ENV

#include "../algorithm/NaiveAlgorithm.h"

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


const string Simulation::getShipPlanFilePath(int &status)
{
    vector<string> shipPlansVec;
    putDirFileListToVec(curTravelFolder, shipPlansVec, ".ship_plan");

    auto numOfShipPlanFiles = shipPlansVec.size();
    switch (numOfShipPlanFiles)
    {
        case 0:
            simValidator.errorHandle.reportError(BadFirstLineOrShipPlanFileCannotBeRead);
            status = -1;
            writeToErrorFile("Didn't find any ship plan file. Terminating travel.",
                             MessageSeverity::ERROR, simValidator.errorFilePath);
            return "";

        case 1:
            return shipPlansVec[0];

        default:
            writeToErrorFile("found more than one ship plan file. Taken the first.",
                             MessageSeverity::ERROR, simValidator.errorFilePath);
            return shipPlansVec[0];
    }

}

const string Simulation::getRouteFilePath(int &status)
{
    vector<string> shipRoutesVec;
    putDirFileListToVec(curTravelFolder, shipRoutesVec, ".route");

    auto numOfRouteFiles = shipRoutesVec.size();
    switch (numOfRouteFiles)
    {
        case 0:
            simValidator.errorHandle.reportError(emptyFileOrRouteFileCannotBeRead);
            status = -1;
            writeToErrorFile("Didn't find any ship route file. Terminating travel.",
                             MessageSeverity::ERROR, simValidator.errorFilePath);
            return "";

        case 1:
            return shipRoutesVec[0];

        default:
            writeToErrorFile("Found more than one route file. Taken the first.",
                             MessageSeverity::ERROR, simValidator.errorFilePath);
            return shipRoutesVec[0];
    }
}


// Simulation public class method implementation

int Simulation::readShipPlan(const string &path)
{
    vector<vector<string>> vecLines;
    if (!readToVecLine(path, vecLines))
    {
        writeToErrorFile("Unable to read ship plan file", MessageSeverity::ERROR, simValidator.errorFilePath);
        simValidator.errorHandle.reportError(Errors::BadFirstLineOrShipPlanFileCannotBeRead);
        return simValidator.getErrorBits();
    }

    if (vecLines.empty())
    {
        writeToErrorFile("Unable to read ship plan file", MessageSeverity::ERROR, simValidator.errorFilePath);
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


    if (!simValidator.validateDuplicateXYCordsWithDifferentData(vecLines))
    {
        return simValidator.getErrorBits();
    }

    for (const auto &vecLine : vecLines)
    {
        if (!simValidator.validateShipPlanFloorsFormat(vecLine)) { continue; }
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

    return simValidator.getErrorBits();
}



/////// EX2 Part


void writeToAlgoErroCode(const string &outputDirPath, const string &travelName, int algorithmReturnCode)
{

    string msg;
    for (int i = 0; i <= 18; i++)
    {
        int err = 1u << i;
        if ((err & algorithmReturnCode) == err)
        {
            msg += std::to_string(i) + ", ";
        }
    }
    createDirIfNotExists(outputDirPath);
    std::ofstream algoCodeFile(outputDirPath + "/algoReturnCode.txt", std::ios_base::app);

    if (msg.empty()) { msg = "0"; }
    algoCodeFile << travelName << ": " << msg << std::endl;
    algoCodeFile.close();

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

void handleAlgorithmReturnCode(int algorithmReturnFlag, const string &currInputPath,
                               const string &errorFile)
{
    if (algorithmReturnFlag != 0)
    {
        auto msg = "Algoithm get instructions return error code: " + std::to_string(algorithmReturnFlag) +
                   +" from file: " + currInputPath;
        log(msg, MessageSeverity::WARNING);

        writeToErrorFile(msg, MessageSeverity::WARNING, errorFile);
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

    // get the error directory and file
    const string errorDirectory = outputDirPath + "/errors";
    string errorFile = errorDirectory + "/" + algoName + "_" + travelName + ".errors";
    simValidator.setErrorFile(errorFile);

    // init simulator travel state
    initSimulationTravelState(travelDirPath);

    // get shipPlan and shipRoute file paths
    int status = 0;

    const string shipPlanPath = getShipPlanFilePath(status);
    if (status == -1) { return; }

    const string shipRoutePath = getRouteFilePath(status);
    if (status == -1) { return; }

    // init simulator for this travel
    const auto shipPlanErrCode = readShipPlan(shipPlanPath);
    if (ErrorHandle::isFatalError(shipPlanErrCode)) { return; }

    const auto shipRouteErrCode = readShipRoute(shipRoutePath);
    if (ErrorHandle::isFatalError(shipRouteErrCode)) { return; }

    // Add the current travel to the travel names
    allTravelsNames.insert(travelName);

    // init algorithm for this travel
    const auto algoPlanErrCode = algoPtr->readShipPlan(shipPlanPath); //TODO: report error in simulator
    //writeToAlgoErroCode(outputDirPath, travelName + " ship plan code: ", algoPlanErrCode);
    if (ErrorHandle::isFatalError(algoPlanErrCode)) { return; }

    const auto algoRouteErrCode = algoPtr->readShipRoute(shipRoutePath); //TODO: report error in simulator
    //writeToAlgoErroCode(outputDirPath, travelName + " ship route code: ", algoRouteErrCode);
    if (ErrorHandle::isFatalError(algoRouteErrCode)) { return; }


    // construct the new algorithm-travel pair output dir
    const string newOutputDir = outputDirPath + "/" + algoName + "_" + travelName + "__crane_instructions";
    createDirIfNotExists(newOutputDir);



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
        //writeToAlgoErroCode(outputDirPath, travelName + " cargo file: " + port.toStr(), algorithmReturnCode);

        // handle return code of the Algorithm
        handleAlgorithmReturnCode(algorithmReturnCode, currInputPath, errorFile);

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
    //loadedAlgorithmFactories.emplace_back(mock, "algo2");

#endif

    for (auto &loadedAlgoFactoryNamePair :loadedAlgorithmFactories)
    {
        for (const auto &travelFolder :travelDirPaths)
        {
            runAlgorithmTravelPair(travelFolder, loadedAlgoFactoryNamePair, outputDirPath);
            log("Travel Finished!!!");
        }
    }

    writeSimulationOutput(outputDirPath);
}

void Simulation::loadAlgorithms(const string &algorithmsRootDit, const string &outputDir)
{

    logStartingDecorator();
    log("Loading Algorithms:", MessageSeverity::INFO);

    vector<string> algoFilesVec;
    putDirFileListToVec(algorithmsRootDit, algoFilesVec, ".so");

    auto &registrar = AlgorithmRegistrar::getInstance();

    for (auto &soFilePath: algoFilesVec)
    {

        if (!registrar.loadSharedObject(soFilePath))
        {
            auto msg = "Unable to load algorithm: " + getPathFileName(soFilePath, true);
            //writeToErrorFile(msg, MessageSeverity::ERROR, outputDir + "/errors/general.errors");
            (void) msg;
            (void) outputDir;
            continue;
        }

        int addedAlgorithms = registrar.howManyAdded();  // how many algorithms were added

        if (addedAlgorithms != 1)
        {
            auto msg = std::to_string(addedAlgorithms) + " algorithms registered. Unable to load algorithm: " +
                       getPathFileName(soFilePath, true);
            (void) msg;
            (void) outputDir;
            //writeToErrorFile(msg, MessageSeverity::ERROR, outputDir + "/errors/general.errors");
            continue;
        }

        const string algoFileName = getPathFileName(soFilePath, true);
        const auto algoFactory = registrar.getLast();
        const auto factoryNamePair = std::make_pair(algoFactory, algoFileName);

        loadedAlgorithmFactories.push_back(factoryNamePair);
        log("Loaded algorithm: " + algoFileName, MessageSeverity::INFO);

    }
    log("Algorithms loading is finished: ", MessageSeverity::INFO);

}

//Or's functions:




bool Simulation::validateUnload(const string &id, XYCord xyCord, const SeaPortCode &curPort,
                                std::vector<std::vector<std::string>> &moveContainers)
{
    std::ostringstream msg;

    // check if container can be unloaded from the ship with the given coordinates
    unsigned firstFreeFloor = shipPlan->getUpperCellsMat()[xyCord];
    if (firstFreeFloor == 0) //i.e there are no containers in that coordinate
    {
        msg << "No Containers to unload in that coordinate";
        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }
    unsigned maxOccupiedFloor = firstFreeFloor - 1;
    auto containerOnShip = shipPlan->getCargo()[xyCord][maxOccupiedFloor];
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
        vector<string> containerLine;
        containerLine.push_back(containerOnShip->getID());
        containerLine.push_back(std::to_string(containerOnShip->getWeight()));
        containerLine.push_back(containerOnShip->getDestinationPort().toStr());
        moveContainers.push_back(containerLine);
    }
    //container has passed all simulator validations and can be unloaded!

    Crane::performUnload(shipPlan, xyCord);
    return true;
}

bool Simulation::validateLoad(const string &id, XYCord xyCord, const vector<string> &portContainerLine)
{
    std::ostringstream msg;
    //check container is valid
    if (!simValidator.validateContainerFromFile(portContainerLine, shipRoute, false))   ///re-check with shipRoute
    {
//        msg << "Rejecting Container. It has illegal format.";
//        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    Container newContainer = Container(portContainerLine);

    // check if container has any duplicates on ship
    if (!simValidator.validateDuplicateIDOnShip(id, shipPlan))
    {
//        msg << "Rejecting Container. It's id has a duplicate on ship.";
//        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is full
    if (simValidator.validateShipFull(shipPlan))
    {
//        msg << "Ship is full. Cannot load container.";
//        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    //check if ship is empty in given position
    unsigned freeSpot = shipPlan->getUpperCellsMat()[xyCord];
    unsigned maxHeight = shipPlan->getNumOfFloors(xyCord);
    if (freeSpot == maxHeight)
    {
//        msg << "Position is full. Cannot load container to that position.";
//        log(msg.str(), MessageSeverity::WARNING);
        return false;
    }

    Crane::performLoad(shipPlan, newContainer, xyCord);
    return true;
}

bool
Simulation::validateMove(const string &id, const XYCord &xyCord_U, const SeaPortCode &curPort, const XYCord &xyCord_L)
{
    vector<vector<string>> moveContainers;
    if (!validateUnload(id, xyCord_U, curPort, moveContainers)) { return false; }
    auto line = moveContainers[0];
    if (!validateLoad(id, xyCord_L, line)) { return false; }

    Crane::performMove(shipPlan, xyCord_U, xyCord_L);
    return true;

}

bool Simulation::validateReject(const string &id, const std::vector<std::string> &portContainerLine)
{
    std::ostringstream msg;

    //check if container is invalid
    bool validContainerProperties = simValidator.validateContainerFromFile(portContainerLine,
                                                                           shipRoute,
                                                                           false);   ///re-check with shipRoute

    // check if container has any duplicates on ship
    bool nonDuplicatesOnShip = simValidator.validateDuplicateIDOnShip(id, shipPlan);

    //check if ship is full
    bool shipNotFull = !simValidator.validateShipFull(shipPlan);

    //check if there are no containers with further destination ports
    //TODO- implement

    //container has passed all simulator validations and can be loaded!
    bool isAllValid = validContainerProperties && nonDuplicatesOnShip && shipNotFull;
    return !(isAllValid);
}

bool Simulation::allContainersUnloadedAtPort(const SeaPortCode &curPort)
{
    std::ostringstream msg;
    CargoMat cargo = shipPlan->getCargo();
    vector<XYCord> xyCordVec = shipPlan->getShipXYCordsVec();
    for (auto xyCord : xyCordVec)
    {
        unsigned maxOccupiedFloor = shipPlan->getUpperCellsMat()[xyCord];
        for (unsigned floor = 0; floor < maxOccupiedFloor; floor++)
        {
            if (cargo[xyCord][floor]->getDestinationPort().toStr() == curPort.toStr())
            {
                msg << "Algorithm didn't unload all containers at their dest port." << curPort.toStr();
                log(msg.str(), MessageSeverity::ERROR);
                return false;
            }
        }
    }
    return true;
}


bool Simulation::validateInstructionLine(const vector<string> &instructionLine,
                                         const std::unordered_map<string, vector<vector<string>>> &idLinesMap,
                                         const SeaPortCode &curPort, vector<vector<string>> &moveContainers)
{
    /* instructionLine.size() = 4
     * [0] = U/L/M/R
     * [1] = id
     * [2] = x cord
     * [3] = y cord
     * */

    char cmd = instructionLine[0][0];
    string id = instructionLine[1];
    unsigned xCord = stringToUInt(instructionLine[2]);
    unsigned yCord = stringToUInt(instructionLine[3]);
    unsigned xL, yL;
    XYCord xyCord_L;

    XYCord xyCord = {xCord, yCord};
    switch (cmd)
    {
        case 'U':
            return validateUnload(id, xyCord, curPort, moveContainers);
        case 'L':
            for (auto &container : moveContainers)
            {
                if (instructionLine[1] == container[0])
                {
                    return validateLoad(id, xyCord, container);
                }
            }
            for (auto &idLines : idLinesMap)
            {
                string portContainerId = idLines.first;
                vector<vector<string>> lines = idLines.second;
                if (instructionLine[1] == portContainerId)
                {
                    return validateLoad(id, xyCord,
                                        lines[0]); //because after pos 0 the lines should definitely be rejected- duplicates
                }
            }
            break;
        case 'M':
            xL = stringToUInt(instructionLine[4]);
            yL = stringToUInt(instructionLine[5]);
            xyCord_L = {xL, yL};
            return validateMove(id, xyCord, curPort, xyCord_L);
        case 'R':
            for (auto &idLines : idLinesMap)
            {
                bool validInstruction = false;
                string portContainerId = idLines.first;
                vector<vector<string>> lines = idLines.second;
                if (instructionLine[1] != portContainerId) { continue; }
                for (auto &line : lines)
                {
                    validInstruction = validInstruction || validateReject(id, line);
                }
                return validInstruction;
            }
            break;
        default:
            return false;
    }
    return false;
}


void placeRejectsAtEnd(vector<vector<string>> &vecLinesInstructions)
{
    vector<vector<string>> tmpRejectsVec;
    vector<string> instruction;
    char cmd;
    unsigned cnt = 0;
    while (cnt < vecLinesInstructions.size())
    {
        instruction = vecLinesInstructions[cnt];
        cmd = instruction[0][0];
        if (cmd != 'R')
        {
            cnt++;
            continue;
        }
        vecLinesInstructions.erase(vecLinesInstructions.begin() + cnt);
        tmpRejectsVec.push_back(instruction);
    }
    for (auto &rejectIns : tmpRejectsVec)
    {
        vecLinesInstructions.push_back(rejectIns);
    }
}

void createIDtoPortLinesMapping(const vector<vector<string>> &containersVecLines,
                                std::unordered_map<string, vector<vector<string>>> &idLinesMap)
{
    string curContainerLineID;
//    bool duplicatedID = false;
//    unsigned linesSize = containersVecLines.size();
    for (auto &curContainerLine : containersVecLines)
    {
        curContainerLineID = curContainerLine[0];
        if (curContainerLineID.size() != 11)      //TODO replace with proper function
        {
            continue;
        }
        if (idLinesMap.find(curContainerLineID) == idLinesMap.end())
        {
            vector<vector<string>> idLines;
            idLinesMap.insert(make_pair(curContainerLineID, idLines));
        }
        idLinesMap[curContainerLineID].push_back(curContainerLine);
//        for(unsigned j=0; j<i; j++)
//        {
//            tmpContainerLine = containersVecLines[j];
//            tmpContainerLineID = tmpContainerLine[0];
//
//            if(tmpContainerLineID == curContainerLineID)
//            {
//
//                duplicatedID = true;
//                break;
//            }
//        }
//        duplicatedID ?  duplicated.push_back(curContainerLine) : rest.push_back(curContainerLine);
//        duplicatedID = false;
//    }
    }
}


int Simulation::performAndValidateAlgorithmInstructions(const string &portFilePath, const string &instructionsFilePath,
                                                        const SeaPortCode &curPort)
{
/* The simulator will check the following
 * legal output format - in advance
 * x,y legal coordinates - in advance
 * UNLOAD - legal pull in UNLOAD(from the top) using the container Id
 * LOAD - legal insert (at the top of the coordinate) using the container Id
 * Valid Container Id - in advance
 * */

    (void) portFilePath;
    (void) curPort;

    // TODO: check that every line in the file was rejected ignored or perforemd

    // read output lines to vector
    vector<vector<string>> vecLinesInstructions;
    readToVecLine(instructionsFilePath, vecLinesInstructions);

    int instructionCounter = 0;
    for (const auto &lineVec : vecLinesInstructions)
    {
        if (!lineVec.empty() && lineVec[0] != "R")
        {
            instructionCounter++;
        }
    }

    return instructionCounter;
}
