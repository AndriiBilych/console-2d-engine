/*Graphs program features 8 trogonometric graphs with 8 moving carets.
* The names are specified in a string array "names". CalculatePositions() handles
* the actual coordinates calculations.
*/
#pragma once
#include <cmath>
#include <vector>
#include <algorithm>
#include "Engine.h"

struct Vector2
{
public:
	Vector2(int x, int y) { this->x = x; this->y = y; }
	int x, y;
};

class Graphs : public Engine
{
public:
	Graphs(int, int, int, int);

	bool Start();
	bool Update(float);
	void DrawGraph(int, int, int);
	void CalculatePositions();
private:
	const int graphCount = 8;
	const int graphWidth = 17;
	const int graphHeight = 17;
	const int space = 3;
	bool direction;
    int stepCounter;
    float stepTimer;
	std::vector<Vector2> positions;
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