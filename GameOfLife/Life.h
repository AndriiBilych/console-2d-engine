#pragma once
//#include <vector>
#include <algorithm>
#include "Engine.h"

class Life : public Engine
{
public:
    Life(int, int, int, int);
    bool Start();
    bool Update(float);
    int GetLiveNeighboursAmount(int, int);
    void CalculateNextState();
    bool IsWithinScreen(int, int);
    ~Life();
private:
    int statesAmount;
    bool* states;
    bool* nextGeneration;
    bool isPaused;
    float timePassed;
    float timer;
    int screenWidth;
    int screenHeight;
    Position offsets[8] = {
        { -1, -1 },
        { 0, -1 },
        { 1, -1 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { -1, 1 },
        { -1, 0 }
    };
};

