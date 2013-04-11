// CPU.h
// Author: Jason Blanchard
// Define CPU class, which will control the emulation by fetch/decode/execute
// instructions through communication with the MMU.

#ifndef _CPU_H
#define _CPU_H

#include "definitions.h"
#include "HeaderInfo.h"
#include "MMU.h"

class CPU {
public:
	CPU(MMU *mmu, Emulator *emu, HeaderInfo *hi);
	~CPU();

	void handleInterrupts();
	int run();
	void test(); // will hold what i'm currently testing on the CPU

private:
	HeaderInfo *hi_;
	MMU *mmu_; // memory object
	Emulator *emu_;

	// Registers
	// A, B, C, D, E, F, H, and L are all 8 bits, SP and PC are 16bits
	BYTE A_, B_, C_, D_, E_, F_, H_, L_;
	WORD SP_, PC_;

	BYTE curr_op;

	// Number of cycles done by last instruction
	int cycles_done_;

	// we will not process anything except interrupts if halted.
	bool halted_;

	// Opcode functions.
	void XX(); // no opcode assigned
	
	// 00
	void NOP();
	void LD_BC_nn();
	void LD_pBC_A();
	void INC_BC();
	void INC_B();
	void DEC_B();
	void LD_B_n();
	void RLCA();
	void LD_pnn_SP();
	void ADD_HL_BC();
	void LD_A_pBC();
	void DEC_BC();
	void INC_C();
	void DEC_C();
	void LD_C_n();
	void RRCA();

	// 10
	void STOP();
	void LD_DE_nn();
	void LD_pDE_A();
	void INC_DE();
	void INC_D();
	void DEC_D();
	void LD_D_n();
	void RLA();
	void JR_n();
	void ADD_HL_DE();
	void LD_A_pDE();
	void DEC_DE();
	void INC_E();
	void DEC_E();
	void LD_E_n();
	void RRA();

	// 20
	void JR_NZ_n();
	void LD_HL_nn();
	void LDI_pHL_A();
	void INC_HL();
	void INC_H();
	void DEC_H();
	void LD_H_n();
	void DAA();
	void JR_Z_n();
	void ADD_HL_HL();
	void LDI_A_pHL();
	void DEC_HL();
	void INC_L();
	void DEC_L();
	void LD_L_n();
	void CPL();

	// 30
	void JR_NC_n();
	void LD_SP_nn();
	void LDD_pHL_A();
	void INC_SP();
	void INC_pHL();
	void DEC_pHL();
	void LD_pHL_n();
	void SCF();
	void JR_C_n();
	void ADD_HL_SP();
	void LDD_A_pHL();
	void DEC_SP();
	void INC_A();
	void DEC_A();
	void LD_A_n();
	void CCF();

	// 40
	void LD_B_B();
	void LD_B_C();
	void LD_B_D();
	void LD_B_E();
	void LD_B_H();
	void LD_B_L();
	void LD_B_pHL();
	void LD_B_A();
	void LD_C_B();
	void LD_C_C();
	void LD_C_D();
	void LD_C_E();
	void LD_C_H();
	void LD_C_L();
	void LD_C_pHL();
	void LD_C_A();

	// 50
	void LD_D_B();
	void LD_D_C();
	void LD_D_D();
	void LD_D_E();
	void LD_D_H();
	void LD_D_L();
	void LD_D_pHL();
	void LD_D_A();
	void LD_E_B();
	void LD_E_C();
	void LD_E_D();
	void LD_E_E();
	void LD_E_H();
	void LD_E_L();
	void LD_E_pHL();
	void LD_E_A();

	// 60
	void LD_H_B();
	void LD_H_C();
	void LD_H_D();
	void LD_H_E();
	void LD_H_H();
	void LD_H_L();
	void LD_H_pHL();
	void LD_H_A();
	void LD_L_B();
	void LD_L_C();
	void LD_L_D();
	void LD_L_E();
	void LD_L_H();
	void LD_L_L();
	void LD_L_pHL();
	void LD_L_A();

	// 70
	void LD_pHL_B();
	void LD_pHL_C();
	void LD_pHL_D();
	void LD_pHL_E();
	void LD_pHL_H();
	void LD_pHL_L();
	void HALT();
	void LD_pHL_A();
	void LD_A_B();
	void LD_A_C();
	void LD_A_D();
	void LD_A_E();
	void LD_A_H();
	void LD_A_L();
	void LD_A_pHL();
	void LD_A_A();

	// 80
	void ADD_A_B();
	void ADD_A_C();
	void ADD_A_D();
	void ADD_A_E();
	void ADD_A_H();
	void ADD_A_L();
	void ADD_A_pHL();
	void ADD_A_A();
	void ADC_A_B();
	void ADC_A_C();
	void ADC_A_D();
	void ADC_A_E();
	void ADC_A_H();
	void ADC_A_L();
	void ADC_A_pHL();
	void ADC_A_A();

