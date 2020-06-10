#include "CPU.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <bitset>

CPU::CPU(const std::string& sBootRom, const std::string& sFileName): m_Memory(sBootRom, sFileName)
{
	Reset();
}

CPU::CPU(const std::string& sFileName) : m_Memory(sFileName)
{
	Reset();
}

void CPU::Reset()
{
	// Set default state of the registers (memory handled seperately)
	if (m_Memory.m_bBootRom) m_ProgramCounter = 0x0;
	else m_ProgramCounter = 0x100;
	m_RegisterAF.reg = 0x01B0;
	m_RegisterBC.reg = 0x0013;
	m_RegisterDE.reg = 0x00D8;
	m_RegisterHL.reg = 0x014D;
	m_StackPointer = 0xFFFE;

	// Banking
	if (m_Memory.ReadByte(0x147) == 0)
		m_Memory.m_bBanking = false;
	else m_Memory.m_bBanking = true;

	m_MasterInterupts = true;
	//ticks = 60;
	ticks = 0;

	m_bHalted = false;
	m_bStopped = false;
}

unsigned int CPU::Update()
{

	if ((m_bHalted && m_MasterInterupts) || m_bStopped) return ticks++;
	if (m_bCrashed) return -1;
#ifdef _DEBUG
	uint16_t lastProgramCounter = m_ProgramCounter;
#endif

	// Fetch current instruction and increment program counter
	uint8_t instruction = m_Memory.ReadByte(m_ProgramCounter++);

	if (m_bHalted) m_ProgramCounter--;

	// Determine if opcode takes paremeters and work them out, then fix the program counter
	uint16_t operand = 0;
	if (instructions[instruction].length == 1) operand = (uint16_t)m_Memory.ReadByte(m_ProgramCounter);
	if (instructions[instruction].length == 2) operand = m_Memory.ReadShort(m_ProgramCounter);
	m_ProgramCounter += instructions[instruction].length;

	switch (instructions[instruction].length)
	{
		case 0: // No arguments
			((void(*)(CPU*))instructions[instruction].function)(this);
			break;

		case 1: // Byte
			((void(*)(CPU*, uint8_t))instructions[instruction].function)(this, (uint8_t)operand);
			break;

		case 2: // Short
			((void(*)(CPU*, uint16_t))instructions[instruction].function)(this, operand);
			break;
	}

	// If crashed, get back to the problematic program counter
#ifdef _DEBUG
	if (m_bCrashed) return lastProgramCounter;
#endif

	ticks += instructionTicks[instruction] * 2;
	return ticks;
}

void CPU::CheckForInterrupts()
{

	// Check if each interrupt is enabled
	uint8_t interruptTable = m_Memory.m_InterruptsEnabled;
	bool bVblankEnabled = interruptTable	& VBLANK_FLAG_BIT;
	bool bLCDEnabled = interruptTable		& LCD_FLAG_BIT;
	bool bTimerEnabled = interruptTable		& TIMER_FLAG_BIT;
	bool bJoypadEnabled = interruptTable	& JOYPAD_FLAG_BIT;

	// Check if each interrupt has been flagged
	uint8_t interruptFlags = m_Memory.m_InterruptFlags;
	bool bVblank =	interruptFlags & VBLANK_FLAG_BIT;
	bool bLCD =		interruptFlags & LCD_FLAG_BIT;
	bool bTimer =	interruptFlags & TIMER_FLAG_BIT;
	bool bJoypad =	interruptFlags & JOYPAD_FLAG_BIT;

	if ((bVblank) || (bLCD) || (bTimer) || (bJoypad)) m_bStopped = false;

	// If needed, execute interrupts in order of priority
	if ((bVblank && bVblankEnabled) || (bLCD && bLCDEnabled) || (bTimer && bTimerEnabled) || (bJoypad && bJoypadEnabled))
	{
		// No more halting or stopping
		m_bHalted = false;
		m_bStopped = false;

		// Check for master interrupt switch
		if (!m_MasterInterupts) return;

		if (bVblank && bVblankEnabled) ServiceInterrupt(0, VBLANK_FLAG_BIT);
		if (bLCD && bLCDEnabled) ServiceInterrupt(1, LCD_FLAG_BIT);
		if (bTimer && bTimerEnabled) ServiceInterrupt(2, TIMER_FLAG_BIT);
		if (bJoypad && bJoypadEnabled) ServiceInterrupt(3, JOYPAD_FLAG_BIT);
	}

	// Save program counter

	// Do stuff

}

void CPU::ServiceInterrupt(uint8_t interrupt, uint8_t bit)
{
	// Disable master interrupts
	m_MasterInterupts = false;

	// Disable the specific interrupt
	uint8_t interruptFlags = m_Memory.m_InterruptFlags;
	interruptFlags &= ~bit;
	m_Memory.m_InterruptFlags = interruptFlags;

	// Save current execution address
	m_Memory.WriteShortToStack(&m_StackPointer, m_ProgramCounter);

	// Jump to the interrupt handler code
	switch (interrupt)
	{
		case 0: m_ProgramCounter = VBLANK; break;
		case 1: m_ProgramCounter = LCD; break;
		case 2: m_ProgramCounter = TIMER; break;
		case 3: { m_ProgramCounter = JOYPAD; m_bStopped = false; break; }
	}
}

