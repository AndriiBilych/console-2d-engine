/*Todo:
* Simplify/optimize code (
	stl algorithms,
	new operator invocations,
	const references and const functions,
	LPCWSTR tutorial - writing title - DisplayChess(),
* add chess to main readme
* features list in readme
* 
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