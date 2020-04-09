#include <utility>
#include <string>
#include <vector>


class Container {
private:
    unsigned weight{};
    std::string destinationPort;
    std::string id;

public:
    Container() = default;

    Container(unsigned weight, std::string destinationPort, std::string id)
            : weight(weight),
              destinationPort(std::move(destinationPort)),
              id(std::move(id)) {}


    int getWeight() { return weight; }

    std::string getDestinationPort() { return destinationPort; }

    std::string getID() { return id; }
};

typedef std::vector<std::vector<std::vector<Container>>> CargoMat;

struct CargoData {
    std::string id;
    unsigned weight;
    std::string destPort;
};