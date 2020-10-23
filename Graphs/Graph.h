/*First project made with windows console - a few trigonometric functions visualised
Made by Andrii Bilych
*/
#pragma once
#include <cmath>
#include "Engine.h"

struct Vector2
{
public:
	int x, y;
};

class Graphs : public Engine
{
public:
	Graphs();
	~Graphs();

	bool Start();
	bool Update(float);
	void DrawGraph(int, int, int);
	void CalculatePositions();
private:
	const size_t graphCount = 8;
	const int graphWidth = 17;
	const int graphHeight = 17;
	const int space = 3;
	bool direction;
    int stepCounter;
    float stepTimer;
	Vector2* positions;
	std::string names[8] {
		"y = x",
		"y = -x",
		"y = x^2",
		"y = -x^2",
		"y = sin(x)",
		"y = cos(x)",
		"y = atan(x)",
		"y = -atan(x)"
	};
};