/*Very small and basic framework for building graphical applications inside Windows console
Made by Andrii Bilych
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

private:
	std::chrono::high_resolution_clock::time_point begin, end;
	float* ref;
};

class Engine
{
public:
	void ConstructConsole(int width, int height, int fontWidth, int fontHeight)
	{
		//Initialise variables for screen buffer
		m_screenWidth = width;
		m_screenHeight = height;
		m_screen = new wchar_t[m_screenWidth * m_screenHeight + 1];
		m_hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(m_hConsole);
		m_dwBytesWritten = 0;
		std::memset(m_keyNewState, 0, 256 * sizeof(short));
		std::memset(m_keyOldState, 0, 256 * sizeof(short));
		std::memset(m_keys, 0, 256 * sizeof(sKeyState));

		//Initialise variables for font
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontWidth;
		cfi.dwFontSize.Y = fontHeight;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(m_hConsole, true, &cfi);

		m_isRunning = true;
		deltaTime = .0f; //Time between screen refreshes

		ClearScreen();

		m_screen[m_screenWidth * m_screenHeight] = '\0';
	}

	void ClearScreen()
	{
		for (int i = 0; i < m_screenWidth * m_screenHeight; i++)
			m_screen[i] = ' ';
	}

	void StartExecution()
	{
		Start();
		
		m_isRunning = true;
		std::thread engineThread = std::thread(&Engine::EngineLoop, this);

		engineThread.join();
	}

	void EngineLoop()
	{
		while (m_isRunning)
		{
			Timer timer(&deltaTime);

			if (!Update(deltaTime))
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
			swprintf(&m_screen[(m_screenHeight - 1) * m_screenWidth], 50, L"deltaTime: %.5fs FPS: %5.0f", deltaTime, 1.0f / deltaTime);

			WriteConsoleOutputCharacter(m_hConsole, m_screen, m_screenWidth * m_screenHeight, { 0,0 }, &m_dwBytesWritten);
		}
	}

	void Draw(int x, int y, wchar_t c)
	{
		m_screen[y * m_screenWidth + x] = c;
	}

	void DrawTextToBuffer(int x, int y, std::string_view str)
	{
		for (int i = 0; i < str.length(); i++)
			m_screen[y * m_screenWidth + x + i] = str[i];
	}

	~Engine()
	{
		delete[] m_screen;
		m_screen = nullptr;
	}


	virtual bool Start() = 0;
	virtual bool Update(float) = 0;
private:
	size_t m_screenWidth;
	size_t m_screenHeight;
	wchar_t* m_screen;
	HANDLE m_hConsole;
	DWORD m_dwBytesWritten;
	CONSOLE_FONT_INFOEX cfi;
	bool m_isRunning;
	float deltaTime;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	struct sKeyState
	{
		bool pressed;
		bool released;
		bool held;
	} m_keys[256];
public:
	sKeyState GetKey(int n) { return m_keys[n]; }
};