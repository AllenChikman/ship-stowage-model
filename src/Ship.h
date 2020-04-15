#include <utility>

#ifndef SHIP_STOWAGE_MODEL_SHIP_H
#define SHIP_STOWAGE_MODEL_SHIP_H


#include <utility>
#include <string>
#include <vector>
#include "Container.h"
#include "WeightBalanceCalculator.h"


struct XYCord
{
    unsigned x;
    unsigned y;
};


struct UIntMat
{
    std::vector<std::vector<unsigned>> mat;

    explicit UIntMat(unsigned long long int n, const std::vector<unsigned> &vec)
            : mat(std::vector<std::vector<unsigned>>(n, vec)) {}

    std::vector<unsigned> &operator[](std::size_t idx) { return mat[idx]; }

    const std::vector<unsigned> &operator[](std::size_t idx) const { return mat[idx]; }

    unsigned &operator[](XYCord cord) { return mat[cord.x][cord.y]; }

    const unsigned &operator[](XYCord cord) const { return mat[cord.x][cord.y]; }

};


class ShipPlan
{

private:
    const unsigned width;     //x
    const unsigned length;    //y
    const unsigned height;    //z
    std::vector<XYCord> shipXYCords;
    //std::vector<XYCord> shipXYZCords;
    UIntMat startingHeight;
    UIntMat firstCellAvailable;
    CargoMat cargo;
    ShipWeightBalanceCalculator balanceCalculator;

public:
    ShipPlan(unsigned width, unsigned length, unsigned height, const UIntMat &startingHeight,
             ShipWeightBalanceCalculator balanceCalculator)
            : width(width),
              length(length),
              height(height),
              shipXYCords(std::vector<XYCord>(0)),
              balanceCalculator(balanceCalculator),
              startingHeight(startingHeight),
              firstCellAvailable(startingHeight),
              cargo(CargoMat(width, std::vector<std::vector<std::optional<Container>>>
                      (length, std::vector<std::optional<Container>>(height, std::nullopt))))
    {
        for (unsigned i = 0; i < width; ++i)
        {
            for (unsigned j = 0; j < height; ++j)
            {
                shipXYCords.push_back(XYCord{i, j});
            }
        }
    }

    ~ShipPlan() = default;

    unsigned getWidth() { return width; }

    unsigned getLength() { return length; }

    unsigned getHeight() { return height; }

    const std::vector<XYCord> &getShipXYCordsVec() { return shipXYCords; }

    UIntMat getStartingHeight() { return startingHeight; }

    UIntMat &getFirstAvailableCellMat() { return firstCellAvailable; }

    CargoMat &getCargo() { return cargo; }

    ShipWeightBalanceCalculator getBalanceCalculator() { return balanceCalculator; }

};

#endif //SHIP_STOWAGE_MODEL_SHIP_H