#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <utility>
#include <string>
#include <vector>
#include "Container.h"
#include "../interfaces/WeightBalanceCalculator.h"


struct XYCord
{
    unsigned x;
    unsigned y;
};


struct UIntMat
{
    std::vector<std::vector<unsigned>> mat;

    explicit UIntMat(unsigned width, unsigned length)
            : mat(std::vector<std::vector<unsigned>>(width, std::vector<unsigned>(length, 0))) {}

    std::vector<unsigned> &operator[](std::size_t idx) { return mat[idx]; }

    const std::vector<unsigned> &operator[](std::size_t idx) const { return mat[idx]; }

    unsigned &operator[](XYCord cord) { return mat[cord.x][cord.y]; }

    const unsigned &operator[](XYCord cord) const { return mat[cord.x][cord.y]; }

};

struct CargoMat
{
    std::vector<std::vector<std::vector<std::optional<Container>>>> tripMat;

    explicit CargoMat(unsigned width, unsigned length, unsigned height)
            : tripMat(std::vector<std::vector<std::vector<std::optional<Container>>>>
                              (width, std::vector<std::vector<std::optional<Container>>>
                                      (length, std::vector<std::optional<Container>>
                                              (height, std::nullopt)))) {}

    std::vector<std::vector<std::optional<Container>>> &operator[](std::size_t idx) { return tripMat[idx]; }

    const std::vector<std::vector<std::optional<Container>>> &operator[](std::size_t idx) const { return tripMat[idx]; }

    std::vector<std::optional<Container>> &operator[](XYCord cord) { return tripMat[cord.x][cord.y]; }

    const std::vector<std::optional<Container>> &operator[](XYCord cord) const { return tripMat[cord.x][cord.y]; }


};

class ShipPlan
{

private:
    const unsigned width;       //x
    const unsigned length;      //y
    const unsigned maxHeight;   //z

    std::vector<XYCord> shipXYCords;

    CargoMat cargo;
    UIntMat upperCellsMat;

    WeightBalanceCalculator balanceCalculator;

    void fillShipXYCords()
    {
        for (unsigned i = 0; i < width; ++i)
        {
            for (unsigned j = 0; j < length; ++j)
            {
                shipXYCords.push_back(XYCord{i, j});
            }
        }
    }

public:
    ShipPlan(unsigned width, unsigned length, unsigned maximalHeight,
             WeightBalanceCalculator balanceCalculator)
            : width(width),
              length(length),
              maxHeight(maximalHeight),
              shipXYCords(std::vector<XYCord>(0)),
              cargo(width, length, maximalHeight),
              upperCellsMat(width, length),
              balanceCalculator(balanceCalculator)
    {
        fillShipXYCords();
    }

    ~ShipPlan() = default;

    unsigned getMaxHeight() { return maxHeight; }

    unsigned getNumOfFloors(XYCord xyCord) { return static_cast<unsigned int>(cargo[xyCord].size()); }

    const std::vector<XYCord> &getShipXYCordsVec() { return shipXYCords; }

    UIntMat &getUpperCellsMat() { return upperCellsMat; }

    CargoMat &getCargo() { return cargo; }

    WeightBalanceCalculator getBalanceCalculator() { return balanceCalculator; }

};

#endif //SHIP_STOWAGE_MODEL_SHIP_H