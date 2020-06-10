#pragma once

#include <memory>
#include <string>

class Cartridge
{
public:

	Cartridge(const std::string& sFileName);
	Cartridge() {};
	~Cartridge();

	void Reset(const std::string& sFileName);

	bool m_bMBC1;
	bool m_bMBC2;

	uint8_t* m_Memory = nullptr;

	std::string GetTitle();

};

