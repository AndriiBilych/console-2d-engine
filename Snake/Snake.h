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
    void DisplaySnake();
    void HandleInput();
    void MoveSnake();
    ~Snake();
private:
    float timePassed;
    float timer;
    Position increment;
    std::vector<Position> body;
    Direction moveDir;
};

