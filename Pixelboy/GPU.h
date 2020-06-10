#pragma once

#include <stdint.h>

struct InterruptReturns
{
	bool bVblank;
	bool bLCD;
};

enum Colour
{
	WHITE,
	LIGHT_GRAY,
	DARK_GRAY,
	BLACK
};

class GPU
{

public:

	GPU(uint8_t* vram, uint8_t* oam);
	GPU() {};
	void Reset(uint8_t* vram, uint8_t* oam);

	InterruptReturns Update(int cycles);

	uint8_t m_Scanline;
	uint8_t m_Control;
	uint8_t m_ScrollX;
	uint8_t m_ScrollY;
	uint8_t m_WindowX;
	uint8_t m_WindowY;
	uint8_t m_LCDStatus;
	uint8_t m_Coincidence;

	uint8_t* m_Vram;
	uint8_t* m_Oam;

	uint8_t m_BackgroundPalette;
	uint8_t m_SpritePalettes[2];

	uint8_t m_ScreenData[160][144][3];

	int m_ScanlineCounter;

private:

	bool SetLCDStatus();
	bool IsLCDEnabled();

	void DrawScanLine();
	void RenderTiles();
	void RenderSprites();

	Colour GetColour(uint8_t colourNumber, uint8_t palette);

	// returns if a bit is set
	template <typename t>
	bool TestBit(t data, int position)
	{
		t mask = 1 << position;
		return (data & mask) ? true : false;
	}

	///////////////////////////////////////////////////////////////////////////////////

	template <typename t>
	t BitSet(t data, int position)
	{
		t mask = 1 << position;
		data |= mask;
		return data;
	}

	template< typename t >
	t BitReset(t data, int position)
	{
		t mask = 1 << position;
		data &= ~mask;
		return data;
	}

	template <typename t>
	t BitGetVal(t data, int position)
	{
		t mask = 1 << position;
		return (data & mask) ? 1 : 0;
	}

};

