/*Chess
*/
#pragma once
#include "Engine.h"
#include "Command.h"
#include <vector>

class Chess :
    public Engine
{
public:
    Chess(int, int, int, int);
    bool Start();
    bool Update(float);
    void DisplayChess();
    void DisableEnPassants();
    bool IsWithinBoard(Position);
    bool IsPositionAttacked(Position, bool);
    bool IsPositionAttackedByRook(Piece, Position);
    bool IsPositionAttackedByBishop(Piece, Position);
    bool IsInCheck(bool);
    bool IsCheckmate(bool);
    bool LookForChecks(Piece);
    Piece* GetPieceByCoordinate(Position);
    Piece* GetPieceByCoordinate(signed short, signed short);
    Piece* GetKingPiece(bool);
    bool IsMovePossible(short, short);
    void SetPossibleMovementsVector(Piece, std::vector<Position>&);
    void SetRookMovementVector(Piece, std::vector<Position>&);
    void SetBishopMovementVector(Piece, std::vector<Position>&);
    ~Chess();
private:
    bool playAsWhite;
    bool whiteTurn;
    int checkerboardOriginX;
    int checkerboardOriginY;
    signed short highlightedX;
    signed short highlightedY;
    //char pawn = 'P';
    //char rook = 'R';
    //char bishop = 'B';
    //char knight = 'N';
    //char queen = 'Q';
    //char king = 'K';
    //Characters _chars;
    std::vector<Piece> pieces;
    std::vector<Position> possibleMovements;
    //CommandHistory commandHistory;
    Command* lastCommand;
    Position knightMoves[8] = {
        { -1, -2 },
        { -2, -1 },
        { +1, -2 },
        { +2, -1 },
        { -1, +2 },
        { -2, +1 },
        { +1, +2 },
        { +2, +1 }
    };
    Position kingMoves[8] = {
        { -1, -1 },
        { 0, -1 },
        { 1, -1 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { -1, 1 },
        { -1, 0 }
    };
};

