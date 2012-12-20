// main.cpp
// Author: Jason Blanchard
// Serves as the entry point for the application.

#include <iostream>
#include <fstream>

#include "Emulator.h"

int main(int argc, char *argv[]) {
    Emulator *emu = new Emulator();
    // Handle command line argument issues. Currently, command line will only accept file name
    if (argc != 2) {
        emu->writeLog(emulog::ERROR, "Incompatible command line arguments.");
    }
    else {
        emu->initialize(std::string(argv[1]));
    }

    delete emu;

    return 0;
}