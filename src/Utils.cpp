#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <filesystem>
#include "Utils.h"


// prints

void log(const string &message, MessageSeverity severity, std::ostream &outputStream)
{
    string severityStr;
    switch (severity)
    {
        case MessageSeverity::WARNING :
            severityStr = "Warning";
            break;
        case MessageSeverity::ERROR :
            severityStr = "Error";
            break;
        default:
            severityStr = "Info";
    }

    outputStream << severityStr << ": " << message << std::endl;

}

void logStartingDecorator(std::ostream &outputStream)
{
    outputStream << std::endl;
    outputStream << "---------------------------------------------------------------------" << std::endl;
}

// Static functions for parsing purposes

unsigned stringToUInt(const string &str)
{
    // TODO: check validity
    return static_cast<unsigned int>(std::stoi(str));
}

void strCleanWhitespaces(string &str)
{
    str.erase(std::remove_if(str.begin(),
                             str.end(),
                             [](unsigned char x) { return std::isspace(x); }),
              str.end());
}

vector<string> splitByDelimiter(const string &line, char delimiter)
{
    vector<string> res;

    std::stringstream ss(line);

    while (ss.good())
    {
        string substr;
        getline(ss, substr, delimiter);
        res.push_back(substr);
    }

    return res;
}

vector<string> splitByWhiteSpace(const string &line)
{
    vector<string> tokens;
    std::istringstream iss(line);
    copy(std::istream_iterator<string>(iss),
         std::istream_iterator<string>(),
         std::back_inserter(tokens));
    return tokens;
}


//readers

bool readToVecLine(const string &path, vector<vector<string>> &vecLines)
{

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in)
    {
        log("Cannot open the File: " + path, MessageSeverity::ERROR);
        return false;
    }

    string line;
    // Read the next line from File until it reaches the end.
    while (std::getline(in, line))
    {
        // Line contains string of length > 0 then save it in vector
        if (!line.empty() && line[0] != '#')
        {
            strCleanWhitespaces(line);
            vecLines.push_back(splitByDelimiter(line, ','));
        }
    }
    in.close();
    return true;
}

bool readToVec(const string &path, vector<string> &vec)
{

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in)
    {
        log("Cannot open the File : " + path, MessageSeverity::ERROR);
        return false;
    }

    string line;
    // Read the next line from File until it reaches the end.
    while (std::getline(in, line))
    {
        // Line contains string of length > 0 then save it in vector
        if (!line.empty() && line[0] != '#')
        {
            strCleanWhitespaces(line);
            vec.push_back(line);
        }
    }
    in.close();
    return true;
}

// filesystem functions

void createDirIfNotExists(const string &path)
{

    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }

}

void putDirFileListToVec(const string &curPath, vector<string> &dirFileVec, const string &extent)
{
    for (const auto &entry : std::filesystem::directory_iterator(curPath))
    {
        const string path = entry.path().string();
        if (!extent.empty() && path.find(extent) == string::npos) { continue; }
        dirFileVec.push_back(path);
    }
}