void CPU::UpdateTimers(int cycles)
{
	m_Memory.m_DividerRegister += cycles;
	if (m_Memory.m_DividerCounter >= 255)
	{
		m_Memory.m_DividerCounter = 0;
		m_Memory.m_DividerRegister++;
	}

	if (IsClockEnabled())
	{
		m_Memory.m_TimerCounter -= cycles;

		// If we need to update the timer
		if (m_Memory.m_TimerCounter <= 0)
		{
			// Set frequency
			switch (m_Memory.m_TimerFrequency & 0x3)
			{
				case 0: m_Memory.m_TimerCounter = 1024; break; // freq 4096
				case 1: m_Memory.m_TimerCounter = 16; break;// freq 262144
				case 2: m_Memory.m_TimerCounter = 64; break;// freq 65536
				case 3: m_Memory.m_TimerCounter = 256; break;// freq 16382
			}

			// If about to overflow
			if (m_Memory.m_Timer == 255)
			{
				m_Memory.m_Timer = m_Memory.m_TimerResetValue;
				RequestInterrupt(TIMER_FLAG_BIT);
			}
			else m_Memory.m_Timer++;
		}
	}
}

bool CPU::IsClockEnabled()
{
	return (m_Memory.m_TimerFrequency & 0b100) ? true : false;
}

void CPU::RequestInterrupt(uint16_t nInterruptID)
{
	// Set corresponding bit in the interrupt request register (0xFF0F)
	uint8_t currentFlags = m_Memory.m_InterruptFlags;
	currentFlags |= nInterruptID;
	m_Memory.m_InterruptFlags = currentFlags;
}

void CPU::KeyPressed(int key)
{
	m_bStopped = false;

	bool bPreviouslyUnset = false;

	// See if we need to request interrupt
	if ((m_Memory.m_JoypadState & (1 << key)) == false) bPreviouslyUnset = true;

	// Invert bits if needed
	m_Memory.m_JoypadState &= ~(1 << key);

	// Button or directional?
	bool bButton = key > 3;

	bool bRequestInterrupt = false;

	if (bButton && !(m_Memory.m_JoypadReq & 0b100000)) bRequestInterrupt = true;
	else if (!bButton && !(m_Memory.m_JoypadReq & 0b10000)) bRequestInterrupt = true;

	// Request interrupt
	if (bRequestInterrupt && !bPreviouslyUnset) RequestInterrupt(JOYPAD_FLAG_BIT);

}

void CPU::KeyReleased(int key)
{
	m_bStopped = false;
	m_Memory.m_JoypadState |= (1 << key);
}

void CPU::OR(CPU* cpu, uint8_t value)
{
	cpu->m_RegisterAF.high |= value;

	if (cpu->m_RegisterAF.high) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(CARRY); cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);
}

void CPU::AND(CPU* cpu, uint8_t value)
{
	cpu->m_RegisterAF.high &= value;

	if (cpu->m_RegisterAF.high) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->SetFlag(HALF_CARRY);
	cpu->ClearFlag(CARRY); cpu->ClearFlag(SUBTRACT);
}

void CPU::XOR(CPU* cpu, uint8_t value)
{
	cpu->m_RegisterAF.high ^= value;

	if (cpu->m_RegisterAF.high) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(CARRY); cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(HALF_CARRY);
}

uint8_t CPU::Inc(CPU* cpu, uint8_t value) // Correct
{
	value++;

	if ((value & 0x0f) == 0x00) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);

	return value;
}

uint8_t CPU::Dec(CPU* cpu, uint8_t value)
{
	value--;

	if ((value & 0x0f) == 0x0F) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if (value) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);

	cpu->SetFlag(SUBTRACT);

	return value;
}

void CPU::Add(CPU* cpu, uint8_t* destination, uint8_t value)
{
	/*
	unsigned int result = *destination + value;
	*destination = result;

	if (result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);

	if ((*destination & 0xf) + (value & 0xf) > 0xf) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if ((result & 0x100) != 0) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);
	*/

	if ((*destination & 0xf) + (value & 0xf) > 0xf) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	unsigned int result = *destination + value;
	*destination = (uint8_t)result;

	if (*destination == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);

	if ((result & 0x100) != 0) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

}

void CPU::Add2(CPU* cpu, uint16_t* destination, uint16_t value) 
{
	unsigned int result = *destination + value;

	cpu->ClearFlag(SUBTRACT);

	if ((*destination & 0xfff) + (value & 0xfff) > 0xfff) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if ((result & 0x10000) != 0) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	*destination = (uint16_t)result;

	/*
	unsigned long result = *destination + value;

	if (result & 0xffff0000) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	*destination = (unsigned short)(result & 0xffff);

	if (((*destination & 0x0f) + (value & 0x0f)) > 0x0f) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	// zero flag left alone

	cpu->ClearFlag(SUBTRACT);
	*/
}

void CPU::Sub(CPU* cpu, uint8_t value) 
{
	cpu->SetFlag(SUBTRACT);

	if (value > cpu->m_RegisterAF.high) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if ((value & 0x0f) > (cpu->m_RegisterAF.high & 0x0f)) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->m_RegisterAF.high -= value;

	if (cpu->m_RegisterAF.high) cpu->ClearFlag(ZERO);
	else cpu->SetFlag(ZERO);
}

void CPU::RST(CPU* cpu, uint8_t value)
{
	cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_ProgramCounter);
	cpu->m_ProgramCounter = value;
}

void CPU::Undefined(CPU* cpu)
{
	cpu->m_bCrashed = true;
}

