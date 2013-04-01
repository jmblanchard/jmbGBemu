// CPU.cpp
// Author: Jason Blanchard
// Implement CPU class, which will control the emulation by fetch/decode/execute
// instructions through communication with the MMU.

#include "CPU.h"

// Definition of both opcodes_ and cb_opcodes_
fn CPU::opcodes_[] = {
	// 00
	&CPU::NOP,			&CPU::LD_BC_nn,		&CPU::LD_pBC_A,		&CPU::INC_BC,
	&CPU::INC_B,		&CPU::DEC_B,		&CPU::LD_B_n,		&CPU::RLCA,
	&CPU::LD_pnn_SP,	&CPU::ADD_HL_BC,	&CPU::LD_A_pBC,		&CPU::DEC_BC,
	&CPU::INC_C,		&CPU::DEC_C,		&CPU::LD_C_n,		&CPU::RRCA,

	// 10
	&CPU::STOP,			&CPU::LD_DE_nn,		&CPU::LD_pDE_A,		&CPU::INC_DE,
	&CPU::INC_D,		&CPU::DEC_D,		&CPU::LD_D_n,		&CPU::RLA,
	&CPU::JR_n,			&CPU::ADD_HL_DE,	&CPU::LD_A_pDE,		&CPU::DEC_DE,
	&CPU::INC_E,		&CPU::DEC_E,		&CPU::LD_E_n,		&CPU::RRA,

	// 20
	&CPU::JR_NZ_n,		&CPU::LD_HL_nn,		&CPU::LDI_pHL_A,	&CPU::INC_HL,
	&CPU::INC_H,		&CPU::DEC_H,		&CPU::LD_H_n,		&CPU::DAA,
	&CPU::JR_Z_n,		&CPU::ADD_HL_HL,	&CPU::LDI_A_pHL,	&CPU::DEC_HL,
	&CPU::INC_L,		&CPU::DEC_L,		&CPU::LD_L_n,		&CPU::CPL,

	// 30
	&CPU::JR_NC_n,		&CPU::LD_SP_nn,		&CPU::LDD_pHL_A,	&CPU::INC_SP,
	&CPU::INC_pHL,		&CPU::DEC_pHL,		&CPU::LD_pHL_n,		&CPU::SCF,
	&CPU::JR_C_n,		&CPU::ADD_HL_SP,	&CPU::LDD_A_pHL,	&CPU::DEC_SP,
	&CPU::INC_A,		&CPU::DEC_A,		&CPU::LD_A_n,		&CPU::CCF,

	// 40
	&CPU::LD_B_B,		&CPU::LD_B_C,		&CPU::LD_B_D,		&CPU::LD_B_E,
	&CPU::LD_B_H,		&CPU::LD_B_L,		&CPU::LD_B_pHL,		&CPU::LD_B_A,
	&CPU::LD_C_B,		&CPU::LD_C_C,		&CPU::LD_C_D,		&CPU::LD_C_E,
	&CPU::LD_C_H,		&CPU::LD_C_L,		&CPU::LD_C_pHL,		&CPU::LD_C_A,

	// 50
	&CPU::LD_D_B,		&CPU::LD_D_C,		&CPU::LD_D_D,		&CPU::LD_D_E,
	&CPU::LD_D_H,		&CPU::LD_D_L,		&CPU::LD_D_pHL,		&CPU::LD_D_A,
	&CPU::LD_E_B,		&CPU::LD_E_C,		&CPU::LD_E_D,		&CPU::LD_E_E,
	&CPU::LD_E_H,		&CPU::LD_E_L,		&CPU::LD_E_pHL,		&CPU::LD_E_A,

	// 60
	&CPU::LD_H_B,		&CPU::LD_H_C,		&CPU::LD_H_D,		&CPU::LD_H_E,
	&CPU::LD_H_H,		&CPU::LD_H_L,		&CPU::LD_H_pHL,		&CPU::LD_H_A,
	&CPU::LD_L_B,		&CPU::LD_L_C,		&CPU::LD_L_D,		&CPU::LD_L_E,
	&CPU::LD_L_H,		&CPU::LD_L_L,		&CPU::LD_L_pHL,		&CPU::LD_L_A,

	// 70
	&CPU::LD_pHL_B,		&CPU::LD_pHL_C,		&CPU::LD_pHL_D,		&CPU::LD_pHL_E,
	&CPU::LD_pHL_H,		&CPU::LD_pHL_L,		&CPU::HALT,			&CPU::LD_pHL_A,
	&CPU::LD_A_B,		&CPU::LD_A_C,		&CPU::LD_A_D,		&CPU::LD_A_E,
	&CPU::LD_A_H,		&CPU::LD_A_L,		&CPU::LD_A_pHL,		&CPU::LD_A_A,

	// 80
	&CPU::ADD_A_B,		&CPU::ADD_A_C,		&CPU::ADD_A_D,		&CPU::ADD_A_E,
	&CPU::ADD_A_H,		&CPU::ADD_A_L,		&CPU::ADD_A_pHL,	&CPU::ADD_A_A,
	&CPU::ADC_A_B,		&CPU::ADC_A_C,		&CPU::ADC_A_D,		&CPU::ADC_A_E,
	&CPU::ADC_A_H,		&CPU::ADC_A_L,		&CPU::ADC_A_pHL,	&CPU::ADC_A_A,

	// 90
	&CPU::SUB_B,		&CPU::SUB_C,		&CPU::SUB_D,		&CPU::SUB_E,
	&CPU::SUB_H,		&CPU::SUB_L,		&CPU::SUB_pHL,		&CPU::SUB_A,
	&CPU::SBC_A_B,		&CPU::SBC_A_C,		&CPU::SBC_A_D,		&CPU::SBC_A_E,
	&CPU::SBC_A_H,		&CPU::SBC_A_L,		&CPU::SBC_A_pHL,	&CPU::SBC_A_A,

	// A0
	&CPU::AND_B,		&CPU::AND_C,		&CPU::AND_D,		&CPU::AND_E,
	&CPU::AND_H,		&CPU::AND_L,		&CPU::AND_pHL,		&CPU::AND_A,
	&CPU::XOR_B,		&CPU::XOR_C,		&CPU::XOR_D,		&CPU::XOR_E,
	&CPU::XOR_H,		&CPU::XOR_L,		&CPU::XOR_pHL,		&CPU::XOR_A,

	// B0
	&CPU::OR_B,			&CPU::OR_C,			&CPU::OR_D,			&CPU::OR_E,
	&CPU::OR_H,			&CPU::OR_L,			&CPU::OR_pHL,		&CPU::OR_A,
	&CPU::CP_B,			&CPU::CP_C,			&CPU::CP_D,			&CPU::CP_E,
	&CPU::CP_H,			&CPU::CP_L,			&CPU::CL_pHL,		&CPU::CP_A,
	
	// C0
	&CPU::RET_NZ,		&CPU::POP_BC,		&CPU::JP_NZ_pnn,	&CPU::JP_pnn,
	&CPU::CALL_NZ_pnn,	&CPU::PUSH_BC,		&CPU::ADD_A_n,		&CPU::RST_00H,
	&CPU::RET_Z,		&CPU::RET,			&CPU::JP_Z_pnn,		&CPU::PREFIX_CB,
	&CPU::CALL_Z_pnn,	&CPU::CALL_pnn,		&CPU::ADC_A_n,		&CPU::RST_08H,

	// D0
	&CPU::RET_NC,		&CPU::POP_DE,		&CPU::JP_NC_pnn,	&CPU::XX,
	&CPU::CALL_NC_pnn,	&CPU::PUSH_DE,		&CPU::SUB_n,		&CPU::RST_10H,
	&CPU::RET_C,		&CPU::RETI,			&CPU::JP_C_pnn,		&CPU::XX,
	&CPU::CALL_C_pnn,	&CPU::XX,			&CPU::SBC_A_n,		&CPU::RST_18H,

	// E0
	&CPU::LDH_pnn_A,	&CPU::POP_HL,		&CPU::LD_pC_A,		&CPU::XX,
	&CPU::XX,			&CPU::PUSH_HL,		&CPU::AND_n,		&CPU::RST_20H,
	&CPU::ADD_SP_n,		&CPU::JP_pHL,		&CPU::LD_pnn_A,		&CPU::XX,
	&CPU::XX,			&CPU::XX,			&CPU::XOR_n,		&CPU::RST_28H,

	// F0
	&CPU::LDH_A_pnn,	&CPU::POP_AF,		&CPU::LD_A_pC,		&CPU::DI,
	&CPU::XX,			&CPU::PUSH_AF,		&CPU::OR_n,			&CPU::RST_30H,
	&CPU::LDHL_SP_n,	&CPU::LD_SP_HL,		&CPU::LD_A_pnn,		&CPU::EI,
	&CPU::XX,			&CPU::XX,			&CPU::CP_n,			&CPU::RST_38H
};

