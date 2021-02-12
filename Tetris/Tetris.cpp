#include "Tetris.h"

Tetris::Tetris(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	currentShape.index = 6;
	currentShape.statesAmount = one;
	
	currentShape.p = Position(1, 1);

	timePassed = .0f;
	timer = 1.0f;
	
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
	/*else if (GetKey(VK_DOWN).pressed) {
		currentShape.index = currentShape.index == 5 ? 6 : 5;
		std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);
	}*/

	if (GetKey(VK_RIGHT).pressed) 
		currentShape.p += Position(1, 0);
	else if (GetKey(VK_LEFT).pressed) 
		currentShape.p -= Position(1, 0);
	
	//Timer for descending
	timePassed += timeElapsed;
	if (timePassed >= timer)
	{
		if (currentShape.p.y < GetScreenHeight() - 1)
			currentShape.p += Position(0, 1);

		timePassed -= timer;
	}

	return true;
}

void Tetris::DrawShape() {
	for (int i = 0; i < 4; i++) {
		Draw(currentShape.p + currentShape.coords[i], ' ', BG_GREY);
	}
}

void Tetris::RandomShape() {

}

int Tetris::NextRotation(int) {

}

Position Tetris::GetLowestPoint(Position rotations[4]) {

}

Tetris::~Tetris() {

}