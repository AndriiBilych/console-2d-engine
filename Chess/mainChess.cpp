/*Todo:
* add chess to main readme
* features list in readme
* 
* Extras----------------------------------------------------------------------
* 
* Simplify/optimize code (
	#define debug
	more detailed debugLog,
	const_iterators,
	decrease number of new invocations - GetPiecesThatCanMove - GetNotTakenPieces
	)
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
* Highlight last opponent move
* Address warnings
* 
* Bugs----------------------------------------------------------------------
* When computer is in check castling was observed to be possible to execute by a computer
* When stepping through the game squares behind taken figures stay gray
*/
#include "Chess.h"

int main(int argc)
{
	Chess game(20, 12, 40, 40);
	game.StartExecution();

	return 0;
}