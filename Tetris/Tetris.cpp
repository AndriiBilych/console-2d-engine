#include "Tetris.h"

Tetris::Tetris(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	currentShape.index = 8;
	currentShape.statesAmount = one;
	currentShape.p = Position(1, 1);
	currentShape.color = BG_GREEN;
	srand(time(NULL));

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

	DrawSquares();

	DrawShape();

	if (GetKey(VK_UP).pressed) {
		//currentShape.index = currentShape.index == 5 ? 6 : 5;
		//std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);
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
		if (lowestPoint.y == GetScreenHeight() - 1 || IsColliding()) {
			for (auto coords : currentShape.coords) {
				squares.emplace_back(Square(currentShape.p + coords, currentShape.color));
			}
			NewShape();
		}
		else {
			currentShape.p += Position(0, 1);
		}

		timePassed -= timer;
	}

	return true;
}

bool Tetris::IsColliding() {
	auto lowestPoint = currentShape.p + currentShape.coords[3];
	bool result = false;
	for (auto coord :currentShape.coords) {
		if (currentShape.p.y + coord.y == lowestPoint.y 
			&& !(GetChar(currentShape.p.x + coord.x, currentShape.p.y + coord.y + 1).Attributes == 0)) {
			result = true;
		}
	}
	return result;
}

void Tetris::DrawShape() {
	for (int i = 0; i < 4; i++) {
		Draw(currentShape.p + currentShape.coords[i], ' ', currentShape.color);
	}
}

void Tetris::DrawSquares() {
	for (auto s : squares) {
		Draw(s.p, ' ', s.color);
	}
}

void Tetris::NewShape() {
	auto i = rand() % 7;
	currentShape.index = indices[i];
	currentShape.statesAmount = (i == 0 ? one : ((i == 1 || i == 2 || i == 3) ? two : four));
	currentShape.color = colors[i];
	currentShape.p = Position(4, 0);
	std::copy(&(figures[indices[i]][0]), &(figures[indices[i]][4]), currentShape.coords);
}

int Tetris::NextRotation(int) {

	return 0;
}

Tetris::~Tetris() {

}