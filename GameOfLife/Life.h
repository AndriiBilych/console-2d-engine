#pragma once
//#include <vector>
//#include <algorithm>
#include "Engine.h"

class Life : public Engine
{
public:
    Life(int, int, int, int);
    bool Start();
    bool Update(float);
    ~Life();
};

