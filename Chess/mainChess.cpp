/*Todo:
* Optimise code for playing as black
* Simplify code ( 
	IsPositionAttacked() - LookForChecks() - IsInCheck() - IsCheckmate(),
	SetPossibleMovementsVector(),
	Update(),
	emplace_back(),
	new operator invocations,
	const references and functions,
	merge command(
		capture&move(remove duplicating move command from capture command), 
		pawn double move, 
		promotion capture, 
		castling, 
		en passant))
* game recording
* Restart
* AI
* stepping through completed game(undoing untaken enPassant problem) (maybe?)
* Choose promotion figure problem(threads?) (maybe?)
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