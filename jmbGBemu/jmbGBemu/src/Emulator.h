// Emulator.h
// Author: Jason Blanchard
// Define Emulator class, which will be in charge of setting up all Game Boy systems,
// running the emulation, and cleaning up the simulation once finished.

#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <fstream>
#include <string>

#include "Log.h"

class Emulator {
public:
    Emulator();
    ~Emulator();

    void initialize(std::string filename);
    void run();
    void shutdown();
    void writeLog(std::string type, std::string message);

private:
    std::string filename_;
    emulog::LogFile *logFile;

};

#endif