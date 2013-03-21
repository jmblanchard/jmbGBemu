// CPU.cpp
// Author: Jason Blanchard
// Implement CPU class, which will control the emulation by fetch/decode/execute
// instructions through communication with the MMU.

#include "CPU.h"

// Definition of both opcodes_ and cb_opcodes_
fn CPU::opcodes_[] = {
	// 00
	NOP,		LD_BC_nn,		LD_pBC_A,		INC_BC,
	INC_B,		DEC_B,			LD_B_n,			RLCA,
	LD_pnn_SP,	ADD_HL_BC,		LD_A_pBC,		DEC_BC,
	INC_C,		DEC_C,			LD_C_n,			RRCA,

	// 10
	STOP,		LD_DE_nn,		LD_pDE_A,		INC_DE,
	INC_D,		DEC_D,			LD_D_n,			RLA,
	JR_n,		ADD_HL_DE,		LD_A_pDE,		DEC_DE,
	INC_E,		DEC_E,			LD_E_n,			RRA,

	// 20
	JR_NZ_n,	LD_HL_nn,		LDI_pHL_A,		INC_HL,
	INC_H,		DEC_H,			LD_H_n,			DAA,
	JR_Z_n,		ADD_HL_HL,		LDI_A_pHL,		DEC_HL,
	INC_L,		DEC_L,			LD_L_n,			CPL,

	// 30
	JR_NC_n,	LD_SP_nn,		LDD_pHL_A,		INC_SP,
	INC_pHL,	DEC_pHL,		LD_pHL_n,		SCF,
	JR_C_n,		ADD_HL_SP,		LDD_A_pHL,		DEC_SP,
	INC_A,		DEC_A,			LD_A_n,			CCF,

	// 40
	LD_B_B,		LD_B_C,			LD_B_D,			LD_B_E,
	LD_B_H,		LD_B_L,			LD_B_pHL,		LD_B_A,
	LD_C_B,		LD_C_C,			LD_C_D,			LD_C_E,
	LD_C_H,		LD_C_L,			LD_C_pHL,		LD_C_A,

	// 50
	LD_D_B,		LD_D_C,			LD_D_D,			LD_D_E,
	LD_D_H,		LD_D_L,			LD_D_pHL,		LD_D_A,
	LD_E_B,		LD_E_C,			LD_E_D,			LD_E_E,
	LD_E_H,		LD_E_L,			LD_E_pHL,		LD_E_A,

	// 60
	LD_H_B,		LD_H_C,			LD_H_D,			LD_H_E,
	LD_H_H,		LD_H_L,			LD_H_pHL,		LD_H_A,
	LD_L_B,		LD_L_C,			LD_L_D,			LD_L_E,
	LD_L_H,		LD_L_L,			LD_L_pHL,		LD_L_A,

	// 70
	LD_pHL_B,	LD_pHL_C,		LD_pHL_D,		LD_pHL_E,
	LD_pHL_H,	LD_pHL_L,		HALT,			LD_pHL_A,
	LD_A_B,		LD_A_C,			LD_A_D,			LD_A_E,
	LD_A_H,		LD_A_L,			LD_A_pHL,		LD_A_A,

	// 80
	ADD_A_B,	ADD_A_C,		ADD_A_D,		ADD_A_E,
	ADD_A_H,	ADD_A_L,		ADD_A_pHL,		ADD_A_A,
	ADC_A_B,	ADC_A_C,		ADC_A_D,		ADC_A_E,
	ADC_A_H,	ADC_A_L,		ADC_A_pHL,		ADC_A_A,

	// 90
	SUB_B,		SUB_C,			SUB_D,			SUB_E,
	SUB_H,		SUB_L,			SUB_pHL,		SUB_A,
	SBC_A_B,	SBC_A_C,		SBC_A_D,		SBC_A_E,
	SBC_A_H,	SBC_A_L,		SBC_A_pHL,		SBC_A_A,

	// A0
	AND_B,		AND_C,			AND_D,			AND_E,
	AND_H,		AND_L,			AND_pHL,		AND_A,
	XOR_B,		XOR_C,			XOR_D,			XOR_E,
	XOR_H,		XOR_L,			XOR_pHL,		XOR_A,

	// B0
	OR_B,		OR_C,			OR_D,			OR_E,
	OR_H,		OR_L,			OR_pHL,			OR_A,
	CP_B,		CP_C,			CP_D,			CP_E,
	CP_H,		CP_L,			CL_pHL,			CP_A,
	
	// C0
	RET_NZ,		POP_BC,			JP_NZ_pnn,		JP_pnn,
	CALL_NZ_pnn,PUSH_BC,		ADD_A_n,		RST_00H,
	RET_Z,		RET,			JP_Z_pnn,		PREFIX_CB,
	CALL_Z_pnn,	CALL_pnn,		ADC_A_n,		RST_08H,

	// D0
	RET_NC,		POP_DE,			JP_NC_pnn,		XX,
	CALL_NC_pnn,PUSH_DE,		SUB_n,			RST_10H,
	RET_C,		RETI,			JP_C_pnn,		XX,
	CALL_C_pnn,	XX,				SBC_A_n,		RST_18H,

	// E0
	LDH_pnn_A,	POP_HL,			LD_pC_A,		XX,
	XX,			PUSH_HL,		AND_n,			RST_20H,
	ADD_SP_n,	JP_pHL,			LD_pnn_A,		XX,
	XX,			XX,				XOR_n,			RST_28H,

	// F0
	LDH_A_pnn,	POP_AF,			LD_A_pC,		DI,
	XX,			PUSH_AF,		OR_n,			RST_30H,
	LDHL_SP_n,	LD_SP_HL,		LD_A_pnn,		EI,
	XX,			XX,				CP_n,			RST_38H
};

