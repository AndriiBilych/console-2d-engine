/*Description: Basic framework for drawing 2d graphics inside Windows console,
* class Engine is built for inheriting by other class and overwriting Start(), Update(float) and a constructor.
* These 2 functions should return true to run the application. Start() is called at the beginning of a program,
* Update(float) is called every time Engine draws character buffer to console, basically every frame.
* After inheriting and creating an object of the inherited class the program can be started by calling StartExecution()
* on created object.
* Features: unicode character support, character color change, console window resizing,
* drawing pixel-like characters, timer class for benchmarking(can be used to determine, how fast a certain scope is executed
* by adding Timer object to a scope and passing holder variable to a Timer constructor).
* 
* Made by Andrii Bilych
*/
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <chrono>
#include <string_view>

class Timer
{
public:
	Timer(float* num)
	{
		begin = std::chrono::high_resolution_clock::now();
		ref = num;
	}
	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = end - begin;

		*ref = duration.count();
	}

protected:
	std::chrono::high_resolution_clock::time_point begin, end;
	float* ref;
};

class Engine
{
public:
	Engine(int width, int height, int fontWidth, int fontHeight)
	{
		m_deltaTime = .0f; //Time between screen refreshes
		m_isRunning = true;
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		//Initialise variables for screen buffer
		m_screenWidth = width;
		m_screenHeight = height;

		//Initialise variables for font
		m_cfi.cbSize = sizeof(m_cfi);
		m_cfi.nFont = 0;
		m_cfi.dwFontSize.X = fontWidth;
		m_cfi.dwFontSize.Y = fontHeight;
		m_cfi.FontFamily = FF_DONTCARE;
		m_cfi.FontWeight = FW_NORMAL;
		wcscpy_s(m_cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(m_hConsole, true, &m_cfi);

		// Set Physical Console Window Size
		m_rect = { 0, 0, (short)m_screenWidth - 1, (short)m_screenHeight - 1 };
		SetConsoleWindowInfo(m_hConsole, TRUE, &m_rect);

		//Allocate and assign memory for the screen Buffer
		m_screen = new CHAR_INFO[m_screenWidth * m_screenHeight + 1];
		std::memset(m_screen, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);

		m_bufferSize = { (short)m_screenWidth, (short)m_screenHeight};
		m_bufferPosition = { 0, 0 };

		//Assign memory to key states
		std::memset(m_keyNewState, 0, 256 * sizeof(short));
		std::memset(m_keyOldState, 0, 256 * sizeof(short));
		std::memset(m_keys, 0, 256 * sizeof(keyState));
	}

	void StartExecution()
	{
		ClearScreen();
		Start();
		
		m_isRunning = true;
		std::thread engineThread = std::thread(&Engine::EngineLoop, this);

		engineThread.join();
	}

	void ClearScreen()
	{
		for (int i = 0; i < m_screenWidth * m_screenHeight; i++)
			m_screen[i].Char.UnicodeChar = ' ';
	}

	void Draw(int x, int y, short c = 0x2588, short color = 0x000F)
	{
		if (x >= 0 && x < m_screenWidth && y >= 0 && y < m_screenHeight)
		{
			m_screen[y * m_screenWidth + x].Char.UnicodeChar = c;
			m_screen[y * m_screenWidth + x].Attributes = color;
		}
	}

	void DrawTextToBuffer(int x, int y, std::string_view str)
	{
		for (int i = 0; i < str.length(); i++)
			Draw(x + i, y, str[i]);
	}

protected:
	void EngineLoop()
	{
		while (m_isRunning)
		{
			Timer timer(&m_deltaTime);

			if (!Update(m_deltaTime))
				m_isRunning = false;

			// Handle Keyboard Input
			for (int i = 0; i < 256; i++)
			{
				m_keyNewState[i] = GetAsyncKeyState(i);

				m_keys[i].pressed = false;
				m_keys[i].released = false;

				if (m_keyNewState[i] != m_keyOldState[i])
				{
					if (m_keyNewState[i] & 0x8000)
					{
						m_keys[i].pressed = !m_keys[i].held;
						m_keys[i].held = true;
					}
					else
					{
						m_keys[i].released = true;
						m_keys[i].held = false;
					}
				}

				m_keyOldState[i] = m_keyNewState[i];
			}

			//Add stats
			//swprintf(&m_screen[(m_screenHeight - 1) * m_screenWidth], 50, L"deltaTime: %.5fs FPS: %5.0f", deltaTime, 1.0f / deltaTime);

			WriteConsoleOutput(m_hConsole, m_screen, { (short)m_screenWidth, (short)m_screenHeight }, { 0,0 }, &m_rect);
		}
	}

	~Engine()
	{
		delete[] m_screen;
		m_screen = nullptr;
	}

public:
	virtual bool Start() = 0;
	virtual bool Update(float) = 0;
private:
	int m_screenWidth;
	int m_screenHeight;
	CHAR_INFO* m_screen;
	HANDLE m_hConsole;
	SMALL_RECT m_rect;
	CONSOLE_FONT_INFOEX m_cfi;
	COORD m_bufferSize;
	COORD m_bufferPosition;
	bool m_isRunning;
	float m_deltaTime;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	struct keyState
	{
		bool pressed;
		bool released;
		bool held;
	} m_keys[256];
public:
	int GetScreenWidth() { return m_screenWidth; }
	int GetScreenHeight() { return m_screenHeight; }
	keyState GetKey(unsigned short n) { 
		if (n < 256)
			return m_keys[n];
		else
			throw "Accessing illegal keyState";
	}
};