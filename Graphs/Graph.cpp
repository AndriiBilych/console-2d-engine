#include "Graph.h"

#define PI 3.14f

Graphs::Graphs()
{
    positions = new Vector2[graphCount * graphWidth];
    direction = true;
    stepCounter = 0;
    stepTimer = .0f;
}

Graphs::~Graphs()
{
    delete[] positions;
    positions = nullptr;
}

bool Graphs::Start()
{
    CalculatePositions();
    return true;
}

bool Graphs::Update(float deltaTime)
{
    stepTimer += deltaTime;

    for (int i = 0; i < graphCount / 2; i++)
        DrawGraph(i * (graphWidth + space), 1, i);

    for (int i = 0; i < graphCount / 2; i++)
        DrawGraph(i * (graphWidth + space), (graphHeight + space), i + graphCount / 2);

    //Exit by pressing Escape
    if (GetKey(VK_ESCAPE).pressed)
        return false;

    //.1s timer for moving the '@' symbol
    if (stepTimer >= .1f)
    {
        stepTimer -= .1f;
        direction ? stepCounter++ : stepCounter--;
        if (stepCounter == 0 || stepCounter == graphWidth - 1)
            direction = direction ? false : true;
    }

	return true;
}

void Graphs::DrawGraph(int xOffset, int yOffset, int index) 
{
    DrawTextToBuffer(xOffset, yOffset - 1, names[index]);
    for (int y = 0; y < graphHeight; y++)
        for (int x = 0; x < graphWidth; x++)
        {
            auto c = 
                (x == positions[index * graphWidth + x].x && y == positions[index * graphWidth + x].y) ? 
                (x == stepCounter) ? '@' : '.' : ' ';
            auto flippedY = graphHeight - 1 - y;
            Draw(x + xOffset, flippedY + yOffset, c);
        }
}

void Graphs::CalculatePositions()
{
    for (int j = 0; j < graphCount; j++)
        for (int i = 0; i < graphWidth; i++)
            positions[j * graphWidth + i].x = i;
    
    int i = 0;
    for (; i < graphWidth; i++)
        positions[i].y = positions[i].x;

    for (; i < graphWidth * 2; i++)
        positions[i].y = graphHeight - 1 - positions[i].x;

    for (; i < graphWidth * 3; i++)
        positions[i].y = (int)pow(graphHeight / 2 - positions[i].x, 2) / (graphHeight / 4);

    for (; i < graphWidth * 4; i++)
        positions[i].y = graphHeight - 1 - (int)pow(graphHeight / 2 - positions[i].x, 2) / (graphHeight / 4);

    //Derived variant since normal sine function ranges from 1 to -1 in height and PI intervals in length
    for (; i < graphWidth * 5; i++)
        positions[i].y = round((graphHeight / 2 - 1) * sin(positions[i].x / 2.0f - PI / 2.0f) + graphHeight / 2); 

    for (; i < graphWidth * 6; i++)
        positions[i].y = round(graphHeight / 2 - (graphHeight / 2 - 1) * cos(positions[i].x / 2.0f - PI));

    for (; i < graphWidth * 7; i++)
        positions[i].y = (int)(2 * PI * atan(positions[i].x));

    for (; i < graphWidth * 8; i++)
        positions[i].y = graphHeight - 1 - (int)(2 * (PI * atan(positions[i].x)));
}