#include <regex>
#include "Ship.h"


//TODO: unit-test this c'tor to see elements are well initialized

//Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
Container::Container(unsigned weight, const SeaPortCode &destinationPort,
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

Ship::Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan,
        WeightBalance &balanceCalculator):shipRoute(shipRoute),
                                            shipPlan(shipPlan),
                                            balanceCalculator(balanceCalculator){}

