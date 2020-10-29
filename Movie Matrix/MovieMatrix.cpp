#include "MovieMatrix.h"

MovieMatrix::MovieMatrix(int width, int height, int fontWidth, int fontHeight) 
    : Engine(width, height, fontWidth, fontHeight)
{
    srand(time(NULL));
    timer = .0f;
    timerCharacterChange = .0f;
    timerColorChange = .0f;
    timerEnding = .0f;
    color = 0x0002;
    wrapAround = true;
    appearanceCounter = 1;
    columnLength = 30;
    screenHeight = GetScreenHeight();
    screenWidth = GetScreenWidth();
    columnPositions.reserve(screenWidth / 2);
    activePositions.reserve(screenWidth / 2);
    characters = new wchar_t[screenWidth / 2 * columnLength];
}

bool MovieMatrix::Start()
{
    //Generate random coordinates for the columns of characters
    for (int i = 0; i < screenWidth / 2; i++)
        columnPositions.emplace_back(Position(i * 2 + rand() % 2, -(columnLength + rand() % (int)(screenHeight / .8f))));

    //Generate random string of characters
    for (int i = 0; i < columnPositions.size() * columnLength; i++)
        characters[i] = rand() % 93 + 33;

    //Assign first active column
    auto third = columnPositions.size() / 3;
    auto index = rand() % third + third;
    activePositions.emplace_back(columnPositions[index]);
    columnPositions.erase(columnPositions.begin() + index);

	return true;
}

bool MovieMatrix::Update(float deltaTime)
{
    timer += deltaTime;
    timerCharacterChange += deltaTime;
    timerColorChange += deltaTime;
    timerEnding += deltaTime;

    ClearScreen();

    //Draw lines and wrap columns around the screen
    for (int i = 0; i < activePositions.size(); i++)
        for (int j = 0; j < columnLength; j++)
        {
            int y = activePositions[i].y + j;
            short color = j == columnLength - 1 ? 0x0007 : this->color;//Marks last character of the column gray
            if (y > screenHeight)
                y -= screenHeight + 1;

            Draw(activePositions[i].x, y, characters[columnLength * i + j], color);
        }

    //Move active column positions down and wrap coordinates around the screen every .05s
    if (timer >= .05f)
    {
        for (auto& i : activePositions)
        {
            i.y++;
            if (i.y > screenHeight && wrapAround)
                i.y = 0;
        }

        timer -= .05f;
    }

    //Randomize half of the characters every .5s and handle smooth column appearance
    if (timerCharacterChange >= .5f)
    {
        for (int i = 0; i < screenWidth / 2 * columnLength / 2; i++)
            characters[rand() % (screenWidth / 2 * columnLength)] = rand() % 93 + 33;

        //Gradual increase of active columns
        for (int i = 0; i < appearanceCounter && columnPositions.size() > 0; i++)
        {
            auto index = rand() % columnPositions.size();
            activePositions.emplace_back(columnPositions[index]);
            columnPositions.erase(columnPositions.begin() + index);
        }
        
        if (appearanceCounter <= columnPositions.size() / 2)
            appearanceCounter <<= 1;

        timerCharacterChange -= .5f;
    }

    //Switch color every 5s
    if (timerColorChange >= 5.0f)
    {
        color = color == 0x0002 ? 0x0005 : 0x0002;
        timerColorChange -= 5.0f;
    }

    //Close the console after all characters went off screen 10s
    if (timerEnding >= 10.0f)
    {
        if (wrapAround)
            wrapAround = false;
        else
            return false;
        timerEnding -= 10.0f;
    }

	return true;
}

MovieMatrix::~MovieMatrix()
{
    delete[] characters;
    characters = nullptr;
}
