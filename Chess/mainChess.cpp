/*Todo:
* Simplify/optimize code (
	all_of - none_of - any_of,
	stl algorithms,
	IsPositionAttacked() - LookForChecks() - IsInCheck() - IsCheckmate(),
	SetPossibleMovementsVector(),
	Update(),
	emplace_back(),
	new operator invocations,
	const references and const functions,
	if (condition) return bool; => return condition,
	makeMove function,
	possibleMovements dependency bullshit IsMovePossible(),
	merge command(
		capture&move(remove duplicating move command from capture command), 
		pawn double move, 
		promotion capture, 
		castling, 
		en passant))
* game recording
* Restart
* captured score difference
* features list in readme
* stepping through the game after completion
* status of the game update in the title of the window
* 
* Bugs: 
* enpassant is available through a piece like a horse
* 
* Extras----------------------------------------------------------------------
* 
* Checkout chess github repositories
* Checkout minimax(game theory)
* alpha-beta prunning
* Piece-Square table
* machine learning
*/
#include "Chess.h"

int main(int argc)
{
	Chess game(20, 12, 40, 40);
	game.StartExecution();

	return 0;
}