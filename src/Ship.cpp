#include <regex>
#include "Ship.h"

bool SeaPortCode::isSeaportCode(const std::string &portSymbol) {
    if (portSymbol.size() != 5) {
        return false;
    }

    return std::regex_match(portSymbol, std::regex("^[A-Za-z]+$"));
}

/*Ship::Ship(const vector<SeaPortCode> &shipRoute, ShipPlan shipPlan,
        WeightBalance &balanceCalculator):shipRoute(shipRoute),
                                            shipPlan(shipPlan),
                                            balanceCalculator(balanceCalculator){}*/