void CPU::ADC(CPU* cpu, uint8_t value)
{
	uint8_t reg = cpu->m_RegisterAF.high;
	uint8_t carry = cpu->GetFlag(CARRY) ? 1 : 0;

	unsigned int result_full = reg + value + carry;
	uint8_t result = (uint8_t)(result_full);

	if (result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(SUBTRACT);

	if (((reg & 0xf) + (value & 0xf) + carry) > 0xf) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if (result_full > 0xff) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_RegisterAF.high = result;

	/*
	value += cpu->GetFlag(CARRY) ? 1 : 0;
	int result = cpu->m_RegisterAF.high + value;

	if (result & 0xff00) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if (value == cpu->m_RegisterAF.high) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	if (((value & 0x0f) + (cpu->m_RegisterAF.high & 0x0f)) > 0x0f) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->SetFlag(SUBTRACT);

	cpu->m_RegisterAF.high = (uint8_t)(result & 0xff);
	*/
}

void CPU::CP(CPU* cpu, uint8_t value)
{
	if (cpu->m_RegisterAF.high == value) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	if (value > cpu->m_RegisterAF.high) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if ((value & 0x0f) > (cpu->m_RegisterAF.high & 0x0f)) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->SetFlag(SUBTRACT);
}

void CPU::SBC(CPU* cpu, uint8_t value)
{
	uint8_t carry = cpu->GetFlag(CARRY) ? 1 : 0;
	uint8_t reg = cpu->m_RegisterAF.high;

	int result_full = reg - value - carry;
	uint8_t result = (uint8_t)(result_full);

	if (result == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->SetFlag(SUBTRACT);

	if (result_full < 0) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if (((reg & 0xf) - (value & 0xf) - carry) < 0) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->m_RegisterAF.high = result;

	/*
	value += cpu->GetFlag(CARRY) ? 1 : 0;

	cpu->SetFlag(SUBTRACT);

	if (value > cpu->m_RegisterAF.high) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if (value == cpu->m_RegisterAF.high) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	if ((value & 0x0f) > (cpu->m_RegisterAF.high & 0x0f)) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->m_RegisterAF.high -= value;
	*/
}

/* -------------------- Opcodes -------------------- */

void CPU::NOP(CPU* cpu) { }

void CPU::HALT(CPU* cpu)
{
	if (cpu->m_MasterInterupts) cpu->m_bHalted = true;
	else
	{
		// Halt bug
		if (cpu->m_Memory.m_InterruptsEnabled != 0 && cpu->m_Memory.m_InterruptFlags != 0)
		{
			std::cerr << "HALT BUG" << std::endl;
		}

		// Otherwise stop counting for one instruction
		cpu->m_bHalted = true;
		//cpu->m_ProgramCounter++;
	}
}

void CPU::STOP(CPU* cpu) { } //{ cpu->m_bStopped = true; }

void CPU::LD_NN_A(CPU* cpu, uint16_t value)
{
	cpu->m_Memory.WriteByte(value, cpu->m_RegisterAF.high);
}

void CPU::LD_NN_SP(CPU* cpu, uint16_t value)
{
	cpu->m_Memory.WriteShort(value, cpu->m_StackPointer);
}

void CPU::LDI_A_HL(CPU* cpu) // Correct
{
	// Put value at address HL into A. Increment HL.
	cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg);
	cpu->m_RegisterHL.reg++;
}

void CPU::LDI_HL_A(CPU* cpu)
{
	cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg++, cpu->m_RegisterAF.high);
}

void CPU::CALL_NN(CPU* cpu, uint16_t value)
{
	cpu->m_Memory.WriteShortToStack(&(cpu->m_StackPointer), cpu->m_ProgramCounter);
	cpu->m_ProgramCounter = value;
}

void CPU::CALL_NZ_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(ZERO)) cpu->ticks += 12;
	else 
	{
		cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_ProgramCounter);
		cpu->m_ProgramCounter = value;
		cpu->ticks += 24;
	}
}
void CPU::CALL_C_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(CARRY))
	{
		cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_ProgramCounter);
		cpu->m_ProgramCounter = value;
		cpu->ticks += 24;
	}
	else cpu->ticks += 12;
}
void CPU::CALL_NC_NN(CPU* cpu, uint16_t value)
{
	if (!cpu->GetFlag(CARRY))
	{
		cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_ProgramCounter);
		cpu->m_ProgramCounter = value;
		cpu->ticks += 24;
	}
	else cpu->ticks += 12;
}
void CPU::CALL_Z_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(ZERO)) 
	{
		cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_ProgramCounter);
		cpu->m_ProgramCounter = value;
		cpu->ticks += 24;
	}
	else cpu->ticks += 12;
}

void CPU::CPL(CPU* cpu)
{
	cpu->m_RegisterAF.high = ~cpu->m_RegisterAF.high;
	cpu->SetFlag(SUBTRACT);
	cpu->SetFlag(HALF_CARRY);
}

void CPU::CCF(CPU* cpu)
{
	if (cpu->GetFlag(CARRY)) cpu->ClearFlag(CARRY);
	else cpu->SetFlag(CARRY);

	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);
}

void CPU::SCF(CPU* cpu)
{
	cpu->SetFlag(CARRY);
	cpu->ClearFlag(SUBTRACT);
	cpu->ClearFlag(HALF_CARRY);
}


