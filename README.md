2D Console engine
=====

Description
---------
	This is an implementation of a simple 2d engine in windows console with projects and animations included.
Engine is a greatly simplified version of olcConsoleGameEngine by javidx9. 

	All graphics are implemented by employing ASCII characters and are displayed by using Windows function WriteConsoleOutput. 
Engine accepts keyboard and mouse input by utilizing GetAsyncKeyState and ReadConsoleInput accordingly. Output of these functions 
is then converted to three states for each key: pressed, released, held. Besides Engine class Engine.h also contains: Timer class 
for timing functions and any scopes, Position class for representing two-dimensional coordinates and simplifying certain operations, 
Command and CommandHistory classes for the use of undo/redo functionality and recording certain events

	The entire engine is written in the header Engine.h. To use the engine a project class must inherit from the 
Engine class. See projects for details.
	
Projects: 
- Chess engine

Animations: 
- trigonometric graphs
- matrix from the movie

Compiling & Running
---------
The project was built with standard MSVC and Visual Studio 2019

Chess engine
---------
Standard game of Chess, full implementation of all the rules and mechanics

![Screenshot](Screenshot 1.png)
![Screenshot](Screenshot 2.png)
![Screenshot](Screenshot 3.png)

Animations

MovieMatrix
---------
A console representation of the matrix inspired by the movie.

![Not Loaded](https://media.giphy.com/media/UbZUJMRA4j7v4WYj2K/giphy.gif)

Graphs
---------
First project features 8 trigonometric functions and a pointers moving on the graph coordinates.

![Not Loaded](https://media.giphy.com/media/K5HnGAKasGZGSGjuSq/giphy.gif)
