#include "Tetris.h"

Tetris::Tetris(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	currentShape.index = 8;
	currentShape.statesAmount = one;
	currentShape.p = Position(1, 1);
	currentShape.color = BG_GREEN;

	isSpedUp = false;

	timePassed = .0f;
	timer = 1.0f;
	speed = 1.0f;
	
	ChangeTitle(L"Tetris");
}

bool Tetris::Start() {
	std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);

	return true;
}

bool Tetris::Update(float timeElapsed) {

	ClearScreen();

	DrawShape();

	if (GetKey(VK_UP).pressed) {
		currentShape.index = currentShape.index == 5 ? 6 : 5;
		std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);
	}
	//faster descend
	else if (GetKey(VK_DOWN).held && !isSpedUp) {
		speed *= 35;
		isSpedUp = true;
	}
	else if (GetKey(VK_DOWN).released && isSpedUp) {
		speed /= 35;
		isSpedUp = false;
	}

	if (GetKey(VK_RIGHT).pressed) 
		currentShape.p += Position(1, 0);
	else if (GetKey(VK_LEFT).pressed) 
		currentShape.p -= Position(1, 0);
	
	//Timer for descending
	timePassed += timeElapsed * speed;
	if (timePassed >= timer)
	{
		auto lowestPoint = currentShape.p + currentShape.coords[3];
		if (lowestPoint.y < GetScreenHeight() - 1)
			currentShape.p += Position(0, 1);
		else {
			for (auto p : currentShape.coords) {
				squares.emplace_back(Square(p, currentShape.color));
			}
			NewShape();
		}

		timePassed -= timer;
	}

	return true;
}

bool Tetris::IsColliding(Square s) {

	return true;
}

void Tetris::DrawShape() {
	for (int i = 0; i < 4; i++) {
		Draw(currentShape.p + currentShape.coords[i], ' ', currentShape.color);
	}
}


void Tetris::NewShape() {
	currentShape.index = 0;
	currentShape.statesAmount = one;
	currentShape.color = BG_RED;
	currentShape.p = Position(0, 0);
	std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);
}

int Tetris::NextRotation(int) {

	return 0;
}

Tetris::~Tetris() {

}