fn CPU::cb_opcodes_[] = {
	// 00
	RLC_B,		RLC_C,			RLC_D,			RLC_E,
	RLC_H,		RLC_L,			RLC_pHL,		RLC_A,
	RRC_B,		RRC_C,			RRC_D,			RRC_E,
	RRC_H,		RRC_L,			RRC_pHL,		RRC_A,

	// 10
	RL_B,		RL_C,			RL_D,			RL_E,
	RL_H,		RL_L,			RL_pHL,			RL_A,
	RR_B,		RR_C,			RR_D,			RR_E,
	RR_H,		RR_L,			RR_pHL,			RR_A,

	// 20
	SLA_B,		SLA_C,			SLA_D,			SLA_E,
	SLA_H,		SLA_L,			SLA_pHL,		SLA_A,
	SRA_B,		SRA_C,			SRA_D,			SRA_E,
	SRA_H,		SRA_L,			SRA_pHL,		SRA_A,

	// 30
	SWAP_B,		SWAP_C,			SWAP_D,			SWAP_E,
	SWAP_H,		SWAP_L,			SWAP_pHL,		SWAP_A,
	SRL_B,		SRL_C,			SRL_D,			SRL_E,
	SRL_H,		SRL_L,			SRL_pHL,		SRL_A,

	// 40
	BIT_0_B,	BIT_0_C,		BIT_0_D,		BIT_0_E,
	BIT_0_H,	BIT_0_L,		BIT_0_pHL,		BIT_0_A,
	BIT_1_B,	BIT_1_C,		BIT_1_D,		BIT_1_E,
	BIT_1_H,	BIT_1_L,		BIT_1_pHL,		BIT_1_A,

	// 50
	BIT_2_B,	BIT_2_C,		BIT_2_D,		BIT_2_E,
	BIT_2_H,	BIT_2_L,		BIT_2_pHL,		BIT_2_A,
	BIT_3_B,	BIT_3_C,		BIT_3_D,		BIT_3_E,
	BIT_3_H,	BIT_3_L,		BIT_3_pHL,		BIT_3_A,

	// 60
	BIT_4_B,	BIT_4_C,		BIT_4_D,		BIT_4_E,
	BIT_4_H,	BIT_4_L,		BIT_4_pHL,		BIT_4_A,
	BIT_5_B,	BIT_5_C,		BIT_5_D,		BIT_5_E,
	BIT_5_H,	BIT_5_L,		BIT_5_pHL,		BIT_5_A,

	// 70
	BIT_6_B,	BIT_6_C,		BIT_6_D,		BIT_6_E,
	BIT_6_H,	BIT_6_L,		BIT_6_pHL,		BIT_6_A,
	BIT_7_B,	BIT_7_C,		BIT_7_D,		BIT_7_E,
	BIT_7_H,	BIT_7_L,		BIT_7_pHL,		BIT_7_A,

	// 80
	RES_0_B,	RES_0_C,		RES_0_D,		RES_0_E,
	RES_0_H,	RES_0_L,		RES_0_pHL,		RES_0_A,
	RES_1_B,	RES_1_C,		RES_1_D,		RES_1_E,
	RES_1_H,	RES_1_L,		RES_1_pHL,		RES_1_A,

	// 90
	RES_2_B,	RES_2_C,		RES_2_D,		RES_2_E,
	RES_2_H,	RES_2_L,		RES_2_pHL,		RES_2_A,
	RES_3_B,	RES_3_C,		RES_3_D,		RES_3_E,
	RES_3_H,	RES_3_L,		RES_3_pHL,		RES_3_A,

	// A0
	RES_4_B,	RES_4_C,		RES_4_D,		RES_4_E,
	RES_4_H,	RES_4_L,		RES_4_pHL,		RES_4_A,
	RES_5_B,	RES_5_C,		RES_5_D,		RES_5_E,
	RES_5_H,	RES_5_L,		RES_5_pHL,		RES_5_A,

	// B0
	RES_6_B,	RES_6_C,		RES_6_D,		RES_6_E,
	RES_6_H,	RES_6_L,		RES_6_pHL,		RES_6_A,
	RES_7_B,	RES_7_C,		RES_7_D,		RES_7_E,
	RES_7_H,	RES_7_L,		RES_7_pHL,		RES_7_A,

	// C0
	SET_0_B,	SET_0_C,		SET_0_D,		SET_0_E,
	SET_0_H,	SET_0_L,		SET_0_pHL,		SET_0_A,
	SET_1_B,	SET_1_C,		SET_1_D,		SET_1_E,
	SET_1_H,	SET_1_L,		SET_1_pHL,		SET_1_A,

	// D0
	SET_2_B,	SET_2_C,		SET_2_D,		SET_2_E,
	SET_2_H,	SET_2_L,		SET_2_pHL,		SET_2_A,
	SET_3_B,	SET_3_C,		SET_3_D,		SET_3_E,
	SET_3_H,	SET_3_L,		SET_3_pHL,		SET_3_A,

	// E0
	SET_4_B,	SET_4_C,		SET_4_D,		SET_4_E,
	SET_4_H,	SET_4_L,		SET_4_pHL,		SET_4_A,
	SET_5_B,	SET_5_C,		SET_5_D,		SET_5_E,
	SET_5_H,	SET_5_L,		SET_5_pHL,		SET_5_A,

	// F0
	SET_6_B,	SET_6_C,		SET_6_D,		SET_6_E,
	SET_6_H,	SET_6_L,		SET_6_pHL,		SET_6_A,
	SET_7_B,	SET_7_C,		SET_7_D,		SET_7_E,
	SET_7_H,	SET_7_L,		SET_7_pHL,		SET_7_A
};

