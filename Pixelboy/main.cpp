
/*
	Uses a slightly modified version of olcPixelGameEngine.h in order
	to have more control over the window title.
*/

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "tinyfiledialogs.h"

#include <chrono>
#include <bitset>
#include <string>

#include "CPU.h"
#include "Cartridge.h"

class GameboyWindow : public olc::PixelGameEngine
{
private:
	
	CPU m_CPU;
	unsigned int m_nLastTicks;
	
#if _DEBUG	
	bool bDidInstruction = false;
	bool bGoSlow = false;
	uint16_t m_CrashAddress;
#endif

	bool m_bDidSplashScreen;
	float m_SplashScreenMilliseconds;
	const int SPLASH_SCREEN_SECONDS = 3;

public:

	GameboyWindow()
	{
		char const * lFilterPatterns[2] = { "*.gb", "*.rom" };

		// Use "tiny file dialogs" to get a ROM
		char const * selection = tinyfd_openFileDialog(
			"Select ROM",		// Title
			"",					// Initial directory (optional)
			2,					// Numbers of filters
			lFilterPatterns,	// Filters
			NULL,				// Optional filter description
			0					// Only allow one file
		);

		m_CPU.m_Memory.Reset(selection, false);
		m_CPU.m_Memory.m_GPU.Reset(m_CPU.m_Memory.m_Vram, m_CPU.m_Memory.m_Oam);
		m_CPU.Reset();
	}

	bool OnUserCreate() override
	{
		m_nLastTicks = 0;

		// Init joypad
		for (int i = 0; i < 8; ++i) m_CPU.KeyReleased(i);

		// Set window title to be the name of the game
		sAppName = "PixelGameBoy - " + m_CPU.m_Memory.m_Cartridge.GetTitle();

		m_bDidSplashScreen = false;

		return true;
	}

	bool DrawSplashScreen(float fElapsedTime)
	{
		const olc::vf2d scale = olc::vf2d(0.5f, 0.5f);

		const std::string sSplashScreen[3] = { "Made using the olc::PixelGameEngine", "Copyright 2018 - 2020 OneLoneCoder.com" };

		// Draw splash screen
		Clear(olc::BLACK);
		DrawStringDecal(olc::vf2d(ScreenWidth() / 2.0f - GetTextSize(sSplashScreen[0]).x / 4.0f, ScreenHeight() / 2.0f + 00.0f), sSplashScreen[0], olc::YELLOW, scale);
		DrawStringDecal(olc::vf2d(ScreenWidth() / 2.0f - GetTextSize(sSplashScreen[1]).x / 4.0f, ScreenHeight() / 2.0f + 10.0f), sSplashScreen[1], olc::WHITE, scale);

		// Keep track of time
		m_SplashScreenMilliseconds += fElapsedTime;
		if (m_SplashScreenMilliseconds >= SPLASH_SCREEN_SECONDS) m_bDidSplashScreen = true;

		return true;
	}

#ifdef _DEBUG
	void DrawDebugMenu()
	{
		const float x = 165;
		const float y = 5;
		const olc::vf2d scale(0.5f, 0.5f);

		auto hexToString = [](int hex) { return (static_cast<std::stringstream const&>(std::stringstream() << "0x" << std::hex << hex)).str(); };
		auto binaryToString = [](int binary) { return std::bitset<8>(binary).to_string(); };

		auto DrawRegisters = [&](const float x, const float y)
		{
			DrawStringDecal(olc::vf2d(x + 00, y + 0), "AF: " + hexToString(m_CPU.m_RegisterAF.reg), olc::YELLOW, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 5), "BC: " + hexToString(m_CPU.m_RegisterBC.reg), olc::YELLOW, scale);
			DrawStringDecal(olc::vf2d(x + 50, y + 0), "DE: " + hexToString(m_CPU.m_RegisterDE.reg), olc::YELLOW, scale);
			DrawStringDecal(olc::vf2d(x + 50, y + 5), "HL: " + hexToString(m_CPU.m_RegisterHL.reg), olc::YELLOW, scale);

			DrawStringDecal(olc::vf2d(x + 00, y + 15), "Stack pointer:   " + hexToString(m_CPU.m_StackPointer), olc::YELLOW, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 20), "Program counter: " + hexToString(m_CPU.m_ProgramCounter), olc::YELLOW, scale);

