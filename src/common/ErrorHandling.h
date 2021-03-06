enum Errors
{
    noError = 0,
    floorsExceedMaxHeight = 1u << 0u,                     // ignore
    posExceedsXYLimits = 1u << 1u,                        // ignore
    badLineFormatAfterFirstLine = 1u << 2u,               // ignore
    BadFirstLineOrShipPlanFileCannotBeRead = 1u << 3u,    // fatal
    duplicatedXYWithDifferentData = 1u << 4u,             // fatal

    portAppearsMoreThanOnceConsecutively = 1u << 5u,      // ignore
    wrongSeaPortCode = 1u << 6u,                          // ignore
    emptyFileOrRouteFileCannotBeRead = 1u << 7u,          // fatal
    atMostOneValidPort = 1u << 8u,                        // fatal

    reserved = 1u << 9u,

    duplicateIDOnPort = 1u << 10u,                        // reject (container)
    duplicateIDOnShip = 1u << 11u,                        // reject (container)
    badContainerWeight = 1u << 12u,                       // reject (container)
    badContainerDestPort = 1u << 13u,                     // reject (container)
    ContainerIDCannotBeRead = 1u << 14u,                  // ignore
    badContainerID = 1u << 15u,                           // reject (container)
    containerFileCannotBeRead = 1u << 16u,                // ignore
    lastPortHasContainersWaiting = 1u << 17u,             // reject (container)
    containersExceedShipCapacity = 1u << 18u              // reject (container)


};

class ErrorHandle
{
private:
    unsigned errorBits;
public:
    unsigned getErrorBits() { return errorBits; }

    void clear() { errorBits = noError; }

    void reportError(Errors error) { errorBits |= error; }

    static bool isFatalError(int errorCode)
    {
        unsigned fatalErrors = BadFirstLineOrShipPlanFileCannotBeRead | duplicatedXYWithDifferentData |
                               emptyFileOrRouteFileCannotBeRead | atMostOneValidPort;
        return (errorCode & fatalErrors) != 0;
    }
};


