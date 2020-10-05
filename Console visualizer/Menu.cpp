/*Todo: Rewrite graph benchmarking into a separate Timer class;
* Try to use enter, escape or space keys for menu;
* Look into other ways of getting input from keyboard
*/

#include "Graph.h"

int main()
{
    const char* input = NULL;

    while (true)
    {
        if (GetKeyState((unsigned short)'Q') - 1 < -126)
        {
            break;
        }
        
        if (GetKeyState((unsigned short)'F') - 1 < -126)
        {
            displayGraphs();
        }

        system("cls");
        std::cout << "Trigonometric functions movement <--\n";
        std::cout << "('Q' to quit/'F' to choose)";
    }

    return 0;
}
