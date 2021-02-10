Conway's Game of Life
=====

![Screenshot](Screenshot1.png)
![Screenshot](Screenshot2.png)

Description
---------
The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells, 
each of which is in one of two possible states, alive or dead. Every cell interacts with its eight 
neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each 
step in time, the following transitions occur:

1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

These rules, which compare the behavior of the automaton to real life, can be condensed into the following:

1. Any live cell with two or three live neighbours survives.
2. Any dead cell with three live neighbours becomes a live cell.
3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.

The calculations occur at the same time, which means next generation is stored in a separate buffer and then
is copied to the main array of states.

Features
---------
-All rules above apply
-Simulation can be paused/resumed by pressing spacebar
-During pause left click will activate clicked cell, right click will disable clicked cell