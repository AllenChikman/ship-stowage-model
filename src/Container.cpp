#include "Container.h"
#include <algorithm>
#include <regex>
#include <string>
#include <map>
#include <math.h>


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
bool validateCheckDigit(const std::string &id)
{
    std::map<char, int> letterMap = {{'A',10}, {'B',12}, {'C',13}, {'D',14}, {'E',15}, {'F',16}, {'G',17}, {'H',18}, {'I',19}, {'J',20}, {'K',21}, {'L',23}, {'M',24}, {'N',25}, {'O',26}, {'P',27}, {'Q',28}, {'R',29}, {'S',30}, {'T',31}, {'U',32}, {'V',34}, {'W',35}, {'X',36}, {'Y',37}, {'Z',38}, {'0',0},{'1',1},{'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},{'8',8},{'9',9}};
    double sum = 0.0, tmp = 0.0;
    int n = (int)id.length() - 1;

    for (int i = 0; i < n; i++)
    {
        sum += letterMap[id[i]] * (int)pow(2,i);
    }
    tmp = floor(sum/11) * 11;
    return (sum-tmp) == letterMap[id[n]];
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
    bool sameSec4 = std::regex_match(checkDig, std::regex("^[0-9]+$"));
//    bool sameSec4 = validateCheckDigit(ID);
    return sameSec1 && sameSec2 && sameSec3 && sameSec4;
}


