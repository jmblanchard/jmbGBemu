// definitions.h
// Author: Jason Blanchard
// Definition of some necessary types and other global stuff for emulator.

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#include <cstdint>

// forward declarations
class Emulator;
class CPU;
class MMU;
class HeaderInfo;

typedef void (CPU::*fn)(); // typedef for function pointers
typedef uint8_t BYTE;
typedef uint16_t WORD;

const int FRAME_RATE = 60;
const int CLOCKS_PER_FRAME = 70224;
const int CLOCKS_MODE_0 = 204;
const int CLOCKS_MODE_1 = 4560;
const int CLOCKS_MODE_2 = 80;
const int CLOCKS_MODE_3 = 172;

enum Mode {
	MODE_0 = 0,
	MODE_1,
	MODE_2,
	MODE_3
};

enum Button {
	BUTTON_UP = 0,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_A,
	BUTTON_B,
	BUTTON_START,
	BUTTON_SELECT
};

#endif