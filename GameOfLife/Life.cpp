#include "Life.h"

Life::Life(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
}

bool Life::Start() {

	return true;
}

bool Life::Update(float timeElapsed) {
	Draw(30, 20, ' ', 0x70);

	return true;
}

Life::~Life() {

}