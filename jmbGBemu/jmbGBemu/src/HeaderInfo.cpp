// HeaderInfo.cpp
// Author: Jason Blanchard
// Defines the HeaderInfo class, which will hold information read in from
// the header of the ROM file.

#include "HeaderInfo.h"

void HeaderInfo::print_info() {
	std::cout << "Game name: ";
	for (int i = 0; i < 16; ++i) {
		if (game_name_[i] != 0x00)
			std::cout << game_name_[i];
	}
	std::cout << "\n";

	if (gbc_flag_ == 0x80)
		std::cout << "Game Boy Color game.\n";
	
	if (gb_sgb_flag_ == 0x00)
		std::cout << "Original Game Boy game.\n";
	else
		std::cout << "Super Game Boy game.\n";

	std::cout << "Cartridge type: ";
	if (cartridge_type_ == 0x00)
		std::cout << "ROM only.\n";
	else if (cartridge_type_ == 0x01)
		std::cout << "ROM+MBC1.\n";

	std::cout << "ROM size: ";
	if (rom_size_ == 0x00)
		std::cout << "32 KB; 2 banks.\n";
	else if (rom_size_ == 0x01)
		std::cout << "64 KB; 4 banks.\n";

	std::cout << "RAM size: ";
	if (ram_size_ == 0x00)
		std::cout << "None.\n";
	else if (ram_size_ == 0x01 || ram_size_ == 0x02)
		std::cout << "8KB; 1 bank.\n";
	else if (ram_size_ == 0x03)
		std::cout << "32KB; 4 banks.\n";

	std::cout << "Destination code: ";
	if (destination_code_ == 0x00)
		std::cout << "Japanese.\n";
	else
		std::cout << "non-Japanese.\n";
}