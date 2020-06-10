#include "Cartridge.h"

#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <algorithm>    // std::find

// In case of error, use "tinyfiledialogs" to display an error pop-up
#include "tinyfiledialogs.h"

#define CARTRIDGE_SIZE 0x200000

Cartridge::Cartridge(const std::string& sFileName)
{
	Reset(sFileName);
}

void Cartridge::Reset(const std::string& sFileName)
{
	// Load file
	std::ifstream input(sFileName, std::ios::binary);
	std::vector<uint8_t>  buffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(input), {});

	// Error handling
	if (buffer.size() == 0 || buffer.size() > CARTRIDGE_SIZE)
	{
		std::cerr << "Error: unable to load ROM " + sFileName << std::endl;
		if (buffer.size() > CARTRIDGE_SIZE) std::cerr << "ROM too big" << std::endl;

		// Make error box
		tinyfd_messageBox("Error", std::string("Could not load file " + sFileName).c_str(), "ok", "error", 1);

		exit(-1);
	}

	// Allocate space, zero out rest of memory and load into cartridge memory
	m_Memory = new uint8_t[CARTRIDGE_SIZE];
	for (unsigned int i = 0; i < CARTRIDGE_SIZE; ++i)	m_Memory[i] = 0;
	for (unsigned int i = 0; i < buffer.size(); ++i)	m_Memory[i] = buffer[i];

	// Detect banking type
	m_bMBC1 = false;
	m_bMBC2 = false;
	switch (m_Memory[0x147])
	{
		case 1: m_bMBC1 = true;
		case 2: m_bMBC1 = true;
		case 3: m_bMBC1 = true;
		case 5: m_bMBC2 = true;
		case 6: m_bMBC2 = true;
		default: break;
	}
}

std::string Cartridge::GetTitle()
{
	// Title exists at 0x134 - 0x143 in upper case ASCII, with empty bytes 
	// being filled with 00's. With the CGB it's actually only 11 bytes, #
	// so for sanity's sake let's stick to 11 bytes too.
	std::string sTitle = "";

	int titlePtr = 0x134;
	while (m_Memory[titlePtr] != '0')
	{
		sTitle += m_Memory[titlePtr];
		titlePtr++;
	}

	// Get rid of null terminator so we don't break the FPS counter
	sTitle.erase(std::find(sTitle.begin(), sTitle.end(), '\0'), sTitle.end());

	return sTitle;
}

Cartridge::~Cartridge()
{
	delete[] m_Memory;
}