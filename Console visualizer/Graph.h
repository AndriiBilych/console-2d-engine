#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>
#include <WinUser.h>
#include <thread>

//Initial values 
const int screenWidth = 100;
const int screenHeight = 40;

const int graphWidth = 20;
const int graphHeight = 10;

const int graphCount = 8;

struct Vector2
{
public:
	int x, y;
};

void addToScreenBuffer(int, int, std::string_view, wchar_t*);
void displayGraphs(wchar_t*, HANDLE*, DWORD*);
void initBuffer(wchar_t*);
void addGraphToScreenBuffer(int, int, Vector2*, int*, wchar_t*);
void calculatePositions(Vector2* pos);