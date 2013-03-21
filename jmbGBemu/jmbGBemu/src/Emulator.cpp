// Emulator.cpp
// Author: Jason Blanchard
// Implement Emulator class, which will be in charge of setting up all Game Boy systems,
// running the emulation, and cleaning up the simulation once finished.

#include "Emulator.h"

// Take filename of ROM into emulator and begin initialization
Emulator::Emulator() {
}

// Destructor of emulator. Call shutdown
Emulator::~Emulator() {
    shutdown();
}

// Runs emulation, allowing systems to work together
void Emulator::run() {
}

// Handles cleanup of emulators systems
void Emulator::shutdown() {
}

// Initializes the emulator
void Emulator::initialize(std::string filename) {
    filename_ = filename;
}