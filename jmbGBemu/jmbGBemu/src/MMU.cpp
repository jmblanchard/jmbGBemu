// MMU.cpp
// Author: Jason Blanchard
// Implementation of MMU class which will map the memory of the Game Boy system.

#include "MMU.h"
#include "Emulator.h"

MMU::MMU(std::string filename, HeaderInfo *hi, Emulator *emu) {
	emu_ = emu;
	hi_ = hi;
	loadROM(filename);

	memset(video_ram_, 0, 0x1FFF);
	for (int i = 0; i < 4; ++i)
		memset(switchable_ram_bank_[i], 0, 0x1FFF);
	curr_ram_bank_ = 0;
	memset(internal_ram_, 0, 0x1FFF);
	memset(echo_internal_ram_, 0, 0x1E00);
	memset(oam_, 0, 0x9F);

	// setting up i/o ports, which are kinda unique
	memset(io_ports_, 0, 0x4B);
	io_ports_[0x10] = 0x80; // start sound registers
	io_ports_[0x11] = 0xBF;
	io_ports_[0x12] = 0xF3;
	io_ports_[0x14] = 0xBF;
	io_ports_[0x16] = 0x3F;
	io_ports_[0x19] = 0xBF;
	io_ports_[0x1A] = 0x7F;
	io_ports_[0x1B] = 0xFF;
	io_ports_[0x1C] = 0x9F;
	io_ports_[0x1E] = 0xBF;
	io_ports_[0x20] = 0xFF;
	io_ports_[0x23] = 0xBF;
	io_ports_[0x24] = 0x77;
	io_ports_[0x25] = 0xF3;
	io_ports_[0x26] = 0xF1;
	io_ports_[0x40] = 0x91; // LCDC
	io_ports_[0x47] = 0xFC; // BGP
	io_ports_[0x48] = 0xFF; // OBP0
	io_ports_[0x49] = 0xFF; // OBP1
	
	interrupt_enable_register_ = 0x00;

	ram_enable_ = false;
	ime_ = false;

	right_pressed_ = false;
	left_pressed_ = false;
	up_pressed_ = false;
	down_pressed_ = false;
	a_pressed_ = false;
	b_pressed_ = false;
	start_pressed_ = false;
	select_pressed_ = false;

	screen = emu_->getScreen();
	COLOR_WHITE = SDL_MapRGB(screen->format, 224, 248, 208);
	COLOR_LGREY = SDL_MapRGB(screen->format, 136, 192, 112);
	COLOR_DGREY = SDL_MapRGB(screen->format, 52, 104, 86);
	COLOR_BLACK = SDL_MapRGB(screen->format, 0, 0, 0);

	for (int x = 0; x < 256; ++x)
		for (int y = 0; y < 256; ++y)
			bg_data_[x][y] = 0;
	recalc_bg_data_ = false;
}

MMU::~MMU() { }

void MMU::readByte(WORD address, BYTE &dest) {
	if (address < 0x4000) {
		dest = rom_bank_0_[address];
	} else if (address < 0x8000) {
		dest = switchable_rom_bank_[curr_rom_bank_][address-0x4000];
	} else if (address < 0xA000) {
		dest = video_ram_[address-0x8000];
	} else if (address < 0xC000) {
		dest = switchable_ram_bank_[curr_ram_bank_][address-0xA000];
	} else if (address < 0xE000) {
		dest = internal_ram_[address-0xC000];
	} else if (address >= 0xFE00 && address < 0xFEA0) {
		dest = oam_[address-0xFE00];
	} else if (address >= 0xFF00 && address < 0xFF4C) {
		dest = io_ports_[address-0xFF00];
	} else if (address >= 0xFF80 && address < 0xFFFF) {
		dest = stack_ram_[address-0xFF80];
	} else if (address == 0xFFFF) {
		dest = interrupt_enable_register_;
	}
}

