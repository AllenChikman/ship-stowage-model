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
    return this->width;
}
unsigned ShipPlan::getLength()
{
    return this->length;
}
unsigned ShipPlan::getHeight()
{
    return this->height;
}
UIntMat ShipPlan::getStartingHeight()
{
    return this->startingHeight;
}
CargoMat ShipPlan::getCargo()
{
    return this->cargo;
}

//Ship

ShipPlan Ship::getShipPlan()
{
    return this->shipPlan;
}

//Ship::Ship(const WeightBalance &balanceCalculator) : balanceCalculator(balanceCalculator) {}
Container::Container(int weight, string destinationPort, string id) : weight(weight), destinationPort(destinationPort),
                                                                      id(id) {

}
int Container::getWeight()
{
    return this->weight;
}
string Container::getDestinationPort()
{
    return this->destinationPort;
}
string Container::getID()
{
    return this->id;
}
