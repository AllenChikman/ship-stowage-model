
#ifndef SHIP_STOWAGE_MODEL_CONTAINER_H
#define SHIP_STOWAGE_MODEL_CONTAINER_H

#include <utility>
#include <string>
#include <vector>

class SeaPortCode
{
    std::string seaPortCode;

public:
    explicit SeaPortCode(std::string str);

    static bool isValidCode(const std::string &portSymbol);

    const std::string &toStr() const { return seaPortCode; }

};

class Container
{
private:
    std::string id;
    unsigned weight;
    SeaPortCode destinationPort;

public:
    Container(std::string id, unsigned weight, SeaPortCode destinationPort)
            : id(std::move(id)),
              weight(weight),
              destinationPort(std::move(destinationPort)) {}


    unsigned getWeight() { return weight; }

    SeaPortCode getDestinationPort() const { return destinationPort; }

    std::string getID() const { return id; }

    bool hasWrongID() { return false; }   //#TODO implement this func
};

typedef std::vector<std::vector<std::vector<Container>>> CargoMat;

#endif //SHIP_STOWAGE_MODEL_CONTAINER_H*/
