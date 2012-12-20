// Log.h
// Author: Jason Blanchard
// Define functions and macros to output to logfiles.

#ifndef _LOG_H
#define _LOG_H

#include <fstream>
#include <string>

namespace emulog {

const std::string ERROR("Error: ");
const std::string WARNING("Warning: ");
const std::string INFO("Info: ");

class LogFile {
public:
    LogFile();
    ~LogFile();

    void setFile(std::string filename);
    void write(std::string type, std::string message);
    void closeFile();

private:
    std::ofstream file;
};

}

#endif