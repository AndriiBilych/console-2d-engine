#include "Tetris.h"

Tetris::Tetris(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	srand(time(NULL));
	NewShape();

	isSpedUp = false;
	isGameover = false;

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

	if (!isGameover) {
		isGameover = IsLost();

		DestroyRowAndUpdate();

		if (GetKey(VK_UP).pressed) {
			NextRotation();
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

		if (GetKey(VK_RIGHT).pressed && !IsCollidingRight()) 
			currentShape.p += Position(1, 0);
		else if (GetKey(VK_LEFT).pressed && !IsCollidingLeft()) 
			currentShape.p -= Position(1, 0);
	
		//debugging
		if (GetKey(VK_SPACE).pressed)
			isPaused = !isPaused;

		//Timer for descending
		timePassed += isPaused ? 0 : timeElapsed * speed;
		if (timePassed >= timer)
		{
			auto lowestPoint = currentShape.p + currentShape.coords[3];
			if (lowestPoint.y == GetScreenHeight() - 1 || IsCollidingDown()) {
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
	}
	else {
		ChangeTitle(L"You Lost!");
	}

	return true;
}

bool Tetris::IsLost() {
	return std::any_of(begin(squares), end(squares), [](Square s) { return s.p.y == -1; });
}

bool Tetris::IsCollidingDown() {
	for (auto coord : currentShape.coords) {
		auto calculatedPos = currentShape.p + coord + Position(0, 1);
		if (IsWithinCoords(calculatedPos) 
			&& ((IsWithinSquares(calculatedPos) || calculatedPos.y == GetScreenHeight()))) {
			return true;
		}
	}
	return false;
}

bool Tetris::IsCollidingLeft() {
	for (auto coord : currentShape.coords) {
		auto calculatedPos = currentShape.p + coord + Position(-1, 0);
		if (IsWithinCoords(calculatedPos) 
			&& ((IsWithinSquares(calculatedPos) || calculatedPos.x < 0))) {
			return true;
		}
	}
	return false;
}

bool Tetris::IsCollidingRight() {
	for (auto coord : currentShape.coords) {
		auto calculatedPos = currentShape.p + coord + Position(1, 0);
		if (IsWithinCoords(calculatedPos) 
			&& ((IsWithinSquares(calculatedPos) || calculatedPos.x == GetScreenWidth()))) {
			return true;
		}
	}
	return false;
}

bool Tetris::IsWithinCoords(Position calculatedPos) {
	return std::none_of(currentShape.coords, &currentShape.coords[4], [=](Position p) { return calculatedPos == currentShape.p + p; });
}

bool Tetris::IsWithinSquares(Position calculatedPos) {
	return std::any_of(begin(squares), end(squares), [=](Square s) { return calculatedPos == s.p; });;
}

void Tetris::DestroyRowAndUpdate() {
	auto row = GetFullRow();
	if (row != -1) {
		//Erase row
		std::vector<Square>::iterator it = end(squares);
		do {
			if (it != end(squares))
				squares.erase(it);
			it = std::find_if(begin(squares), end(squares), [=](Square s) { return s.p.y == row; });
		} while (it != end(squares));

		//Update squares
		for (int i = 0; i < squares.size(); i++)
		{
			if (squares[i].p.y < row)
				squares[i].p.y++;
		}
	}
}

int Tetris::GetFullRow() {
	for (int i = 0; i < GetScreenHeight(); i++)
	{
		auto b = std::count_if(begin(squares), end(squares), [=](Square s) { return s.p.y == i; });
		if ( b == GetScreenWidth())
			return i;
	}
	return -1;
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
	currentShape.p = Position(5, -1);
	std::copy(&(figures[indices[i]][0]), &(figures[indices[i]][4]), currentShape.coords);
}

void Tetris::NextRotation() {
	if (currentShape.statesAmount == two)
		currentShape.index += std::find(indices, &indices[7], currentShape.index) == &indices[7] ? -1 : 1;
	else if (currentShape.statesAmount == four) {
		currentShape.index++;
		if (std::find(indices, &indices[7], currentShape.index) != &indices[7] || currentShape.index > 18)
			currentShape.index -= 4;
	}
	std::copy(&(figures[currentShape.index][0]), &(figures[currentShape.index][4]), currentShape.coords);
}

Tetris::~Tetris() {

}