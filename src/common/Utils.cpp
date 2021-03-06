#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <filesystem>

#include "Utils.h"


void compareOutput(const string &travelPath)
{
    vector<string> outputFiles;
    vector<string> expectedFiles;
    putDirFileListToVec(travelPath + "/output", outputFiles, ".out_cargo_data");
    putDirFileListToVec(travelPath + "/expectedOutput", expectedFiles, ".out_cargo_data");
    bool success = true;
    for (const string &expectedPath  : expectedFiles)
    {
        for (const string &outputPath : outputFiles)
        {
            if (std::filesystem::path(expectedPath).filename() == std::filesystem::path(outputPath).filename())
            {
                vector<string> output;
                vector<string> expected;

                readToVec(expectedPath, expected);
                readToVec(outputPath, output);

                for (size_t i = 0; i < expected.size(); ++i)
                {
                    if (expected[i] != output[i])
                    {
                        std::cout << std::filesystem::path(expectedPath).filename() << ": " << std::endl;
                        std::cout << "output: " << output[i] << std::endl;
                        std::cout << "expected: " << expected[i] << std::endl;
                        success = false;
                    }
                }

            }
        }
    }

    if (success)
    {
        std::cout << "All files under: " << travelPath << " are the same!" << std::endl;
    }
}

// prints


void writeToErrorFile(const string &message, MessageSeverity severity, const string &errorFilePath)
{
    auto errorDirectoryPath = getDirectoryOfPath(errorFilePath);
    createDirIfNotExists(errorDirectoryPath);
    std::ofstream errorFile(errorFilePath, std::ios_base::app);
    log(message, severity, errorFile);
    errorFile.close();

}


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
            if(!line.empty())
            {
                vecLines.push_back(splitByDelimiter(line, ','));
            }
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


// vector manipulations

void clearDuplicatedConsecutiveStrings(vector<string> &strVec)
{
    unsigned pos = 1;
    while (pos < strVec.size())
    {
        if (strVec[pos] == strVec[pos - 1])
        {
            strVec.erase(strVec.begin() + pos);
        }
        else
        {
            pos++;
        }
    }
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

string getDirectoryOfPath(const string &fullPath)
{
    std::filesystem::path p(fullPath);
    return p.parent_path().string();
}

string getPathFileName(const string &fullPath, bool removeExtention)
{
    std::filesystem::path p(fullPath);
    std::filesystem::path lastComponent(p.filename());
    if (removeExtention)
    {
        return lastComponent.stem().string();
    }
    return lastComponent.string();
}

void createEmptyFile(const string &fullPath)
{
    std::ofstream outfile(fullPath);
    outfile.close();
}

void putSubDirectoriesToVec(const string &curPath, vector<string> &dirVec)
{
    for (const auto &entry : std::filesystem::directory_iterator(curPath))
    {
        const string path = entry.path().string();
        if (!entry.is_directory()) { continue; }
        dirVec.push_back(path);
    }
}

bool isFileEmpty(const string &fullPath)
{
    std::ifstream file(fullPath);
    return file.peek() == std::ifstream::traits_type::eof();
}
