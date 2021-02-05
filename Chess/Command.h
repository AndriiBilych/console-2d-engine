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
        this->color = color;
        isFirstMove = true;
        isTaken = false;
        isEnPassantAvailable = false;
        score = 0;
    }

    Piece(signed short x, signed short y, Characters c, bool color, signed short score) : pos(x, y) {
        symbol = c;
        this->color = color;
        isFirstMove = true;
        isTaken = false;
        isEnPassantAvailable = false;
        this->score = score;
    }

    Position pos;
    Characters symbol;
    bool color;
    bool isFirstMove;
    bool isEnPassantAvailable;
    bool isTaken;
    signed short score;
};

class Command {
public:
	virtual ~Command() {}
	virtual void execute() = 0;
	virtual void undo() = 0;
};

class MoveCommand : public Command {
public:
	MoveCommand(Piece* piece, Position newPos) 
        : _piece(piece), _pos(newPos), _posPrev(piece->pos) {}

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
    PromotionCommand(Piece* piece, Position newPos) 
        : _piece(piece), _pos(newPos), _posPrev(piece->pos), _symbol(queen), _symbolPrev(pawn) {}

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
    PromotionCaptureCommand(Piece* piece, Piece* capturedPiece)
        : _piece(piece), _pieceCaptured(capturedPiece), _pos(capturedPiece->pos), _posPrev(piece->pos), _symbol(queen), _symbolPrev(pawn) {}

	virtual void execute() override {
		_piece->pos = _pos;
        _piece->symbol = _symbol;
        _pieceCaptured->isTaken = true;
	}

    virtual void undo() override {
        _piece->pos = _posPrev;
        _piece->symbol = _symbolPrev;
        _pieceCaptured->pos = _pos;
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
    PawnDoubleMoveCommand(Piece* piece, Position newPos) 
        : _piece(piece), _pos(newPos), _posPrevious(piece->pos) {}

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
    CaptureCommand(Piece* piece, Piece* capturedPiece) 
        : _piece(piece), _pieceCaptured(capturedPiece), _pos(capturedPiece->pos), _posPrevious(piece->pos){}

	virtual void execute() override {
		_piece->pos = _pos;
        _pieceCaptured->isTaken = true;
	}

    virtual void undo() override {
        _piece->pos = _posPrevious;
        _pieceCaptured->pos = _pos;
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
    EnPassantCaptureCommand(Piece* piece, Piece* capturedPiece, Position pos)
        : _piece(piece), _pieceCaptured(capturedPiece), _pos(pos), _posPrevious(piece->pos) {}

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
    CastleCommand(Piece* king, Piece* rook, Position kingPos, Position rookPos)
        : _king(king), _rook(rook), _kingPos(kingPos), _rookPos(rookPos), _kingPosPrev(king->pos), _rookPosPrev(rook->pos) {}

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

class CommandHistory {
public:
    void AddCommand(Command* cmd) {
        cmd->execute();
        m_commands[iterator] = cmd;
        iterator++;
        size++;
    }
    void UndoLast() {
        if (iterator > 0)
            m_commands[--iterator]->undo();
    }
    void RedoLast() {
        if (iterator < size)
            m_commands[iterator++]->execute();
    }

private:
    int iterator = 0;
    int size = 0;
    Command* m_commands[200];
};