void MMU::writeByte(WORD address, BYTE val) {
	// THIS ASSUMES ONLY MBC1 OR NONE CARTRIDGES WILL BE USED FOR SIMPLICITY
	if (address < 0x2000) {
		// writing into ROM location 0-1FFF will enable or disable
		// writing to external RAM.
		if (val == 0x00)
			ram_enable_ = false;
		else if (val == 0x0A)
			ram_enable_ = true;
	} else if (address < 0x4000) {
		if (num_rom_banks_ > 2) {
			// writing into ROM location 2000-3FFF will choose the
			// ROM bank to be used in the switchable ROM bank location.
			if (val & 0)
				val |= 1;

			curr_rom_bank_ = (val & 0x1F) - 1;
		}
	} else if (address < 0x6000) {
		// normally, a game could have more than 5 bits worth of ROM banks,
		// but for simplification I'm ignoring this possibility. So, do
		// nothing here.
	} else if (address < 0x8000) {
		// normally, a game could have a different RAM/ROM banking mode
		// that can be selected here. For simplicity's sake I'm ignoring this.
		// Do nothing here.
	} else if (address < 0xA000) {
		// if STAT register (0xFF41) shows that we are in H-Blank
		// or V-Blank, we can write to video memory.
		//if ((io_ports_[0x41] & 0x03) != 0x03) {
			video_ram_[address-0x8000] = val;
			recalc_bg_data_ = true;
		//}
	} else if (address < 0xC000) {
		if (ram_enable_) {
			switchable_ram_bank_[curr_ram_bank_][address-0xA000] = val;
		}
	} else if (address < 0xE000) {
		internal_ram_[address-0xC000] = val;
	} else if (address >= 0xFE00 && address < 0xFEA0) {
		// if STAT register (0xFF41) shows we are in H-Blank or V-Blank
		// we can write to OAM sprite memory
		//if (!(io_ports_[0x41] & 0x02)) {
			oam_[address-0xFE00] = val;
			recalc_bg_data_ = true;
		//}
	} else if (address >= 0xFF00 && address < 0xFF4C) {
		// special cases here based on the address to be written.
		address = address - 0xFF00;
		switch (address) {
		case 0x00: {// Joypad input register 0xFF00
			// we check to see which buttons we're looking at
			BYTE selectBit = val & 0xF0;
			BYTE build = selectBit | 0x0F;

			// If we select neither P14 or P15, we reset the lower 4 bits to 1.
			// else If we select P14, we check if any direction keys are pressed
			// else If we select P15, we check if any button keys are pressed
			// if any of these buttons are pressed we TOGGLE THE BIT TO 0.
			if (selectBit == 0x20) {
				if (right_pressed_) {
					build ^= 0x01;
					//std::cout << "Right button pressed.\n";
				}
				if (left_pressed_) {
					build ^= 0x02;
					//std::cout << "Left button pressed.\n";
				}
				if (up_pressed_) {
					build ^= 0x04;
					//std::cout << "Up button pressed.\n";
				}
				if (down_pressed_) {
					build ^= 0x08;
					//std::cout << "Down button pressed.\n";
				}
			} else if (selectBit == 0x10) {
				if (a_pressed_) {
					build ^= 0x01;
					//std::cout << "A button pressed.\n";
				}
				if (b_pressed_) {
					build ^= 0x02;
					//std::cout << "B button pressed.\n";
				}
				if (start_pressed_) {
					build ^= 0x04;
					//std::cout << "Start button pressed.\n";
				}
				if (select_pressed_) {
					build ^= 0x08;
					//std::cout << "Select button pressed.\n";
				}
			}

			io_ports_[address] = build;
		}

			break;
		case 0x04: // DIV register 0xFF04
			// any writing to this resets to 0.
			io_ports_[address] = 0x00;
			break;
		case 0x05: // TIMA register 0xFF05
			// we don't want to write anything into the timer.
			break;
		case 0x07: // TAC register, timer control 0xFF07
			io_ports_[address] = val;

			// turn timer on or off based on bit 2, update in Emulator for timing purposes
			timer_running_ = (val >> 2) & 0x01;
			emu_->setTimerRunning(timer_running_);
			// select the clock based on bits 0-1, update in Emulator for timing purposes
			timer_clock_select_ = val & 0x03;
			emu_->setTimerMode(timer_clock_select_);
			break;
		case 0x40: // LCD Control Register - 0xFF40
			// handle the change of tile data and tile map
			// display
			if (((io_ports_[address] & 0x10) != (val & 0x10)) 
				|| ((io_ports_[address] & 0x08) != (val & 0x08))) {
				loadBGMapData(val & 0x08, val & 0x10);
			}

			io_ports_[address] = val;
			break;
		case 0x41: // STAT - LCDC status - 0xFF41
			val = val & 0xF8; // we can only read the bottom 3 bits
			
			// if we enable an interrupt, enable it on our IE register too
			// not sure if this is correct GB behavior
			if (val > 0) {
				interrupt_enable_register_ |= 0x02;
			}

			val |= io_ports_[address]; // make sure we don't clear out mode flag and coincidence flag
			io_ports_[address] = val;
			break;
		case 0x42:
		case 0x43:
			recalc_bg_data_ = true;
			io_ports_[address] = val;
			break;
		case 0x44: // LY - LCDC y coord - 0xFF44
			// THIS IS READ ONLY, CAN'T CHANGE
			break;
		case 0x46: // DMA - 0xFF46
			// this takes a byte address and transfers that area of memory
			// to OAM
			if (val > 0x00 && val <= 0xF1) {
				WORD wval = (WORD)val << 8;
				BYTE *src = &rom_bank_0_[wval-0x4000];
				if (wval < 0x4000) {
				} else if (wval < 0x8000) {
					src = &switchable_rom_bank_[curr_rom_bank_][wval-0x4000];
				} else if (wval < 0xA000) {
					src = &video_ram_[wval-0x8000];
				} else if (wval < 0xC000) {
					src = &switchable_ram_bank_[curr_ram_bank_][wval-0xA000];
				} else if (wval < 0xE000) {
					src = &internal_ram_[wval-0xC000];
				}

				memcpy(oam_, src, 0x9F);
			}

			break;
		default:
			io_ports_[address] = val;
			break;
		}
	} else if (address >= 0xFF80 && address < 0xFFFF) {
		stack_ram_[address-0xFF80] = val;
	} else if (address == 0xFFFF) {
		interrupt_enable_register_ = val;
	}
}