void CPU::RLA(CPU* cpu)
{
	int carry = cpu->GetFlag(CARRY) ? 1 : 0;

	if (cpu->m_RegisterAF.high & 0x80) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_RegisterAF.high <<= 1;
	cpu->m_RegisterAF.high += carry;

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(ZERO); cpu->ClearFlag(HALF_CARRY);
}
void CPU::RRA(CPU* cpu)
{
	int carry = (cpu->GetFlag(CARRY) ? 1 : 0) << 7;

	if (cpu->m_RegisterAF.high & 0x01) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_RegisterAF.high >>= 1;
	cpu->m_RegisterAF.high += carry;

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(ZERO); cpu->ClearFlag(HALF_CARRY);
}
void CPU::RRCA(CPU* cpu)
{
	uint8_t carry = cpu->m_RegisterAF.high & 0x01;
	if (carry) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_RegisterAF.high >>= 1;
	if (carry) cpu->m_RegisterAF.high |= 0x80;

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(ZERO); cpu->ClearFlag(HALF_CARRY);
}
void CPU::RLCA(CPU* cpu)
{
	uint8_t carry = (cpu->m_RegisterAF.high & 0x80) >> 7;
	if (carry) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_RegisterAF.high <<= 1;
	cpu->m_RegisterAF.high += carry;

	cpu->ClearFlag(SUBTRACT); cpu->ClearFlag(ZERO); cpu->ClearFlag(HALF_CARRY);
}

/*
	Decimal adjust register A.
	 This instruction adjusts register A so that the
	 correct representation of Binary Coded Decimal (BCD)
	 is obtained.
*/
void CPU::DAA(CPU* cpu)
{
	if (!cpu->GetFlag(SUBTRACT))
	{
		if (cpu->GetFlag(CARRY) || cpu->m_RegisterAF.high > 0x99) { cpu->m_RegisterAF.high += 0x60; cpu->SetFlag(CARRY); }
		if (cpu->GetFlag(HALF_CARRY) || (cpu->m_RegisterAF.high & 0x0f) > 0x09) { cpu->m_RegisterAF.high += 0x06;  }
	}
	else
	{
		if (cpu->GetFlag(CARRY)) { cpu->m_RegisterAF.high -= 0x60; }
		if (cpu->GetFlag(HALF_CARRY)) { cpu->m_RegisterAF.high -= 0x06; }
	}

	if (cpu->m_RegisterAF.high == 0) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	cpu->ClearFlag(HALF_CARRY);
}

void CPU::CP_A(CPU* cpu) { CP(cpu, cpu->m_RegisterAF.high);  }
void CPU::CP_B(CPU* cpu) { CP(cpu, cpu->m_RegisterBC.high); }
void CPU::CP_C(CPU* cpu) { CP(cpu, cpu->m_RegisterBC.low); }
void CPU::CP_D(CPU* cpu) { CP(cpu, cpu->m_RegisterDE.high); }
void CPU::CP_E(CPU* cpu) { CP(cpu, cpu->m_RegisterDE.low); }
void CPU::CP_H(CPU* cpu) { CP(cpu, cpu->m_RegisterHL.high); }
void CPU::CP_L(CPU* cpu) { CP(cpu, cpu->m_RegisterHL.low); }
void CPU::CP_HL(CPU* cpu) { CP(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::CP_N(CPU* cpu, uint8_t value)
{
	cpu->SetFlag(SUBTRACT);

	if (cpu->m_RegisterAF.high == value) cpu->SetFlag(ZERO);
	else cpu->ClearFlag(ZERO);

	if (value > cpu->m_RegisterAF.high) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if ((value & 0x0f) > (cpu->m_RegisterAF.high & 0x0f)) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);
}

void CPU::LD_SP_HL(CPU* cpu) { cpu->m_StackPointer = cpu->m_RegisterHL.reg; }
void CPU::LD_SP_NN(CPU* cpu, uint16_t value) { cpu->m_StackPointer = value; }

void CPU::LD_HL_N(CPU* cpu, uint8_t value) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, value); }
void CPU::LD_HL_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterHL.reg = value; }
void CPU::LD_HL_SP_N(CPU* cpu, uint8_t value)
{
	unsigned int result = cpu->m_StackPointer + (signed char)value;

	if (((cpu->m_StackPointer ^ ((int8_t)value) ^ (result & 0xFFFF)) & 0x100) == 0x100) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	if (((cpu->m_StackPointer ^ ((int8_t)value) ^ (result & 0xFFFF)) & 0x10) == 0x10) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->ClearFlag(ZERO);
	cpu->ClearFlag(SUBTRACT);

	cpu->m_RegisterHL.reg = (unsigned short)result;
}

void CPU::RET(CPU* cpu)  { cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&(cpu->m_StackPointer)); }
void CPU::RETI(CPU* cpu) { cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&(cpu->m_StackPointer)); cpu->m_MasterInterupts = true; }
void CPU::RET_Z(CPU* cpu)
{
	if (cpu->GetFlag(ZERO)) 
	{
		cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer);
		cpu->ticks += 20;
	}
	else cpu->ticks += 8;
}
void CPU::RET_C(CPU* cpu)
{
	if (cpu->GetFlag(CARRY)) 
	{
		cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer);
		cpu->ticks += 20;
	}
	else cpu->ticks += 8;
}
void CPU::RET_NZ(CPU* cpu)
{
	if (cpu->GetFlag(ZERO)) cpu->ticks += 8;
	else
	{
		cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer);
		cpu->ticks += 20;
	}
}
void CPU::RET_NC(CPU* cpu)
{
	if (cpu->GetFlag(CARRY)) cpu->ticks += 8;
	else
	{
		cpu->m_ProgramCounter = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer);
		cpu->ticks += 20;
	}
}

