#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <filesystem>
#include "Utils.h"


// prints

void log(const std::string &message, MessageSeverity severity, std::ostream &outputStream)
{
    std::string severityStr;
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

unsigned stringToUInt(const std::string &str)
{
    // TODO: check validity
    return static_cast<unsigned int>(std::stoi(str));
}

void strCleanWhitespaces(std::string &str)
{
    str.erase(std::remove_if(str.begin(),
                             str.end(),
                             [](unsigned char x) { return std::isspace(x); }),
              str.end());
}

std::vector<std::string> splitByDelimiter(const std::string &line, char delimiter)
{
    std::vector<std::string> res;

    std::stringstream ss(line);

    while (ss.good())
    {
        std::string substr;
        getline(ss, substr, delimiter);
        res.push_back(substr);
    }

    return res;
}

std::vector<std::string> splitByWhiteSpace(const std::string &line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(tokens));
    return tokens;
}


//readers

bool readToVecLine(const std::string &path, std::vector<std::vector<std::string>> &vecLines)
{

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in)
    {
        log("Cannot open the File: " + path, MessageSeverity::ERROR);
        return false;
    }

    std::string line;
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

bool readToVec(const std::string &path, std::vector<std::string> &vec)
{

    // Open the File
    std::ifstream in(path.c_str());

    // Check if object is valid
    if (!in)
    {
        log("Cannot open the File : " + path, MessageSeverity::ERROR);
        return false;
    }

    std::string line;
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

void createDirIfNotExists(const std::string &path)
{

    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }

}

void putDirFileListToVec(const std::string &curPath, std::vector<std::string> &dirFileVec, const std::string &extent)
{
    for (const auto &entry : std::filesystem::directory_iterator(curPath))
    {
        const std::string path = entry.path().string();
        if (!extent.empty() && path.find(extent) == std::string::npos) { continue; }
        dirFileVec.push_back(path);
    }
}

