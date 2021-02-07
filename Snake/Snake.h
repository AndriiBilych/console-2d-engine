#pragma once
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
    ~Snake();
private:
    Position headPos;
    Direction moveDir;
};

