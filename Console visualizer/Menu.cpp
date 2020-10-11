/*Todo: 
* improve AddToScreenBuffer with generics
* improve stats display 
*/

#include "Timer.h"
#include "Graph.h"

int main()
{
    // Create Screen Buffer
    wchar_t* screen = new wchar_t[screenWidth * screenHeight + 1];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;
    
    while (true)
    {
        initBuffer(screen);
        
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            Sleep(350);
            break;
        }
        
        if (GetKeyState(VK_RETURN) & 0x8000)
        {
            Sleep(350);
            displayGraphs(screen, &hConsole, &dwBytesWritten);
        }

        //Display menu
        addToScreenBuffer(0, 0, "Trigonometric functions movement [Enter] to open", screen);
        addToScreenBuffer(0, 1, "[Esc] to quit", screen);

        screen[screenWidth * screenHeight] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
    }

    delete[] screen;
    screen = nullptr;
    return 0;
}