void CPU::LD_A_B(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterBC.high; }
void CPU::LD_A_C(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterBC.low; }
void CPU::LD_A_D(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterDE.high; }
void CPU::LD_A_E(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterDE.low; }
void CPU::LD_A_H(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterHL.high; }
void CPU::LD_A_L(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_RegisterHL.low; }
void CPU::LD_A_N(CPU* cpu, uint8_t value) { cpu->m_RegisterAF.high = value; }
void CPU::LD_A_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(value); }
void CPU::LD_A_BC(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(cpu->m_RegisterBC.reg); }
void CPU::LD_A_DE(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(cpu->m_RegisterDE.reg); }
void CPU::LD_A_HL(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_A_FF_C(CPU* cpu) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(0xff00 + cpu->m_RegisterBC.low); }

void CPU::LD_E_A(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterAF.high; }
void CPU::LD_E_B(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterBC.high; }
void CPU::LD_E_C(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterBC.low; }
void CPU::LD_E_D(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterDE.high; }
void CPU::LD_E_H(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterHL.high; }
void CPU::LD_E_L(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_RegisterHL.low; }
void CPU::LD_E_HL(CPU* cpu) { cpu->m_RegisterDE.low = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_E_N(CPU* cpu, uint8_t value) { cpu->m_RegisterDE.low = value; }
void CPU::LD_E_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterDE.low = cpu->m_Memory.ReadByte(value); }

void CPU::LD_D_A(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterAF.high; }
void CPU::LD_D_B(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterBC.high; }
void CPU::LD_D_C(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterBC.low; }
void CPU::LD_D_E(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterDE.low; }
void CPU::LD_D_H(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterHL.high; }
void CPU::LD_D_L(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_RegisterHL.low; }
void CPU::LD_D_HL(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_D_N(CPU* cpu, uint8_t value) { cpu->m_RegisterDE.high = value; }
void CPU::LD_D_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterDE.high = cpu->m_Memory.ReadByte(value); }

void CPU::LD_DE_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterDE.reg = value; }
void CPU::LD_DEP_A(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterDE.reg, cpu->m_RegisterAF.high); }

void CPU::LD_FF_C_A(CPU* cpu) { cpu->m_Memory.WriteByte(0xFF00 + cpu->m_RegisterBC.low, cpu->m_RegisterAF.high); }
void CPU::LD_FF_N_A(CPU* cpu, uint8_t value) { cpu->m_Memory.WriteByte(0xFF00 + value, cpu->m_RegisterAF.high); }
void CPU::LD_FF_A_N(CPU* cpu, uint8_t value) { cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(0xFF00 + value); }

void CPU::LD_B_A(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterAF.high; }
void CPU::LD_B_C(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterBC.low; }
void CPU::LD_B_D(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterDE.high; }
void CPU::LD_B_E(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterDE.low; }
void CPU::LD_B_H(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterHL.high; }
void CPU::LD_B_L(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_RegisterHL.low; }
void CPU::LD_B_HL(CPU* cpu) { cpu->m_RegisterBC.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_B_N(CPU* cpu, uint8_t value) { cpu->m_RegisterBC.high = value; }

void CPU::LD_BC_A(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterBC.reg, cpu->m_RegisterAF.high); }
void CPU::LD_BC_NN(CPU* cpu, uint16_t value) { cpu->m_RegisterBC.reg = value; }

void CPU::JP_HL(CPU* cpu) { cpu->m_ProgramCounter = cpu->m_RegisterHL.reg; }
void CPU::JP_NN(CPU* cpu, uint16_t value) { cpu->m_ProgramCounter = value; }
void CPU::JP_Z_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(ZERO)) 
	{
		cpu->m_ProgramCounter = value;
		cpu->ticks += 16;
	}
	else cpu->ticks += 12;
}
void CPU::JP_NZ_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(ZERO)) cpu->ticks += 12;
	else 
	{
		cpu->m_ProgramCounter = value;
		cpu->ticks += 16;
	}
}
void CPU::JP_C_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(CARRY))
	{
		cpu->m_ProgramCounter = value;
		cpu->ticks += 16;
	}
	else cpu->ticks += 12;
}
void CPU::JP_NC_NN(CPU* cpu, uint16_t value)
{
	if (cpu->GetFlag(CARRY)) cpu->ticks += 12;
	else
	{
		cpu->m_ProgramCounter = value;
		cpu->ticks += 16;
	}
}

void CPU::JR_NZ_N(CPU* cpu, uint8_t value)
{
	if (cpu->GetFlag(ZERO)) cpu->ticks += 8;
	else
	{
		cpu->m_ProgramCounter += (signed char)value;
		cpu->ticks += 12;
	}
}
void CPU::JR_NC_N(CPU* cpu, uint8_t value)
{
	if (cpu->GetFlag(CARRY)) cpu->ticks += 8;
	else
	{
		cpu->m_ProgramCounter += (signed char)value;
		cpu->ticks += 12;
	}
}
void CPU::JR_C_N(CPU* cpu, uint8_t value)
{
	if (cpu->GetFlag(CARRY))
	{
		cpu->m_ProgramCounter += (signed char)value;
		cpu->ticks += 12;
	}
	else cpu->ticks += 8;
}
void CPU::JR_Z_N(CPU* cpu, uint8_t value)
{
	if (cpu->GetFlag(ZERO))
	{
		cpu->m_ProgramCounter += (signed char)value;
		cpu->ticks += 12;
	}
	else cpu->ticks += 8;
}
void CPU::JR_N(CPU* cpu, uint8_t value) { cpu->m_ProgramCounter += (signed char)value; }

