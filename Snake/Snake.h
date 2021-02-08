#pragma once
#include <vector>
#include <algorithm>
#include "Engine.h"

enum Direction {
    north,
    south,
    east,
    west
};

class Snake : public Engine
{
public:
    Snake(int, int, int, int);
    bool Start();
    bool Update(float);
    void DisplayGame();
    void HandleInput();
    void MoveSnake();
    void SpawnFood();
    bool IsWithinScreen(Position pos) const;
    ~Snake();
private:
    float timePassedMove;
    float timerMove;
    float timePassedFood;
    float timerFood;
    int scrHeight;
    int scrWidth;
    int bodySize;
    int foodMax;
    bool isGameover;
    Position increment;
    std::vector<Position> body;
    std::vector<Position> food;
    Direction moveDir;
};

