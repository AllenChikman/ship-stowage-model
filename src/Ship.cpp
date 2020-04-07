#include <utility>
#include <regex>
#include "Ship.h"


//TODO: unit-test this c'tor to see elements are well initialized
ShipPlan::ShipPlan(unsigned width, unsigned length,
                   unsigned height, UIntMat startingHeight) : width(width),
                                                              length(length),
                                                              height(height),
                                                              startingHeight(std::move(startingHeight)) {

    cargo = CargoMat(width, vector<vector<Container>>(length, vector<Container>(height)));
}

//Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
Container::Container(unsigned weight, const string &destinationPort,
                     const string &id) : weight(weight),
                                         destinationPort(destinationPort),
                                         id(id) {

}

bool SeaPortCode::isSeaportCode(const std::string &portSymbol) {
    if (portSymbol.size() != 5) {
        return false;
    }

    return std::regex_match(portSymbol, std::regex("^[A-Za-z]+$"));
}
