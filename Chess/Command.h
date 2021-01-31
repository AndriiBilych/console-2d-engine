#pragma once
enum Characters {
    pawn = 'P',
    rook = 'R',
    bishop = 'B',
    knight = 'N',
    queen = 'Q',
    king = 'K'
};

class Position {
public:
    Position(signed short x, signed short y) : x(x), y(y) {}
    Position operator+ (const Position& other) const { return Position(x + other.x, y + other.y); }
    bool operator== (const Position& other) const { return x == other.x && y == other.y; }
    bool operator!= (const Position& other) const { return x != other.x || y != other.y; }

    signed short x, y;
};

class Piece {
public:
    Piece() : pos(0, 0) {
        symbol = pawn;
        this->isWhite = isWhite;
        isFirstMove = true;
        isTaken = false;
        isEnPassantAvailable = false;
    }

    Piece(signed short x, signed short y, Characters c, bool isWhite) : pos(x, y) {
        symbol = c;
        this->isWhite = isWhite;
        isFirstMove = true;
        isTaken = false;
        isEnPassantAvailable = false;
    }

    Position pos;
    Characters symbol;
    bool isWhite;
    bool isFirstMove;
    bool isEnPassantAvailable;
    bool isTaken;
};

class Command {
public:
	virtual ~Command() {}
	virtual void execute() = 0;
	virtual void undo() = 0;
};

class MoveCommand : public Command {
public:
	MoveCommand(Piece* piece, Position pos, Position posPrev) 
        : _piece(piece), _pos(pos), _posPrev(posPrev){}

	virtual void execute() override {
		_piece->pos = _pos;
	}

    virtual void undo() override {
        _piece->pos = _posPrev;
    }
private:
	Piece* _piece;
	Position _pos;
	Position _posPrev;
};

class PromotionCommand : public Command {
public:
    PromotionCommand(Piece* piece, Position pos, Position posPrev, Characters symbol, Characters symbolPrev) 
        : _piece(piece), _pos(pos), _posPrev(posPrev), _symbol(symbol), _symbolPrev(symbolPrev) {}

	virtual void execute() override {
		_piece->pos = _pos;
        _piece->symbol = _symbol;
	}

    virtual void undo() override {
        _piece->pos = _posPrev;
        _piece->symbol = _symbolPrev;
    }
private:
	Piece* _piece;
	Position _pos;
	Position _posPrev;
    Characters _symbol;
    Characters _symbolPrev;
};

class PromotionCaptureCommand : public Command {
public:
    PromotionCaptureCommand(Piece* piece, Piece* pieceCaptured, Position pos, Position posPrev, Characters symbol, Characters symbolPrev)
        : _piece(piece), _pieceCaptured(pieceCaptured), _pos(pos), _posPrev(posPrev), _symbol(symbol), _symbolPrev(symbolPrev) {}

	virtual void execute() override {
		_piece->pos = _pos;
        _piece->symbol = _symbol;
        _pieceCaptured->isTaken = true;
	}

    virtual void undo() override {
        _piece->pos = _posPrev;
        _piece->symbol = _symbolPrev;
        _pieceCaptured->isTaken = false;
    }
private:
	Piece* _piece;
	Piece* _pieceCaptured;
	Position _pos;
	Position _posPrev;
    Characters _symbol;
    Characters _symbolPrev;
};

class PawnDoubleMoveCommand : public Command {
public:
    PawnDoubleMoveCommand(Piece* piece, Position pos, Position posPrevious) 
        : _piece(piece), _pos(pos), _posPrevious(posPrevious) {}

    virtual void execute() override {
        _piece->pos = _pos;
        _piece->isEnPassantAvailable = true;
    }

    virtual void undo() override {
        _piece->pos = _posPrevious;
        _piece->isEnPassantAvailable = false;
    }
private:
    Piece* _piece;
    Position _pos;
    Position _posPrevious;
};

class CaptureCommand : public Command {
public:
    CaptureCommand(Piece* piece, Piece* capturedPiece, Position pos, Position posPrevious) 
        : _piece(piece), _pieceCaptured(capturedPiece), _pos(pos), _posPrevious(posPrevious){}

	virtual void execute() override {
		_piece->pos = _pos;
        _pieceCaptured->isTaken = true;
	}

    virtual void undo() override {
        _piece->pos = _posPrevious;
        _pieceCaptured->isTaken = false;
    }
private:
	Piece* _piece;
	Piece* _pieceCaptured;
	Position _pos;
	Position _posPrevious;
};

class EnPassantCaptureCommand : public Command {
public:
    EnPassantCaptureCommand(Piece* piece, Piece* capturedPiece, Position pos, Position posPrevious)
        : _piece(piece), _pieceCaptured(capturedPiece), _pos(pos), _posPrevious(posPrevious) {}

    virtual void execute() override {
        _piece->pos = _pos;
        _pieceCaptured->isTaken = true;
        _pieceCaptured->isEnPassantAvailable = false;//for safety (doesn't really do anything)
    }

    virtual void undo() override {
        _piece->pos = _posPrevious;
        _pieceCaptured->pos = Position(_pos.x, _posPrevious.y);
        _pieceCaptured->isTaken = false;
        _pieceCaptured->isEnPassantAvailable = true;//for safety (doesn't really do anything)
    }
private:
    Piece* _piece;
    Piece* _pieceCaptured;
    Position _pos;
    Position _posPrevious;
};

class CastleCommand : public Command {
public:
    CastleCommand(Piece* king, Piece* rook, Position kingPos, Position kingPosPrev, Position rookPos, Position rookPosPrev)
        : _king(king), _rook(rook), _kingPos(kingPos), _rookPos(rookPos), _kingPosPrev(kingPosPrev), _rookPosPrev(rookPosPrev) {}

    virtual void execute() override {
        _king->pos = _kingPos;
        _rook->pos = _rookPos;
        _rook->isFirstMove = false;
    }

    virtual void undo() override {
        _king->pos = _kingPosPrev;
        _rook->pos = _rookPosPrev;
        _rook->isFirstMove = true;
    }
private:
    Piece* _king;
    Piece* _rook;
    Position _kingPos;
    Position _rookPos;
    Position _kingPosPrev;
    Position _rookPosPrev;
};

//class CommandHistory {
//public:
//    void AddComand(Command* cmd) {
//        cmd->execute();
//    }
//    Command* m_commands[200];
//};
