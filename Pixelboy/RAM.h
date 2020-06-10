#pragma once

#include "Cartridge.h"
#include "GPU.h"

/*
	Memory mapped reading from memory:
		0000-3FFF 16KB ROM Bank 00 (in cartridge, fixed at bank 00)
		4000-7FFF 16KB ROM Bank 01..NN (in cartridge, switchable bank number)
		8000-9FFF 8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
		A000-BFFF 8KB External RAM (in cartridge, switchable bank, if any)
		C000-CFFF 4KB Work RAM Bank 0 (WRAM)
		D000-DFFF 4KB Work RAM Bank 1 (WRAM) (switchable bank 1-7 in CGB Mode)
		E000-FDFF Same as C000-DDFF (ECHO) (typically not used)
		FE00-FE9F Sprite Attribute Table (OAM)
		FEA0-FEFF Not Usable
		FF00-FF7F I/O Ports
		FF80-FFFE High RAM (HRAM)
		FFFF Interrupt Enable Register
*/

class Memory
{
public:

	Memory(const std::string& sBootRom, const std::string& sFileName);
	Memory(const std::string& sFileName);
	Memory() {};
	void Reset(const std::string& sFileName, bool bBootRom);
	~Memory();

	uint8_t ReadByte(uint16_t address);
	void WriteByte(uint16_t address, uint8_t data);

	void WriteShortToStack(uint16_t* stackPointer, uint16_t address);
	uint16_t ReadShortFromStack(uint16_t* stackPointer);

	void WriteShort(uint16_t address, uint16_t value);
	uint16_t ReadShort(uint16_t address);

	// Banking
	bool m_bBanking;
	uint8_t m_RamBanks[0x8000];
	uint8_t	m_CurrentROMBank;
	uint8_t	m_CurrentRAMBank;
	bool m_bEnableRAM;
	bool m_bROMBanking;

	// Interrupts
	uint8_t m_InterruptFlags;
	uint8_t m_InterruptsEnabled;

	// Joypad
	uint8_t m_JoypadState;
	uint8_t m_JoypadReq;

	// Graphics
	GPU m_GPU;

	uint8_t m_Sram[0x2000];
	uint8_t m_Io[0x100];
	uint8_t m_Vram[0x2000];
	uint8_t m_Oam[0x100];
	uint8_t m_Wram[0x2000];
	uint8_t m_Hram[0x80];

	// Timers
	uint8_t m_Timer;
	uint8_t m_TimerFrequency;
	uint8_t m_TimerResetValue;
	int m_TimerCounter;

	// Divider register
	uint8_t m_DividerRegister;
	int m_DividerCounter;

	bool m_bBootRom;

	Cartridge m_Cartridge;

private:
	Cartridge m_BootRom;

	/* http://www.codeslinger.co.uk/pages/projects/gameboy/banking.html */

	void HandleBanking(uint16_t address, uint8_t data);
	void EnableRamBank(uint16_t address, uint8_t data);
	void DoChangeLoROMBank(uint8_t data);
	void DoChangeHiROMBank(uint8_t data);
	void DoRAMBankChange(uint8_t data);
	void DoChangeROMRAMMode(uint8_t data);

};

