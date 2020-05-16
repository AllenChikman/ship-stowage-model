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
//bool Container::isValidID(const string& id)  {
//    std::map<char, int> mymap = {{'A',10}, {'B',12}, {'C',13}, {'D',14}, {'E',15}, {'F',16}, {'G',17}, {'H',18}, {'I',19}, {'J',20}, {'K',21}, {'L',23}, {'M',24}, {'N',25}, {'O',26}, {'P',27}, {'Q',28}, {'R',29}, {'S',30}, {'T',31}, {'U',32}, {'V',34}, {'W',35}, {'X',36}, {'Y',37}, {'Z',38}, {'0',0},{'1',1},{'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},{'8',8},{'9',9}};
//    std::regex reg = std::basic_regex("[A-Z]{3}[UZJ][0-9]{6}[0-9]");
//    std::cout << std::regex_match(id, reg) << std::endl;
//    double sum = 0;
//    int n = (int)id.length() - 1;
//    double tmp = 0.0;
//    for (int i = 0; i < n; i++)
//    {
//        int u = mymap[id[i]] * pow(2,i);
//        sum += u;
//    }
//    tmp = sum/11;
//    tmp = floor(tmp);
//    tmp = tmp * 11;
//    std::cout << ((sum-tmp) == mymap[(id[n])]) << std::endl;
//}