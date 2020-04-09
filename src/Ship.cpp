#include <regex>
#include "Ship.h"


//TODO: unit-test this c'tor to see elements are well initialized


bool SeaPortCode::isSeaportCode(const std::string &portSymbol) {
    if (portSymbol.size() != 5) {
        return false;
    }

    return std::regex_match(portSymbol, std::regex("^[A-Za-z]+$"));
}
