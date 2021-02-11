#pragma once
//#include <vector>
#include <algorithm>
#include "Engine.h"

class Tetris : public Engine
{
public:
    Tetris(int, int, int, int);
    bool Start();
    bool Update(float);
    ~Tetris();
};

