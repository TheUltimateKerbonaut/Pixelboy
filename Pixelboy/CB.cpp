#include "CB.h"

#include <iostream>
#include <sstream>
#include <string>

void CPU::CB_N(CPU* cpu, uint8_t value)
{
	lastValue = value;
	auto hexToString = [](int hex) { return (static_cast<std::stringstream const&>(std::stringstream() << "0x" << std::hex << hex)).str(); };
	
	//std::cout << "Executing CB instruction with value " << hexToString(value) << " (" << extendedInstructions[value].sMnemonic << ")" << std::endl;

	((void(*)(CPU*))extendedInstructions[value].function)(cpu);
	cpu->ticks += extendedInstructionTicks[value];
}

void Undefined(CPU* cpu)
{
	auto hexToString = [](int hex) { return (static_cast<std::stringstream const&>(std::stringstream() << "0x" << std::hex << hex)).str(); };
	std::cout << "Undefined CB instruction " << hexToString(lastValue) << "!" << std::endl;
	cpu->m_bCrashed = true;
}

uint8_t SWAP(CPU* cpu, uint8_t value)
{
	// Swap upper and lower nibbles of value, reset H, N and C, 
	// and set the zero flag if the result is zero
	value = ((value & 0xF) << 4) | ((value & 0xF0) >> 4);

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(HALF_CARRY); cpu->ClearFlag(CARRY); cpu->ClearFlag(SUBTRACT);

	return value;
}

void BIT(CPU* cpu, uint8_t bit, uint8_t value) 
{
	if (value & bit) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);
	cpu->SetFlag(HALF_CARRY);
}

uint8_t RL(CPU* cpu, uint8_t value)
{
	uint8_t carry = cpu->GetFlag(CARRY) ? 1 : 0;

	bool will_carry = value & 0b10000000;
	if (will_carry) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	uint8_t result = (uint8_t)(value << 1);
	result |= carry;

	if(result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);
	
	return result;

	/*
	int carry = (value & 0x80) >> 7;

	if (value & 0x80) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	value <<= 1;
	value += carry;

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);

	return value;
	*/
}

uint8_t SLA(CPU* cpu, uint8_t value) 
{
	if (value & 0x80) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	value <<= 1;

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);

	return value;
}

uint8_t SRL(CPU* cpu, uint8_t value)
{
	if (value & 0x01) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	value >>= 1;

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);

	return value;
}

uint8_t RR(CPU* cpu, uint8_t value)
{
	uint8_t carry = cpu->GetFlag(CARRY) ? 1 : 0;

	bool willCarry = value & 0b1;
	if (willCarry) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	uint8_t result = (uint8_t)(value >> 1);
	result |= (carry << 7);

	if (result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);

	return result;

	/*
	value >>= 1;
	if (cpu->GetFlag(CARRY)) value |= 0x80;

	if (value & 0x01) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);

	return value;
	*/
}

uint8_t SET(CPU* cpu, uint8_t bit ,uint8_t value)
{
	value |= bit;
	return value;
}

uint8_t RLC(CPU* cpu, uint8_t value)
{
	int carry = (value & 0x80) >> 7;

	if (value & 0x80) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	value <<= 1;
	value += carry;

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);

	return value;
}

uint8_t RRC(CPU* cpu, uint8_t value)
{
	int carry = value & 0x01;

	value >>= 1;

	if (carry) 
	{
		cpu->SetFlag(CARRY);
		value |= 0x80;
	}
	else cpu->ClearFlag(CARRY);

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);

	return value;
}

uint8_t SRA(CPU* cpu, uint8_t value)
{
	uint8_t carry_bit = value & 0b1;
	uint8_t top_bit = value & 0b10000000;

	uint8_t result = (uint8_t)(value >> 1);
	if (top_bit) result |= 1 << 7;
	else result &= ~(0 << 7);

	if (result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	if (carry_bit) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->ClearFlag(HALF_CARRY); cpu->ClearFlag(SUBTRACT);

	return result;
}

void SWAP_A(CPU* cpu) { cpu->m_RegisterAF.high = SWAP(cpu, cpu->m_RegisterAF.high); }
void SWAP_B(CPU* cpu) { cpu->m_RegisterBC.high = SWAP(cpu, cpu->m_RegisterBC.high); }
void SWAP_C(CPU* cpu) { cpu->m_RegisterBC.low = SWAP(cpu, cpu->m_RegisterBC.low); }
void SWAP_D(CPU* cpu) { cpu->m_RegisterDE.high = SWAP(cpu, cpu->m_RegisterDE.high); }
void SWAP_E(CPU* cpu) { cpu->m_RegisterDE.low = SWAP(cpu, cpu->m_RegisterDE.low); }
void SWAP_H(CPU* cpu) { cpu->m_RegisterHL.high = SWAP(cpu, cpu->m_RegisterHL.high); }
void SWAP_L(CPU* cpu) { cpu->m_RegisterHL.low = SWAP(cpu, cpu->m_RegisterHL.low); }
void SWAP_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SWAP(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void RES_0_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 0); }
void RES_0_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 0); }
void RES_0_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 0); }
void RES_0_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 0); }
void RES_0_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 0); }
void RES_0_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 0); }
void RES_0_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 0); }
void RES_0_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 0)); }