	// 90
	void SUB_B();
	void SUB_C();
	void SUB_D();
	void SUB_E();
	void SUB_H();
	void SUB_L();
	void SUB_pHL();
	void SUB_A();
	void SBC_A_B();
	void SBC_A_C();
	void SBC_A_D();
	void SBC_A_E();
	void SBC_A_H();
	void SBC_A_L();
	void SBC_A_pHL();
	void SBC_A_A();

	// A0
	void AND_B();
	void AND_C();
	void AND_D();
	void AND_E();
	void AND_H();
	void AND_L();
	void AND_pHL();
	void AND_A();
	void XOR_B();
	void XOR_C();
	void XOR_D();
	void XOR_E();
	void XOR_H();
	void XOR_L();
	void XOR_pHL();
	void XOR_A();

	// B0
	void OR_B();
	void OR_C();
	void OR_D();
	void OR_E();
	void OR_H();
	void OR_L();
	void OR_pHL();
	void OR_A();
	void CP_B();
	void CP_C();
	void CP_D();
	void CP_E();
	void CP_H();
	void CP_L();
	void CL_pHL();
	void CP_A();
	
	// C0
	void RET_NZ();
	void POP_BC();
	void JP_NZ_pnn();
	void JP_pnn();
	void CALL_NZ_pnn();
	void PUSH_BC();
	void ADD_A_n();
	void RST_00H();
	void RET_Z();
	void RET();
	void JP_Z_pnn();
	void PREFIX_CB();
	void CALL_Z_pnn();
	void CALL_pnn();
	void ADC_A_n();
	void RST_08H();

	// D0
	void RET_NC();
	void POP_DE();
	void JP_NC_pnn();
	// XX
	void CALL_NC_pnn();
	void PUSH_DE();
	void SUB_n();
	void RST_10H();
	void RET_C();
	void RETI();
	void JP_C_pnn();
	// XX
	void CALL_C_pnn();
	// XX
	void SBC_A_n();
	void RST_18H();

	// E0
	void LDH_pnn_A();
	void POP_HL();
	void LD_pC_A();
	// XX
	// XX
	void PUSH_HL();
	void AND_n();
	void RST_20H();
	void ADD_SP_n();
	void JP_pHL();
	void LD_pnn_A();
	// XX
	// XX
	// XX
	void XOR_n();
	void RST_28H();

	// F0
	void LDH_A_pnn();
	void POP_AF();
	void LD_A_pC();
	void DI();
	// XX
	void PUSH_AF();
	void OR_n();
	void RST_30H();
	void LDHL_SP_n();
	void LD_SP_HL();
	void LD_A_pnn();
	void EI();
	// XX
	// XX
	void CP_n();
	void RST_38H();

	// Now the CB opcodes.
	// 00
	void RLC_B();
	void RLC_C();
	void RLC_D();
	void RLC_E();
	void RLC_H();
	void RLC_L();
	void RLC_pHL();
	void RLC_A();
	void RRC_B();
	void RRC_C();
	void RRC_D();
	void RRC_E();
	void RRC_H();
	void RRC_L();
	void RRC_pHL();
	void RRC_A();

	// 10
	void RL_B();
	void RL_C();
	void RL_D();
	void RL_E();
	void RL_H();
	void RL_L();
	void RL_pHL();
	void RL_A();
	void RR_B();
	void RR_C();
	void RR_D();
	void RR_E();
	void RR_H();
	void RR_L();
	void RR_pHL();
	void RR_A();

	// 20
	void SLA_B();
	void SLA_C();
	void SLA_D();
	void SLA_E();
	void SLA_H();
	void SLA_L();
	void SLA_pHL();
	void SLA_A();
	void SRA_B();
	void SRA_C();
	void SRA_D();
	void SRA_E();
	void SRA_H();
	void SRA_L();
	void SRA_pHL();
	void SRA_A();

	// 30
	void SWAP_B();
	void SWAP_C();
	void SWAP_D();
	void SWAP_E();
	void SWAP_H();
	void SWAP_L();
	void SWAP_pHL();
	void SWAP_A();
	void SRL_B();
	void SRL_C();
	void SRL_D();
	void SRL_E();
	void SRL_H();
	void SRL_L();
	void SRL_pHL();
	void SRL_A();

	// 40
	void BIT_0_B();
	void BIT_0_C();
	void BIT_0_D();
	void BIT_0_E();
	void BIT_0_H();
	void BIT_0_L();
	void BIT_0_pHL();
	void BIT_0_A();
	void BIT_1_B();
	void BIT_1_C();
	void BIT_1_D();
	void BIT_1_E();
	void BIT_1_H();
	void BIT_1_L();
	void BIT_1_pHL();
	void BIT_1_A();

