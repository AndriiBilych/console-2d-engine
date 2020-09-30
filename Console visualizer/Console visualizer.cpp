#include <chrono>
#include <iostream>
#include <cmath>

#define PI 3.14f

//Initial values for the graph size, amount, string length and labels
const int width = 20;
const int height = 10;
const int graphColumnsCount = 4;
const int graphRowsCount = 2;
constexpr int length = graphColumnsCount * (height + 1) * (width + 1);
const char names[graphColumnsCount * graphRowsCount][width] = {
    {"y = -x"},
    {"y = x"},
    {"y = -x^2"},
    {"y = x^2"},
    {"y = sin(x)"},
    {"y = cos(x)"},
    {"y = atan(x)"},
    {"y = -atan(x)"}
};

//X is the same for all graphs for the sake of synchronized movement
int xPos = 0;
int yPos[graphColumnsCount * graphRowsCount] = { 0 };
bool direction = true;
char consoleBuffer[graphRowsCount][length + 1];

void calculatePositions();
void draw();

int main()
{
    //Holders for time between refreshes
    std::chrono::high_resolution_clock::time_point begin, end;

    //Stores information about amount and time of refreshes
    int frameCount = 0;
    int frameBuffer = 0;
    float timer = 0;
    float timePassed = .0f;
    
    while (true)
    {
        //Starts timer and refreshes console
        begin = std::chrono::high_resolution_clock::now();
        system("cls");

        //Calculates y positions and prints consoleBuffer to console
        calculatePositions();
        draw();
        
        //Timer logic and refresh counter
        if (timer >= 1.0f)
        {
            frameBuffer = frameCount;
            frameCount = 0;
            timer = 0;
        }
        else if (timer >= 1.0f / 20.0f)
        {
            if (xPos <= 0)
                direction = true;

            if (xPos >= width - 1)
                direction = false;

            direction ? xPos++ : xPos--;
        }

        //Stop timer and calculate amount of refreshes
        end = std::chrono::high_resolution_clock::now();
        frameCount++;
        timePassed = std::chrono::duration<float>(end - begin).count();
        timer += timePassed;

        std::cout << "Refreshes per second: " << frameBuffer << "\nTime between refreshes: " << timePassed << "s";
    }

    return 0;
}

void calculatePositions()
{
    /*Most of the functions are either flipped or modified since symbols on console are printed from top left corner,
    also square grid doesn't look square in console, so a 20x10 grid is used and that is why all functions are extended*/
    yPos[0] = xPos / 2; //Diagonal line
    yPos[1] = height - 1 - xPos / 2; //Diagonal line flipped
    yPos[2] = (xPos - height) * (xPos - height) / (height - 1); //Parabola
    yPos[3] = height - 1 - (xPos - height) * (xPos - height) / (height - 1); //Parabola flipped
    yPos[4] = round((height / 2 - 1) * sin(xPos / 2.0f - PI / 2.0f) + height / 2); //Sin function represented on a 20x10 grid
    yPos[5] = round(height / 2 - (height / 2 - 1) * cos(xPos / 2.0f - PI)); //Cos function represented on a 20x10 grid
    yPos[6] = 2 * PI * atan(xPos); //atan function represented on a 20x10 grid
    yPos[7] = height - 1 - 2 * (PI * atan(xPos)); //atan function flipped represented on a 20x10 grid
}

void draw()
{
    /*Since each row of a consoleBuffer displays first rows of each individual graph, the most
    reasonable solution was to use triple loops. y - loop cycles through each row of symbols,
    i - loop cycles through parts of each graph on the current y-row, x - loop cycles through 
    each character in the current part of the current graph*/

    //First loop handles two rows of graphs, index represents current row of graphs
    for (int index = 0; index < graphRowsCount; index++)
    {
        /*This part puts names of the graphs on the first row of console buffer.*/
        for (int i = 0; i < graphColumnsCount; i++) {
            for (int x = 0; x < width; x++) {

                //[i * (width + 1) + x] represents indices for characters from one row of the consoleBuffer
                /*[index * graphColumnsCount + i] represents indices for pointers to arrays of name characters,
                and is adjusted for index because graphs are split into two rows*/
                consoleBuffer[index][i * (width + 1) + x] =
                    (names[index * graphColumnsCount + i][x] != NULL) ? names[index * graphColumnsCount + i][x] : ' ';
            }
            //Represents a character between parts of each graph on the current y-row
            consoleBuffer[index][i * (width + 1) + width] = ' ';
        }
        //Represents a character at the end of a current y-row
        consoleBuffer[index][graphColumnsCount * (width + 1) - 1] = '\n';

        /*This part builds graphs in a console buffer.*/
        for (int y = 1; y < height + 1; y++) {
            for (int i = 0; i < graphColumnsCount; i++) {
                for (int x = 0; x < width; x++) {

                    /*[y * graphColumnsCount * (width + 1) + i * (width + 1) + x] represents indices for characters from one row 
                    of the consoleBuffer, same as with names, but adjusted for y-loop.
                    Each character in a row is checked for x and y coordinate and then replaced with @ if x and y match 
                    calculated position for a graph.
                    [index * graphColumnsCount + i] represents index for calculated y coordinate of the corresponding graph.
                    calculated position yPos is adjusted by 1 to prevent overlapping yPos coordinates and names*/
                    consoleBuffer[index][y * graphColumnsCount * (width + 1) + i * (width + 1) + x] = 
                        (x == xPos && y == yPos[index * graphColumnsCount + i] + 1) ? '@' : '.';
                }
                //Represents a character between parts of each graph on the current y-row, but adjusted for y-loop
                consoleBuffer[index][y * graphColumnsCount * (width + 1) + i * (width + 1) + width] = ' ';
            }
            //Represents a character at the end of a current y-row, but adjusted for y-loop
            consoleBuffer[index][(y + 1) * graphColumnsCount * (width + 1) - 1] = '\n';
        }
        //Represents last character in a string
        consoleBuffer[index][length] = '\0';
        
        std::cout << consoleBuffer[index] << std::endl;
    }
}
