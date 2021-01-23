/*Todo:
* Simplify code (
	all_of - none_of - any_of,
	stl algorithms,
	IsPositionAttacked() - LookForChecks() - IsInCheck() - IsCheckmate(),
	SetPossibleMovementsVector(),
	Update(),
	emplace_back(),
	new operator invocations,
	const references and const functions,
	merge command(
		capture&move(remove duplicating move command from capture command), 
		pawn double move, 
		promotion capture, 
		castling, 
		en passant))
* game recording
* Restart
* AI
* Checkout chess github repositories
* captured score difference
* features list in readme
*/
#include "Chess.h"

int main(int argc)
{
	Chess game(20, 12, 40, 40);
	game.StartExecution();

	return 0;
}