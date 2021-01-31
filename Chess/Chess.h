#pragma once
#include <vector>
#include <algorithm>
#include "Engine.h"
#include "Command.h"

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
    void AIMove(bool);
    bool IsPositionAttacked(Position, bool);
    bool IsPositionAttackedByRook(Piece, Position);
    bool IsPositionAttackedByBishop(Piece, Position);
    bool IsInCheck(bool);
    bool IsInCheck(bool, Piece&);
    bool IsCheckmate(bool);
    bool LookForChecks(Piece);
    bool CanCheckBeBlocked(Piece, bool);
    Piece* GetPieceByCoordinate(Position);
    Piece* GetPieceByCoordinate(signed short, signed short);
    Piece* GetKingPiece(bool);
    bool IsMovePossible(Position);
    void SetPossibleMovementsVector(Piece, std::vector<Position>&);
    void SetRookMovementVector(Piece, std::vector<Position>&);
    void SetBishopMovementVector(Piece, std::vector<Position>&);
    ~Chess();
private:
    bool randomTeam;
    bool playerColor;
    bool currentTurn;
    bool playWithComputer;
    int checkerboardOriginX;
    int checkerboardOriginY;
    signed short highlightedX;
    signed short highlightedY;
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

