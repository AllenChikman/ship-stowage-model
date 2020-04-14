#ifndef SHIP_STOWAGE_MODEL_UTILS_H
#define SHIP_STOWAGE_MODEL_UTILS_H


#include <iostream>

const std::string CSV_DELIM = ", ";; // NOLINT(cert-err58-cpp)

enum class MessageSeverity
{
    INFO,
    WARNING,
    ERROR
};


void log(const std::string &message, MessageSeverity severity = MessageSeverity::INFO,
         std::ostream &outputStream = std::cout);

unsigned stringToUInt(const std::string &str);

bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines);

bool readToVec(const std::string &path, std::vector<std::string> &vec);

void createDirIfNotExists(const std::string &path);

void putDirListToVec(const std::string &curPath, std::vector<std::string> &dirVec);

#endif //SHIP_STOWAGE_MODEL_UTILS_H