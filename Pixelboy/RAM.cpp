#include "RAM.h"

#include <iostream>
#include <sstream>
#include <cstring>

#include "Cartridge.h"

// State of IO memory at boot time
const uint8_t ioReset[0x100] = 
{
	0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
	0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
	0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
	0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
	0x45, 0xEC, 0x52, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
	0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE, 0x46, 0x97, 0x33, 0x5E,
	0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5,
	0xD0, 0x7A, 0x00, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
	0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00
};

Memory::Memory(const std::string& sBootRom, const std::string& sFileName) : m_BootRom(sBootRom), m_Cartridge(sFileName), m_GPU(m_Vram, m_Oam)
{
	Reset(sFileName, true);
}

Memory::Memory(const std::string& sFileName) : m_Cartridge(sFileName), m_GPU(m_Vram, m_Oam)
{
	Reset(sFileName, false);
}

void Memory::Reset(const std::string& sFileName, bool bBootRom)
{
	// Load ROM if need be
	if (m_Cartridge.m_Memory == nullptr) m_Cartridge.Reset(sFileName);

	// Zero out memory and copy for IO
	memset(m_Sram, 0, sizeof(m_Sram));
	memcpy(m_Io, ioReset, sizeof(m_Io));
	memset(m_Vram, 0, sizeof(m_Vram));
	memset(m_Oam, 0, sizeof(m_Oam));
	memset(m_Wram, 0, sizeof(m_Wram));
	memset(m_Hram, 0, sizeof(m_Hram));

	// Set default state of memory
	WriteByte(0xFF05, 0);		WriteByte(0xFF06, 0);		WriteByte(0xFF07, 0);
	WriteByte(0xFF10, 0x80);	WriteByte(0xFF11, 0xBF);	WriteByte(0xFF12, 0xF3);
	WriteByte(0xFF14, 0xBF);	WriteByte(0xFF16, 0x3F);	WriteByte(0xFF17, 0x00);
	WriteByte(0xFF19, 0xBF);	WriteByte(0xFF1A, 0x7A);	WriteByte(0xFF1B, 0xFF);
	WriteByte(0xFF1C, 0x9F);	WriteByte(0xFF1E, 0xBF);	WriteByte(0xFF20, 0xFF);
	WriteByte(0xFF21, 0x00);	WriteByte(0xFF22, 0x00);	WriteByte(0xFF23, 0xBF);
	WriteByte(0xFF24, 0x77);	WriteByte(0xFF25, 0xF3);	WriteByte(0xFF26, 0xF1);
	WriteByte(0xFF40, 0x91);	WriteByte(0xFF42, 0x00);	WriteByte(0xFF43, 0x00);
	WriteByte(0xFF45, 0x00);	WriteByte(0xFF47, 0xFC);	WriteByte(0xFF48, 0xFF);
	WriteByte(0xFF49, 0xFF);	WriteByte(0xFF4A, 0x00);	WriteByte(0xFF4B, 0x00);
	WriteByte(0xFFFF, 0x00);

	// Banking
	m_CurrentROMBank = 1;
	m_CurrentRAMBank = 0;
	m_bBanking = m_Cartridge.m_bMBC1 || m_Cartridge.m_bMBC2;
	m_bEnableRAM = false;
	m_bROMBanking = false;
	memset(&m_RamBanks, 0, sizeof(m_RamBanks));

	// Interrupts
	m_InterruptFlags = 0;
	m_InterruptsEnabled = 0;

	m_JoypadState = 0;
	m_JoypadReq = 0;

	m_bBootRom = bBootRom;

	m_Timer = 0;
	m_TimerFrequency = 0;
	m_TimerResetValue = 0;
	m_DividerRegister = 0;
	m_DividerCounter = 0;
	m_TimerCounter = 1024;
}

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

