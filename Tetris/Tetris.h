#pragma once
#include <array>
#include <algorithm>
#include "Engine.h"

class Tetris : public Engine
{
public:
    Tetris(int, int, int, int);
    bool Start();
    bool Update(float);
    void DrawShape(int, Position);
    ~Tetris();
private:
    Position figures[7][4] = {
        { Position(0, 0), Position(1, 0), Position(0, 1), Position(1, 1) }, //square boi
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(0, 3) }, //long boi
        { Position(1, 0), Position(2, 0), Position(0, 1), Position(1, 1) }, //s boi
        { Position(0, 0), Position(1, 0), Position(1, 1), Position(2, 1) }, //z boi
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(1, 2) }, //L boi
        { Position(1, 0), Position(1, 1), Position(1, 2), Position(0, 2) }, //J boi
        { Position(0, 0), Position(1, 0), Position(2, 0), Position(1, 1) } //t boi
    };
};