void MMU::readWord(WORD address, WORD &dest) {
	if (address < 0x4000) {
		dest = (rom_bank_0_[address+1] << 8) | (rom_bank_0_[address]);
	} else if (address < 0x8000) {
		dest = (switchable_rom_bank_[curr_rom_bank_][address-0x4000+0x01] << 8) | 
			(switchable_rom_bank_[curr_rom_bank_][address-0x4000]);
	} else if (address < 0xA000) {
		dest = (video_ram_[address-0x8000+0x01] << 8) | video_ram_[address-0x8000];
	} else if (address < 0xC000) {
		dest = (switchable_ram_bank_[curr_ram_bank_][address-0xA000+0x01] << 8) |
			switchable_ram_bank_[curr_ram_bank_][address-0xA000];
	} else if (address < 0xE000) {
		dest = (internal_ram_[address-0xC000+0x01] << 8) | internal_ram_[address-0xC000];
	} else if (address >= 0xFE00 && address < 0xFEA0) {
		dest = (oam_[address-0xFE00+0x01] << 8) | oam_[address-0xFE00];
	} else if (address >= 0xFF00 && address < 0xFF4C) {
		dest = (io_ports_[address-0xFF00+0x01] << 8) | io_ports_[address-0xFF00];
	} else if (address >= 0xFF80 && address < 0xFFFF) {
		dest = (stack_ram_[address-0xFF80+0x01] << 8) | stack_ram_[address-0xFF80];
	} else if (address == 0xFFFF) {
		std::cout << "Can't read WORD from location 0xFFFF\n";
	}
}

