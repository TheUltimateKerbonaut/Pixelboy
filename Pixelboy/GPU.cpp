#include "GPU.h"

#include <iostream>
#include <sstream>
#include <thread>

GPU::GPU(uint8_t* vram, uint8_t* oam)
{
	Reset(vram, oam);
}

void GPU::Reset(uint8_t* vram, uint8_t* oam)
{
	m_Vram = vram;
	m_Oam = oam;

	m_Scanline = 0;
	m_Control = 0;
	m_ScrollX = 0;
	m_ScrollY = 0;
	m_WindowX = 0;
	m_WindowY = 0;
	m_LCDStatus = 0;
	m_Coincidence = 0;

	m_ScanlineCounter = 456;

	m_BackgroundPalette = 0;
	m_SpritePalettes[0] = 0; m_SpritePalettes[1] = 1;
}

/*
	https://github.com/CTurt/Cinoop/blob/master/source/gpu.c
	http://www.codeslinger.co.uk/pages/projects/gameboy/lcd.html
	http://www.codeslinger.co.uk/pages/projects/gameboy/graphics.html
	http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Graphics
*/

InterruptReturns GPU::Update(int cycles)
{
	InterruptReturns returnVariable;
	returnVariable.bVblank = false;
	returnVariable.bLCD = SetLCDStatus();

	if (IsLCDEnabled()) m_ScanlineCounter -= cycles; // Move the scanline down
	else { returnVariable.bVblank = false; return returnVariable; } // If the LCD is off, don't bother

	if (m_ScanlineCounter <= 0)
	{
		// Move onto next scanline
		//m_Scanline++;

		m_ScanlineCounter = 456;

		// V-Blank interrupt
		if (m_Scanline == 144) returnVariable.bVblank = true;

		// Reset to 0 if needed
		else if (m_Scanline > 153) m_Scanline = 0;

		// Draw the current scanline
		else if (m_Scanline < 144) DrawScanLine();

		m_Scanline++;
	}

	return returnVariable;
	
}

bool GPU::SetLCDStatus()
{
	bool bInterrupt = false;

	if (!IsLCDEnabled())
	{
		// set the mode to 1 during lcd disabled and reset scanline
		m_ScanlineCounter = 456;
		m_Scanline = 0;
		m_LCDStatus &= 252;
		m_LCDStatus = BitSet(m_LCDStatus, 0);
		return false;
	}

	uint8_t currentmode = m_LCDStatus & 0x3;

	uint8_t mode = 0;
	bool reqInt = false;

	// in vblank so set mode to 1
	if (m_Scanline >= 144)
	{
		mode = 1;
		m_LCDStatus = BitSet(m_LCDStatus, 0);
		m_LCDStatus = BitReset(m_LCDStatus, 1);
		reqInt = TestBit(m_LCDStatus, 4);
	}
	else
	{
		int mode2bounds = 456 - 80;
		int mode3bounds = mode2bounds - 172;

		// mode 2
		if (m_ScanlineCounter >= mode2bounds)
		{
			mode = 2;
			m_LCDStatus = BitSet(m_LCDStatus, 1);
			m_LCDStatus = BitReset(m_LCDStatus, 0);
			reqInt = TestBit(m_LCDStatus, 5);
		}
		// mode 3
		else if (m_ScanlineCounter >= mode3bounds)
		{
			mode = 3;
			m_LCDStatus = BitSet(m_LCDStatus, 1);
			m_LCDStatus = BitSet(m_LCDStatus, 0);
		}
		// mode 0
		else
		{
			mode = 0;
			m_LCDStatus = BitReset(m_LCDStatus, 1);
			m_LCDStatus = BitReset(m_LCDStatus, 0);
			reqInt = TestBit(m_LCDStatus, 3);
		}
	}

	// just entered a new mode so request interupt
	if (reqInt && (mode != currentmode))
		bInterrupt = true;

	// check the conincidence flag
	if (m_Scanline == m_Coincidence)
	{
		m_LCDStatus = BitSet(m_LCDStatus, 2);
		if (TestBit(m_LCDStatus, 6))
			bInterrupt = true;
	}
	else
	{
		m_LCDStatus = BitReset(m_LCDStatus, 2);
	}

	return bInterrupt;
}

bool GPU::IsLCDEnabled()
{
	// The 7th bit of the control register enables and disables the LCD
	return (m_Control & 0b10000000);
}

