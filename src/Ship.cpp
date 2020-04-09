#include <regex>
#include<ostream>
#include <iostream>
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

//void ShipPlan::printShipPlan()
//{
//    unsigned maxLength = getLength(), maxWidth = getWidth();
//    for(int x=0; x<maxLength; x++)
//    {
//        std::cout << "|";
//        for(int y=0; y<maxWidth; y++)
//        {
//            std::cout << getStartingHeight()[x][y] << "|" ;
//        }
//        std::cout << "\n" << "\n" ;
//    }
//}