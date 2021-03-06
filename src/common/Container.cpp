#include "Container.h"
#include <algorithm>
#include <regex>
#include <cmath>

//SeaPortCode functions
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


inline int code(char ch)
{
    int firstCode = 10;
    if (ch <= '9')
        return ch - '0';

    // Following implements the encoding table above - Hard coding the table is perfectly fine and provides better runtime efficiency!
    auto characterPos = static_cast<float>(ch - 'A');
    return firstCode + characterPos + std::ceil(characterPos / firstCode);
}

bool checkDigit(const std::string &id)
{
    int checkDigitPos = 10;
    int checkDigitMultiplierBase = 2;
    int divisionValue = 11;
    int numericBase = 10;
    int sum = 0;
    for (int i=0; i<checkDigitPos; ++i)
    {
        sum += pow(checkDigitMultiplierBase, i) * code(id[i]);
    }

    int check_digit = sum - std::floor(sum / divisionValue) * divisionValue;
    return (check_digit % numericBase) == code(id[checkDigitPos]);
}

bool Container::isValidID() const
{
    std::string ownerCode = id.substr(0, 3);
    std::string categoryId = id.substr(3, 1);
    std::string serialNum = id.substr(4, 6);
    std::string checkDig = id.substr(10, 1);

    bool sameSec1 = std::regex_match(ownerCode, std::regex("^[A-Z]+$"));
    bool sameSec2 = std::regex_match(categoryId, std::regex("^[U,Z,J]+$"));
    bool sameSec3 = std::regex_match(serialNum, std::regex("^[0-9]+$"));
    bool sameSec4 = checkDigit(id);
    return sameSec1 && sameSec2 && sameSec3 && sameSec4;
}

