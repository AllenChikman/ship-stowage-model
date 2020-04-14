#include "Container.h"
#include <algorithm>
#include <regex>


SeaPortCode::SeaPortCode(std::string str)
{
    origStrName = str;
    seaPortCode = std::move(str);
    std::transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
}


bool SeaPortCode::isValidCode(const std::string &portSymbol)
{
    if (portSymbol.size() != 5) { return false; }
    return std::regex_match(portSymbol, std::regex("^[A-Za-z]+$"));
}
