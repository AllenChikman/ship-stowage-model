#include "StowageAlgorithm.h"
#include "Simulation.h"

void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath) {
    std::vector<std::vector<std::string>> vecLines;
    readToVecLine(inputPath, vecLines);
    std::vector<CargoData> cargoData;

    for (const auto &lineVec : vecLines) {
        cargoData.push_back({lineVec[0], stringToUInt(lineVec[1]), lineVec[2]});
    }

}