CPU::CPU(MMU *mmu) {
	mmu_ = mmu;
	A_ = B_ = C_ = D_ = F_ = H_ = L_ = 0;
	SP_ = 0xFFFE;
	PC_ = 0x100;
}

CPU::~CPU() { }

// Opcode functions.
void CPU::XX() {
}

// 00
void CPU::NOP(){ }
void CPU::LD_BC_nn(){ }
void CPU::LD_pBC_A(){ }
void CPU::INC_BC(){ }
void CPU::INC_B(){ }
void CPU::DEC_B(){ }
void CPU::LD_B_n(){ }
void CPU::RLCA(){ }
void CPU::LD_pnn_SP(){ }
void CPU::ADD_HL_BC(){ }
void CPU::LD_A_pBC(){ }
void CPU::DEC_BC(){ }
void CPU::INC_C(){ }
void CPU::DEC_C(){ }
void CPU::LD_C_n(){ }
void CPU::RRCA(){ }

// 10
void CPU::STOP(){ }
void CPU::LD_DE_nn(){ }
void CPU::LD_pDE_A(){ }
void CPU::INC_DE(){ }
void CPU::INC_D(){ }
void CPU::DEC_D(){ }
void CPU::LD_D_n(){ }
void CPU::RLA(){ }
void CPU::JR_n(){ }
void CPU::ADD_HL_DE(){ }
void CPU::LD_A_pDE(){ }
void CPU::DEC_DE(){ }
void CPU::INC_E(){ }
void CPU::DEC_E(){ }
void CPU::LD_E_n(){ }
void CPU::RRA(){ }