/*
	Control register:
		Bit 7 - LCD Display Enable (0=Off, 1=On)
		Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
		Bit 5 - Window Display Enable (0=Off, 1=On)
		Bit 4 - BG & Window Tile Data Select (0=8800-97FF, 1=8000-8FFF)
		Bit 3 - BG Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
		Bit 2 - OBJ (Sprite) Size (0=8x8, 1=8x16)
		Bit 1 - OBJ (Sprite) Display Enable (0=Off, 1=On)
		Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)
*/

// http://www.codeslinger.co.uk/pages/projects/gameboy/graphics.html 1000 times over

void GPU::DrawScanLine()
{
	bool bDrawBackground = (m_Control & 0xb1);
	if (bDrawBackground) RenderTiles();

	bool bDrawSprites = (m_Control & 0b10);
	if (bDrawSprites) RenderSprites();
}


void GPU::RenderTiles()
{
	uint16_t tileData = 0;
	uint16_t backgroundMemory = 0;
	bool bUnsigned = true;
	bool bUsingWindow = false;

	// Check if the window is enabled
	if ((m_Control & 0b100000))
	{
		// Is the current scanline within the window's Y pos?
		if (m_WindowY <= m_Scanline) bUsingWindow = true;
	}

	// Determine which tile data we are using
	if (TestBit(m_Control, 4)) tileData = 0x8000;
	else
	{
		// NOTE: This memory region uses signed bytes as tile identifiers!
		tileData = 0x8800;
		bUnsigned = false;
	}

	// Determine background memory
	if (!bUsingWindow)
	{
		if (TestBit(m_Control, 3)) backgroundMemory = 0x9C00;
		else backgroundMemory = 0x9800;
	}
	else
	{
		if (TestBit(m_Control, 6)) backgroundMemory = 0x9C00;
		else backgroundMemory = 0x9800;
	}

	// The y position is used to calculate which of the 
	// vertical 32 tiles the current scanline is drawing
	uint8_t yPos = 0;
	if (!bUsingWindow) yPos = m_ScrollY + m_Scanline;
	else yPos = m_Scanline - m_WindowY;

	// Which pixel of the tile's 8 vertical ones is the scanline on?
	uint16_t tileRow = (((uint8_t)(yPos / 8)) * 32);

	// Draw the 160 horizontal pixels for this scanline
	for (int pixel = 0; pixel < 160; ++pixel)
	{
		uint8_t xPos = pixel + m_ScrollX;

		// Translate to window space if nessecary
		if (bUsingWindow && pixel >= m_WindowX) xPos = pixel - m_WindowX;

		// Determine which of the 32 horizontal tiles we are drawing
		uint16_t tileCol = (xPos / 8);
		int16_t tileNumber;

		// Get the tile identity number - signed or otherwise
		uint16_t tileAddress = backgroundMemory + tileRow + tileCol;
		if (bUnsigned) tileNumber = (uint8_t)m_Vram[tileAddress - 0x8000];
		else tileNumber = (int8_t)m_Vram[tileAddress - 0x8000];

		// Deduce where the tile identifier is
		uint16_t tileLocation = tileData;

		if (bUnsigned) tileLocation += tileNumber * 16;
		else tileLocation += (tileNumber + 128) * 16;

		// Find the current vertical line
		uint16_t line = yPos % 8;
		line *= 2; // Each vertical line takes 2 bytes in memory
		uint8_t data1 = m_Vram[tileLocation - 0x8000 + line ];
		uint8_t data2 = m_Vram[tileLocation - 0x8000 + line + 1];

		// Pixel 0 is bit 7, pixel 1 is bit 6, etc...
		int colourBit = xPos % 8;
		colourBit -= 7;
		colourBit *= -1;

		// Combine data 1 and 2 to get the colour id for this pixel
		int colourNumber = BitGetVal(data2, colourBit);
		colourNumber <<= 1;
		colourNumber |= BitGetVal(data1, colourBit);

		// Convert colour id to palette colour from 0xFF47
		Colour colour = GetColour(colourNumber, m_BackgroundPalette);
		int red = 0;
		int green = 0;
		int blue = 0;

		// setup the RGB values
		switch (colour)
		{
			case WHITE: red = 255; green = 255; blue = 255; break;
			case LIGHT_GRAY:red = 0xCC; green = 0xCC; blue = 0xCC; break;
			case DARK_GRAY: red = 0x77; green = 0x77; blue = 0x77; break;
		}

		// Safety check to check we are in bounds
		if (m_Scanline < 0 || m_Scanline > 143 || pixel < 0 || pixel > 159) continue;

		// Now we can finally write to the screen!
		m_ScreenData[pixel][m_Scanline][0] = red;
		m_ScreenData[pixel][m_Scanline][1] = green;
		m_ScreenData[pixel][m_Scanline][2] = blue;
	}
}

