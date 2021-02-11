#include "Tetris.h"

Tetris::Tetris(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	ChangeTitle(L"Tetris");
}

bool Tetris::Start() {

	return true;
}

bool Tetris::Update(float timeElapsed) {
	DrawShape(0, Position(1, 1));
	DrawShape(1, Position(5, 1));
	DrawShape(2, Position(8, 1));
	DrawShape(3, Position(1, 4));
	DrawShape(4, Position(1, 8));
	DrawShape(5, Position(1, 12));
	DrawShape(6, Position(1, 16));

	return true;
}

void Tetris::DrawShape(int index, Position p) {
	for (int i = 0; i < 4; i++) {
		Draw(p + figures[index][i], ' ', BG_GREY);
	}
}

Tetris::~Tetris() {

}