// 20
void CPU::JR_NZ_n(){ }
void CPU::LD_HL_nn(){ }
void CPU::LDI_pHL_A(){ }
void CPU::INC_HL(){ }
void CPU::INC_H(){ }
void CPU::DEC_H(){ }
void CPU::LD_H_n(){ }
void CPU::DAA(){ }
void CPU::JR_Z_n(){ }
void CPU::ADD_HL_HL(){ }
void CPU::LDI_A_pHL(){ }
void CPU::DEC_HL(){ }
void CPU::INC_L(){ }
void CPU::DEC_L(){ }
void CPU::LD_L_n(){ }
void CPU::CPL(){ }

// 30
void CPU::JR_NC_n(){ }
void CPU::LD_SP_nn(){ }
void CPU::LDD_pHL_A(){ }
void CPU::INC_SP(){ }
void CPU::INC_pHL(){ }
void CPU::DEC_pHL(){ }
void CPU::LD_pHL_n(){ }
void CPU::SCF(){ }
void CPU::JR_C_n(){ }
void CPU::ADD_HL_SP(){ }
void CPU::LDD_A_pHL(){ }
void CPU::DEC_SP(){ }
void CPU::INC_A(){ }
void CPU::DEC_A(){ }
void CPU::LD_A_n(){ }
void CPU::CCF(){ }

// 40
void CPU::LD_B_B(){ }
void CPU::LD_B_C(){ }
void CPU::LD_B_D(){ }
void CPU::LD_B_E(){ }
void CPU::LD_B_H(){ }
void CPU::LD_B_L(){ }
void CPU::LD_B_pHL(){ }
void CPU::LD_B_A(){ }
void CPU::LD_C_B(){ }
void CPU::LD_C_C(){ }
void CPU::LD_C_D(){ }
void CPU::LD_C_E(){ }
void CPU::LD_C_H(){ }
void CPU::LD_C_L(){ }
void CPU::LD_C_pHL(){ }
void CPU::LD_C_A(){ }

// 50
void CPU::LD_D_B(){ }
void CPU::LD_D_C(){ }
void CPU::LD_D_D(){ }
void CPU::LD_D_E(){ }
void CPU::LD_D_H(){ }
void CPU::LD_D_L(){ }
void CPU::LD_D_pHL(){ }
void CPU::LD_D_A(){ }
void CPU::LD_E_B(){ }
void CPU::LD_E_C(){ }
void CPU::LD_E_D(){ }
void CPU::LD_E_E(){ }
void CPU::LD_E_H(){ }
void CPU::LD_E_L(){ }
void CPU::LD_E_pHL(){ }
void CPU::LD_E_A(){ }

// 60
void CPU::LD_H_B(){ }
void CPU::LD_H_C(){ }
void CPU::LD_H_D(){ }
void CPU::LD_H_E(){ }
void CPU::LD_H_H(){ }
void CPU::LD_H_L(){ }
void CPU::LD_H_pHL(){ }
void CPU::LD_H_A(){ }
void CPU::LD_L_B(){ }
void CPU::LD_L_C(){ }
void CPU::LD_L_D(){ }
void CPU::LD_L_E(){ }
void CPU::LD_L_H(){ }
void CPU::LD_L_L(){ }
void CPU::LD_L_pHL(){ }
void CPU::LD_L_A(){ }

