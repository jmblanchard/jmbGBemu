// definitions.h
// Author: Jason Blanchard
// Definition of some necessary types and other global stuff for emulator.

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

// forward declarations
class CPU;
class MMU;
class HeaderInfo;

typedef void (CPU::*fn)(); // typedef for function pointers
typedef unsigned char BYTE;
typedef unsigned short WORD;

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

#endif