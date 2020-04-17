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
bool Container::isValidID() const
{

    if(id.size() != 11) {return false;}
    std::string ownerCode = id.substr(0, 3);
    std::string categoryId = id.substr(3, 1);
    std::string serialNum = id.substr(4, 6);
    std::string checkDig = id.substr(10, 1);

    bool sameSec1 = std::regex_match(ownerCode, std::regex("^[A-Z]+$"));
    bool sameSec2 = std::regex_match(categoryId, std::regex("^[U,Z,J]+$"));
    bool sameSec3 = std::regex_match(serialNum, std::regex("^[0-9]+$"));
    bool sameSec4 = std::regex_match(checkDig, std::regex("^[0-9]+$"));
    return sameSec1 && sameSec2 && sameSec3 && sameSec4;
}