// 70
void CPU::LD_pHL_B(){ }
void CPU::LD_pHL_C(){ }
void CPU::LD_pHL_D(){ }
void CPU::LD_pHL_E(){ }
void CPU::LD_pHL_H(){ }
void CPU::LD_pHL_L(){ }
void CPU::HALT(){ }
void CPU::LD_pHL_A(){ }
void CPU::LD_A_B(){ }
void CPU::LD_A_C(){ }
void CPU::LD_A_D(){ }
void CPU::LD_A_E(){ }
void CPU::LD_A_H(){ }
void CPU::LD_A_L(){ }
void CPU::LD_A_pHL(){ }
void CPU::LD_A_A(){ }

// 80
void CPU::ADD_A_B(){ }
void CPU::ADD_A_C(){ }
void CPU::ADD_A_D(){ }
void CPU::ADD_A_E(){ }
void CPU::ADD_A_H(){ }
void CPU::ADD_A_L(){ }
void CPU::ADD_A_pHL(){ }
void CPU::ADD_A_A(){ }
void CPU::ADC_A_B(){ }
void CPU::ADC_A_C(){ }
void CPU::ADC_A_D(){ }
void CPU::ADC_A_E(){ }
void CPU::ADC_A_H(){ }
void CPU::ADC_A_L(){ }
void CPU::ADC_A_pHL(){ }
void CPU::ADC_A_A(){ }

// 90
void CPU::SUB_B(){ }
void CPU::SUB_C(){ }
void CPU::SUB_D(){ }
void CPU::SUB_E(){ }
void CPU::SUB_H(){ }
void CPU::SUB_L(){ }
void CPU::SUB_pHL(){ }
void CPU::SUB_A(){ }
void CPU::SBC_A_B(){ }
void CPU::SBC_A_C(){ }
void CPU::SBC_A_D(){ }
void CPU::SBC_A_E(){ }
void CPU::SBC_A_H(){ }
void CPU::SBC_A_L(){ }
void CPU::SBC_A_pHL(){ }
void CPU::SBC_A_A(){ }

// A0
void CPU::AND_B(){ }
void CPU::AND_C(){ }
void CPU::AND_D(){ }
void CPU::AND_E(){ }
void CPU::AND_H(){ }
void CPU::AND_L(){ }
void CPU::AND_pHL(){ }
void CPU::AND_A(){ }
void CPU::XOR_B(){ }
void CPU::XOR_C(){ }
void CPU::XOR_D(){ }
void CPU::XOR_E(){ }
void CPU::XOR_H(){ }
void CPU::XOR_L(){ }
void CPU::XOR_pHL(){ }
void CPU::XOR_A(){ }

// B0
void CPU::OR_B(){ }
void CPU::OR_C(){ }
void CPU::OR_D(){ }
void CPU::OR_E(){ }
void CPU::OR_H(){ }
void CPU::OR_L(){ }
void CPU::OR_pHL(){ }
void CPU::OR_A(){ }
void CPU::CP_B(){ }
void CPU::CP_C(){ }
void CPU::CP_D(){ }
void CPU::CP_E(){ }
void CPU::CP_H(){ }
void CPU::CP_L(){ }
void CPU::CL_pHL(){ }
void CPU::CP_A(){ }

// C0
void CPU::RET_NZ(){ }
void CPU::POP_BC(){ }
void CPU::JP_NZ_pnn(){ }
void CPU::JP_pnn(){ }
void CPU::CALL_NZ_pnn(){ }
void CPU::PUSH_BC(){ }
void CPU::ADD_A_n(){ }
void CPU::RST_00H(){ }
void CPU::RET_Z(){ }
void CPU::RET(){ }
void CPU::JP_Z_pnn(){ }
void CPU::PREFIX_CB(){ }
void CPU::CALL_Z_pnn(){ }
void CPU::CALL_pnn(){ }
void CPU::ADC_A_n(){ }
void CPU::RST_08H(){ }

