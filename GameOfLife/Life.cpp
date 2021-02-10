#include "Life.h"

Life::Life(int width, int height, int fontWidth, int fontHeight)
	: Engine(width, height, fontWidth, fontHeight) {
	screenWidth = width;
	screenHeight = height;
	statesAmount = width * height;

	isPaused = true;

	timePassed = .0f;
	timer = .1f;

	states = new bool[statesAmount];
	nextGeneration = new bool[statesAmount];

	std::memset(states, 0, sizeof(bool) * statesAmount);
	std::memset(nextGeneration, 0, sizeof(bool) * statesAmount);

	ChangeTitle(L"Conway's game of life (paused) - space to run/pause");
}

bool Life::Start() {

	return true;
}

bool Life::Update(float timeElapsed) {
	//Draw states
	for (size_t y = 0; y < screenHeight; y++)
		for (size_t x = 0; x < screenWidth; x++)
			Draw(x, y, ' ', states[y * screenWidth + x] ? 0x70 : 0x00);
	
	//Change title
	if (isPaused) 
		ChangeTitle(L"Conway's game of life (paused) - space to run/pause");
	else
		ChangeTitle(L"Conway's game of life (running) - space to run/pause");

	//Pause simulation
	if (GetKey(VK_SPACE).pressed)
		isPaused = !isPaused;

	//Change state of the pressed cell
	if (GetMouse(0).pressed && isPaused) {
		auto xPos = GetMouseX();
		auto yPos = GetMouseY();

		states[yPos * screenWidth + xPos] = !states[yPos * screenWidth + xPos];
		nextGeneration[yPos * screenWidth + xPos] = !nextGeneration[yPos * screenWidth + xPos];
	}

	//Calculate next generation
	if (!isPaused) {
		timePassed += timeElapsed;
		if (timePassed >= timer) {

			CalculateNextState();
			
			timePassed -= timer;
		}
	}

	return true;
}

void Life::CalculateNextState() {
	for (size_t y = 0; y < screenHeight; y++) {
		for (size_t x = 0; x < screenWidth; x++) {
			auto amount = GetLiveNeighboursAmount(x, y);
			//Any dead cell with three live neighbours becomes a live cell
			if (!states[y * screenWidth + x]) {
				if (amount == 3)
					nextGeneration[y * screenWidth + x] = true;
			}
			//Any live cell with two or three live neighbours survives. All other cells die
			else {
				if (amount == 3 || amount == 2)
					nextGeneration[y * screenWidth + x] = true;
				else
					nextGeneration[y * screenWidth + x] = false;
			}

		}
	}
	std::copy(nextGeneration, &nextGeneration[statesAmount], states);
}

int Life::GetLiveNeighboursAmount(int x, int y) {
	int result = 0;
	std::for_each(offsets, &offsets[8], [&](Position p) {
		if (IsWithinScreen(x + p.x, y + p.y) && states[(y + p.y) * screenWidth + (x + p.x)]) result++; });
	return result;
}

bool Life::IsWithinScreen(int x, int y) {
	return x >= 0 && x < screenWidth && y >= 0 && y < screenHeight;
}

Life::~Life() {

}