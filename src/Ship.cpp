//
// Created by Allen on 21/03/2020.
//

#include "Ship.h"

Container::Container(int weight, string destinationPort, string id) {

}



//TODO: unit-test this c'tor to see elements are well initialized
FloorPlan::FloorPlan(int width, int length): width(width),length(length) {

/*    cargo.resize(static_cast<unsigned long long int>(width));
    for (auto &v : cargo){
        v.resize(static_cast<unsigned long long int>(length));
    }*/

}

FloorPlan::~FloorPlan() {}


Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
