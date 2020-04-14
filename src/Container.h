
#ifndef SHIP_STOWAGE_MODEL_CONTAINER_H
#define SHIP_STOWAGE_MODEL_CONTAINER_H

#include <utility>
#include <string>
#include <vector>


class SeaPortCode
{
    std::string seaPortCode;

public:
    explicit SeaPortCode(std::string str) : seaPortCode(std::move(str))
    {
        // std::transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
    }

    static bool isSeaportCode(const std::string &portSymbol);

    const std::string &toStr() const { return seaPortCode; }

};

class Container
{
private:
    std::string id;
    unsigned weight{};
    SeaPortCode destinationPort;

public:
    Container(std::string id, unsigned weight, SeaPortCode destinationPort)
            : weight(weight),
              destinationPort(std::move(destinationPort)),
              id(std::move(id)) {}


    unsigned getWeight() { return weight; }

    SeaPortCode getDestinationPort() const { return destinationPort; }

    std::string getID() const { return id; }

    bool hasWrongID() { return false; }   //#TODO implement this func
};

typedef std::vector<std::vector<std::vector<Container>>> CargoMat;

#endif //SHIP_STOWAGE_MODEL_CONTAINER_H*/
