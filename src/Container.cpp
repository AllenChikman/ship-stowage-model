#include "Container.h"
#include <regex>

bool SeaPortCode::isSeaportCode(const std::string &portSymbol)
{
    if (portSymbol.size() != 5)
    {
        return false;
    }

    return std::regex_match(portSymbol, std::regex("^[A-Za-z]+$"));
}
