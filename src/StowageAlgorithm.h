#include <string>
#include <fstream>
#include "Container.h"
//#include "Port.h"

enum Operation{
    U, L, M, R
};



class Algorithm{
public:
    ShipPlan *shipPlan;
    SeaPortCode curSeaPortCode;
    void operateOnShip(const Container &container, std::ofstream &instructions, Operation op = Operation::U,
            unsigned x = 0, unsigned y = 0, unsigned z = 0, unsigned unloadContainersToMove = 0);
    void getInstructionsForCargo(const std::string &inputPath, const std::string &outputPath);
};



