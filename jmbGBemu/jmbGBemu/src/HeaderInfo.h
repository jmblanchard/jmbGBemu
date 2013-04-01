// HeaderInfo.h
// Author: Jason Blanchard
// Defines the HeaderInfo class, which will hold information read in from
// the header of the ROM file.

#ifndef _HEADERINFO_H
#define _HEADERINFO_H

#include "definitions.h"
#include <iostream>

class HeaderInfo {
public:
	char game_name_[16];
	BYTE gbc_flag_; // 80h = GBC, anything else = not GBC
	BYTE gb_sgb_flag_; // 00h = GB, 03h = Super GB
	BYTE cartridge_type_; // different cartridge types, we only deal with ROM only (00h)
						  // and ROM+MBC1 (01h)
	BYTE rom_size_; // 00h = 2 banks, 01h = 4 banks
	BYTE ram_size_; // 00h = none, 01h = 1 bank, 03h = 4 banks
	BYTE destination_code_; // 00h = Japanese, 01h = non-Japanese

	void print_info();
};

#endif