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

class Position {
public:
	Position(int x, int y) : x(x), y(y) {}
	Position operator+ (const Position& other) const { return Position(x + other.x, y + other.y); }
	bool operator== (const Position& other) const { return x == other.x && y == other.y; }
	bool operator!= (const Position& other) const { return x != other.x || y != other.y; }

	int x, y;
};

class Command {
public:
	virtual ~Command() {}
	virtual void execute() = 0;
	virtual void undo() = 0;
};

class CommandHistory {
public:
	void AddCommand(Command* cmd) {
		cmd->execute();
		m_commands[iterator] = cmd;
		iterator++;
		size++;
	}
	void RemoveLast() {
		UndoLast();
		size--;
	}
	void UndoLast() {
		if (iterator > 0)
			m_commands[--iterator]->undo();
	}
	void RedoLast() {
		if (iterator < size)
			m_commands[iterator++]->execute();
	}

private:
	int iterator = 0;
	int size = 0;
	Command* m_commands[200];
};

class Engine
{
public:
	Engine(int width, int height, int fontWidth, int fontHeight)
	{
		m_deltaTime = .0f; //Time between screen refreshes
		m_isRunning = true;
		m_bConsoleInFocus = true;
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		//Initialise variables for screen buffer
		m_screenWidth = width;
		m_screenHeight = height;

		m_mousePosX = 0;
		m_mousePosY = 0;

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

		// Set flags to allow mouse input		
		SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
			
		//Allocate and assign memory for the screen Buffer
		m_screen = new CHAR_INFO[m_screenWidth * m_screenHeight + 1];
		std::memset(m_screen, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);
		swprintf_s(m_titleString, 256, L"Default");

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

	void ChangeTitle(const wchar_t* s)
	{
		swprintf_s(m_titleString, 256, s);
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

			//Handle Keyboard Input
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

			//Handle Mouse Input - Check for window events
			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
			if (events > 0)
				ReadConsoleInput(m_hConsoleIn, inBuf, events, &events);

			//Handle events
			for (DWORD i = 0; i < events; i++)
			{
				switch (inBuf[i].EventType)
				{
				case FOCUS_EVENT:
				{
					m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
				}
				break;

				case MOUSE_EVENT:
				{
					switch (inBuf[i].Event.MouseEvent.dwEventFlags)
					{
					case MOUSE_MOVED:
					{
						m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}
					break;

					case 0:
					{
						for (int m = 0; m < 5; m++)
							m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

					}
					break;

					default:
						break;
					}
				}
				break;

				default:
					break;
					// We don't care just at the moment
				}
			}

			//Handle mouse states
			for (int m = 0; m < 5; m++)
			{
				m_mouse[m].pressed = false;
				m_mouse[m].released = false;

				if (m_mouseNewState[m] != m_mouseOldState[m])
				{
					if (m_mouseNewState[m])
					{
						m_mouse[m].pressed = true;
						m_mouse[m].held = true;
					}
					else
					{
						m_mouse[m].released = true;
						m_mouse[m].held = false;
					}
				}

				m_mouseOldState[m] = m_mouseNewState[m];
			}

			//Update window title
			SetConsoleTitle(m_titleString);

			//Update console
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
	wchar_t m_titleString[256];
	CHAR_INFO* m_screen;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rect;
	CONSOLE_FONT_INFOEX m_cfi;
	COORD m_bufferSize;
	COORD m_bufferPosition;
	bool m_isRunning;
	bool m_bConsoleInFocus;
	float m_deltaTime;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	struct keyState
	{
		bool pressed;
		bool released;
		bool held;
	} m_keys[256], m_mouse[5];
	int m_mousePosX;
	int m_mousePosY;
public:
	int GetScreenWidth() { return m_screenWidth; }
	int GetScreenHeight() { return m_screenHeight; }
	CHAR_INFO GetChar(int x, int y) { return m_screen[y * m_screenWidth + x]; }
	keyState GetKey(unsigned short n) { 
		if (n < 256)
			return m_keys[n];
		else
			throw "Accessing illegal keyState";
	}
	int GetMouseX() { return m_mousePosX; }
	int GetMouseY() { return m_mousePosY; }
	keyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
};