/*Todo:
* Simplify/optimize code (
	stl algorithms,
	IsPositionAttacked() - LookForChecks() - IsInCheck() - IsCheckmate(),
	new operator invocations,
	const references and const functions,
	if (condition) return bool; => return condition,
	makeMove function,
	possibleMovements dependency bullshit in IsMovePossible(),
	merge command(
		capture&move(remove duplicating move command from capture command), 
		pawn double move, 
		promotion capture, 
		castling, 
		en passant)),
	simplify captureCommand like in IsCaptureLegal()
* game recording
* captured score difference
* stepping through the game after completion
* features list in readme
* 
* Bugs-----------------------------------------------------------------------
* When playing with a computer if after player move the figure of opposite color was selected
the hilighting and possible moves are displayed incorrectly
* When playing against another player possible moves are displayed if the opposite color piece is selected 
even if it's the other team's turn
* Extras----------------------------------------------------------------------
* 
* Checkout chess github repositories
* Checkout minimax(game theory)
* alpha-beta prunning
* Piece-Square table
* machine learning
* menu(for boolean values)
* Restart
*/
#include "Chess.h"

int main(int argc)
{
	Chess game(20, 12, 40, 40);
	game.StartExecution();

	return 0;
}