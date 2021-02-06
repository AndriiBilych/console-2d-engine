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
	rewrite SetPossibleMovements to return  a vector of moves
* add chess to main readme
* features list in readme
* 
* Bugs-----------------------------------------------------------------------
* When playing with a computer if after player move the figure of opposite color was selected
the hilighting and possible moves are displayed incorrectly
* When playing against another player possible moves are displayed if the opposite color piece is selected 
even if it's the other team's turn
* AddCommand is working wrong - size ends up bigger than actual size and bigger than iterator
* Extras----------------------------------------------------------------------
* 
* Checkout chess github repositories
* Checkout minimax(game theory)
* alpha-beta prunning
* Piece-Square table
* machine learning
* menu(for boolean values)
* Restart
* Notation:
	!: good move,
	?: poor move,
	0-0 kingside castle
	0-0-0 queenside castle
	ambiguous notation for when two figures can make the same move(knights and rooks)
* Draw
	dead position,
	repetition
*/
#include "Chess.h"

int main(int argc)
{
	Chess game(20, 12, 40, 40);
	game.StartExecution();

	return 0;
}