void RES_1_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 1); }
void RES_1_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 1); }
void RES_1_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 1); }
void RES_1_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 1); }
void RES_1_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 1); }
void RES_1_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 1); }
void RES_1_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 1); }
void RES_1_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 1)); }

void RES_2_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 2); }
void RES_2_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 2); }
void RES_2_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 2); }
void RES_2_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 2); }
void RES_2_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 2); }
void RES_2_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 2); }
void RES_2_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 2); }
void RES_2_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 2)); }

void RES_3_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 3); }
void RES_3_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 3); }
void RES_3_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 3); }
void RES_3_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 3); }
void RES_3_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 3); }
void RES_3_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 3); }
void RES_3_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 3); }
void RES_3_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 3)); }

void RES_4_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 4); }
void RES_4_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 4); }
void RES_4_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 4); }
void RES_4_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 4); }
void RES_4_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 4); }
void RES_4_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 4); }
void RES_4_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 4); }
void RES_4_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 4)); }

void RES_5_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 5); }
void RES_5_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 5); }
void RES_5_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 5); }
void RES_5_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 5); }
void RES_5_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 5); }
void RES_5_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 5); }
void RES_5_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 5); }
void RES_5_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 5)); }

void RES_6_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 6); }
void RES_6_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 6); }
void RES_6_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 6); }
void RES_6_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 6); }
void RES_6_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 6); }
void RES_6_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 6); }
void RES_6_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 6); }
void RES_6_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 6)); }

void RES_7_A(CPU* cpu) { cpu->m_RegisterAF.high &= ~(1 << 7); }
void RES_7_B(CPU* cpu) { cpu->m_RegisterBC.high &= ~(1 << 7); }
void RES_7_C(CPU* cpu) { cpu->m_RegisterBC.low &= ~(1 << 7); }
void RES_7_D(CPU* cpu) { cpu->m_RegisterDE.high &= ~(1 << 7); }
void RES_7_E(CPU* cpu) { cpu->m_RegisterDE.low &= ~(1 << 7); }
void RES_7_H(CPU* cpu) { cpu->m_RegisterHL.high &= ~(1 << 7); }
void RES_7_L(CPU* cpu) { cpu->m_RegisterHL.low &= ~(1 << 7); }
void RES_7_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg) & ~(1 << 7)); }

