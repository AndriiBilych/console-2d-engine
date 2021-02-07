#include "Snake.h"

Snake::Snake(int width, int height, int fontWidth, int fontHeight) 
	: Engine(width, height, fontWidth, fontHeight) {
	moveDir = north;
	headPos = Position(10, 10);
}

bool Snake::Start() {

	return true;
}

bool Snake::Update(float timeElapsed) {
	Draw(headPos.x, headPos.y, ' ', 0xC0);

	return true;
}

Snake::~Snake() {

}