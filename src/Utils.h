#include <iostream>

enum class MessageSeverity {
    INFO,
    WARNING,
    ERROR
};


void log(const std::string &message, MessageSeverity severity = MessageSeverity::INFO,
         std::ostream &outputStream = std::cout);

unsigned stringToUInt(const std::string &str);

bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines);

bool readToVec(const std::string &path, std::vector<std::string> &vec);