// D0
void CPU::RET_NC(){ }
void CPU::POP_DE(){ }
void CPU::JP_NC_pnn(){ }
// XX
void CPU::CALL_NC_pnn(){ }
void CPU::PUSH_DE(){ }
void CPU::SUB_n(){ }
void CPU::RST_10H(){ }
void CPU::RET_C(){ }
void CPU::RETI(){ }
void CPU::JP_C_pnn(){ }
// XX
void CPU::CALL_C_pnn(){ }
// XX
void CPU::SBC_A_n(){ }
void CPU::RST_18H(){ }

// E0
void CPU::LDH_pnn_A(){ }
void CPU::POP_HL(){ }
void CPU::LD_pC_A(){ }
// XX
// XX
void CPU::PUSH_HL(){ }
void CPU::AND_n(){ }
void CPU::RST_20H(){ }
void CPU::ADD_SP_n(){ }
void CPU::JP_pHL(){ }
void CPU::LD_pnn_A(){ }
// XX
// XX
// XX
void CPU::XOR_n(){ }
void CPU::RST_28H(){ }

// F0
void CPU::LDH_A_pnn(){ }
void CPU::POP_AF(){ }
void CPU::LD_A_pC(){ }
void CPU::DI(){ }
// XX
void CPU::PUSH_AF(){ }
void CPU::OR_n(){ }
void CPU::RST_30H(){ }
void CPU::LDHL_SP_n(){ }
void CPU::LD_SP_HL(){ }
void CPU::LD_A_pnn(){ }
void CPU::EI(){ }
// XX
// XX
void CPU::CP_n(){ }
void CPU::RST_38H(){ }

// Now the CB opcodes.
// 00
void CPU::RLC_B(){ }
void CPU::RLC_C(){ }
void CPU::RLC_D(){ }
void CPU::RLC_E(){ }
void CPU::RLC_H(){ }
void CPU::RLC_L(){ }
void CPU::RLC_pHL(){ }
void CPU::RLC_A(){ }
void CPU::RRC_B(){ }
void CPU::RRC_C(){ }
void CPU::RRC_D(){ }
void CPU::RRC_E(){ }
void CPU::RRC_H(){ }
void CPU::RRC_L(){ }
void CPU::RRC_pHL(){ }
void CPU::RRC_A(){ }

// 10
void CPU::RL_B(){ }
void CPU::RL_C(){ }
void CPU::RL_D(){ }
void CPU::RL_E(){ }
void CPU::RL_H(){ }
void CPU::RL_L(){ }
void CPU::RL_pHL(){ }
void CPU::RL_A(){ }
void CPU::RR_B(){ }
void CPU::RR_C(){ }
void CPU::RR_D(){ }
void CPU::RR_E(){ }
void CPU::RR_H(){ }
void CPU::RR_L(){ }
void CPU::RR_pHL(){ }
void CPU::RR_A(){ }

// 20
void CPU::SLA_B(){ }
void CPU::SLA_C(){ }
void CPU::SLA_D(){ }
void CPU::SLA_E(){ }
void CPU::SLA_H(){ }
void CPU::SLA_L(){ }
void CPU::SLA_pHL(){ }
void CPU::SLA_A(){ }
void CPU::SRA_B(){ }
void CPU::SRA_C(){ }
void CPU::SRA_D(){ }
void CPU::SRA_E(){ }
void CPU::SRA_H(){ }
void CPU::SRA_L(){ }
void CPU::SRA_pHL(){ }
void CPU::SRA_A(){ }

// 30
void CPU::SWAP_B(){ }
void CPU::SWAP_C(){ }
void CPU::SWAP_D(){ }
void CPU::SWAP_E(){ }
void CPU::SWAP_H(){ }
void CPU::SWAP_L(){ }
void CPU::SWAP_pHL(){ }
void CPU::SWAP_A(){ }
void CPU::SRL_B(){ }
void CPU::SRL_C(){ }
void CPU::SRL_D(){ }
void CPU::SRL_E(){ }
void CPU::SRL_H(){ }
void CPU::SRL_L(){ }
void CPU::SRL_pHL(){ }
void CPU::SRL_A(){ }