uint8_t Memory::ReadByte(uint16_t address)
{
	auto hexToString = [](int hex)
	{
		return (static_cast<std::stringstream const&>(std::stringstream() << "0x" << std::hex << hex)).str();
	};

	if (address < 0x8000) // Cartridge memory
	{
		if (m_bBootRom && address < 256) return m_BootRom.m_Memory[address];

		if (address < 0x4000 || !m_bBanking) return m_Cartridge.m_Memory[address];
		else // Banking
		{ 
			uint16_t newAddress = address - 0x4000;
			return m_Cartridge.m_Memory[newAddress + (m_CurrentROMBank * 0x4000)];
		}
	}

	else if (address >= 0xA000 && address <= 0xBFFF)
	{ 
		uint16_t newAddress = address - 0xA000;
		return m_RamBanks[newAddress + (m_CurrentRAMBank * 0x2000)];
	}

	else if (address >= 0x8000 && address <= 0x9fff) return m_Vram[address - 0x8000];
	else if (address >= 0xC000 && address <= 0xDFFF) return m_Wram[address - 0xC000];
	else if (address >= 0xE000 && address <= 0xFDFF) return m_Wram[address - 0xE000];
	else if (address >= 0xFE00 && address <= 0xFEFF) return m_Oam[address - 0xFE00];
	
	else if (address == 0xFF04) return m_DividerRegister;

	// GPU and LCD
	else if (address == 0xff40) return m_GPU.m_Control;
	else if (address == 0xff41) return m_GPU.m_LCDStatus;
	else if (address == 0xff42) return m_GPU.m_ScrollY;
	else if (address == 0xff43) return m_GPU.m_ScrollX;
	else if (address == 0xff44) return m_GPU.m_Scanline; // read only
	else if (address == 0xff45) return m_GPU.m_Coincidence;
	else if (address == 0xff4A) return m_GPU.m_WindowY;
	else if (address == 0xff4B) return m_GPU.m_WindowX;

	else if (address == 0xFF00) // Input
	{
		uint8_t returnValue = 0;

		switch (m_JoypadReq)
		{
			case 0x10: returnValue = m_JoypadState >> 4; break;
			case 0x20: returnValue = m_JoypadState & 0xF; break;
			default: break;
		}

		return returnValue;

		/*
		uint8_t res = m_JoypadReq;
		res ^= 0xFF; // Flip all the bits

		// Standard buttons
		if (!(res & 0x10000))
		{
			uint8_t topJoypad = m_JoypadState >> 4;
			topJoypad |= 0xF0;
			res &= topJoypad;
		}
		else if (!(res & 0x100000))
		{
			uint8_t bottomJoypad = m_JoypadState & 0xF;
			bottomJoypad |= 0xF0;
			res &= bottomJoypad;
		}

		return res;*/
	}

	// Interrupts
	else if (address == 0xFF0F) return m_InterruptFlags;
	else if (address == 0xFFFF) return m_InterruptsEnabled;

	// More memory
	else if (address >= 0xff80 && address <= 0xfffe) return m_Hram[address - 0xff80];
	else if (address >= 0xff00 && address <= 0xff7f) return m_Io[address - 0xff00];

	// Boot rom
	else if (address == 0xFF50) return m_bBootRom;

	// Timer
	else if (address == 0xFF05) return m_Timer;
	else if (address == 0xFF06) return m_TimerResetValue;
	else if (address == 0xFF07) return m_TimerFrequency;

	std::cerr << "Invalid memory address " << hexToString(address) << std::endl;
	exit(-1);
	return 0;
}

void Memory::WriteByte(uint16_t address, uint8_t data)
{
	auto hexToString = [](int hex)
	{
		return (static_cast<std::stringstream const&>(std::stringstream() << "0x" << std::hex << hex)).str();
	};

	// Banking
	if (address < 0x8000) HandleBanking(address, data);

	else if (address >= 0x8000 && address <= 0x9fff) m_Vram[address - 0x8000] = data;

	// Banking
	else if ((address >= 0xA000) && (address < 0xC000))
	{
		if (m_bEnableRAM)
		{
			uint16_t newAddress = address - 0xA000;
			m_RamBanks[newAddress + (m_CurrentRAMBank * 0x2000)] = data;
		}
	}

	else if (address >= 0xC000 && address <= 0xDFFF) m_Wram[address - 0xC000] = data;
	else if (address >= 0xE000 && address <= 0xFDFF) 
	{
		// Echo RAM
		m_Wram[address - 0xE000 - 0x2000] = data; 
		m_Wram[address - 0xE000] = data;
	}
	else if (address >= 0xFE00 && address <= 0xFEFF) m_Oam[address - 0xFE00] = data;
	else if (address >= 0xFF80 && address <= 0xFFFE) m_Hram[address - 0xFF80] = data;

	// GPU and LCD
	else if (address == 0xff40) m_GPU.m_Control = data;
	else if (address == 0xff41) m_GPU.m_LCDStatus = data;
	else if (address == 0xff42) m_GPU.m_ScrollY = data;
	else if (address == 0xff43) m_GPU.m_ScrollX = data;
	else if (address == 0xff44) m_GPU.m_Scanline = 0; // reset if trying to write
	else if (address == 0xff45) m_GPU.m_Coincidence = data;
	else if (address == 0xff4A) m_GPU.m_WindowY = data;
	else if (address == 0xff4B) m_GPU.m_WindowX = data;

	// OMA DMA
	else if (address == 0xFF46)
	{
		uint16_t address = data << 8; // source address is data * 100
		for (int i = 0; i < 0xA0; i++)
		{
			WriteByte(0xFE00 + i, ReadByte(address + i));
		}
	}

	// Write-only Pallettes
	else if (address == 0xFF47 || address == 0xFF48 || address == 0xFF49)
	{
		if (address == 0xFF47) m_GPU.m_BackgroundPalette = data;
		if (address == 0xFF48) m_GPU.m_SpritePalettes[0] = data;
		else m_GPU.m_SpritePalettes[1] = data;
	}

	// Input
	else if (address == 0xFF00) m_JoypadReq = data;
	

	else if (address == 0xFF04) m_DividerRegister = 0;

	else if (address == 0xFF0F) m_InterruptFlags = data;
	else if (address == 0xFFFF) m_InterruptsEnabled = data;

	// Boot rom
	else if (address == 0xFF50) m_bBootRom = false;

	// Timer
	else if (address == 0xFF05) m_Timer = data;
	else if (address == 0xFF06) m_TimerResetValue = data;
	else if (address == 0xFF07)
	{
		uint8_t currentFrequency = m_TimerFrequency & 0x3;
		m_TimerFrequency = data;
		uint8_t newFrequency = m_TimerFrequency & 0x3;
		if (currentFrequency != newFrequency)
		{
			switch (m_TimerFrequency & 0x3)
			{
				case 0: m_TimerCounter = 1024; break; // freq 4096
				case 1: m_TimerCounter = 16; break;// freq 262144
				case 2: m_TimerCounter = 64; break;// freq 65536
				case 3: m_TimerCounter = 256; break;// freq 16382
			}
		}
	}

	// Other IO
	else if (address >= 0xFF00 && address <= 0xFF7F) m_Io[address - 0xFF00] = data;
}

