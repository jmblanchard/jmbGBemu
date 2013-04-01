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
	// our emulation loop
	while (running_) {

		// our loop between modes 2, 3, and 0
		while (current_clocks_ < 65664) {
			cpu_->run();
		}

		// time for V-Blank
		current_mode_ = MODE_1;
		while (current_clocks_ < 70224) {
		}

		current_clocks_ = 0;
		current_mode_ = MODE_2;
		clocks_until_next_mode_ = CLOCKS_MODE_2;
	}
}

// Handles cleanup of emulators systems
void Emulator::shutdown() {
	delete cpu_;
	delete mmu_;
	delete hi_;
}

// Initializes the emulator
void Emulator::initialize(std::string filename) {
    filename_ = filename;
	hi_ = new HeaderInfo();
	mmu_ = new MMU(filename_, hi_);
	cpu_ = new CPU(mmu_, hi_);

	running_ = true;
	current_clocks_ = 0;
	current_mode_ = MODE_2;
	clocks_until_next_mode_ = CLOCKS_MODE_2;
}

void Emulator::setRunning(bool b) {
	running_ = b;
}