			std::string flags = "Flags: ";
			flags += m_CPU.GetFlag(ZERO) ? "1" : "0";		flags += m_CPU.GetFlag(SUBTRACT) ? "1" : "0";
			flags += m_CPU.GetFlag(HALF_CARRY) ? "1" : "0"; flags += m_CPU.GetFlag(CARRY) ? "1" : "0";
			DrawStringDecal(olc::vf2d(x + 00, y + 25), flags, olc::YELLOW, scale);
		};

		auto DrawMemory = [&](const float x, const float y)
		{
			int nInstructions = 0;
			uint16_t nPC = m_CPU.m_ProgramCounter - 5;
			while (nInstructions < 10)
			{
				DrawStringDecal(olc::vf2d(x + 00, y + nInstructions * 5), hexToString(nPC) + "  " + m_CPU.instructions[m_CPU.m_Memory.ReadByte(nPC)].sMnemonic, nInstructions == 5 ? olc::RED : olc::WHITE, scale);
				nPC += m_CPU.instructions[m_CPU.m_Memory.ReadByte(nPC)].length + 1; // Skip to next instruction (length is important)
				nInstructions++;
			}
		};

		auto DrawLCD = [&](const float x, const float y)
		{
			DrawStringDecal(olc::vf2d(x + 00, y + 00), "LCD control:  " + hexToString(m_CPU.m_Memory.m_GPU.m_Control), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 05), "LCD scanline: " + hexToString(m_CPU.m_Memory.m_GPU.m_Scanline), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 10), "LCD scrollX:  " + hexToString(m_CPU.m_Memory.m_GPU.m_ScrollX), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 15), "LCD scrollY:  " + hexToString(m_CPU.m_Memory.m_GPU.m_ScrollY), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 80, y + 10), "windowX:  " + hexToString(m_CPU.m_Memory.m_GPU.m_WindowX), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 80, y + 15), "windowY:  " + hexToString(m_CPU.m_Memory.m_GPU.m_WindowY), olc::GREEN, scale);
		};

		auto DrawInterrupts = [&](const float x, const float y)
		{
			DrawStringDecal(olc::vf2d(x + 00, y + 00), "Interrupts enabled:  " + binaryToString(m_CPU.m_Memory.m_InterruptsEnabled), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 05), "Interrupts flagged:  " + binaryToString(m_CPU.m_Memory.m_InterruptFlags), olc::GREEN, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 10), "Master interrupts :  " + std::string((m_CPU.m_MasterInterupts) ? "True" : "False"), olc::GREEN, scale);
		};

		auto DrawBanking = [&](const float x, const float y)
		{
			DrawStringDecal(olc::vf2d(x + 00, y + 00), "MCB1:  " + std::string((m_CPU.m_Memory.m_Cartridge.m_bMBC1) ? "True" : "False"), olc::BLUE, scale);
			DrawStringDecal(olc::vf2d(x + 60, y + 00), "MCB2:  " + std::string((m_CPU.m_Memory.m_Cartridge.m_bMBC2) ? "True" : "False"), olc::BLUE, scale);
			DrawStringDecal(olc::vf2d(x + 00, y + 05), "ROM bank:  " + std::to_string(m_CPU.m_Memory.m_CurrentROMBank), olc::BLUE, scale);
			DrawStringDecal(olc::vf2d(x + 60, y + 05), "RAM bank:  " + std::to_string(m_CPU.m_Memory.m_CurrentRAMBank), olc::BLUE, scale);
		};

		auto DrawInput = [&](const float x, const float y)
		{
			DrawStringDecal(olc::vf2d(x + 00, y + 00), "Joypad state:  " + binaryToString(m_CPU.m_Memory.m_JoypadState), olc::BLUE, scale);
		};

		DrawRegisters(x, y);
		DrawMemory(x, y + 35);
		DrawLCD(x, y + 90);
		DrawInterrupts(x, y + 115);
		//DrawBanking(x, y + 130);
		DrawInput(x, y + 135);

		if (m_CPU.m_bCrashed) DrawStringDecal(olc::vf2d(10, 10), "CRASHED: " + hexToString(m_CrashAddress), olc::RED, scale);
		if (m_CPU.m_bStopped) DrawStringDecal(olc::vf2d(10, 15), "STOPPED", olc::RED, scale);
		if (m_CPU.m_bHalted) DrawStringDecal(olc::vf2d(10, 20), "HALTED", olc::RED, scale);
	}
