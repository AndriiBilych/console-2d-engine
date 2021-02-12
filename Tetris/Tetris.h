#pragma once
#include <array>
#include <algorithm>
#include "Engine.h"

enum StatesAmount {
    one,
    two,
    four
};

struct Shape {
public:
    Position coords[4];
    int index;
    StatesAmount statesAmount;
    Position p;
};

class Tetris : public Engine
{
public:
    Tetris(int, int, int, int);
    bool Start();
    bool Update(float);
    void DrawShape();
    void RandomShape();
    int NextRotation(int);
    Position GetLowestPoint(Position[4]);
    ~Tetris();
private:
    float timer;
    float timePassed;
    Shape currentShape;
    Position figures[19][4] = {
        { Position(0, 0), Position(1, 0), Position(0, 1), Position(1, 1) }, //0 square boi

        { Position(0, 0), Position(0, 1), Position(0, 2), Position(0, 3) }, //1 long boi - vertical
        { Position(0, 2), Position(1, 2), Position(2, 2), Position(3, 2) }, //2 long boi - horizontal

        { Position(0, 0), Position(0, 1), Position(1, 1), Position(1, 2) }, //3 s boi - vertical
        { Position(1, 0), Position(2, 0), Position(0, 1), Position(1, 1) }, //4 s boi - horizontal

        { Position(1, 0), Position(1, 1), Position(0, 1), Position(0, 2) }, //5 z boi - vertical
        { Position(0, 0), Position(1, 0), Position(1, 1), Position(2, 1) }, //6 z boi - horizontal

        //-----------------------------------------------------------------------------------
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(1, 2) }, //7 L boi - north
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(1, 2) }, //8 L boi - east
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(1, 2) }, //9 L boi - south
        { Position(0, 0), Position(0, 1), Position(0, 2), Position(1, 2) }, //10 L boi - west

        { Position(1, 0), Position(1, 1), Position(1, 2), Position(0, 2) }, //11 J boi - north
        { Position(1, 0), Position(1, 1), Position(1, 2), Position(0, 2) }, //12 J boi - east
        { Position(1, 0), Position(1, 1), Position(1, 2), Position(0, 2) }, //13 J boi - south
        { Position(1, 0), Position(1, 1), Position(1, 2), Position(0, 2) }, //14 J boi - west

        { Position(0, 0), Position(1, 0), Position(2, 0), Position(1, 1) }, //15 t boi - north
        { Position(0, 0), Position(1, 0), Position(2, 0), Position(1, 1) }, //16 t boi - east
        { Position(0, 0), Position(1, 0), Position(2, 0), Position(1, 1) }, //17 t boi - south
        { Position(0, 0), Position(1, 0), Position(2, 0), Position(1, 1) } //18 t boi - west
    };
};