// 40
void CPU::BIT_0_B(){ }
void CPU::BIT_0_C(){ }
void CPU::BIT_0_D(){ }
void CPU::BIT_0_E(){ }
void CPU::BIT_0_H(){ }
void CPU::BIT_0_L(){ }
void CPU::BIT_0_pHL(){ }
void CPU::BIT_0_A(){ }
void CPU::BIT_1_B(){ }
void CPU::BIT_1_C(){ }
void CPU::BIT_1_D(){ }
void CPU::BIT_1_E(){ }
void CPU::BIT_1_H(){ }
void CPU::BIT_1_L(){ }
void CPU::BIT_1_pHL(){ }
void CPU::BIT_1_A(){ }

// 50
void CPU::BIT_2_B(){ }
void CPU::BIT_2_C(){ }
void CPU::BIT_2_D(){ }
void CPU::BIT_2_E(){ }
void CPU::BIT_2_H(){ }
void CPU::BIT_2_L(){ }
void CPU::BIT_2_pHL(){ }
void CPU::BIT_2_A(){ }
void CPU::BIT_3_B(){ }
void CPU::BIT_3_C(){ }
void CPU::BIT_3_D(){ }
void CPU::BIT_3_E(){ }
void CPU::BIT_3_H(){ }
void CPU::BIT_3_L(){ }
void CPU::BIT_3_pHL(){ }
void CPU::BIT_3_A(){ }

// 60
void CPU::BIT_4_B(){ }
void CPU::BIT_4_C(){ }
void CPU::BIT_4_D(){ }
void CPU::BIT_4_E(){ }
void CPU::BIT_4_H(){ }
void CPU::BIT_4_L(){ }
void CPU::BIT_4_pHL(){ }
void CPU::BIT_4_A(){ }
void CPU::BIT_5_B(){ }
void CPU::BIT_5_C(){ }
void CPU::BIT_5_D(){ }
void CPU::BIT_5_E(){ }
void CPU::BIT_5_H(){ }
void CPU::BIT_5_L(){ }
void CPU::BIT_5_pHL(){ }
void CPU::BIT_5_A(){ }

// 70
void CPU::BIT_6_B(){ }
void CPU::BIT_6_C(){ }
void CPU::BIT_6_D(){ }
void CPU::BIT_6_E(){ }
void CPU::BIT_6_H(){ }
void CPU::BIT_6_L(){ }
void CPU::BIT_6_pHL(){ }
void CPU::BIT_6_A(){ }
void CPU::BIT_7_B(){ }
void CPU::BIT_7_C(){ }
void CPU::BIT_7_D(){ }
void CPU::BIT_7_E(){ }
void CPU::BIT_7_H(){ }
void CPU::BIT_7_L(){ }
void CPU::BIT_7_pHL(){ }
void CPU::BIT_7_A(){ }

// 80
void CPU::RES_0_B(){ }
void CPU::RES_0_C(){ }
void CPU::RES_0_D(){ }
void CPU::RES_0_E(){ }
void CPU::RES_0_H(){ }
void CPU::RES_0_L(){ }
void CPU::RES_0_pHL(){ }
void CPU::RES_0_A(){ }
void CPU::RES_1_B(){ }
void CPU::RES_1_C(){ }
void CPU::RES_1_D(){ }
void CPU::RES_1_E(){ }
void CPU::RES_1_H(){ }
void CPU::RES_1_L(){ }
void CPU::RES_1_pHL(){ }
void CPU::RES_1_A(){ }

// 90
void CPU::RES_2_B(){ }
void CPU::RES_2_C(){ }
void CPU::RES_2_D(){ }
void CPU::RES_2_E(){ }
void CPU::RES_2_H(){ }
void CPU::RES_2_L(){ }
void CPU::RES_2_pHL(){ }
void CPU::RES_2_A(){ }
void CPU::RES_3_B(){ }
void CPU::RES_3_C(){ }
void CPU::RES_3_D(){ }
void CPU::RES_3_E(){ }
void CPU::RES_3_H(){ }
void CPU::RES_3_L(){ }
void CPU::RES_3_pHL(){ }
void CPU::RES_3_A(){ }