void MMU::writeWord(WORD address, WORD val) {
	if (address < 0x2000) {
		if ((val & 0xFF) == 0x0000)
			ram_enable_ = false;
		else if ((val & 0x0A) == 0x000A)
			ram_enable_ = true;
	} else if (address < 0x4000) {
		if (num_rom_banks_ > 2) {
			// writing into ROM location 2000-3FFF will choose the
			// ROM bank to be used in the switchable ROM bank location.
			if (val & 0)
				val |= 1;

			curr_rom_bank_ = (val & 0x1F) - 1;
		}
	} else if (address < 0x8000) {
	} else if (address < 0xA000) {
		// Can only write to video RAM in modes 0-2
		//if ((io_ports_[0x41] & 0x03) != 0x03) {
			video_ram_[address-0x8000+1] = (val >> 8) & 0x00FF;
			video_ram_[address-0x8000] = (val & 0x00FF);
		//}
	} else if (address < 0xC000) {
		if (ram_enable_) {
			switchable_ram_bank_[curr_ram_bank_][address-0xA000+1] = (val >> 8) & 0x00FF;
			switchable_ram_bank_[curr_ram_bank_][address-0xA000] = (val & 0x00FF);
		}
	} else if (address < 0xE000) {
		internal_ram_[address-0xC000+1] = (val >> 8) & 0x00FF;
		internal_ram_[address-0xC000] = (val & 0x00FF);
	} else if (address >= 0xFE00 && address < 0xFEA0) {
		// if STAT register (0xFF41) shows we are in H-Blank or V-Blank
		// we can write to OAM sprite memory
		//if (!(io_ports_[0x41] & 0x02)) {
			oam_[address-0xFE00+1] = (val >> 8) & 0x00FF;
			oam_[address-0xFE00] = (val & 0x00FF);
		//}
	} else if (address >= 0xFF00 && address < 0xFF4C) {
		std::cout << "Writing to I/O registers with WORD, consider changing to BYTE since this fails.\n";
	} else if (address >= 0xFF80 && address < 0xFFFF) {
		stack_ram_[address-0xFF80+1] = (val >> 8) & 0x00FF;
		stack_ram_[address-0xFF80] = (val & 0x00FF);
	} else if (address == 0xFFFF) {
		std::cout << "Can't write WORD to location 0xFFFF\n";
	}
}

// Increment the DIV register. This should overflow automatically and works as intended.
void MMU::updateDiv() {
	io_ports_[0x04] += 0x01;
}

// Increment TIMA register. If it overflows, we need to generate an interrupt, and load in the
// value from the TMA register (0xFF06)
void MMU::updateTima() {
	io_ports_[0x05] += 0x01;

	// we have overflowed
	if (io_ports_[0x05] == 0x00) {
		io_ports_[0x0F] |= 0x04; // set flag in IF, bit 2, to say interrupt has happened
		io_ports_[0x05] = io_ports_[0x06];
	}
}

// Increment LY register (0xFF44). If we go over 153, we reset to 0. If we compare to LYC
// register (0xFF45) and are equal, we set the STAT coincident flag and generate an interrupt
// if that is enabled in the STAT register.
void MMU::updateLY() {
	io_ports_[0x44] += 0x01;

	if (io_ports_[0x44] >= 154)
		io_ports_[0x44] = 0x00;

	if (io_ports_[0x44] == io_ports_[0x45]) {
		io_ports_[0x41] |= 0x02;

		// check to see if LYC=LY Coincidence interrupt is enabled, if so, set interrupt
		if (io_ports_[0x41] & 0x40)
			io_ports_[0x0F] |= 0x02;
	} else if(io_ports_[0x41] & 0x02) {
		// clear coincidence flag if no longer valid
		io_ports_[0x41] ^= 0x02;
	}
}

// Set the LCDC mode to what we pass in.
void MMU::setLCDCMode(Mode m) {
	// clear bottom 2 bits
	io_ports_[0x41] &= ~(0x03);

	if (m == MODE_0) {
		// check for LCDC interrupt and set if needed
		if (io_ports_[0x41] & 0x08)
			io_ports_[0x0F] |= 0x02;
	} else if (m == MODE_1) {
		io_ports_[0x41] |= 0x01;
		// this is V-Blank so we need to set it's interrupt
		io_ports_[0x0F] |= 0x01;
		// check for LCDC interrupt
		if (io_ports_[0x41] & 0x10)
			io_ports_[0x0F] |= 0x02;
	} else if (m == MODE_2) {
		io_ports_[0x41] |= 0x02;
		// interrupt
		if (io_ports_[0x41] & 0x20)
			io_ports_[0x0F] |= 0x02;
	} else if (m == MODE_3) {
		io_ports_[0x41] |= 0x03;
		// no interrupt here
	}
}

void MMU::setButtonPressed(Button b) {
	switch (b) {
	case BUTTON_UP:
		up_pressed_ = true;
		break;
	case BUTTON_DOWN:
		down_pressed_ = true;
		break;
	case BUTTON_LEFT:
		left_pressed_ = true;
		break;
	case BUTTON_RIGHT:
		right_pressed_ = true;
		break;
	case BUTTON_A:
		a_pressed_ = true;
		break;
	case BUTTON_B:
		b_pressed_ = true;
		break;
	case BUTTON_START:
		start_pressed_ = true;
		break;
	case BUTTON_SELECT:
		select_pressed_ = true;
		break;
	default:
		break;
	}

	// set flag for button press
	io_ports_[0x0F] |= 0x10;
}

