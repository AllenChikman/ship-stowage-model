#ifndef SHIP_STOWAGE_MODEL_UTILS_H
#define SHIP_STOWAGE_MODEL_UTILS_H


#include <iostream>

using std::vector;
using std::string;

const string CSV_DELIM = ", "; // NOLINT(cert-err58-cpp)




void compareOutput(const string &travelPath);

enum class MessageSeverity
{
    INFO,
    WARNING,
    ERROR,
    Reject
};

void logStartingDecorator(std::ostream &outputStream = std::cout);

void log(const string &message, MessageSeverity severity = MessageSeverity::INFO,
         std::ostream &outputStream = std::cout);

unsigned stringToUInt(const string &str);

bool readToVecLine(const string &path, vector<vector<string>> &vecLines);

bool readToVec(const string &path, vector<string> &vec);

void createDirIfNotExists(const string &path);

void putDirFileListToVec(const string &curPath, vector<string> &dirVec, const string &extent = "");

void strCleanWhitespaces(string &str);

vector<string> splitByDelimiter(const string &line, char delimiter);

string getDirectoryOfPath(const string &fullPath);

string getPathFileName(const string &fullPath);


#endif //SHIP_STOWAGE_MODEL_UTILS_H
