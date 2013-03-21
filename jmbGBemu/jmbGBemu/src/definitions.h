// definitions.h
// Author: Jason Blanchard
// Definition of some necessary types and other global stuff for emulator.

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

class CPU;

typedef void (CPU::*fn)();
typedef unsigned char BYTE;
typedef unsigned short WORD;

#endif