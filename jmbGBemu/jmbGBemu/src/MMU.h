// MMU.h
// Author: Jason Blanchard
// Definition of MMU class which will map the memory of the Game Boy system.

#ifndef _MMU_H
#define _MMU_H

#include <string>
#include <iostream>
#include <fstream>
#include <cstring> // for memcpy

#include "definitions.h"
#include "HeaderInfo.h"

class MMU {
public:
	MMU(std::string filename, HeaderInfo *hi);
	~MMU();

	void readByte(WORD address, BYTE &dest);
	void writeByte(WORD address, BYTE val);

	void readWord(WORD address, WORD &dest);
	void writeWord(WORD address, WORD val);

	// direct access to Interrupt Master Enable flag
	bool ime_;

private:

	// MEMORY MAP
	// Interrupt Enable Register
	// ---------------------------- FFFF
	// Stack RAM
	// ---------------------------- FF80
	// Empty unusable
	// ---------------------------- FF4C
	// I/O Ports
	// ---------------------------- FF00
	// Empty unusable
	// ---------------------------- FEA0
	// Sprite Attrib Memory (OAM)
	// ---------------------------- FE00
	// Echo of Internal RAM
	// ---------------------------- E000
	// Internal RAM
	// ---------------------------- C000
	// switchable RAM bank
	// ---------------------------- A000
	// Video RAM
	// ---------------------------- 8000
	// switchable ROM bank
	// ---------------------------- 4000
	// ROM bank 0
	// ---------------------------- 0000

	// note, remember to only access size - 1 (e.g. 0x0000-0x3FFF)
	BYTE rom_bank_0_[0x4000];
	BYTE switchable_rom_bank_[128][0x4000];
	BYTE video_ram_[0x2000];
	BYTE switchable_ram_bank_[4][0x2000];
	BYTE internal_ram_[0x2000];
	BYTE echo_internal_ram_[0x1E00];
	BYTE oam_[0xA0];
	BYTE io_ports_[0x4C];
	BYTE stack_ram_[0x7F];
	BYTE interrupt_enable_register_;

	HeaderInfo *hi_;
	int num_rom_banks_;
	int curr_rom_bank_;
	int num_ram_banks_;
	int curr_ram_bank_;
	bool ram_enable_;

	// Whether or not any of the joypad keys are pressed
	bool right_pressed_;
	bool left_pressed_;
	bool up_pressed_;
	bool down_pressed_;
	bool a_pressed_;
	bool b_pressed_;
	bool start_pressed_;
	bool select_pressed_;

	// timer
	bool timer_running_;
	BYTE timer_clock_select_;

	void loadROM(std::string filename);
};

#endif