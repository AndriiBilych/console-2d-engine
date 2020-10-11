#include "Graph.h"
#include "Timer.h"

#include <cmath>

#define PI 3.14f

void displayGraphs(wchar_t* screen, HANDLE* hConsole, DWORD* dwBytesWritten) {
    Vector2 positions[graphCount * graphWidth];
    std::string names[graphCount];

    bool direction = true;
    int iterator = 0;

    float deltaTime = 0;
    float secTimer = 0;
    float iteratorTimer = 0;
    int refreshes = 0;
    int rps = 0;

    names[0] = "y = x";
    names[1] = "y = -x";
    names[2] = "y = x^2";
    names[3] = "y = -x^2";
    names[4] = "y = sin(x)";
    names[5] = "y = cos(x)";
    names[6] = "y = atan(x)";
    names[7] = "y = -atan(x)";

    calculatePositions(positions);

    initBuffer(screen);
    
    while (true)
    {
        Timer timer(&deltaTime);
        secTimer += deltaTime;
        iteratorTimer += deltaTime;
        refreshes++;

        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            Sleep(350);
            break;
        }

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < graphCount / 2; j++)
            {
                addToScreenBuffer(
                    (graphWidth + 1) * j, 
                    (graphHeight + 2) * i, 
                    names[i * graphCount / 2 + j],
                    screen
                );
                addGraphToScreenBuffer(
                    (graphWidth + 1) * j, 
                    (graphHeight + 2) * i + 1, 
                    &positions[graphWidth * (i * graphCount / 2 + j)],
                    &iterator,
                    screen
                );
            }
        }

        //Add stats to buffer
        addToScreenBuffer(0, (graphHeight + 2) * 2, "deltaTime: ", deltaTime, screen);
        addToScreenBuffer(0, (graphHeight + 2) * 2 + 1, "rps: ", rps, screen);
        addToScreenBuffer(0, (graphHeight + 2) * 2 + 2, "[Escape] to quit", screen);

        //1s timer for calculating stats
        if (secTimer >= 1.0f)
        {
            secTimer -= 1.0f;
            rps = refreshes;
            refreshes = 0;
        }

        //.1s timer for moving the iterator
        if (iteratorTimer >= .1f)
        {
            iteratorTimer -= .1f;
            direction ? iterator++ : iterator--;
            if (iterator == 0 || iterator == graphWidth - 1)
                direction = direction ? false : true;
        }
        
        screen[screenWidth * screenHeight] = '\0';
        WriteConsoleOutputCharacter(*hConsole, screen, screenWidth * screenHeight, { 0,0 }, dwBytesWritten);
    }
}

void addToScreenBuffer(int x, int y, std::string_view str, wchar_t* screen)
{
    for (int i = 0; i < str.length(); i++)
        screen[y * screenWidth + x + i] = str[i];
}

void addToScreenBuffer(int x, int y, const char* str, float num, wchar_t* screen)
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
        screen[y * screenWidth + x + i] = str[i];

    char holder[20] = { 0 };
    sprintf_s(holder, "%f s", num);
    for (int j = 0; holder[j] != '\0'; i++, j++)
        screen[y * screenWidth + x + i] = holder[j];
}

void addToScreenBuffer(int x, int y, const char* str, int num, wchar_t* screen)
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
        screen[y * screenWidth + x + i] = str[i];

    char holder[10] = { 0 };
    sprintf_s(holder, "%i", num);
    for (int j = 0; holder[j] != '\0'; i++, j++)
        screen[y * screenWidth + x + i] = holder[j];
}

void initBuffer(wchar_t* screen)
{
    for (int i = 0; i < screenWidth * screenHeight; i++)
        screen[i] = ' ';
}

void addGraphToScreenBuffer(int offsetX, int offsetY, Vector2* pos, int* iterator, wchar_t* screen)
{
    for (int y = offsetY; y < graphHeight + offsetY; y++)
    {
        for (int x = offsetX; x < graphWidth + offsetX; x++)
        {
            /*If current character's indices(x, y) suffice graph positions and iterator coordinate assign '@',
            * if x is different from iterator assign '+', and if both x and y don'f suffice
            * graph positions assign '.'
            */
            screen[y * screenWidth + x] =
                (x == pos[x - offsetX].x + offsetX && graphHeight - 1 - y + 2 * offsetY == pos[x - offsetX].y + offsetY) ?
                (x - offsetX == *iterator) ? '@' : '+' : '.';
        }
    }
}

void calculatePositions(Vector2* pos)
{
    for (int j = 0; j < graphCount; j++)
        for (int i = 0; i < graphWidth; i++)
            pos[j * graphWidth + i].x = i;
    
    int i = 0;
    for (; i < graphWidth; i++)
        pos[i].y = pos[i].x / 2;

    for (; i < graphWidth * 2; i++)
        pos[i].y = graphHeight - 1 - pos[i].x / 2;

    for (; i < graphWidth * 3; i++)
        pos[i].y = (int)pow(graphHeight - 1 - pos[i].x, 2) / (graphHeight - 1);

    for (; i < graphWidth * 4; i++)
        pos[i].y = graphHeight - 1 - (int)pow(graphHeight - 1 - pos[i].x, 2) / (graphHeight - 1);

    //Derived variant since normal sine function ranges from 1 to -1 in height and PI intervals in length
    for (; i < graphWidth * 5; i++)
        pos[i].y = round((graphHeight / 2 - 1) * sin(pos[i].x / 2.0f - PI / 2.0f) + graphHeight / 2); 

    for (; i < graphWidth * 6; i++)
        pos[i].y = round(graphHeight / 2 - (graphHeight / 2 - 1) * cos(pos[i].x / 2.0f - PI));

    for (; i < graphWidth * 7; i++)
        pos[i].y = (int)(2 * PI * atan(pos[i].x));

    for (; i < graphWidth * 8; i++)
        pos[i].y = graphHeight - 1 - (int)(2 * (PI * atan(pos[i].x)));
}