// A0
void CPU::RES_4_B(){ }
void CPU::RES_4_C(){ }
void CPU::RES_4_D(){ }
void CPU::RES_4_E(){ }
void CPU::RES_4_H(){ }
void CPU::RES_4_L(){ }
void CPU::RES_4_pHL(){ }
void CPU::RES_4_A(){ }
void CPU::RES_5_B(){ }
void CPU::RES_5_C(){ }
void CPU::RES_5_D(){ }
void CPU::RES_5_E(){ }
void CPU::RES_5_H(){ }
void CPU::RES_5_L(){ }
void CPU::RES_5_pHL(){ }
void CPU::RES_5_A(){ }

// B0
void CPU::RES_6_B(){ }
void CPU::RES_6_C(){ }
void CPU::RES_6_D(){ }
void CPU::RES_6_E(){ }
void CPU::RES_6_H(){ }
void CPU::RES_6_L(){ }
void CPU::RES_6_pHL(){ }
void CPU::RES_6_A(){ }
void CPU::RES_7_B(){ }
void CPU::RES_7_C(){ }
void CPU::RES_7_D(){ }
void CPU::RES_7_E(){ }
void CPU::RES_7_H(){ }
void CPU::RES_7_L(){ }
void CPU::RES_7_pHL(){ }
void CPU::RES_7_A(){ }

// C0
void CPU::SET_0_B(){ }
void CPU::SET_0_C(){ }
void CPU::SET_0_D(){ }
void CPU::SET_0_E(){ }
void CPU::SET_0_H(){ }
void CPU::SET_0_L(){ }
void CPU::SET_0_pHL(){ }
void CPU::SET_0_A(){ }
void CPU::SET_1_B(){ }
void CPU::SET_1_C(){ }
void CPU::SET_1_D(){ }
void CPU::SET_1_E(){ }
void CPU::SET_1_H(){ }
void CPU::SET_1_L(){ }
void CPU::SET_1_pHL(){ }
void CPU::SET_1_A(){ }

// D0
void CPU::SET_2_B(){ }
void CPU::SET_2_C(){ }
void CPU::SET_2_D(){ }
void CPU::SET_2_E(){ }
void CPU::SET_2_H(){ }
void CPU::SET_2_L(){ }
void CPU::SET_2_pHL(){ }
void CPU::SET_2_A(){ }
void CPU::SET_3_B(){ }
void CPU::SET_3_C(){ }
void CPU::SET_3_D(){ }
void CPU::SET_3_E(){ }
void CPU::SET_3_H(){ }
void CPU::SET_3_L(){ }
void CPU::SET_3_pHL(){ }
void CPU::SET_3_A(){ }

// E0
void CPU::SET_4_B(){ }
void CPU::SET_4_C(){ }
void CPU::SET_4_D(){ }
void CPU::SET_4_E(){ }
void CPU::SET_4_H(){ }
void CPU::SET_4_L(){ }
void CPU::SET_4_pHL(){ }
void CPU::SET_4_A(){ }
void CPU::SET_5_B(){ }
void CPU::SET_5_C(){ }
void CPU::SET_5_D(){ }
void CPU::SET_5_E(){ }
void CPU::SET_5_H(){ }
void CPU::SET_5_L(){ }
void CPU::SET_5_pHL(){ }
void CPU::SET_5_A(){ }

// F0
void CPU::SET_6_B(){ }
void CPU::SET_6_C(){ }
void CPU::SET_6_D(){ }
void CPU::SET_6_E(){ }
void CPU::SET_6_H(){ }
void CPU::SET_6_L(){ }
void CPU::SET_6_pHL(){ }
void CPU::SET_6_A(){ }
void CPU::SET_7_B(){ }
void CPU::SET_7_C(){ }
void CPU::SET_7_D(){ }
void CPU::SET_7_E(){ }
void CPU::SET_7_H(){ }
void CPU::SET_7_L(){ }
void CPU::SET_7_pHL(){ }
void CPU::SET_7_A(){ }