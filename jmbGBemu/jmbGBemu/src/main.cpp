// main.cpp
// Author: Jason Blanchard
// Serves as the entry point for the application.

#include <iostream>
#include <fstream>

#include "Emulator.h"

int main(int argc, char *argv[]) {
    // Handle command line argument issues. Currently, command line will only accept file name
    if (argc != 2) {
		std::cout << "Incorrect number of arguments! Include filename of ROM.\n";
    }
    else {
		Emulator *emu = new Emulator();
        emu->initialize(std::string(argv[1]));
		delete emu;
    }


    return 0;
}