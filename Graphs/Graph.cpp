#include "Graph.h"

#define PI 3.14f

Graphs::Graphs(int width, int height, int fontWidth, int fontHeight)
    : Engine(width, height, fontWidth, fontHeight)
{
    positions.reserve(graphCount * graphWidth);
    direction = true;
    stepCounter = 0;
    stepTimer = .0f;
}

bool Graphs::Start()
{
    CalculatePositions();
    return true;
}

bool Graphs::Update(float deltaTime)
{
    stepTimer += deltaTime;

    //Draw first layer
    for (int i = 0; i < graphCount / 2; i++)
        DrawGraph(i * (graphWidth + space), 1, i);

    //Draw second layer
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
            //if the current buffer coordinate equals corresponding graph coordinate put '.' character,
            //if the current buffer coordinate equals caret(step) coordinate put '@' character,
            //else leave blank
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
            positions.emplace_back(Vector2(i, 0));
    
    auto begin = positions.begin();

    //Character buffer draws character from top left corner to bottom right corner, thus to match the functions
    //coordinates have to be flipped
    std::for_each(begin, begin + graphWidth, 
        [](Vector2 &pos) { pos.y = pos.x; });
    std::for_each(begin + graphWidth, begin + graphWidth * 2, 
        [=](Vector2 &pos) { pos.y = graphHeight - 1 - pos.x; });
    std::for_each(begin + graphWidth * 2, begin + graphWidth * 3, 
        [=](Vector2 &pos) { pos.y = (int)pow(graphHeight / 2 - pos.x, 2) / (graphHeight / 4); });
    std::for_each(begin + graphWidth * 3, begin + graphWidth * 4, 
        [=](Vector2& pos) { pos.y = graphHeight - 1 - (int)pow(graphHeight / 2 - pos.x, 2) / (graphHeight / 4); });

    std::for_each(begin + graphWidth * 4, begin + graphWidth * 5, 
        [=](Vector2 &pos) { pos.y = (int)round((graphHeight / 2 - 1) * sin(pos.x / 2.0f - PI / 2.0f) + graphHeight / 2); });
    std::for_each(begin + graphWidth * 5, begin + graphWidth * 6, 
        [=](Vector2 &pos) { pos.y = (int)round(graphHeight / 2 - (graphHeight / 2 - 1) * cos(pos.x / 2.0f - PI)); });
    std::for_each(begin + graphWidth * 6, begin + graphWidth * 7, 
        [=](Vector2 &pos) { pos.y = (int)(2 * PI * atan(pos.x)); });
    std::for_each(begin + graphWidth * 7, begin + graphWidth * 8, 
        [=](Vector2 &pos) { pos.y = graphHeight - 1 - (int)(2 * (PI * atan(pos.x))); });
}