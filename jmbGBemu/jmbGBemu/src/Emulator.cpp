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
	int cpu_cycle_complete_; // CPU will return number of cycles just run

	// SOME TEST STUFF
	//mmu_->test();
	//cpu_->test();

	
	// our emulation loop
	start_ticks_ = SDL_GetTicks();
	while (running_) {
		// handle input
		handleInput();

		// run CPU and timings
		cpu_->handleInterrupts();
		cpu_cycle_complete_ = cpu_->run();
		updateClocks(cpu_cycle_complete_);
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
	screen = SDL_SetVideoMode(160, 144, 32, SDL_SWSURFACE);

    filename_ = filename;
	hi_ = new HeaderInfo();
	mmu_ = new MMU(filename_, hi_, this);
	cpu_ = new CPU(mmu_, this, hi_);

	running_ = true;
	current_clocks_ = 0;
	div_clocks_ = 0;
	timer_clocks_ = 0;
	current_mode_ = MODE_2;
	timer_mode_ = MODE_0;
	clocks_until_next_mode_ = CLOCKS_MODE_2;
	timer_running_ = false;
	timer_mode_clocks_[0] = 1024;
	timer_mode_clocks_[1] = 16;
	timer_mode_clocks_[2] = 64;
	timer_mode_clocks_[3] = 256;

	start_ticks_ = 0;
}

void Emulator::setRunning(bool b) {
	running_ = b;
}

void Emulator::updateClocks(int cycles) {
	current_clocks_ += cycles;
	clocks_until_next_mode_ -= cycles;

	div_clocks_ += cycles;
	if (div_clocks_ >= 256) {
		mmu_->updateDiv();
		div_clocks_ -= 256;
	}

	if (timer_running_) {
		timer_clocks_ += cycles;

		if (timer_clocks_ >= timer_mode_clocks_[(int)timer_mode_]) {
			mmu_->updateTima();
			timer_clocks_ -= timer_mode_clocks_[(int)timer_mode_];
		}
	}

	// Here we set the different modes for the LCDC, and figure out our frame clocks, etc.
	if (clocks_until_next_mode_ <= 0) {
		if (current_clocks_ < 65664) {
			if (current_mode_ == MODE_0) {
				current_mode_ = MODE_2;
				mmu_->updateLY();
				clocks_until_next_mode_ = CLOCKS_MODE_2;
				mmu_->setLCDCMode(MODE_2);
			} else if (current_mode_ == MODE_2) {
				current_mode_ = MODE_3;
				clocks_until_next_mode_ = CLOCKS_MODE_3;
				mmu_->setLCDCMode(MODE_3);
			} else if (current_mode_ == MODE_3) {
				current_mode_ = MODE_0;
				clocks_until_next_mode_ = CLOCKS_MODE_0;
				mmu_->setLCDCMode(MODE_0);
			}
		} else if (current_clocks_ < 70224) { 
			// we have entered V-Blank
			current_mode_ = MODE_1;
			clocks_until_next_mode_ = CLOCKS_MODE_1;
			mmu_->setLCDCMode(MODE_1);
		} else { 
			// we're out of V-Blank and restarting the cycle,
			// this is the end of frame so we need to spin if we have any time
			// left
			current_clocks_ = 0;
			current_mode_ = MODE_2;
			clocks_until_next_mode_ = CLOCKS_MODE_2;
			mmu_->setLCDCMode(MODE_2);
			mmu_->updateLY();

			// TO DO: we're going to draw here
			mmu_->renderScreen();

			spinUntilNextFrame();
			start_ticks_ = SDL_GetTicks();
		}
	}
}

void Emulator::setTimerRunning(bool b) {
	timer_running_ = b;
}

void Emulator::setTimerMode(BYTE b) {
	timer_mode_ = Mode(b);
}

SDL_Surface *Emulator::getScreen() {
	return screen;
}

void Emulator::handleInput() {
	while (SDL_PollEvent(&evnt)) {
		if (evnt.type == SDL_KEYDOWN) {
			if (evnt.key.keysym.sym == SDLK_RETURN) {
				mmu_->setButtonPressed(BUTTON_START);
			}
			if (evnt.key.keysym.sym == SDLK_RSHIFT) {
				mmu_->setButtonPressed(BUTTON_SELECT);
			}
			if (evnt.key.keysym.sym == SDLK_a) {
				mmu_->setButtonPressed(BUTTON_A);
			}
			if (evnt.key.keysym.sym == SDLK_s) {
				mmu_->setButtonPressed(BUTTON_B);
			}
			if (evnt.key.keysym.sym == SDLK_RIGHT) {
				mmu_->setButtonPressed(BUTTON_RIGHT);
			}
			if (evnt.key.keysym.sym == SDLK_LEFT) {
				mmu_->setButtonPressed(BUTTON_LEFT);
			}
			if (evnt.key.keysym.sym == SDLK_UP) {
				mmu_->setButtonPressed(BUTTON_UP);
			}
			if (evnt.key.keysym.sym == SDLK_DOWN) {
				mmu_->setButtonPressed(BUTTON_DOWN);
			}
			if (evnt.key.keysym.sym == SDLK_ESCAPE) {
				running_ = false;
			}
		} else if (evnt.type == SDL_KEYUP) {
			if (evnt.key.keysym.sym == SDLK_RETURN) {
				mmu_->setButtonReleased(BUTTON_START);
			}
			if (evnt.key.keysym.sym == SDLK_RSHIFT) {
				mmu_->setButtonReleased(BUTTON_SELECT);
			}
			if (evnt.key.keysym.sym == SDLK_a) {
				mmu_->setButtonReleased(BUTTON_A);
			}
			if (evnt.key.keysym.sym == SDLK_s) {
				mmu_->setButtonReleased(BUTTON_B);
			}
			if (evnt.key.keysym.sym == SDLK_RIGHT) {
				mmu_->setButtonReleased(BUTTON_RIGHT);
			}
			if (evnt.key.keysym.sym == SDLK_LEFT) {
				mmu_->setButtonReleased(BUTTON_LEFT);
			}
			if (evnt.key.keysym.sym == SDLK_UP) {
				mmu_->setButtonReleased(BUTTON_UP);
			}
			if (evnt.key.keysym.sym == SDLK_DOWN) {
				mmu_->setButtonReleased(BUTTON_DOWN);
			} 
		} else if (evnt.type == SDL_QUIT) {
			running_ = false;
		}
	}
}

void Emulator::spinUntilNextFrame() {
	uint32_t x = SDL_GetTicks() - start_ticks_;
	if (x < 1000 / 60) {
		SDL_Delay((1000/60) - x);
	}
}