void GPU::RenderSprites()
{
	bool b8x16 = false;
	if ((m_Control & 0b100)) b8x16 = true;

	for (int sprite = 0; sprite < 40; sprite++)
	{
		// Sprite occupies 4 bytes in the sprite attribute table
		uint8_t index = sprite * 4;
		uint8_t yPos = m_Oam[0xFE00 + index - 0xFE00] - 16;
		uint8_t xPos = m_Oam[0xFE00 + index + 1 - 0xFE00] - 8;
		uint8_t tileLocation = m_Oam[0xFE00 + index + 2 - 0xFE00];
		uint8_t attributes = m_Oam[0xFE00 + index + 3 - 0xFE00];

		bool yFlip = TestBit(attributes, 6);
		bool xFlip = TestBit(attributes, 5);

		int scanline = m_Scanline;

		int ysize = 8;
		if (b8x16)
			ysize = 16;

		// does this sprite intercept with the scanline?
		if ((scanline >= yPos) && (scanline < (yPos + ysize)))
		{
			int line = scanline - yPos;

			// read the sprite in backwards in the y axis
			if (yFlip)
			{
				line -= ysize;
				line *= -1;
			}

			line *= 2; // same as for tiles
			uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
			uint8_t data1 = m_Vram[dataAddress - 0x8000];
			uint8_t data2 = m_Vram[dataAddress - 0x8000 + 1];

			// its easier to read in from right to left as pixel 0 is
			// bit 7 in the colour data, pixel 1 is bit 6 etc...
			for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
			{
				int colourbit = tilePixel;
				// read the sprite in backwards for the x axis
				if (xFlip)
				{
					colourbit -= 7;
					colourbit *= -1;
				}

				// the rest is the same as for tiles
				int colourNum = BitGetVal(data2, colourbit);
				colourNum <<= 1;
				colourNum |= BitGetVal(data1, colourbit);

				uint8_t palette = TestBit(attributes, 4) ? m_SpritePalettes[1] : m_SpritePalettes[0];
				Colour col = GetColour(colourNum, palette);

				// white is transparent for sprites.
				if (col == WHITE)
					continue;

				int red = 0;
				int green = 0;
				int blue = 0;

				switch (col)
				{
					case WHITE: red = 255;green = 255;blue = 255;break;
					case LIGHT_GRAY:red = 0xCC;green = 0xCC;blue = 0xCC;break;
					case DARK_GRAY:red = 0x77;green = 0x77;blue = 0x77;break;
				}

				int xPix = 0 - tilePixel;
				xPix += 7;

				int pixel = xPos + xPix;

				// sanity check
				if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
				{
					continue;
				}

				m_ScreenData[pixel][scanline][0] = red;
				m_ScreenData[pixel][scanline][1] = green;
				m_ScreenData[pixel][scanline][2] = blue;
			}
		}
	}
}

Colour GPU::GetColour(uint8_t colourNumber, uint8_t palette)
{
	Colour res = WHITE;
	int hi = 0;
	int lo = 0;

	// which bits of the colour palette does the colour id map to?
	switch (colourNumber)
	{
		case 0: hi = 1; lo = 0;break;
		case 1: hi = 3; lo = 2;break;
		case 2: hi = 5; lo = 4;break;
		case 3: hi = 7; lo = 6;break;
	}

	// use the palette to get the colour
	int colour = 0;
	colour = BitGetVal(palette, hi) << 1;
	colour |= BitGetVal(palette, lo);

	// convert the game colour to emulator colour
	switch (colour)
	{
		case 0: res = WHITE;break;
		case 1: res = LIGHT_GRAY;break;
		case 2: res = DARK_GRAY;break;
		case 3: res = BLACK;break;
	}

	return res;
}