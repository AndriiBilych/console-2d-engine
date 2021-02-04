/*Todo:
* Simplify/optimize code (
	stl algorithms,
	IsPositionAttacked(),
	SetPossibleMovementsVector()
	new operator invocations,
	const references and const functions,
	LPCWSTR tutorial - writing title - DisplayChess(),
	makeMove function,
	possibleMovements dependency bullshit in IsMovePossible(),
* game recording (game log in a text document)
* captured score difference
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