uint16_t Memory::ReadShort(uint16_t address)
{
	return ReadByte(address) | (ReadByte(address + 1) << 8);
}

void Memory::WriteShort(uint16_t address, uint16_t value)
{
	WriteByte(address, (unsigned char)(value & 0x00ff));
	WriteByte(address + 1, (unsigned char)((value & 0xff00) >> 8));
}

uint16_t Memory::ReadShortFromStack(uint16_t* stackPointer)
{
	uint16_t value = ReadShort(*stackPointer);
	*stackPointer += 2;

	return value;
}

void Memory::WriteShortToStack(uint16_t* stackPointer, uint16_t value)
{
	*stackPointer -= 2;
	WriteShort(*stackPointer, value);
}

void Memory::HandleBanking(uint16_t address, uint8_t data)
{
	// RAM enabling
	if (address < 0x2000)
	{
		if (m_Cartridge.m_bMBC1 || m_Cartridge.m_bMBC2) EnableRamBank(address, data);
	}

	// ROM bank change
	else if ((address >= 0x200) && (address < 0x4000))
	{
		if (m_Cartridge.m_bMBC1 || m_Cartridge.m_bMBC2)
		{
			DoChangeLoROMBank(data);
		}
	}

	// ROM or RAM bank change
	else if ((address >= 0x4000) && (address < 0x6000))
	{
		// there is no rambank in mbc2 so always use rambank 0
		if (m_Cartridge.m_bMBC1)
		{
			if (m_bROMBanking)
			{
				DoChangeHiROMBank(data);
			}
			else
			{
				DoRAMBankChange(data);
			}
		}
	}

	else if ((address >= 0x6000) && (address < 0x8000))
	{
		if (m_Cartridge.m_bMBC1) DoChangeROMRAMMode(data);
	}
}

void Memory::EnableRamBank(uint16_t address, uint8_t data)
{
	if (m_Cartridge.m_bMBC2)
		if ((address & 0b10000) == 1) return;

	uint8_t testData = data & 0xF;
	if (testData == 0xA)
		m_bEnableRAM = true;
	else if (testData == 0x0)
		m_bEnableRAM = false;
}

void Memory::DoChangeLoROMBank(uint8_t data)
{
	if (m_Cartridge.m_bMBC2)
	{
		m_CurrentROMBank = data & 0xF;
		if (m_CurrentROMBank == 0) m_CurrentROMBank++;
		return;
	}

	uint8_t lower5 = data & 31;
	m_CurrentROMBank &= 224; // turn off the lower 5
	m_CurrentROMBank |= lower5;
	if (m_CurrentROMBank == 0) m_CurrentROMBank++;
}

void Memory::DoChangeHiROMBank(uint8_t data)
{
	// turn off the upper 3 bits of the current rom
	m_CurrentROMBank &= 31;

	// turn off the lower 5 bits of the data
	data &= 224;
	m_CurrentROMBank |= data;
	if (m_CurrentROMBank == 0) m_CurrentROMBank++;
}

void Memory::DoRAMBankChange(uint8_t data)
{
	m_CurrentRAMBank = data & 0x3;
}

void Memory::DoChangeROMRAMMode(uint8_t data)
{
	uint8_t newData = data & 0x1;
	m_bROMBanking = (newData == 0) ? true : false;
	if (m_bROMBanking)
		m_CurrentRAMBank = 0;
}

Memory::~Memory()
{
	
}