	// 50
	void BIT_2_B();
	void BIT_2_C();
	void BIT_2_D();
	void BIT_2_E();
	void BIT_2_H();
	void BIT_2_L();
	void BIT_2_pHL();
	void BIT_2_A();
	void BIT_3_B();
	void BIT_3_C();
	void BIT_3_D();
	void BIT_3_E();
	void BIT_3_H();
	void BIT_3_L();
	void BIT_3_pHL();
	void BIT_3_A();

	// 60
	void BIT_4_B();
	void BIT_4_C();
	void BIT_4_D();
	void BIT_4_E();
	void BIT_4_H();
	void BIT_4_L();
	void BIT_4_pHL();
	void BIT_4_A();
	void BIT_5_B();
	void BIT_5_C();
	void BIT_5_D();
	void BIT_5_E();
	void BIT_5_H();
	void BIT_5_L();
	void BIT_5_pHL();
	void BIT_5_A();

	// 70
	void BIT_6_B();
	void BIT_6_C();
	void BIT_6_D();
	void BIT_6_E();
	void BIT_6_H();
	void BIT_6_L();
	void BIT_6_pHL();
	void BIT_6_A();
	void BIT_7_B();
	void BIT_7_C();
	void BIT_7_D();
	void BIT_7_E();
	void BIT_7_H();
	void BIT_7_L();
	void BIT_7_pHL();
	void BIT_7_A();

	// 80
	void RES_0_B();
	void RES_0_C();
	void RES_0_D();
	void RES_0_E();
	void RES_0_H();
	void RES_0_L();
	void RES_0_pHL();
	void RES_0_A();
	void RES_1_B();
	void RES_1_C();
	void RES_1_D();
	void RES_1_E();
	void RES_1_H();
	void RES_1_L();
	void RES_1_pHL();
	void RES_1_A();

	// 90
	void RES_2_B();
	void RES_2_C();
	void RES_2_D();
	void RES_2_E();
	void RES_2_H();
	void RES_2_L();
	void RES_2_pHL();
	void RES_2_A();
	void RES_3_B();
	void RES_3_C();
	void RES_3_D();
	void RES_3_E();
	void RES_3_H();
	void RES_3_L();
	void RES_3_pHL();
	void RES_3_A();

	// A0
	void RES_4_B();
	void RES_4_C();
	void RES_4_D();
	void RES_4_E();
	void RES_4_H();
	void RES_4_L();
	void RES_4_pHL();
	void RES_4_A();
	void RES_5_B();
	void RES_5_C();
	void RES_5_D();
	void RES_5_E();
	void RES_5_H();
	void RES_5_L();
	void RES_5_pHL();
	void RES_5_A();

	// B0
	void RES_6_B();
	void RES_6_C();
	void RES_6_D();
	void RES_6_E();
	void RES_6_H();
	void RES_6_L();
	void RES_6_pHL();
	void RES_6_A();
	void RES_7_B();
	void RES_7_C();
	void RES_7_D();
	void RES_7_E();
	void RES_7_H();
	void RES_7_L();
	void RES_7_pHL();
	void RES_7_A();

	// C0
	void SET_0_B();
	void SET_0_C();
	void SET_0_D();
	void SET_0_E();
	void SET_0_H();
	void SET_0_L();
	void SET_0_pHL();
	void SET_0_A();
	void SET_1_B();
	void SET_1_C();
	void SET_1_D();
	void SET_1_E();
	void SET_1_H();
	void SET_1_L();
	void SET_1_pHL();
	void SET_1_A();

	// D0
	void SET_2_B();
	void SET_2_C();
	void SET_2_D();
	void SET_2_E();
	void SET_2_H();
	void SET_2_L();
	void SET_2_pHL();
	void SET_2_A();
	void SET_3_B();
	void SET_3_C();
	void SET_3_D();
	void SET_3_E();
	void SET_3_H();
	void SET_3_L();
	void SET_3_pHL();
	void SET_3_A();

	// E0
	void SET_4_B();
	void SET_4_C();
	void SET_4_D();
	void SET_4_E();
	void SET_4_H();
	void SET_4_L();
	void SET_4_pHL();
	void SET_4_A();
	void SET_5_B();
	void SET_5_C();
	void SET_5_D();
	void SET_5_E();
	void SET_5_H();
	void SET_5_L();
	void SET_5_pHL();
	void SET_5_A();

	// F0
	void SET_6_B();
	void SET_6_C();
	void SET_6_D();
	void SET_6_E();
	void SET_6_H();
	void SET_6_L();
	void SET_6_pHL();
	void SET_6_A();
	void SET_7_B();
	void SET_7_C();
	void SET_7_D();
	void SET_7_E();
	void SET_7_H();
	void SET_7_L();
	void SET_7_pHL();
	void SET_7_A();

	// Function pointer array used to call the correct opcode function
	// based on the PC.
	static fn opcodes_[];
	static fn cb_opcodes_[];
};

#endif