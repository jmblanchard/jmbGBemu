// Emulator.h
// Author: Jason Blanchard
// Define Emulator class, which will be in charge of setting up all Game Boy systems,
// running the emulation, and cleaning up the simulation once finished.

#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <string>

#include "definitions.h"
#include "HeaderInfo.h"
#include "CPU.h"
#include "MMU.h"

class Emulator {
public:
    Emulator();
    ~Emulator();

    void initialize(std::string filename);
    void run();
    void shutdown();

	void setRunning(bool b);

private:
    std::string filename_;
	CPU *cpu_;
	MMU *mmu_;
	HeaderInfo *hi_;

	bool running_;

	int current_clocks_;
	int clocks_until_next_mode_;
	Mode current_mode_;
};

#endif