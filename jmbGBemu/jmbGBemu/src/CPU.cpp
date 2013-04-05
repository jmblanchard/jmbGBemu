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

CPU::CPU(MMU *mmu, Emulator *emu, HeaderInfo *hi) {
	hi_ = hi;
	emu_ = emu;
	mmu_ = mmu;
	A_ = B_ = C_ = D_ = F_ = H_ = L_ = 0;
	SP_ = 0xFFFE;
	PC_ = 0x100;
	cycles_done_ = 0;
}

CPU::~CPU() { }

void CPU::handleInterrupts() {
}

int CPU::run() {
	// fetch
	if (!halted_) {
		BYTE curr_op;
		mmu_->readByte(PC_++, curr_op);

		// decode and execute
		opcodes_[curr_op];

		return cycles_done_;
	} else {
		return 0;
	}
}

// Opcode functions.
void CPU::XX() {
}

// 00
void CPU::NOP(){
	cycles_done_ = 4;
}

void CPU::LD_BC_nn(){
	WORD temp;
	mmu_->readWord(PC_, temp);
	B_ = (temp >> 8) & 0xFF;
	C_ = temp&0xFF;
	PC_+=2;

	cycles_done_ = 12;
}

void CPU::LD_pBC_A(){
	WORD address = (B_ << 8) | C_;
	mmu_->writeByte(address, A_);

	cycles_done_ = 8;
}

void CPU::INC_BC(){
	C_ += 0x01;
	if (!C_)
		B_ += 0x01;

	cycles_done_ = 8;
}