void CPU::LDD_HL(CPU* cpu) { cpu->m_RegisterDE.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LDD_HL_A(CPU* cpu)
{
	cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterAF.high);
	cpu->m_RegisterHL.reg--;
}
void CPU::LDD_A_HL(CPU* cpu)
{
	cpu->m_RegisterAF.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg);
	cpu->m_RegisterHL.reg--;
}


void CPU::LD_HLP_A(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterAF.high); }
void CPU::LD_HLP_B(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterBC.high); }
void CPU::LD_HLP_C(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterBC.low); }
void CPU::LD_HLP_D(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterDE.high); }
void CPU::LD_HLP_E(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterDE.low); }
void CPU::LD_HLP_H(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterHL.high); }
void CPU::LD_HLP_L(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, cpu->m_RegisterHL.low); }
void CPU::LD_HLP_N(CPU* cpu, uint8_t value) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, value); }

void CPU::DI(CPU* cpu) { cpu->m_MasterInterupts = false; }
void CPU::EI(CPU* cpu) { cpu->m_MasterInterupts = true; }

void CPU::INC_A(CPU* cpu) { cpu->m_RegisterAF.high = Inc(cpu, cpu->m_RegisterAF.high); }
void CPU::INC_B(CPU* cpu) { cpu->m_RegisterBC.high = Inc(cpu, cpu->m_RegisterBC.high); }
void CPU::INC_C(CPU* cpu) { cpu->m_RegisterBC.low = Inc(cpu, cpu->m_RegisterBC.low); }
void CPU::INC_D(CPU* cpu) { cpu->m_RegisterDE.high = Inc(cpu, cpu->m_RegisterDE.high); }
void CPU::INC_E(CPU* cpu) { cpu->m_RegisterDE.low = Inc(cpu, cpu->m_RegisterDE.low); }
void CPU::INC_H(CPU* cpu) { cpu->m_RegisterHL.high = Inc(cpu, cpu->m_RegisterHL.high); }
void CPU::INC_L(CPU* cpu) { cpu->m_RegisterHL.low = Inc(cpu, cpu->m_RegisterHL.low); }
void CPU::INC_BC(CPU* cpu) { cpu->m_RegisterBC.reg++; }
void CPU::INC_DE(CPU* cpu) { cpu->m_RegisterDE.reg++; }
void CPU::INC_SP(CPU* cpu) { cpu->m_StackPointer++; }
void CPU::INC_HL(CPU* cpu) { cpu->m_RegisterHL.reg++; }
void CPU::INC_HLP(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, Inc(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void CPU::DEC_A(CPU* cpu) { cpu->m_RegisterAF.high = Dec(cpu, cpu->m_RegisterAF.high); }
void CPU::DEC_B(CPU* cpu) { cpu->m_RegisterBC.high = Dec(cpu, cpu->m_RegisterBC.high); }
void CPU::DEC_C(CPU* cpu) { cpu->m_RegisterBC.low  = Dec(cpu, cpu->m_RegisterBC.low); }
void CPU::DEC_D(CPU* cpu) { cpu->m_RegisterDE.high = Dec(cpu, cpu->m_RegisterDE.high); }
void CPU::DEC_E(CPU* cpu) { cpu->m_RegisterDE.low = Dec(cpu, cpu->m_RegisterDE.low); }
void CPU::DEC_H(CPU* cpu) { cpu->m_RegisterHL.high = Dec(cpu, cpu->m_RegisterHL.high); }
void CPU::DEC_L(CPU* cpu) { cpu->m_RegisterHL.low = Dec(cpu, cpu->m_RegisterHL.low); }
void CPU::DEC_BC(CPU* cpu) { cpu->m_RegisterBC.reg--; }
void CPU::DEC_DE(CPU* cpu) { cpu->m_RegisterDE.reg--; }
void CPU::DEC_SP(CPU* cpu) { cpu->m_StackPointer--; }
void CPU::DEC_HL(CPU* cpu) { cpu->m_RegisterHL.reg--; }
void CPU::DEC_HLP(CPU* cpu) { cpu->m_Memory.WriteByte(cpu->m_RegisterHL.reg, Dec(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg))); }

void CPU::ADD_A_A(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterAF.high); }
void CPU::ADD_A_B(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterBC.high); }
void CPU::ADD_A_C(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterBC.low); }
void CPU::ADD_A_D(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterDE.high); }
void CPU::ADD_A_E(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterDE.low); }
void CPU::ADD_A_H(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterHL.high); }
void CPU::ADD_A_L(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_RegisterHL.low); }
void CPU::ADD_A_HL(CPU* cpu) { Add(cpu, &(cpu->m_RegisterAF.high), cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::ADD_A_N(CPU* cpu, uint8_t value) { Add(cpu, &(cpu->m_RegisterAF.high), value); }

void CPU::ADD_HL_BC(CPU* cpu) { Add2(cpu, &(cpu->m_RegisterHL.reg), cpu->m_RegisterBC.reg); }
void CPU::ADD_HL_DE(CPU* cpu) { Add2(cpu, &(cpu->m_RegisterHL.reg), cpu->m_RegisterDE.reg); }
void CPU::ADD_HL_HL(CPU* cpu) { Add2(cpu, &(cpu->m_RegisterHL.reg), cpu->m_RegisterHL.reg); }
void CPU::ADD_HL_SP(CPU* cpu) { Add2(cpu, &(cpu->m_RegisterHL.reg), cpu->m_StackPointer); }

void CPU::ADC_A(CPU* cpu) { ADC(cpu, cpu->m_RegisterAF.high); }
void CPU::ADC_B(CPU* cpu) { ADC(cpu, cpu->m_RegisterBC.high); }
void CPU::ADC_C(CPU* cpu) { ADC(cpu, cpu->m_RegisterBC.low); }
void CPU::ADC_D(CPU* cpu) { ADC(cpu, cpu->m_RegisterDE.high); }
void CPU::ADC_E(CPU* cpu) { ADC(cpu, cpu->m_RegisterDE.low); }
void CPU::ADC_H(CPU* cpu) { ADC(cpu, cpu->m_RegisterHL.high); }
void CPU::ADC_L(CPU* cpu) { ADC(cpu, cpu->m_RegisterHL.low); }
void CPU::ADC_HL(CPU* cpu) { ADC(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::ADC_N(CPU* cpu, uint8_t value) { ADC(cpu, value); }

void CPU::ADD_SP_N(CPU* cpu, uint8_t value)
{
	int result = (int)(cpu->m_StackPointer + (int8_t)value);

	cpu->ClearFlag(ZERO);
	cpu->ClearFlag(SUBTRACT);

	if (((cpu->m_StackPointer ^ (int8_t)value ^ (result & 0xFFFF)) & 0x10) == 0x10) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	if (((cpu->m_StackPointer ^ (int8_t)value ^ (result & 0xFFFF)) & 0x100) == 0x100) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_StackPointer = (uint16_t)result;

	/*
	int result = cpu->m_StackPointer + value;

	if (result & 0xffff0000) cpu->SetFlag(CARRY);
	else cpu->ClearFlag(CARRY);

	cpu->m_StackPointer = result & 0xffff;

	if (((cpu->m_StackPointer & 0x0f) + (value & 0x0f)) > 0x0f) cpu->SetFlag(HALF_CARRY);
	else cpu->ClearFlag(HALF_CARRY);

	cpu->ClearFlag(ZERO); cpu->ClearFlag(SUBTRACT);
	*/
}

void CPU::RST_00(CPU* cpu) { RST(cpu, 0x00); }
void CPU::RST_08(CPU* cpu) { RST(cpu, 0x08); }
void CPU::RST_10(CPU* cpu) { RST(cpu, 0x10); }
void CPU::RST_18(CPU* cpu) { RST(cpu, 0x18); }
void CPU::RST_20(CPU* cpu) { RST(cpu, 0x20); }
void CPU::RST_28(CPU* cpu) { RST(cpu, 0x28); }
void CPU::RST_30(CPU* cpu) { RST(cpu, 0x30); }
void CPU::RST_38(CPU* cpu) { RST(cpu, 0x38); }

void CPU::XOR_A(CPU* cpu) { XOR(cpu, cpu->m_RegisterAF.high); }
void CPU::XOR_B(CPU* cpu) { XOR(cpu, cpu->m_RegisterBC.high); }
void CPU::XOR_C(CPU* cpu) { XOR(cpu, cpu->m_RegisterBC.low); }
void CPU::XOR_D(CPU* cpu) { XOR(cpu, cpu->m_RegisterDE.high); }
void CPU::XOR_E(CPU* cpu) { XOR(cpu, cpu->m_RegisterDE.low); }
void CPU::XOR_H(CPU* cpu) { XOR(cpu, cpu->m_RegisterHL.high); }
void CPU::XOR_L(CPU* cpu) { XOR(cpu, cpu->m_RegisterHL.low); }
void CPU::XOR_HL(CPU* cpu) { XOR(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::XOR_N(CPU* cpu, uint8_t value) { XOR(cpu, value); }

void CPU::AND_A(CPU* cpu) { AND(cpu, cpu->m_RegisterAF.high); }
void CPU::AND_B(CPU* cpu) { AND(cpu, cpu->m_RegisterBC.high); }
void CPU::AND_C(CPU* cpu) { AND(cpu, cpu->m_RegisterBC.low); }
void CPU::AND_D(CPU* cpu) { AND(cpu, cpu->m_RegisterDE.high); }
void CPU::AND_E(CPU* cpu) { AND(cpu, cpu->m_RegisterDE.low); }
void CPU::AND_H(CPU* cpu) { AND(cpu, cpu->m_RegisterHL.high); }
void CPU::AND_L(CPU* cpu) { AND(cpu, cpu->m_RegisterHL.low); }
void CPU::AND_HL(CPU* cpu) { AND(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::AND_N(CPU* cpu, uint8_t value) { AND(cpu, value); }

void CPU::OR_A(CPU* cpu) { OR(cpu, cpu->m_RegisterAF.high); }
void CPU::OR_B(CPU* cpu) { OR(cpu, cpu->m_RegisterBC.high); }
void CPU::OR_C(CPU* cpu) { OR(cpu, cpu->m_RegisterBC.low); }
void CPU::OR_D(CPU* cpu) { OR(cpu, cpu->m_RegisterDE.high); }
void CPU::OR_E(CPU* cpu) { OR(cpu, cpu->m_RegisterDE.low); }
void CPU::OR_H(CPU* cpu) { OR(cpu, cpu->m_RegisterHL.high); }
void CPU::OR_L(CPU* cpu) { OR(cpu, cpu->m_RegisterHL.low); }
void CPU::OR_HL(CPU* cpu) { OR(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::OR_N(CPU* cpu, uint8_t value) { OR(cpu, value); }

void CPU::SUB_A(CPU* cpu) { Sub(cpu, cpu->m_RegisterAF.high); }
void CPU::SUB_B(CPU* cpu) { Sub(cpu, cpu->m_RegisterBC.high); }
void CPU::SUB_C(CPU* cpu) { Sub(cpu, cpu->m_RegisterBC.low); }
void CPU::SUB_D(CPU* cpu) { Sub(cpu, cpu->m_RegisterDE.high); }
void CPU::SUB_E(CPU* cpu) { Sub(cpu, cpu->m_RegisterDE.low); }
void CPU::SUB_F(CPU* cpu) { Sub(cpu, cpu->m_RegisterAF.low); }
void CPU::SUB_H(CPU* cpu) { Sub(cpu, cpu->m_RegisterHL.high); }
void CPU::SUB_L(CPU* cpu) { Sub(cpu, cpu->m_RegisterHL.low); }
void CPU::SUB_HL(CPU* cpu) { Sub(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::SUB_N(CPU* cpu, uint8_t value) { Sub(cpu, value); }

void CPU::LD_C_A(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterAF.high; }
void CPU::LD_C_B(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterBC.high; }
void CPU::LD_C_D(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterDE.high; }
void CPU::LD_C_E(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterDE.low; }
void CPU::LD_C_H(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterHL.high; }
void CPU::LD_C_L(CPU* cpu)	{ cpu->m_RegisterBC.low = cpu->m_RegisterHL.low; }
void CPU::LD_C_HL(CPU* cpu) { cpu->m_RegisterBC.low = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_C_N(CPU* cpu, uint8_t value) { cpu->m_RegisterBC.low = value; }

void CPU::LD_L_A(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterAF.high; }
void CPU::LD_L_B(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterBC.high; }
void CPU::LD_L_C(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterBC.low; }
void CPU::LD_L_D(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterDE.high; }
void CPU::LD_L_E(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterDE.low; }
void CPU::LD_L_H(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_RegisterHL.high; }
void CPU::LD_L_HL(CPU* cpu) { cpu->m_RegisterHL.low = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_L_N(CPU* cpu, uint8_t value) { cpu->m_RegisterHL.low = value; }

void CPU::LD_H_A(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterAF.high; }
void CPU::LD_H_B(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterBC.high; }
void CPU::LD_H_C(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterBC.low; }
void CPU::LD_H_D(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterDE.high; }
void CPU::LD_H_E(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterDE.low; }
void CPU::LD_H_L(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_RegisterHL.low; }
void CPU::LD_H_HL(CPU* cpu) { cpu->m_RegisterHL.high = cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg); }
void CPU::LD_H_N(CPU* cpu, uint8_t value) { cpu->m_RegisterHL.high = value; }

void CPU::PUSH_AF(CPU* cpu) { cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_RegisterAF.reg); }
void CPU::PUSH_BC(CPU* cpu) { cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_RegisterBC.reg); }
void CPU::PUSH_DE(CPU* cpu) { cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_RegisterDE.reg); }
void CPU::PUSH_HL(CPU* cpu) { cpu->m_Memory.WriteShortToStack(&cpu->m_StackPointer, cpu->m_RegisterHL.reg); }

void CPU::POP_HL(CPU* cpu) { cpu->m_RegisterHL.reg = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer); }
void CPU::POP_DE(CPU* cpu) { cpu->m_RegisterDE.reg = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer); }
void CPU::POP_BC(CPU* cpu) { cpu->m_RegisterBC.reg = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer); }
void CPU::POP_AF(CPU* cpu) { cpu->m_RegisterAF.reg = cpu->m_Memory.ReadShortFromStack(&cpu->m_StackPointer) & 0xFFF0; } // Cannot write to lower nibble

void CPU::SBC_A(CPU* cpu) { SBC(cpu, cpu->m_RegisterAF.high);  }
void CPU::SBC_B(CPU* cpu) { SBC(cpu, cpu->m_RegisterBC.high); }
void CPU::SBC_C(CPU* cpu) { SBC(cpu, cpu->m_RegisterBC.low); }
void CPU::SBC_D(CPU* cpu) { SBC(cpu, cpu->m_RegisterDE.high); }
void CPU::SBC_E(CPU* cpu) { SBC(cpu, cpu->m_RegisterDE.low); }
void CPU::SBC_H(CPU* cpu) { SBC(cpu, cpu->m_RegisterHL.high); }
void CPU::SBC_L(CPU* cpu) { SBC(cpu, cpu->m_RegisterHL.low); }
void CPU::SBC_HL(CPU* cpu) { SBC(cpu, cpu->m_Memory.ReadByte(cpu->m_RegisterHL.reg)); }
void CPU::SBC_N(CPU* cpu, uint8_t value) { SBC(cpu, value); }

CPU::~CPU()
{
	
}