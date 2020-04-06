#include <utility>

//
// Created by Allen on 21/03/2020.
//

#include "Ship.h"


//TODO: unit-test this c'tor to see elements are well initialized
ShipPlan::ShipPlan(unsigned width, unsigned length, unsigned height, UIntMat startingHeight) : width(width),
                                                                                               length(length),
                                                                                               height(height),
                                                                                               startingHeight(
                                                                                                       std::move(
                                                                                                               startingHeight)) {
    cargo = CargoMat(width, vector<vector<Container>>(length, vector<Container>(height)));


}

//Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
Container::Container(int weight, string destinationPort, string id) : weight(weight), destinationPort(destinationPort),
                                                                      id(id) {

}
