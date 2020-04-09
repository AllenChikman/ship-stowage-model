#include <utility>
#include <string>
#include <vector>


class SeaPortCode {
    std::string seaPortCode;

public:
    explicit SeaPortCode(std::string str) : seaPortCode(std::move(str)) {
       // std::transform(seaPortCode.begin(), seaPortCode.end(), seaPortCode.begin(), ::toupper);
    }
    static bool isSeaportCode(const std::string &portSymbol);
};

class Container {
private:
    unsigned weight{};
    SeaPortCode destinationPort;
    std::string id;

public:
    Container(unsigned weight, SeaPortCode destinationPort, std::string id)
            : weight(weight),
              destinationPort(std::move(destinationPort)),
              id(std::move(id)) {}


    unsigned getWeight() { return weight; }

    SeaPortCode getDestinationPort() { return destinationPort; }

    std::string getID() const { return id; }
};

typedef std::vector<std::vector<std::vector<Container>>> CargoMat;

struct CargoData {
    std::string id;
    unsigned weight;
    SeaPortCode destPort;
};

struct ShipCell {
    unsigned x;
    unsigned y;
    unsigned z;
};