#endif

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Draw splash screen, otherwise run emulator
		if (!m_bDidSplashScreen) return DrawSplashScreen(fElapsedTime);

		auto start = std::chrono::system_clock::now();

		// Input
		if (GetKey(olc::A).bPressed) m_CPU.KeyPressed(4);		if (GetKey(olc::A).bReleased) m_CPU.KeyReleased(4);	
		if (GetKey(olc::S).bPressed) m_CPU.KeyPressed(5);		if (GetKey(olc::S).bReleased) m_CPU.KeyReleased(5);
		if (GetKey(olc::ENTER).bPressed) m_CPU.KeyPressed(7);	if (GetKey(olc::RETURN).bReleased) m_CPU.KeyReleased(7);		
		if (GetKey(olc::SPACE).bPressed) m_CPU.KeyPressed(6);	if (GetKey(olc::SPACE).bReleased) m_CPU.KeyReleased(6);
		if (GetKey(olc::RIGHT).bPressed) m_CPU.KeyPressed(0);	if (GetKey(olc::RIGHT).bReleased) m_CPU.KeyReleased(0);
		if (GetKey(olc::LEFT).bPressed) m_CPU.KeyPressed(1);	if (GetKey(olc::LEFT).bReleased) m_CPU.KeyReleased(1);
		if (GetKey(olc::UP).bPressed) m_CPU.KeyPressed(2);		if (GetKey(olc::UP).bReleased) m_CPU.KeyReleased(2);
		if (GetKey(olc::DOWN).bPressed) m_CPU.KeyPressed(3);	if (GetKey(olc::DOWN).bReleased) m_CPU.KeyReleased(3);

		// Work out amount of clock cycles to do, which is MAX_CLOCKS_PER_SECOND
		// divided by the current FPS
		int nFPS = (int)(1.0f / fElapsedTime);
		const int desiredClockCyles = MAX_CLOCKS_PER_SECOND / nFPS;

#if _DEBUG
		// If shift pressed, go line by line
		bDidInstruction = false;
		if (GetKey(olc::SHIFT).bPressed) bGoSlow = !bGoSlow;
#endif

		int cyclesThisUpdate = 0;
		while (cyclesThisUpdate < desiredClockCyles && !m_CPU.m_bCrashed)
		{
#if _DEBUG
			// If space and shift pressed, go line by line
			if (GetKey(olc::SPACE).bPressed && !bDidInstruction) { bDidInstruction = true; }
			else if (bGoSlow) break;
#endif

			// Update CPU
			unsigned int ticks = m_CPU.Update();
#if _DEBUG
			if (m_CPU.m_bCrashed) { m_CrashAddress = ticks; break; }
#else
			if (m_CPU.m_bCrashed) { break; }
#endif
			cyclesThisUpdate += ticks - m_nLastTicks;

			// Update timers
			m_CPU.UpdateTimers(ticks - m_nLastTicks);

			// Update GPU
			if (!m_CPU.m_bStopped)
			{
				InterruptReturns interrupts = m_CPU.m_Memory.m_GPU.Update(ticks - m_nLastTicks); // Update GPU
				if (interrupts.bVblank) m_CPU.RequestInterrupt(VBLANK_FLAG_BIT);
				if (interrupts.bLCD) m_CPU.RequestInterrupt(LCD_FLAG_BIT);
			}	

			// Do interrupts and keep track of timing
			m_CPU.CheckForInterrupts();
			m_nLastTicks = ticks;
		}

		// Render screen
		Clear(olc::BLACK);
		for (int x = 0; x < 160; ++x)
		{
			for (int y = 0; y < 144; ++y)
			{
				uint8_t* pixel = m_CPU.m_Memory.m_GPU.m_ScreenData[x][y];
#if _DEBUG
				Draw(x+1, y+1, olc::Pixel(pixel[0], pixel[1], pixel[2]));
#else
				Draw(x, y, olc::Pixel(pixel[0], pixel[1], pixel[2]));
#endif
			}
		}

		// Draw debug info
#if _DEBUG
		DrawDebugMenu();
#endif

		return true;
	}

	bool OnUserDestroy() override
	{
		return true;
	}

};


int main()
{

	GameboyWindow window;

	// Vsync in release mode
#if _DEBUG
	if (window.Construct(300, 146, 4, 4, false, false))
		window.Start();
#else
	if (window.Construct(160, 144, 4, 4, false, true))
		window.Start();
#endif

	return 0;
}