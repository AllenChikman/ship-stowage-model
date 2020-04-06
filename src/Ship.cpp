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
    int x = 5;
}
unsigned ShipPlan::getWidth()
{
    return width;
}
unsigned ShipPlan::getLength()
{
    return length;
}
unsigned ShipPlan::getHeight()
{
    return height;
}
UIntMat ShipPlan::getStartingHeight()
{
    return startingHeight;
}
CargoMat ShipPlan::getCargo()
{
    return cargo;
}

//Ship

ShipPlan Ship::getShipPlan()
{
    return shipPlan;
}

//Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
Container::Container(int weight, string destinationPort, string id) : weight(weight), destinationPort(destinationPort),
                                                                      id(id) {

}
int Container::getWeight()
{
    return weight;
}
string Container::getDestinationPort()
{
    return destinationPort;
}
string Container::getID()
{
    return id;
}
