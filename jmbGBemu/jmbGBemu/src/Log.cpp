// Log.cpp
// Author: Jason Blanchard
// Define functions and macros to output to logfiles.

#include "Log.h"

namespace emulog {
    LogFile::LogFile() { }
    LogFile::~LogFile() {
        closeFile();
    }

    void LogFile::setFile(std::string filename) {
        file.open(filename.c_str());
    }

    void LogFile::write(std::string type, std::string message) {
        if (file.good())
            file << type << message;
    }

    void LogFile::closeFile() {
        if (file.good())
            file.close();
    }
}