void MMU::setButtonReleased(Button b) {
	switch (b) {
	case BUTTON_UP:
		up_pressed_ = false;
		break;
	case BUTTON_DOWN:
		down_pressed_ = false;
		break;
	case BUTTON_LEFT:
		left_pressed_ = false;
		break;
	case BUTTON_RIGHT:
		right_pressed_ = false;
		break;
	case BUTTON_A:
		a_pressed_ = false;
		break;
	case BUTTON_B:
		b_pressed_ = false;
		break;
	case BUTTON_START:
		start_pressed_ = false;
		break;
	case BUTTON_SELECT:
		select_pressed_ = false;
		break;
	default:
		break;
	}
}

void MMU::renderScreen() {
	// draw background
	if (recalc_bg_data_) {
		loadBGMapData(io_ports_[0x40] & 0x08, io_ports_[0x40] & 0x10);

		// draw sprites
		if (io_ports_[0x40] & 0x02) {
			if (io_ports_[0x40] & 0x04) {
				// 8x16 sprite mode

				for (int i = 0; i < 40; ++i) {
					if (oam_[i*4] <= 0 || oam_[i*4] >= 160)
						continue;

					WORD offset1 = oam_[i*4+2]*0x10;
					WORD offset2 = (oam_[i*4+2]+0x01)*0x10;

					for (int a = 0; a < 8; ++a) {
						BYTE c, d;
						c = io_ports_[0x43];
						d = io_ports_[0x42];
						BYTE low1 = video_ram_[offset1];
						BYTE hi1 = video_ram_[offset1+1];
						BYTE low2 = video_ram_[offset2];
						BYTE hi2 = video_ram_[offset2+1];

						for (int b = 0; b < 8; ++b) {
							BYTE color = ((low1 >> (7-b)) & 0x01) | ((hi1 >> (7-b) << 1) & 0x02);
							BYTE color2 = ((low2 >> (7-b)) & 0x01) | ((hi2 >> (7-b) << 1) & 0x02);

							if (color == 0x00){}
							else if (color == 0x01) 
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_LGREY;
							else if (color == 0x02) 
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_DGREY;
							else if (color == 0x03)
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_BLACK;

							if (color2 == 0x00) {}
							else if (color2 == 0x01)
								bg_data_[(d+oam_[i*4]+a-8)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_LGREY;
							else if (color2 == 0x02)
								bg_data_[(d+oam_[i*4]+a-8)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_DGREY;
							else if (color2 == 0x03)
								bg_data_[(d+oam_[i*4]+a-8)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_BLACK;
						}
						offset1 += 2;
						offset2 += 2;
					}
				}
			} else {
				// 8x8 sprite mode
				
				// loop through sprites
				for (int i = 0; i < 40; ++i) {
					if (oam_[i*4] <= 0 || oam_[i*4] >= 160)
						continue;

					WORD offset1 = oam_[i*4+2]*0x10;

					for (int a = 0; a < 8; ++a) {
						BYTE c, d;
						c = io_ports_[0x43];
						d = io_ports_[0x42];
						BYTE low1 = video_ram_[offset1];
						BYTE hi1 = video_ram_[offset1+1];

						for (int b = 0; b < 8; ++b) {
							BYTE color = ((low1 >> (7-b)) & 0x01) | ((hi1 >> (7-b) << 1) & 0x02);
							if (oam_[i*4+3] & 0x20)
								color = ((low1 >> b) & 0x01) | ((hi1 >> b << 1) & 0x02);

							if (color == 0x00){}
							else if (color == 0x01) 
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_LGREY;
							else if (color == 0x02) 
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_DGREY;
							else if (color == 0x03)
								bg_data_[(d+oam_[i*4]+a-16)%256][(c+oam_[i*4+1]+b-8)%256] = COLOR_BLACK;
						}
						offset1 += 2;
					}
				}
			}

			int x_pos, y_pos;
			y_pos = io_ports_[0x42];
			x_pos = io_ports_[0x43];
			for (int i = 0; i < 160; ++i) {
				for (int j = 0; j < 144; ++j) {
					Uint32 *p = (Uint32 *)screen->pixels + j*screen->pitch/4 + i;

					*p = bg_data_[(j+y_pos)%256][(i+x_pos)%256];
				}
			}
		}

		recalc_bg_data_ = false;
	}

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void MMU::loadROM(std::string filename) {
	std::cout << "Filename: " << filename << "\n";
	std::ifstream file(filename, std::ios::binary);

	if (file.is_open()) {
		// read in first ROM bank
		file.read((char*)&rom_bank_0_, 0x3FFF);

		// pull header info from ROM bank 0
		memcpy(hi_, (char*)&rom_bank_0_[0x134], 16);
		hi_->gbc_flag_ = rom_bank_0_[0x143];
		hi_->gb_sgb_flag_ = rom_bank_0_[0x146];
		hi_->cartridge_type_ = rom_bank_0_[0x147];
		hi_->rom_size_  = rom_bank_0_[0x148];
		hi_->ram_size_ = rom_bank_0_[0x149];
		hi_->destination_code_ = rom_bank_0_[0x14A];
		hi_->print_info();

		// determine number of banks based on cartridge type
		if (hi_->rom_size_ == 0x00)
			num_rom_banks_ = 2;
		else if (hi_->rom_size_ == 0x01)
			num_rom_banks_ = 4;

		// read these banks in
		file.read((char*)&switchable_rom_bank_[0], 0x3FFF);
		curr_rom_bank_ = 0;

		if (num_rom_banks_ > 2) {
			int n = num_rom_banks_ - 2;
			for (int i = 1; i <= n; ++i) {
				file.read((char*)&switchable_rom_bank_[i], 0x3FFF);
			}
		}

		file.close();

	} else {
		std::cout << "Failed to open ROM file.\n";
	}
}

void MMU::loadBGMapData(int mapSelect, int dataSelect) {
	WORD map_address = mapSelect ? 0x1C00 : 0x1800;
	WORD data_address = dataSelect ? 0x0000 : 0x1000;

	for (int i = 0; i < 32; ++i) {
		for (int j = 0; j < 32; ++j) {
			// we find out  the data offset
			WORD offset = map_address + (i*32 + j);
			BYTE data_id = video_ram_[offset];
			WORD data_offset;

			// data offset could be signed or unsigned, handle here
			if (!data_address) {
				data_offset = data_address + (uint16_t)data_id*16;
			} else {
				data_offset = data_address + (int16_t)data_id*16;
			}

			// transfer data from memory to our bg_data_ structure and
			// choose correct color
			for (int a = 0; a < 8; ++a) {
				BYTE low = video_ram_[data_offset];
				BYTE hi = video_ram_[data_offset+1];

				for (int b = 0; b < 8; ++b) {
					BYTE color = ((low >> (7-b)) & 0x01) | ((hi >> (7-b) << 1) & 0x02);

					if (color == 0x00)
						bg_data_[i*8+a][j*8+b] = COLOR_WHITE;
					else if (color == 0x01)
						bg_data_[i*8+a][j*8+b] = COLOR_LGREY;
					else if (color == 0x02)
						bg_data_[i*8+a][j*8+b] = COLOR_DGREY;
					else if (color == 0x03)
						bg_data_[i*8+a][j*8+b] = COLOR_BLACK;
				}
				data_offset += 2;
			}
		}
	}
}

void MMU::test() {
	BYTE b;
	WORD w;

	// BYTE READING AND WRITING TESTS
	// ------------------------------

	// reading and writing ROM banks
	std::cout << "\nReading/Writing ROM banks.\n";
	rom_bank_0_[0x0000] = 0xFF;
	readByte(0x0000, b);
	if (b == 0xFF)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	rom_bank_0_[0x3FFF] = 0x1F;
	readByte(0x3FFF, b);
	if (b == 0x1F)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	switchable_rom_bank_[0][0x0000] = 0xFF;
	readByte(0x4000, b);
	if (b == 0xFF)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	switchable_rom_bank_[0][0x3FFF] = 0xFF;
	readByte(0x7FFF, b);
	if (b == 0xFF)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing video RAM
	std::cout << "\nReading/Writing video RAM\n";
	io_ports_[0x41] = 0x01; // so we can actually write to RAM

	writeByte(0x8000, 0x11);
	readByte(0x8000, b);
	if (b == 0x11)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0x9000, 0x1F);
	readByte(0x9000, b);
	if (b == 0x1F)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0x9FFF, 0xF1);
	readByte(0x9FFF, b);
	if (b == 0xF1)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing switchable RAM bank
	std::cout << "\nReading/Writing switchable RAM bank.\n";
	writeByte(0x1000, 0x0A);
	if (ram_enable_)
		std::cout << "RAM enable passed.\n";
	else
		std::cout << "RAM enable failed.\n";

	writeByte(0xA000, 0xAB);
	readByte(0xA000, b);
	if (b == 0xAB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xB000, 0xBB);
	readByte(0xB000, b);
	if (b == 0xBB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xBFFF, 0xCB);
	readByte(0xBFFF, b);
	if (b == 0xCB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing internal RAM
	std::cout << "\nReading/Writing internal RAM.\n";
	writeByte(0xC000, 0xAC);
	readByte(0xC000, b);
	if (b == 0xAC)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xD000, 0xBC);
	readByte(0xD000, b);
	if (b == 0xBC)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xDFFF, 0xDC);
	readByte(0xDFFF, b);
	if (b == 0xDC)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing OAM
	std::cout << "\nReading/Writing OAM.\n";
	io_ports_[0x41] = 0x00; // set so we can write to OAM

	writeByte(0xFE00, 0xCC);
	readByte(0xFE00, b);
	if (b == 0xCC)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xFE50, 0xDC);
	readByte(0xFE50, b);
	if (b == 0xDC)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xFE9F, 0x1C);
	readByte(0xFE9F, b);
	if (b == 0x1C)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing I/O ports

	// reading and writing stack RAM
	std::cout << "\nReading/Writing stack RAM.\n";
	writeByte(0xFF80, 0x89);
	readByte(0xFF80, b);
	if (b == 0x89)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xFF8F, 0x99);
	readByte(0xFF8F, b);
	if (b == 0x99)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeByte(0xFFEF, 0x19);
	readByte(0xFFEF, b);
	if (b == 0x19)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// reading and writing IE Register
	std::cout << "\nReading/Writing IE Register.\n";
	writeByte(0xFFFF, 0x11);
	readByte(0xFFFF, b);
	if (b == 0x11)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// WORD READING AND WRITING TESTS
	// -------------------------------

	// read and write from ROM bank 0
	std::cout << "\nRead/Write ROM bank 0.\n";
	rom_bank_0_[0x0000] = 0x12;
	rom_bank_0_[0x0001] = 0x34;
	readWord(0x0000, w);
	if (w == 0x3412)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	rom_bank_0_[0x2000] = 0x22;
	rom_bank_0_[0x2001] = 0x44;
	readWord(0x2000, w);
	if (w == 0x4422)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	rom_bank_0_[0x3FFE] = 0x56;
	rom_bank_0_[0x3FFF] = 0x78;
	readWord(0x3FFE, w);
	if (w == 0x7856)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// read and write from switchable ROM bank.
	std::cout << "\nRead/Write from switchable ROM bank.\n";
	switchable_rom_bank_[0][0x0000] = 0xAB;
	switchable_rom_bank_[0][0x0001] = 0xBC;
	readWord(0x4000, w);
	if (w == 0xBCAB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	switchable_rom_bank_[0][0x1000] = 0xBB;
	switchable_rom_bank_[0][0x1001] = 0xBC;
	readWord(0x5000, w);
	if (w == 0xBCBB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	switchable_rom_bank_[0][0x3FFE] = 0xAB;
	switchable_rom_bank_[0][0x3FFF] = 0xBD;
	readWord(0x7FFE, w);
	if (w == 0xBDAB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	// read and write video RAM.
	std::cout << "\nRead/Write from video RAM.\n";
	writeWord(0x8000, 0xAABB);
	readWord(0x8000, w);
	if (w == 0xAABB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeWord(0x9000, 0xCABB);
	readWord(0x9000, w);
	if (w == 0xCABB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";

	writeWord(0xBFFE, 0xACBB);
	readWord(0xBFFE, w);
	if (w == 0xACBB)
		std::cout << "TEST passed.\n";
	else
		std::cout << "TEST failed.\n";
}