void BIT_0_A(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterAF.high); }
void BIT_0_B(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterBC.high); }
void BIT_0_C(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterBC.low); }
void BIT_0_D(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterDE.high); }
void BIT_0_E(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterDE.low); }
void BIT_0_H(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterHL.high); }
void BIT_0_L(CPU* cpu)  { BIT(cpu, 1 << 0, cpu->m_RegisterHL.low); }
void BIT_0_HL(CPU* cpu) { BIT(cpu, 1 << 0, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_1_A(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterAF.high); }
void BIT_1_B(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterBC.high); }
void BIT_1_C(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterBC.low); }
void BIT_1_D(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterDE.high); }
void BIT_1_E(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterDE.low); }
void BIT_1_H(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterHL.high); }
void BIT_1_L(CPU* cpu)  { BIT(cpu, 1 << 1, cpu->m_RegisterHL.low); }
void BIT_1_HL(CPU* cpu) { BIT(cpu, 1 << 1, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_2_A(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterAF.high); }
void BIT_2_B(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterBC.high); }
void BIT_2_C(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterBC.low); }
void BIT_2_D(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterDE.high); }
void BIT_2_E(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterDE.low); }
void BIT_2_H(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterHL.high); }
void BIT_2_L(CPU* cpu)  { BIT(cpu, 1 << 2, cpu->m_RegisterHL.low); }
void BIT_2_HL(CPU* cpu) { BIT(cpu, 1 << 2, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_3_A(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterAF.high); }
void BIT_3_B(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterBC.high); }
void BIT_3_C(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterBC.low); }
void BIT_3_D(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterDE.high); }
void BIT_3_E(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterDE.low); }
void BIT_3_H(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterHL.high); }
void BIT_3_L(CPU* cpu)  { BIT(cpu, 1 << 3, cpu->m_RegisterHL.low); }
void BIT_3_HL(CPU* cpu) { BIT(cpu, 1 << 3, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_4_A(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterAF.high); }
void BIT_4_B(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterBC.high); }
void BIT_4_C(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterBC.low); }
void BIT_4_D(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterDE.high); }
void BIT_4_E(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterDE.low); }
void BIT_4_H(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterHL.high); }
void BIT_4_L(CPU* cpu)  { BIT(cpu, 1 << 4, cpu->m_RegisterHL.low); }
void BIT_4_HL(CPU* cpu) { BIT(cpu, 1 << 4, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_5_A(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterAF.high); }
void BIT_5_B(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterBC.high); }
void BIT_5_C(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterBC.low); }
void BIT_5_D(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterDE.high); }
void BIT_5_E(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterDE.low); }
void BIT_5_H(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterHL.high); }
void BIT_5_L(CPU* cpu)  { BIT(cpu, 1 << 5, cpu->m_RegisterHL.low); }
void BIT_5_HL(CPU* cpu) { BIT(cpu, 1 << 5, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_6_A(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterAF.high); }
void BIT_6_B(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterBC.high); }
void BIT_6_C(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterBC.low); }
void BIT_6_D(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterDE.high); }
void BIT_6_E(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterDE.low); }
void BIT_6_H(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterHL.high); }
void BIT_6_L(CPU* cpu)  { BIT(cpu, 1 << 6, cpu->m_RegisterHL.low); }
void BIT_6_HL(CPU* cpu) { BIT(cpu, 1 << 6, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void BIT_7_A(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterAF.high); }
void BIT_7_B(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterBC.high); }
void BIT_7_C(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterBC.low); }
void BIT_7_D(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterDE.high); }
void BIT_7_E(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterDE.low); }
void BIT_7_H(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterHL.high); }
void BIT_7_L(CPU* cpu)  { BIT(cpu, 1 << 7, cpu->m_RegisterHL.low); }
void BIT_7_HL(CPU* cpu) { BIT(cpu, 1 << 7, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }

void RL_B(CPU* cpu) { cpu->m_RegisterBC.high = RL(cpu, cpu->m_RegisterBC.high); }
void RL_C(CPU* cpu) { cpu->m_RegisterBC.low = RL(cpu, cpu->m_RegisterBC.low); }
void RL_D(CPU* cpu) { cpu->m_RegisterDE.high = RL(cpu, cpu->m_RegisterDE.high); }
void RL_E(CPU* cpu) { cpu->m_RegisterDE.low = RL(cpu, cpu->m_RegisterDE.low); }
void RL_H(CPU* cpu) { cpu->m_RegisterHL.high = RL(cpu, cpu->m_RegisterHL.high); }
void RL_L(CPU* cpu) { cpu->m_RegisterHL.low = RL(cpu, cpu->m_RegisterHL.low); }
void RL_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, RL(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }
void RL_A(CPU* cpu) { cpu->m_RegisterAF.high = RL(cpu, cpu->m_RegisterAF.high); }

void SLA_A(CPU* cpu) { cpu->m_RegisterAF.high = SLA(cpu, cpu->m_RegisterAF.high); }
void SLA_B(CPU* cpu) { cpu->m_RegisterBC.high = SLA(cpu, cpu->m_RegisterBC.high); }
void SLA_C(CPU* cpu) { cpu->m_RegisterBC.low = SLA(cpu, cpu->m_RegisterBC.low); }
void SLA_D(CPU* cpu) { cpu->m_RegisterDE.high = SLA(cpu, cpu->m_RegisterDE.high); }
void SLA_E(CPU* cpu) { cpu->m_RegisterDE.low = SLA(cpu, cpu->m_RegisterDE.low); }
void SLA_H(CPU* cpu) { cpu->m_RegisterHL.high = SLA(cpu, cpu->m_RegisterHL.high); }
void SLA_L(CPU* cpu) { cpu->m_RegisterHL.low = SLA(cpu, cpu->m_RegisterHL.low); }
void SLA_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SLA(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SRL_A(CPU* cpu) { cpu->m_RegisterAF.high = SRL(cpu, cpu->m_RegisterAF.high); }
void SRL_B(CPU* cpu) { cpu->m_RegisterBC.high = SRL(cpu, cpu->m_RegisterBC.high); }
void SRL_C(CPU* cpu) { cpu->m_RegisterBC.low = SRL(cpu, cpu->m_RegisterBC.low); }
void SRL_D(CPU* cpu) { cpu->m_RegisterDE.high = SRL(cpu, cpu->m_RegisterDE.high); }
void SRL_E(CPU* cpu) { cpu->m_RegisterDE.low = SRL(cpu, cpu->m_RegisterDE.low); }
void SRL_H(CPU* cpu) { cpu->m_RegisterHL.high = SRL(cpu, cpu->m_RegisterHL.high); }
void SRL_L(CPU* cpu) { cpu->m_RegisterHL.low = SRL(cpu, cpu->m_RegisterHL.low); }
void SRL_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SRL(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void RR_A(CPU* cpu) { cpu->m_RegisterAF.high = RR(cpu, cpu->m_RegisterAF.high); }
void RR_B(CPU* cpu) { cpu->m_RegisterBC.high = RR(cpu, cpu->m_RegisterBC.high); }
void RR_C(CPU* cpu) { cpu->m_RegisterBC.low = RR(cpu, cpu->m_RegisterBC.low); }
void RR_D(CPU* cpu) { cpu->m_RegisterDE.high = RR(cpu, cpu->m_RegisterDE.high); }
void RR_E(CPU* cpu) { cpu->m_RegisterDE.low = RR(cpu, cpu->m_RegisterDE.low); }
void RR_H(CPU* cpu) { cpu->m_RegisterHL.high = RR(cpu, cpu->m_RegisterHL.high); }
void RR_L(CPU* cpu) { cpu->m_RegisterHL.low = RR(cpu, cpu->m_RegisterHL.low); }
void RR_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, RR(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_0_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 0, cpu->m_RegisterAF.high); }
void SET_0_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 0, cpu->m_RegisterBC.high); }
void SET_0_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 0, cpu->m_RegisterBC.low); }
void SET_0_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 0, cpu->m_RegisterDE.high); }
void SET_0_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 0, cpu->m_RegisterDE.low); }
void SET_0_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 0, cpu->m_RegisterHL.high); }
void SET_0_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 0, cpu->m_RegisterHL.low); }
void SET_0_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 0, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_1_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 1, cpu->m_RegisterAF.high); }
void SET_1_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 1, cpu->m_RegisterBC.high); }
void SET_1_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 1, cpu->m_RegisterBC.low); }
void SET_1_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 1, cpu->m_RegisterDE.high); }
void SET_1_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 1, cpu->m_RegisterDE.low); }
void SET_1_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 1, cpu->m_RegisterHL.high); }
void SET_1_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 1, cpu->m_RegisterHL.low); }
void SET_1_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 1, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_2_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 2, cpu->m_RegisterAF.high); }
void SET_2_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 2, cpu->m_RegisterBC.high); }
void SET_2_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 2, cpu->m_RegisterBC.low); }
void SET_2_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 2, cpu->m_RegisterDE.high); }
void SET_2_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 2, cpu->m_RegisterDE.low); }
void SET_2_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 2, cpu->m_RegisterHL.high); }
void SET_2_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 2, cpu->m_RegisterHL.low); }
void SET_2_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 2, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_3_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 3, cpu->m_RegisterAF.high); }
void SET_3_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 3, cpu->m_RegisterBC.high); }
void SET_3_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 3, cpu->m_RegisterBC.low); }
void SET_3_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 3, cpu->m_RegisterDE.high); }
void SET_3_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 3, cpu->m_RegisterDE.low); }
void SET_3_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 3, cpu->m_RegisterHL.high); }
void SET_3_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 3, cpu->m_RegisterHL.low); }
void SET_3_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 3, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_4_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 4, cpu->m_RegisterAF.high); }
void SET_4_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 4, cpu->m_RegisterBC.high); }
void SET_4_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 4, cpu->m_RegisterBC.low); }
void SET_4_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 4, cpu->m_RegisterDE.high); }
void SET_4_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 4, cpu->m_RegisterDE.low); }
void SET_4_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 4, cpu->m_RegisterHL.high); }
void SET_4_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 4, cpu->m_RegisterHL.low); }
void SET_4_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 4, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_5_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 5, cpu->m_RegisterAF.high); }
void SET_5_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 5, cpu->m_RegisterBC.high); }
void SET_5_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 5, cpu->m_RegisterBC.low); }
void SET_5_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 5, cpu->m_RegisterDE.high); }
void SET_5_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 5, cpu->m_RegisterDE.low); }
void SET_5_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 5, cpu->m_RegisterHL.high); }
void SET_5_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 5, cpu->m_RegisterHL.low); }
void SET_5_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 5, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_6_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 6, cpu->m_RegisterAF.high); }
void SET_6_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 6, cpu->m_RegisterBC.high); }
void SET_6_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 6, cpu->m_RegisterBC.low); }
void SET_6_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 6, cpu->m_RegisterDE.high); }
void SET_6_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 6, cpu->m_RegisterDE.low); }
void SET_6_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 6, cpu->m_RegisterHL.high); }
void SET_6_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 6, cpu->m_RegisterHL.low); }
void SET_6_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 6, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void SET_7_A(CPU* cpu) { cpu->m_RegisterAF.high = SET(cpu, 1 << 7, cpu->m_RegisterAF.high); }
void SET_7_B(CPU* cpu) { cpu->m_RegisterBC.high = SET(cpu, 1 << 7, cpu->m_RegisterBC.high); }
void SET_7_C(CPU* cpu) { cpu->m_RegisterBC.low = SET(cpu, 1 << 7, cpu->m_RegisterBC.low); }
void SET_7_D(CPU* cpu) { cpu->m_RegisterDE.high = SET(cpu, 1 << 7, cpu->m_RegisterDE.high); }
void SET_7_E(CPU* cpu) { cpu->m_RegisterDE.low = SET(cpu, 1 << 7, cpu->m_RegisterDE.low); }
void SET_7_H(CPU* cpu) { cpu->m_RegisterHL.high = SET(cpu, 1 << 7, cpu->m_RegisterHL.high); }
void SET_7_L(CPU* cpu) { cpu->m_RegisterHL.low = SET(cpu, 1 << 7, cpu->m_RegisterHL.low); }
void SET_7_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SET(cpu, 1 << 7, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void RLC_B(CPU* cpu) { cpu->m_RegisterBC.high = RLC(cpu, cpu->m_RegisterBC.high);  }
void RLC_C(CPU* cpu) { cpu->m_RegisterBC.low = RLC(cpu, cpu->m_RegisterBC.low); }
void RLC_D(CPU* cpu) { cpu->m_RegisterDE.high = RLC(cpu, cpu->m_RegisterDE.high); }
void RLC_E(CPU* cpu) { cpu->m_RegisterDE.low = RLC(cpu, cpu->m_RegisterDE.low); }
void RLC_H(CPU* cpu) { cpu->m_RegisterHL.high = RLC(cpu, cpu->m_RegisterHL.high); }
void RLC_L(CPU* cpu) { cpu->m_RegisterHL.low = RLC(cpu, cpu->m_RegisterHL.low); }
void RLC_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, RLC(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }
void RLC_A(CPU* cpu) { cpu->m_RegisterAF.high = RLC(cpu, cpu->m_RegisterAF.high); }

void RRC_B(CPU* cpu) { cpu->m_RegisterBC.high = RRC(cpu, cpu->m_RegisterBC.high); }
void RRC_C(CPU* cpu) { cpu->m_RegisterBC.low = RRC(cpu, cpu->m_RegisterBC.low); }
void RRC_D(CPU* cpu) { cpu->m_RegisterDE.high = RRC(cpu, cpu->m_RegisterDE.high); }
void RRC_E(CPU* cpu) { cpu->m_RegisterDE.low = RRC(cpu, cpu->m_RegisterDE.low); }
void RRC_H(CPU* cpu) { cpu->m_RegisterHL.high = RRC(cpu, cpu->m_RegisterHL.high); }
void RRC_L(CPU* cpu) { cpu->m_RegisterHL.low = RRC(cpu, cpu->m_RegisterHL.low); }
void RRC_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, RRC(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }
void RRC_A(CPU* cpu) { cpu->m_RegisterAF.high = RRC(cpu, cpu->m_RegisterAF.high); }

void SRA_A(CPU* cpu) { cpu->m_RegisterAF.high = SRA(cpu, cpu->m_RegisterAF.high); }
void SRA_B(CPU* cpu) { cpu->m_RegisterBC.high = SRA(cpu, cpu->m_RegisterBC.high); }
void SRA_C(CPU* cpu) { cpu->m_RegisterBC.low = SRA(cpu, cpu->m_RegisterBC.low); }
void SRA_D(CPU* cpu) { cpu->m_RegisterDE.high = SRA(cpu, cpu->m_RegisterDE.high); }
void SRA_E(CPU* cpu) { cpu->m_RegisterDE.low = SRA(cpu, cpu->m_RegisterDE.low); }
void SRA_H(CPU* cpu) { cpu->m_RegisterHL.high = SRA(cpu, cpu->m_RegisterHL.high); }
void SRA_L(CPU* cpu) { cpu->m_RegisterHL.low = SRA(cpu, cpu->m_RegisterHL.low); }
void SRA_HL(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, SRA(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }