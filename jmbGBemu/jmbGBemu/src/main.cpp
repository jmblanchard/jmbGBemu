// main.cpp
// Author: Jason Blanchard
// Serves as the entry point for the application.

#include <iostream>
#include <fstream>
#include <SDL.h>

#include "Emulator.h"

int main(int argc, char *args[]) {
    // Handle command line argument issues. Currently, command line will only accept file name
    if (argc != 2) {
		std::cout << "Incorrect number of arguments! Include filename of ROM.\n";
    }
    else {
		SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
		Emulator *emu = new Emulator();
        emu->initialize(std::string(args[1]));
		emu->run();
		delete emu;
		SDL_Quit();
    }


    return 0;
}