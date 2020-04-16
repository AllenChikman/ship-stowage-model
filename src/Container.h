
#ifndef SHIP_STOWAGE_MODEL_CONTAINER_H
#define SHIP_STOWAGE_MODEL_CONTAINER_H

#include <utility>
#include <string>
#include <vector>
#include <optional>

class SeaPortCode
{
    std::string seaPortCode;
    std::string origStrName; // for opening user's files

public:
    explicit SeaPortCode(std::string str);

    static bool isValidCode(const std::string &portSymbol);

    const std::string &toStr(bool getOrigName = false) const { return (getOrigName) ? origStrName : seaPortCode; }

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


    unsigned getWeight() const { return weight; }

    SeaPortCode getDestinationPort() const { return destinationPort; }

    std::string getID() const { return id; }

    bool isBelongToPort(const SeaPortCode &portCode) const { return destinationPort.toStr() == portCode.toStr(); }

    bool isValidID() const { return true; }   //#TODO implement this func
};


#endif //SHIP_STOWAGE_MODEL_CONTAINER_H*/