void CPU::INC_B(){
	BYTE temp = B_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((B_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	B_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_B(){
	BYTE temp = B_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((B_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	B_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_B_n(){
	mmu_->readByte(PC_++, B_);

	cycles_done_ = 8;
}

void CPU::RLCA(){
	BYTE newCarry = (A_ & 0x80) >> 3;
	BYTE oldCarry = (F_ & 0x10) >> 4;
	A_ = A_ << 1;
	if (oldCarry)
		A_ |= oldCarry;
	else
		A_ &= !(oldCarry);

	if (newCarry)
		F_ |= newCarry;
	else
		F_ &= !(newCarry);

	F_ &= !(0xE0); // reset other flags

	cycles_done_ = 4;
}

void CPU::LD_pnn_SP(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_+=2;

	mmu_->writeWord(address, SP_);

	cycles_done_ = 20;
}

void CPU::ADD_HL_BC(){
	WORD hl = (H_ << 8) | L_;
	WORD bc = (B_ << 8) | C_;
	WORD sum = hl + bc;

	// did we have a carry?
	if (sum < hl && sum < bc)
		F_ |= 0x10;
	F_ &= !(0x40); // reset N
	// not implementing H flag
	H_ = sum >> 8;
	L_ = sum & 0xFF;

	cycles_done_ = 8;
}

void CPU::LD_A_pBC(){
	WORD address = (B_ << 8) | C_;
	mmu_->readByte(address, A_);

	cycles_done_ = 8;
}

void CPU::DEC_BC(){
	WORD w = (B_ << 8) | C_;
	w--;
	B_ = w >> 8;
	C_ = w & 0xFF;

	cycles_done_ = 8;
}

void CPU::INC_C(){
	BYTE temp = C_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((C_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	C_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_C(){
	BYTE temp = C_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((C_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	C_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_C_n(){
	mmu_->readByte(PC_++, C_);

	cycles_done_ = 8;
}

void CPU::RRCA(){
	BYTE newCarry = A_ & 0x01;
	BYTE oldCarry = F_ & 0x10;
	A_ = A_ >> 1;

	if (oldCarry)
		A_ |= 0x80;
	else
		A_ &= !(0x80);

	if (newCarry)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	F_ &= !(0xE0); // reset other flags

	cycles_done_ = 4;
}

// 10
void CPU::STOP(){
	// not implementing this unless absolutely necessary
}

void CPU::LD_DE_nn(){
	WORD temp;
	mmu_->readWord(PC_, temp);
	D_ = (temp >> 8) & 0xFF;
	E_ = temp&0xFF;
	PC_+=2;

	cycles_done_ = 12;
}

void CPU::LD_pDE_A(){
	WORD address = (D_ << 8) | E_;
	mmu_->writeByte(address, A_);

	cycles_done_ = 8;
}

void CPU::INC_DE(){
	E_ += 0x01;
	if (!E_)
		D_ += 0x01;

	cycles_done_ = 8;
}

void CPU::INC_D(){
	BYTE temp = D_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((D_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	D_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_D(){
	BYTE temp = D_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((D_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	D_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_D_n(){
	mmu_->readByte(PC_++, D_);

	cycles_done_ = 8;
}

void CPU::RLA(){
	BYTE newCarry = (A_ & 0x80) >> 3;
	A_ = A_ << 1;

	if (newCarry) {
		A_ |= newCarry >> 4;
		F_ |= newCarry;
	} else {
		A_ &= !(newCarry >> 4);
		F_ &= !(newCarry);
	}

	F_ &= !(0xE0); // reset other flags

	cycles_done_ = 4;
}

void CPU::JR_n(){
	BYTE n; int8_t m=0;
	mmu_->readByte(PC_++, n);
	m |= n;
	PC_ += m;

	cycles_done_ = 12;
}

void CPU::ADD_HL_DE(){
	WORD hl = (H_ << 8) | L_;
	WORD de = (D_ << 8) | E_;
	WORD sum = hl + de;

	// did we have a carry?
	if (sum < hl && sum < de)
		F_ |= 0x10;
	F_ &= !(0x40); // reset N
	// not implementing H flag
	H_ = sum >> 8;
	L_ = sum & 0xFF;

	cycles_done_ = 8;
}

void CPU::LD_A_pDE(){
	WORD address = (D_ << 8) | E_;
	mmu_->readByte(address, A_);

	cycles_done_ = 8;
}

void CPU::DEC_DE(){
	WORD w = (D_ << 8) | E_;
	w--;
	D_ = w >> 8;
	E_ = w & 0xFF;

	cycles_done_ = 8;
}

void CPU::INC_E(){
	BYTE temp = E_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((E_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	E_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_E(){
	BYTE temp = E_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((E_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	E_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_E_n(){
	mmu_->readByte(PC_++, E_);

	cycles_done_ = 8;
}

void CPU::RRA(){
	BYTE newCarry = A_ & 0x01;
	A_ = A_ >> 1;

	if (newCarry) {
		A_ |= 0x01;
		F_ |= 0x10;
	} else {
		A_ &= !(0x01);
		F_ &= !(0x10);
	}

	cycles_done_ = 4;
}

// 20
void CPU::JR_NZ_n(){
	BYTE n; int8_t m = 0;
	mmu_->readByte(PC_++, n);
	m |= n;

	if (F_ & 0x80) {
		cycles_done_ = 8;
	} else {
		PC_ = PC_ + m;
		cycles_done_ = 12;
	}
}

void CPU::LD_HL_nn(){
	WORD temp;
	mmu_->readWord(PC_, temp);
	H_ = (temp >> 8) & 0xFF;
	L_ = temp&0xFF;
	PC_+=2;

	cycles_done_ = 12;
}

void CPU::LDI_pHL_A(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, A_);

	INC_HL();
	// INC_HL has the cycles done set to 8. This is exactly what we need.
	// So we don't set cycles_done_ here
}

void CPU::INC_HL(){
	L_ += 0x01;
	if (!L_)
		H_ += 0x01;

	cycles_done_ = 8;
}

void CPU::INC_H(){
	BYTE temp = H_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((H_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	H_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_H(){
	BYTE temp = H_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((H_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	H_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_H_n(){
	mmu_->readByte(PC_++, H_);

	cycles_done_ = 8;
}

void CPU::DAA(){
	// I don't care about this opcode, but I will print if a ROM uses
	// it so I know that I have to use it. I will implement it then.
	std::cout << "OPCODE DAA() used. MUST IMPLEMENT.\n";
}

void CPU::JR_Z_n(){
	BYTE n; int8_t m = 0;
	mmu_->readByte(PC_++, n);
	m |= n;

	if (F_ & 0x80) {
		PC_ = PC_ + m;
		cycles_done_ = 12;
	} else {
		cycles_done_ = 8;
	}
}

void CPU::ADD_HL_HL(){
	WORD hl = (H_ << 8) | L_;
	WORD sum = hl + hl;

	// did we have a carry?
	if (sum < hl)
		F_ |= 0x10;
	F_ &= !(0x40); // reset N
	// not implementing H flag
	H_ = sum >> 8;
	L_ = sum & 0xFF;

	cycles_done_ = 8;
}

void CPU::LDI_A_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, A_);

	INC_HL();
	// we don't set cycles_done_ because INC_HL already
	// sets the correct one.
}

void CPU::DEC_HL(){
	WORD w = (H_ << 8) | L_;
	w--;
	H_ = w >> 8;
	L_ = w & 0xFF;

	cycles_done_ = 8;
}

void CPU::INC_L(){
	BYTE temp = L_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((L_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	L_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_L(){
	BYTE temp = L_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((L_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	L_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_L_n(){
	mmu_->readByte(PC_++, L_);

	cycles_done_ = 8;
}

void CPU::CPL(){
	A_ ^= 0xFF;
	F_ |= 0x60; // set the N and H flags

	cycles_done_ = 4;
}

// 30
void CPU::JR_NC_n(){
	BYTE n; int8_t m = 0;
	mmu_->readByte(PC_++, n);
	m |= n;

	if (F_ & 0x10) {
		cycles_done_ = 8;
	} else {
		PC_ = PC_ + m;
		cycles_done_ = 12;
	}
}

void CPU::LD_SP_nn(){
	WORD temp;
	mmu_->readWord(PC_, temp);
	SP_ = temp;
	PC_+=2;

	cycles_done_ = 12;
}

void CPU::LDD_pHL_A(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, A_);

	DEC_HL();
	// We don't need to set cycles_done_ here
	// because DEC_HL sets the correct value.
}

void CPU::INC_SP(){
	SP_++;

	cycles_done_ = 8;
}

void CPU::INC_pHL(){
	BYTE b;
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, b);
	b++;
	mmu_->writeByte(address, b);

	// zero flag
	if (b == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// N flag
	F_ &= !(0x40);

	// HALF CARRY FLAG SHOULD BE SET BUT I'M SKIPPING UNLESS I NEED TO DO DAA

	cycles_done_ = 12;
}

void CPU::DEC_pHL(){
	BYTE b;
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, b);
	b--;
	mmu_->writeByte(address, b);

	// zero flag
	if (b == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// N flag
	F_ |= 0x40;

	// HALF CARRY FLAG SHOULD BE SET BUT I'M SKIPPING UNLESS I NEED TO DO DAA

	cycles_done_ = 12;
}

void CPU::LD_pHL_n(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, PC_++);

	cycles_done_ = 12;
}

void CPU::SCF(){
	F_ &= !(0x60); // reset N and H
	F_ |= 0x10; // set carry flag

	cycles_done_ = 4;
}

void CPU::JR_C_n(){
	BYTE n; int8_t m = 0;
	mmu_->readByte(PC_++, n);
	m |= n;

	if (F_ & 0x10) {
		PC_ = PC_ + m;
		cycles_done_ = 12;
	} else {
		cycles_done_ = 8;
	}
}

void CPU::ADD_HL_SP(){
	WORD hl = (H_ << 8) | L_;
	WORD sum = hl + SP_;

	// did we have a carry?
	if (sum < hl && sum < SP_)
		F_ |= 0x10;
	F_ &= !(0x40); // reset N
	// not implementing H flag
	H_ = sum >> 8;
	L_ = sum & 0xFF;

	cycles_done_ = 8;
}

void CPU::LDD_A_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, A_);

	DEC_HL();
	// we don't set cycles_done_ because DEC_HL already
	// sets the correct one.
}

void CPU::DEC_SP(){
	SP_--;

	cycles_done_ = 8;
}

void CPU::INC_A(){
	BYTE temp = A_ + 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ &= !(0x40); // reset N flag
	if ((A_&0x0F + 0x01)&0x10)
		F_ |= 0x20; // half carry flag

	A_ = temp;

	cycles_done_ = 4;
}

void CPU::DEC_A(){
	BYTE temp = A_ - 0x01;

	if (temp == 0x00)
		F_ |= 0x80; // set Zero flag
	F_ |= 0x40; // set N flag
	if ((A_&0x0F - 0x01) >= 0x0F)
		F_ |= 0x20; // half carry flag

	A_ = temp;

	cycles_done_ = 4;
}

void CPU::LD_A_n(){
	mmu_->readByte(PC_++, A_);

	cycles_done_ = 8;
}

void CPU::CCF(){
	A_ ^= 0xFF;
	F_ &= !(0x60); // reset the N and H flags
	F_ ^= 0x10;

	cycles_done_ = 4;
}

// 40
void CPU::LD_B_B(){
	B_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_B_C(){
	B_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_B_D(){
	B_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_B_E(){
	B_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_B_H(){
	B_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_B_L(){
	B_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_B_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, B_);

	cycles_done_ = 8;
}

void CPU::LD_B_A(){
	B_ = A_;

	cycles_done_ = 4;
}

void CPU::LD_C_B(){
	C_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_C_C(){
	C_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_C_D(){
	C_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_C_E(){
	C_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_C_H(){
	C_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_C_L(){
	C_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_C_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, C_);

	cycles_done_ = 8;
}

void CPU::LD_C_A(){
	C_ = A_;

	cycles_done_ = 4;
}

// 50
void CPU::LD_D_B(){
	D_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_D_C(){
	D_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_D_D(){
	D_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_D_E(){
	D_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_D_H(){
	D_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_D_L(){
	D_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_D_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, D_);

	cycles_done_ = 8;
}

void CPU::LD_D_A(){
	D_ = A_;

	cycles_done_ = 4;
}

void CPU::LD_E_B(){
	E_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_E_C(){
	E_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_E_D(){
	E_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_E_E(){
	E_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_E_H(){
	E_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_E_L(){
	E_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_E_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, E_);

	cycles_done_ = 8;
}

void CPU::LD_E_A(){
	E_ = A_;

	cycles_done_ = 4;
}

// 60
void CPU::LD_H_B(){
	H_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_H_C(){
	H_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_H_D(){
	H_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_H_E(){
	H_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_H_H(){
	H_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_H_L(){
	H_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_H_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, H_);

	cycles_done_ = 8;
}

void CPU::LD_H_A(){
	H_ = A_;

	cycles_done_ = 4;
}

void CPU::LD_L_B(){
	L_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_L_C(){
	L_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_L_D(){
	L_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_L_E(){
	L_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_L_H(){
	L_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_L_L(){
	L_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_L_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, L_);

	cycles_done_ = 8;
}

void CPU::LD_L_A(){
	L_ = A_;

	cycles_done_ = 4;
}

// 70
void CPU::LD_pHL_B(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, B_);

	cycles_done_ = 8;
}

void CPU::LD_pHL_C(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, C_);

	cycles_done_ = 8;
}

void CPU::LD_pHL_D(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, D_);

	cycles_done_ = 8;
}

void CPU::LD_pHL_E(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, E_);

	cycles_done_ = 8;
}

void CPU::LD_pHL_H(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, H_);

	cycles_done_ = 8;
}

void CPU::LD_pHL_L(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, L_);

	cycles_done_ = 8;
}

void CPU::HALT(){
	if (mmu_->ime_) {
		halted_ = true;
	} else {
		PC_++;
	}

	cycles_done_ = 4;
}

void CPU::LD_pHL_A(){
	WORD address = (H_ << 8) | L_;
	mmu_->writeByte(address, A_);

	cycles_done_ = 8;
}

void CPU::LD_A_B(){
	A_ = B_;

	cycles_done_ = 4;
}

void CPU::LD_A_C(){
	A_ = C_;

	cycles_done_ = 4;
}

void CPU::LD_A_D(){
	A_ = D_;

	cycles_done_ = 4;
}

void CPU::LD_A_E(){
	A_ = E_;

	cycles_done_ = 4;
}

void CPU::LD_A_H(){
	A_ = H_;

	cycles_done_ = 4;
}

void CPU::LD_A_L(){
	A_ = L_;

	cycles_done_ = 4;
}

void CPU::LD_A_pHL(){
	WORD address = (H_ << 8) | L_;
	mmu_->readByte(address, A_);

	cycles_done_ = 8;
}

void CPU::LD_A_A(){
	A_ = A_;

	cycles_done_ = 4;
}

// 80
void CPU::ADD_A_B(){
	BYTE sum = A_ + B_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < B_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_C(){
	BYTE sum = A_ + C_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < C_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_D(){
	BYTE sum = A_ + D_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < D_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_E(){
	BYTE sum = A_ + E_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < E_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_H(){
	BYTE sum = A_ + H_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < H_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_L(){
	BYTE sum = A_ + L_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < L_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADD_A_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	BYTE sum = A_ + n;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_ && sum < n)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 8;
}

void CPU::ADD_A_A(){
	BYTE sum = A_ + A_;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum < A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_B(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + B_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= B_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_C(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + C_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= C_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_D(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + D_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= D_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_E(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + E_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= E_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_H(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + H_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= H_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_L(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + L_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= L_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

void CPU::ADC_A_pHL(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);

	BYTE sum = A_ + n + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= n)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 8;
}

void CPU::ADC_A_A(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE sum = A_ + A_ + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 4;
}

// 90
void CPU::SUB_B(){
	BYTE ans = A_ - B_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (B_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_C(){
	BYTE ans = A_ - C_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (C_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_D(){
	BYTE ans = A_ - D_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (D_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_E(){
	BYTE ans = A_ - E_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (E_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_H(){
	BYTE ans = A_ - H_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (H_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_L(){
	BYTE ans = A_ - L_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (L_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SUB_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	BYTE ans = A_ - n;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (n > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 8;
}

void CPU::SUB_A(){
	BYTE ans = A_ - A_;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (A_ > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_B(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - B_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((B_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_C(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - C_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((C_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_D(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - D_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((D_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_E(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - E_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((E_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_H(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - H_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((H_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_L(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - L_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((L_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

void CPU::SBC_A_pHL(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	BYTE ans = A_ - n - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((n+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 8;
}

void CPU::SBC_A_A(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE ans = A_ - A_ - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((A_+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 4;
}

// A0
void CPU::AND_B(){
	A_ &= B_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_C(){
	A_ &= C_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_D(){
	A_ &= D_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_E(){
	A_ &= E_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_H(){
	A_ &= H_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_L(){
	A_ &= L_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::AND_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	A_ &= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 8;
}

void CPU::AND_A(){
	A_ &= A_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 4;
}

void CPU::XOR_B(){
	A_ ^= B_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_C(){
	A_ ^= C_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_D(){
	A_ ^= D_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_E(){
	A_ ^= E_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_H(){
	A_ ^= H_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_L(){
	A_ ^= L_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

void CPU::XOR_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	A_ ^= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 8;
}

void CPU::XOR_A(){
	A_ ^= A_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 4;
}

// B0
void CPU::OR_B(){
	A_ |= B_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_C(){
	A_ |= C_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_D(){
	A_ |= D_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_E(){
	A_ |= E_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_H(){
	A_ |= H_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_L(){
	A_ |= L_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::OR_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	A_ |= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 8;
}

void CPU::OR_A(){
	A_ |= A_;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 4;
}

void CPU::CP_B(){
	if (A_ == B_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < B_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CP_C(){
	if (A_ == C_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < C_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CP_D(){
	if (A_ == D_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < D_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CP_E(){
	if (A_ == E_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < E_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CP_H(){
	if (A_ == H_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < H_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CP_L(){
	if (A_ == L_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < L_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

void CPU::CL_pHL(){
	WORD address = (H_ << 8) | L_;
	BYTE n; mmu_->readByte(address, n);
	if (A_ == n) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < n) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 8;
}

void CPU::CP_A(){
	if (A_ == A_) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < A_) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 4;
}

// C0
void CPU::RET_NZ(){
	if (F_ & 0x80) {
		cycles_done_ = 8;
	} else {
		RET();
		cycles_done_ = 20;
	}
}

void CPU::POP_BC(){
	WORD w;
	mmu_->readWord(SP_, w);
	SP_ += 2;

	B_ = (w >> 8) & 0xFF;
	C_ = w & 0xFF;

	cycles_done_ = 12;
}

void CPU::JP_NZ_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x80) {
		cycles_done_ = 12;
	} else {
		PC_ = address;
		cycles_done_ = 16;
	}
}

void CPU::JP_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;
	PC_ = address;

	cycles_done_ = 16;
}

void CPU::CALL_NZ_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x80) {
		cycles_done_ = 12;
	} else {
		SP_ -= 2;
		mmu_->writeWord(SP_, PC_);
		PC_ = address;

		cycles_done_ = 24;
	}
}

void CPU::PUSH_BC(){
	WORD val = (B_ << 8) | C_;
	SP_ -= 2;
	mmu_->writeWord(SP_, val);

	cycles_done_ = 16;
}

void CPU::ADD_A_n(){
	BYTE n; mmu_->readByte(PC_++, n);

	BYTE sum = A_ + n;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= n)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 8;
}

void CPU::RST_00H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0000;

	cycles_done_ = 16;
}

void CPU::RET_Z(){
	if (F_ & 0x80) {
		RET();
		cycles_done_ = 20;
	} else {
		cycles_done_ = 8;
	}
}

void CPU::RET(){
	mmu_->readWord(SP_, PC_);
	SP_ += 2;

	cycles_done_ = 16;
}

void CPU::JP_Z_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x80) {
		PC_ = address;
		cycles_done_ = 16;
	} else {
		cycles_done_ = 12;
	}
}

void CPU::PREFIX_CB(){ }
void CPU::CALL_Z_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x80) {
		SP_ -= 2;
		mmu_->writeWord(SP_, PC_);
		PC_ = address;

		cycles_done_ = 24;
	} else {
		cycles_done_ = 12;
	}
}

void CPU::CALL_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);
	PC_ = address;

	cycles_done_ = 24;
}

void CPU::ADC_A_n(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE n; mmu_->readByte(PC_++, n);

	BYTE sum = A_ + n + carry;

	F_ &= !(0x40); // reset N flag
	
	// set Z flag
	if (sum == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (sum <= A_ && sum <= n)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = sum;

	cycles_done_ = 8;
}

void CPU::RST_08H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0008;

	cycles_done_ = 16;
}

// D0
void CPU::RET_NC(){
	if (F_ & 0x10) {
		cycles_done_ = 8;
	} else {
		RET();
		cycles_done_ = 20;
	}
}

void CPU::POP_DE(){
	WORD w;
	mmu_->readWord(SP_, w);
	SP_ += 2;

	D_ = (w >> 8) & 0xFF;
	E_ = w & 0xFF;

	cycles_done_ = 12;
}

void CPU::JP_NC_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x10) {
		cycles_done_ = 12;
	} else {
		PC_ = address;
		cycles_done_ = 16;
	}
}

// XX
void CPU::CALL_NC_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x10) {
		cycles_done_ = 12;
	} else {
		SP_ -= 2;
		mmu_->writeWord(SP_, PC_);
		PC_ = address;

		cycles_done_ = 24;
	}
}

void CPU::PUSH_DE(){
	WORD val = (D_ << 8) | E_;
	SP_ -= 2;
	mmu_->writeWord(SP_, val);

	cycles_done_ = 16;
}

void CPU::SUB_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	BYTE ans = A_ - n;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if (n > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 8;
}

void CPU::RST_10H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0010;

	cycles_done_ = 16;
}

void CPU::RET_C(){
	if (F_ & 0x10) {
		RET();
		cycles_done_ = 20;
	} else {
		cycles_done_ = 8;
	}
}

void CPU::RETI(){
	RET();
	mmu_->ime_ = true;

	// no need to set cycles_done_, this is done in RET
}

void CPU::JP_C_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x10) {
		PC_ = address;
		cycles_done_ = 16;
	} else {
		cycles_done_ = 12;
	}
}

// XX
void CPU::CALL_C_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	if (F_ & 0x10) {
		SP_ -= 2;
		mmu_->writeWord(SP_, PC_);
		PC_ = address;

		cycles_done_ = 24;
	} else {
		cycles_done_ = 12;
	}
}

// XX
void CPU::SBC_A_n(){
	BYTE carry;
	if (F_ & 0x10)
		carry = 0x01;
	else
		carry = 0x00;

	BYTE n; mmu_->readByte(PC_++, n);
	BYTE ans = A_ - n - carry;

	F_ |= 0x40; // set N flag

	// set Z flag
	if (ans == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	// set C flag
	if ((n+carry) > A_)
		F_ |= 0x10;
	else
		F_ &= !(0x10);

	A_ = ans;

	cycles_done_ = 8;
}

void CPU::RST_18H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0018;

	cycles_done_ = 16;
}

// E0
void CPU::LDH_pnn_A(){
	BYTE n; mmu_->readByte(PC_++, n);
	WORD address = 0xFF00 | n;

	mmu_->writeByte(address, A_);

	cycles_done_ = 12;
}

void CPU::POP_HL(){
	WORD w;
	mmu_->readWord(SP_, w);
	SP_ += 2;

	H_ = (w >> 8) & 0xFF;
	L_ = w & 0xFF;

	cycles_done_ = 12;
}

void CPU::LD_pC_A(){
	WORD address = 0xFF00 + C_;
	mmu_->writeByte(address, A_);

	cycles_done_ = 8;
}

// XX
// XX
void CPU::PUSH_HL(){
	WORD val = (H_ << 8) | L_;
	SP_ -= 2;
	mmu_->writeWord(SP_, val);

	cycles_done_ = 16;
}

void CPU::AND_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	A_ &= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x50);

	cycles_done_ = 8;
}

void CPU::RST_20H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0020;

	cycles_done_ = 16;
}

void CPU::ADD_SP_n(){
	BYTE n; int8_t m = 0;
	mmu_->readByte(PC_++, n);
	m |= n;

	SP_ += m;

	F_ &= !(0xC0); // reset Z and N flags

	// C flag
	// should be a check here, but come on, its the stack pointer its not going
	// to carry.
	F_ &= !(0x10);
}

void CPU::JP_pHL(){
	WORD address = (H_ << 8) | L_;
	PC_ = address;

	cycles_done_ = 4;
}

void CPU::LD_pnn_A(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	mmu_->writeByte(address, A_);

	cycles_done_ = 16;
}

// XX
// XX
// XX
void CPU::XOR_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	A_ ^= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset all flags

	cycles_done_ = 8;
}

void CPU::RST_28H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0028;

	cycles_done_ = 16;
}

// F0
void CPU::LDH_A_pnn(){
	BYTE n; mmu_->readByte(PC_++, n);
	WORD address = 0xFF00 | n;

	mmu_->readByte(address, A_);

	cycles_done_ = 12;
}

void CPU::POP_AF(){
	WORD w;
	mmu_->readWord(SP_, w);
	SP_ += 2;

	A_ = (w >> 8) & 0xFF;
	F_ = w & 0xFF;

	cycles_done_ = 12;
}

void CPU::LD_A_pC(){
	WORD address = 0xFF00 + C_;
	mmu_->readByte(address, A_);

	cycles_done_ = 8;
}

void CPU::DI(){
	mmu_->ime_ = false;
	cycles_done_ = 4;
}

// XX
void CPU::PUSH_AF(){
	WORD val = (A_ << 8) | F_;
	SP_ -= 2;
	mmu_->writeWord(SP_, val);

	cycles_done_ = 16;
}

void CPU::OR_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	A_ |= n;

	if (A_ == 0x00)
		F_ |= 0x80;
	else
		F_ &= !(0x80);

	F_ &= !(0x70); // reset flags

	cycles_done_ = 8;
}

void CPU::RST_30H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0030;

	cycles_done_ = 16;
}

void CPU::LDHL_SP_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	WORD w = SP_ + n;

	H_ = (w >> 8) & 0xFF;
	L_ = w & 0xFF;

	F_ &= !(0xC0);

	cycles_done_ = 12;
}

void CPU::LD_SP_HL(){
	WORD w = (H_ << 8) | L_;
	SP_ = w;

	cycles_done_ = 8;
}

void CPU::LD_A_pnn(){
	WORD address;
	mmu_->readWord(PC_, address);
	PC_ += 2;

	mmu_->readByte(address, A_);

	cycles_done_ = 16;
}

void CPU::EI(){
	mmu_->ime_ = true;
	cycles_done_ = 4;
}

// XX
// XX
void CPU::CP_n(){
	BYTE n; mmu_->readByte(PC_++, n);
	if (A_ == n) {
		F_ |= 0x80;
		F_ |= 0x40;
		F_ &= !(0x30);
	} else if (A_ < n) {
		F_ |= 0x10;
		F_ &= !(0xE0);
	} else {
		F_ &= !(0xF0);
	}

	cycles_done_ = 8;
}

void CPU::RST_38H(){
	SP_ -= 2;
	mmu_->writeWord(SP_, PC_);

	PC_ = 0x0038;

	cycles_done_ = 16;
}

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