fn CPU::cb_opcodes_[] = {
	// 00
	&CPU::RLC_B,		&CPU::RLC_C,		&CPU::RLC_D,		&CPU::RLC_E,
	&CPU::RLC_H,		&CPU::RLC_L,		&CPU::RLC_pHL,		&CPU::RLC_A,
	&CPU::RRC_B,		&CPU::RRC_C,		&CPU::RRC_D,		&CPU::RRC_E,
	&CPU::RRC_H,		&CPU::RRC_L,		&CPU::RRC_pHL,		&CPU::RRC_A,

	// 10
	&CPU::RL_B,			&CPU::RL_C,			&CPU::RL_D,			&CPU::RL_E,
	&CPU::RL_H,			&CPU::RL_L,			&CPU::RL_pHL,		&CPU::RL_A,
	&CPU::RR_B,			&CPU::RR_C,			&CPU::RR_D,			&CPU::RR_E,
	&CPU::RR_H,			&CPU::RR_L,			&CPU::RR_pHL,		&CPU::RR_A,

	// 20
	&CPU::SLA_B,		&CPU::SLA_C,		&CPU::SLA_D,		&CPU::SLA_E,
	&CPU::SLA_H,		&CPU::SLA_L,		&CPU::SLA_pHL,		&CPU::SLA_A,
	&CPU::SRA_B,		&CPU::SRA_C,		&CPU::SRA_D,		&CPU::SRA_E,
	&CPU::SRA_H,		&CPU::SRA_L,		&CPU::SRA_pHL,		&CPU::SRA_A,

	// 30
	&CPU::SWAP_B,		&CPU::SWAP_C,		&CPU::SWAP_D,		&CPU::SWAP_E,
	&CPU::SWAP_H,		&CPU::SWAP_L,		&CPU::SWAP_pHL,		&CPU::SWAP_A,
	&CPU::SRL_B,		&CPU::SRL_C,		&CPU::SRL_D,		&CPU::SRL_E,
	&CPU::SRL_H,		&CPU::SRL_L,		&CPU::SRL_pHL,		&CPU::SRL_A,

	// 40
	&CPU::BIT_0_B,		&CPU::BIT_0_C,		&CPU::BIT_0_D,		&CPU::BIT_0_E,
	&CPU::BIT_0_H,		&CPU::BIT_0_L,		&CPU::BIT_0_pHL,	&CPU::BIT_0_A,
	&CPU::BIT_1_B,		&CPU::BIT_1_C,		&CPU::BIT_1_D,		&CPU::BIT_1_E,
	&CPU::BIT_1_H,		&CPU::BIT_1_L,		&CPU::BIT_1_pHL,	&CPU::BIT_1_A,

	// 50
	&CPU::BIT_2_B,		&CPU::BIT_2_C,		&CPU::BIT_2_D,		&CPU::BIT_2_E,
	&CPU::BIT_2_H,		&CPU::BIT_2_L,		&CPU::BIT_2_pHL,	&CPU::BIT_2_A,
	&CPU::BIT_3_B,		&CPU::BIT_3_C,		&CPU::BIT_3_D,		&CPU::BIT_3_E,
	&CPU::BIT_3_H,		&CPU::BIT_3_L,		&CPU::BIT_3_pHL,	&CPU::BIT_3_A,

	// 60
	&CPU::BIT_4_B,		&CPU::BIT_4_C,		&CPU::BIT_4_D,		&CPU::BIT_4_E,
	&CPU::BIT_4_H,		&CPU::BIT_4_L,		&CPU::BIT_4_pHL,	&CPU::BIT_4_A,
	&CPU::BIT_5_B,		&CPU::BIT_5_C,		&CPU::BIT_5_D,		&CPU::BIT_5_E,
	&CPU::BIT_5_H,		&CPU::BIT_5_L,		&CPU::BIT_5_pHL,	&CPU::BIT_5_A,

	// 70
	&CPU::BIT_6_B,		&CPU::BIT_6_C,		&CPU::BIT_6_D,		&CPU::BIT_6_E,
	&CPU::BIT_6_H,		&CPU::BIT_6_L,		&CPU::BIT_6_pHL,	&CPU::BIT_6_A,
	&CPU::BIT_7_B,		&CPU::BIT_7_C,		&CPU::BIT_7_D,		&CPU::BIT_7_E,
	&CPU::BIT_7_H,		&CPU::BIT_7_L,		&CPU::BIT_7_pHL,	&CPU::BIT_7_A,

	// 80
	&CPU::RES_0_B,		&CPU::RES_0_C,		&CPU::RES_0_D,		&CPU::RES_0_E,
	&CPU::RES_0_H,		&CPU::RES_0_L,		&CPU::RES_0_pHL,	&CPU::RES_0_A,
	&CPU::RES_1_B,		&CPU::RES_1_C,		&CPU::RES_1_D,		&CPU::RES_1_E,
	&CPU::RES_1_H,		&CPU::RES_1_L,		&CPU::RES_1_pHL,	&CPU::RES_1_A,

	// 90
	&CPU::RES_2_B,		&CPU::RES_2_C,		&CPU::RES_2_D,		&CPU::RES_2_E,
	&CPU::RES_2_H,		&CPU::RES_2_L,		&CPU::RES_2_pHL,	&CPU::RES_2_A,
	&CPU::RES_3_B,		&CPU::RES_3_C,		&CPU::RES_3_D,		&CPU::RES_3_E,
	&CPU::RES_3_H,		&CPU::RES_3_L,		&CPU::RES_3_pHL,	&CPU::RES_3_A,

	// A0
	&CPU::RES_4_B,		&CPU::RES_4_C,		&CPU::RES_4_D,		&CPU::RES_4_E,
	&CPU::RES_4_H,		&CPU::RES_4_L,		&CPU::RES_4_pHL,	&CPU::RES_4_A,
	&CPU::RES_5_B,		&CPU::RES_5_C,		&CPU::RES_5_D,		&CPU::RES_5_E,
	&CPU::RES_5_H,		&CPU::RES_5_L,		&CPU::RES_5_pHL,	&CPU::RES_5_A,

	// B0
	&CPU::RES_6_B,		&CPU::RES_6_C,		&CPU::RES_6_D,		&CPU::RES_6_E,
	&CPU::RES_6_H,		&CPU::RES_6_L,		&CPU::RES_6_pHL,	&CPU::RES_6_A,
	&CPU::RES_7_B,		&CPU::RES_7_C,		&CPU::RES_7_D,		&CPU::RES_7_E,
	&CPU::RES_7_H,		&CPU::RES_7_L,		&CPU::RES_7_pHL,	&CPU::RES_7_A,

	// C0
	&CPU::SET_0_B,		&CPU::SET_0_C,		&CPU::SET_0_D,		&CPU::SET_0_E,
	&CPU::SET_0_H,		&CPU::SET_0_L,		&CPU::SET_0_pHL,	&CPU::SET_0_A,
	&CPU::SET_1_B,		&CPU::SET_1_C,		&CPU::SET_1_D,		&CPU::SET_1_E,
	&CPU::SET_1_H,		&CPU::SET_1_L,		&CPU::SET_1_pHL,	&CPU::SET_1_A,

	// D0
	&CPU::SET_2_B,		&CPU::SET_2_C,		&CPU::SET_2_D,		&CPU::SET_2_E,
	&CPU::SET_2_H,		&CPU::SET_2_L,		&CPU::SET_2_pHL,	&CPU::SET_2_A,
	&CPU::SET_3_B,		&CPU::SET_3_C,		&CPU::SET_3_D,		&CPU::SET_3_E,
	&CPU::SET_3_H,		&CPU::SET_3_L,		&CPU::SET_3_pHL,	&CPU::SET_3_A,

	// E0
	&CPU::SET_4_B,		&CPU::SET_4_C,		&CPU::SET_4_D,		&CPU::SET_4_E,
	&CPU::SET_4_H,		&CPU::SET_4_L,		&CPU::SET_4_pHL,	&CPU::SET_4_A,
	&CPU::SET_5_B,		&CPU::SET_5_C,		&CPU::SET_5_D,		&CPU::SET_5_E,
	&CPU::SET_5_H,		&CPU::SET_5_L,		&CPU::SET_5_pHL,	&CPU::SET_5_A,

	// F0
	&CPU::SET_6_B,		&CPU::SET_6_C,		&CPU::SET_6_D,		&CPU::SET_6_E,
	&CPU::SET_6_H,		&CPU::SET_6_L,		&CPU::SET_6_pHL,	&CPU::SET_6_A,
	&CPU::SET_7_B,		&CPU::SET_7_C,		&CPU::SET_7_D,		&CPU::SET_7_E,
	&CPU::SET_7_H,		&CPU::SET_7_L,		&CPU::SET_7_pHL,	&CPU::SET_7_A
};

CPU::CPU(MMU *mmu, HeaderInfo *hi) {
	hi_ = hi;
	mmu_ = mmu;
	A_ = B_ = C_ = D_ = F_ = H_ = L_ = 0;
	SP_ = 0xFFFE;
	PC_ = 0x100;
}

CPU::~CPU() { }

void CPU::run() { }

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