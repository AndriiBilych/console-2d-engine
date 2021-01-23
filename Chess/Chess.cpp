#include "Chess.h"
#include <string>

Chess::Chess(int width, int height, int fontWidth, int fontHeight)
    : Engine(width, height, fontWidth, fontHeight)
{
    playAsWhite = false;
    turn = true;
    checkerboardOriginX = 1;
    checkerboardOriginY = 0;
    highlightedX = -1;
    highlightedY = -1;
    pieces.reserve(32);

    //Assign symbols and coordinates to pieces
    for (int i = 0; i < 8; i++) { //Pawns
        pieces.emplace_back(Piece(checkerboardOriginX + i, checkerboardOriginY + (playAsWhite ? 6 : 1), pawn, true));
        pieces.emplace_back(Piece(checkerboardOriginX + i, checkerboardOriginY + (playAsWhite ? 1 : 6), pawn, false));
    }
    
    //White figures
    pieces.emplace_back(Piece(checkerboardOriginX, checkerboardOriginY + (playAsWhite ? 7 : 0), rook, true));
    pieces.emplace_back(Piece(checkerboardOriginX + 1, checkerboardOriginY + (playAsWhite ? 7 : 0), knight, true));
    pieces.emplace_back(Piece(checkerboardOriginX + 2, checkerboardOriginY + (playAsWhite ? 7 : 0), bishop, true));
    pieces.emplace_back(Piece(checkerboardOriginX + (playAsWhite ? 3 : 4), checkerboardOriginY + (playAsWhite ? 7 : 0), queen, true));
    pieces.emplace_back(Piece(checkerboardOriginX + (playAsWhite ? 4 : 3), checkerboardOriginY + (playAsWhite ? 7 : 0), king, true));
    pieces.emplace_back(Piece(checkerboardOriginX + 5, checkerboardOriginY + (playAsWhite ? 7 : 0), bishop, true));
    pieces.emplace_back(Piece(checkerboardOriginX + 6, checkerboardOriginY + (playAsWhite ? 7 : 0), knight, true));
    pieces.emplace_back(Piece(checkerboardOriginX + 7, checkerboardOriginY + (playAsWhite ? 7 : 0), rook, true));
    
    //Black figures
    pieces.emplace_back(Piece(checkerboardOriginX, checkerboardOriginY + (playAsWhite ? 0 : 7), rook, false));
    pieces.emplace_back(Piece(checkerboardOriginX + 1, checkerboardOriginY + (playAsWhite ? 0 : 7), knight, false));
    pieces.emplace_back(Piece(checkerboardOriginX + 2, checkerboardOriginY + (playAsWhite ? 0 : 7), bishop, false));
    pieces.emplace_back(Piece(checkerboardOriginX + (playAsWhite ? 3 : 4), checkerboardOriginY + (playAsWhite ? 0 : 7), queen, false));
    pieces.emplace_back(Piece(checkerboardOriginX + (playAsWhite ? 4 : 3), checkerboardOriginY + (playAsWhite ? 0 : 7), king, false));
    pieces.emplace_back(Piece(checkerboardOriginX + 5, checkerboardOriginY + (playAsWhite ? 0 : 7), bishop, false));
    pieces.emplace_back(Piece(checkerboardOriginX + 6, checkerboardOriginY + (playAsWhite ? 0 : 7), knight, false));
    pieces.emplace_back(Piece(checkerboardOriginX + 7, checkerboardOriginY + (playAsWhite ? 0 : 7), rook, false));
}

bool Chess::Start()
{
    return true;
}

bool Chess::Update(float deltaTime)
{
    ClearScreen();

    //Assign coordinates for captured pieces
    for (int i = 0, x_w = checkerboardOriginX + 8, x_b = checkerboardOriginX + 8, y_w = 0, y_b = 7; i < pieces.size(); i++) {
        if (pieces[i].isTaken)
        {
            pieces[i].isWhite ? x_w++ : x_b++;
            pieces[i].pos.x = pieces[i].isWhite ? x_w : x_b;
            pieces[i].pos.y = pieces[i].isWhite ? y_w : y_b;

            if (x_w > 16)
            {
                x_w -= 8;
                y_w++;
            }

            if (x_b > 16)
            {
                x_b -= 8;
                y_b--;
            }
        }
    }

    DisplayChess();

    DisableEnPassants();

    //Game logic
    if (!IsCheckmate(turn) && GetMouse(0).pressed) {
        auto mouseX = GetMouseX();
        auto mouseY = GetMouseY();

        // if clicked within the board
        if (IsWithinBoard(Position(mouseX, mouseY))) { 

            // if there is already a highlighted square
            if (highlightedX != -1 && highlightedY != -1) {
                auto highlightedPos = Position(highlightedX, highlightedY);
                auto highlightedPiece = GetPieceByCoordinate(highlightedPos);
                
                if (highlightedPiece != nullptr)
                {
                    auto clickedPos = Position(mouseX, mouseY);
                    auto clickedPiece = GetPieceByCoordinate(clickedPos);

                    /*if the highlighted square is not empty && if the correct piece is choosen on this turn*/
                    if (turn == highlightedPiece->isWhite) {

                        //Move only to possible movements
                        if (IsMovePossible(clickedPos)) {

                            //Calculate conditions for special moves like castling, pawn double move, en passant, promotions
                            auto captureCondition = clickedPiece != nullptr
                                && clickedPiece->isWhite != highlightedPiece->isWhite;

                            auto castleCondition = clickedPiece == nullptr
                                && highlightedPiece->symbol == king
                                && (clickedPos == highlightedPos + Position(2, 0)
                                    || clickedPos == highlightedPos + Position(-2, 0));

                            auto pawnDoubleMoveCondition = clickedPiece == nullptr
                                && highlightedPiece->symbol == pawn
                                && abs(clickedPos.y - highlightedPos.y) == 2;

                            auto enPassantClickedSide = clickedPos.x - highlightedPos.x > 0; // true - right side, false - left side
                            auto enPassantPiece = GetPieceByCoordinate(highlightedPos + Position(enPassantClickedSide ? 1 : -1, 0));
                            auto enPassantCaptureCondition = enPassantPiece != nullptr
                                && enPassantPiece->isWhite != highlightedPiece->isWhite
                                && enPassantPiece->isEnPassantAvailable;

                            auto promotionCondition = clickedPiece == nullptr
                                && highlightedPiece->symbol == pawn
                                && (highlightedPiece->isWhite == playAsWhite && clickedPos.y == checkerboardOriginY
                                    || !highlightedPiece->isWhite == playAsWhite && clickedPos.y == checkerboardOriginY + 7);

                            auto promotionCaptureCondition = clickedPiece != nullptr
                                && highlightedPiece->symbol == pawn
                                && clickedPiece->isWhite != highlightedPiece->isWhite
                                && (highlightedPiece->isWhite == playAsWhite && clickedPos.y == checkerboardOriginY
                                    || !highlightedPiece->isWhite == playAsWhite && clickedPos.y == checkerboardOriginY + 7);

                            //Choose an appropriate move based on conditions and invoke a corresponding command
                            //En passant and promotion capture should be checked before the capture
                            if (enPassantCaptureCondition)
                                lastCommand = new EnPassantCaptureCommand(
                                    highlightedPiece,
                                    enPassantPiece,
                                    enPassantPiece->pos + Position(0, enPassantPiece->isWhite == playAsWhite ? 1 : -1),
                                    highlightedPos);
                            else if (promotionCaptureCondition) {
                                lastCommand = new PromotionCaptureCommand(
                                    highlightedPiece,
                                    clickedPiece, 
                                    clickedPos, 
                                    highlightedPos, 
                                    queen, 
                                    pawn);
                            }
                            //if choosen to castle
                            else if (castleCondition) {
                                auto rook = GetPieceByCoordinate(
                                    checkerboardOriginX + (clickedPos.x > highlightedPos.x ? 7 : 0),
                                    highlightedPos.y);

                                lastCommand = new CastleCommand(
                                    highlightedPiece,
                                    rook,
                                    clickedPos,
                                    highlightedPos,
                                    clickedPos + Position(clickedPos.x > highlightedPos.x ? -1 : 1, 0),
                                    rook->pos);
                            }
                            //if choosen to move pawn two squares on the first move
                            else if (pawnDoubleMoveCondition)
                                lastCommand = new PawnDoubleMoveCommand(highlightedPiece, clickedPos, highlightedPos);
                            //en passant capture
                            else if (captureCondition)
                                lastCommand = new CaptureCommand(highlightedPiece, clickedPiece, clickedPos, highlightedPos);
                            //Promotion
                            else if (promotionCondition)
                                lastCommand = new PromotionCommand(highlightedPiece, clickedPos, highlightedPos, queen, pawn);
                            //normal movement
                            else 
                                lastCommand = new MoveCommand(highlightedPiece, clickedPos, highlightedPos);
                            
                            lastCommand->execute();
                        
                            if (IsInCheck(turn))
                                lastCommand->undo();
                            else {
                                if (highlightedPiece->isFirstMove) 
                                    highlightedPiece->isFirstMove = false;
                                
                                turn = !turn;
                            }
                        }
                    }
                }

                possibleMovements.clear();
                highlightedX = -1;
                highlightedY = -1;
            }
            //if clicked on another piece
            else {
                auto clicked = GetPieceByCoordinate(mouseX, mouseY);
                
                if (clicked != nullptr)
                    SetPossibleMovementsVector(*clicked, possibleMovements);

                highlightedX = mouseX;
                highlightedY = mouseY;
            }
        }
        // if clicked outside the board
        else { 
            possibleMovements.clear();
            highlightedX = -1;
            highlightedY = -1;
        }
    }

    return true;
}

void Chess::DisplayChess() {
    //Draw board
    for (int y = 0; y < 8; y++) {
        //Draw numbers
        Draw(checkerboardOriginX - 1, checkerboardOriginY + y, std::to_string(playAsWhite ? 8 - y : y + 1)[0], 0xF); 
        //Draw checkerboard
        for (int x = 0; x < 8; x++) 
            if (y % 2 != 0 && x % 2 != 0 || y % 2 == 0 && x % 2 == 0)
                Draw(x + checkerboardOriginX, y + checkerboardOriginY, ' ', 0x70);
            else
                Draw(x + checkerboardOriginX, y + checkerboardOriginY, ' ', 0x80);
    }

    //Draw letters
    for (int x = 0; x < 8; x++) 
        Draw(checkerboardOriginX + x, checkerboardOriginY + 8, playAsWhite ? 97 + x : 97 + 8 - (x + 1), 0xF);

    //Draw who's turn message
    if (!IsCheckmate(turn)) {
        DrawTextToBuffer(0, 10, "Turn: ");
        Draw(6, 10, turn ? 'W' : 'B', turn ? 0x8F : 0x70);
    }
    else
    {
        Draw(6, 10, ' ', 0x0);
        DrawTextToBuffer(0, 10, !turn ? "White" : "Black");
        DrawTextToBuffer(6, 10, "Won");
    }

    //Draw pieces
    for (auto& p : pieces)
        if (!p.isTaken) {
            auto attr = GetChar(p.pos.x, p.pos.y).Attributes;
            Draw(p.pos.x, p.pos.y, p.symbol, 
                p.symbol == king && IsInCheck(p.isWhite) ? attr & 0xF | 0xC0 : attr | (p.isWhite ? 0xF : 0x0));
        }
        else
            Draw(p.pos.x, p.pos.y, p.symbol, 0x80 | (p.isWhite ? 0xF : 0x0));

    //Draw highlighted square and possible movement
    if (highlightedX != -1 && highlightedY != -1) {
        auto symbol = GetChar(highlightedX, highlightedY).Char.UnicodeChar;
        auto symbolAttributes = GetChar(highlightedX, highlightedY).Attributes;

        Draw(highlightedX, highlightedY, symbol, symbolAttributes & 0xF | 0x60);

        if (possibleMovements.size() > 0)
            for (auto& p : possibleMovements) {
                auto piece = GetPieceByCoordinate(p);
                if (piece == nullptr)
                    Draw(p.x, p.y, ' ', 0xA0);
                else
                    Draw(p.x, p.y, piece->symbol, GetChar(p.x, p.y).Attributes & 0xF | 0xC0);
            }
    }
}

void Chess::DisableEnPassants() {
    for (auto& p : pieces) 
        if (p.isWhite == turn && p.isEnPassantAvailable)
            p.isEnPassantAvailable = false;
}

bool Chess::IsWithinBoard(Position pos) {
    return pos.x >= checkerboardOriginX &&
        pos.x <= checkerboardOriginX + 7 &&
        pos.y >= checkerboardOriginY &&
        pos.y <= checkerboardOriginY + 7;
}

//"is that position attacked by any piece of this color"
bool Chess::IsPositionAttacked(Position pos, bool team) {
    for (auto& p : pieces) {
        if (p.isWhite == team)
            switch (p.symbol)
            {
            case pawn:
            {
                auto move = p.pos;
                team == playAsWhite ? move.y -= 1 : move.y += 1;

                if ((move.x - 1 == pos.x || move.x + 1 == pos.x) && move.y == pos.y)
                    return true;
                break;
            }
            case rook:
                if (IsPositionAttackedByRook(p, pos))
                    return true;
                break;
            case bishop:
                if (IsPositionAttackedByBishop(p, pos))
                    return true;
                break;
            case knight:
                for (auto& m : knightMoves) 
                    if (p.pos + m == pos)
                        return true;
                break;
            case queen:
                if (IsPositionAttackedByRook(p, pos) || IsPositionAttackedByBishop(p, pos))
                    return true;
                break;
            case king:
                for (auto& m : kingMoves) 
                    if (p.pos + m == pos)
                        return true;
                break;
            default:
                break;
            }
    }
    return false;
}

bool Chess::IsPositionAttackedByRook(Piece p, Position pos) {
    //Top
    for (auto move = p.pos + Position(0, -1); IsWithinBoard(move); move.y--) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Right
    for (auto move = p.pos + Position(1, 0); IsWithinBoard(move); move.x++) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom
    for (auto move = p.pos + Position(0, 1); IsWithinBoard(move); move.y++){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Left
    for (auto move = p.pos + Position(-1, 0); IsWithinBoard(move); move.x--){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    return false;
}

bool Chess::IsPositionAttackedByBishop(Piece p, Position pos) {
    //Top left
    for (auto move = p.pos + Position(-1, -1); IsWithinBoard(move); move.x--, move.y--) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Top right
    for (auto move = p.pos + Position(1, -1); IsWithinBoard(move); move.x++, move.y--){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom right
    for (auto move = p.pos + Position(1, 1); IsWithinBoard(move); move.x++, move.y++){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom left
    for (auto move = p.pos + Position(-1, 1); IsWithinBoard(move); move.x--, move.y++){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    return false;
}

Piece* Chess::GetPieceByCoordinate(Position pos){
    auto holder = std::find_if(begin(pieces), end(pieces), [pos](Piece p) { return p.pos == pos && !p.isTaken; });
    return holder == end(pieces) ? nullptr : holder._Unwrapped();
}

Piece* Chess::GetPieceByCoordinate(signed short x, signed short y){
    return GetPieceByCoordinate(Position(x, y));
}

Piece* Chess::GetKingPiece(bool team){
    auto holder = std::find_if(begin(pieces), end(pieces), [team](Piece p) { return p.isWhite == team && p.symbol == king; });
    return holder == end(pieces) ? nullptr : holder._Unwrapped();
}

bool Chess::IsMovePossible(Position pos) {
    return std::find_if(begin(possibleMovements), end(possibleMovements), [pos](Position m) { return m == pos; }) != end(possibleMovements);
}

bool Chess::IsInCheck(bool team) {
    return std::find_if(begin(pieces), end(pieces), [team, this](Piece p) { return p.isWhite != team && !p.isTaken && LookForChecks(p); }) != end(pieces);
}

bool Chess::IsInCheck(bool team, Piece& holderPiece) {
    for (int i = 0; i < pieces.size(); i++)
        if (pieces[i].isWhite != team && !pieces[i].isTaken)
            if (LookForChecks(pieces[i])) {
                holderPiece = pieces[i];
                return true;
            }
    return false;
}

bool Chess::IsCheckmate(bool team) {
    Piece checkingPiece = pieces[0];
    if (IsInCheck(team, checkingPiece)) {
        std::vector<Position> moves;
        SetPossibleMovementsVector(*(GetKingPiece(team)), moves);

        //Can king run, can the attacking piece be captured
        if (moves.size() == 0 && !IsPositionAttacked(checkingPiece.pos, !checkingPiece.isWhite) && !CanCheckBeBlocked(checkingPiece, team))
            return true;
    }
    return false;
}

bool Chess::LookForChecks(Piece clickedPiece) {
    std::vector<Position> movementVector;
    switch (clickedPiece.symbol)
    {
    case pawn:
    {
        auto pos = clickedPiece.pos;
        clickedPiece.isWhite == playAsWhite ? pos.y -= 1 : pos.y += 1;

        //Captures
        auto leftPiece = GetPieceByCoordinate(pos.x - 1, pos.y);
        auto rightPiece = GetPieceByCoordinate(pos.x + 1, pos.y);
        
        if (leftPiece != nullptr && leftPiece->isWhite != clickedPiece.isWhite && leftPiece->symbol == 'K') 
            return true;
        if (rightPiece != nullptr && rightPiece->isWhite != clickedPiece.isWhite && rightPiece->symbol == 'K') 
            return true;
        
        break;
    }
    case rook:
        SetRookMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite) {
                return true;
            }
        }
        break;
    case bishop:
        SetBishopMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite) {
                return true;
            }
        }
        break;
    case knight:
    {
        for (auto& p : knightMoves) {
            Position pos = clickedPiece.pos + p;
            if (IsWithinBoard(pos)) {
                auto piece = GetPieceByCoordinate(pos);
                if (piece != nullptr && piece->isWhite != clickedPiece.isWhite && piece->symbol == 'K') {
                    return true;
                }
            }
        }
        break;
    }
    case queen:
        SetRookMovementVector(clickedPiece, movementVector);
        SetBishopMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite) {
                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}

bool Chess::CanCheckBeBlocked(Piece checkingPiece, bool team) {
    std::vector<Position> possibleBlocks;
    std::vector<Position> possibleMoves;
    
    //Find possible positions that can block the check
    if (checkingPiece.symbol != pawn && checkingPiece.symbol != knight) {
        auto kingPiece = GetKingPiece(team);
        //vertical check
        if (kingPiece->pos.x == checkingPiece.pos.x) 
            //checkingPiece above the king
            if (kingPiece->pos.y > checkingPiece.pos.y) 
                for (int i = kingPiece->pos.y - 1; i > checkingPiece.pos.y; i--)
                    possibleBlocks.push_back(Position(kingPiece->pos.x, i));
            //checkingPiece under the king
            else 
                for (int i = kingPiece->pos.y + 1; i < checkingPiece.pos.y; i++)
                    possibleBlocks.push_back(Position(kingPiece->pos.x, i));
        //horizontal check
        else if (kingPiece->pos.y == checkingPiece.pos.y) 
            //checkingPiece left from the king
            if (kingPiece->pos.x > checkingPiece.pos.x) 
                for (int i = kingPiece->pos.x - 1; i > checkingPiece.pos.x; i--)
                    possibleBlocks.push_back(Position(i, kingPiece->pos.y));
            //checkingPiece right from the king
            else 
                for (int i = kingPiece->pos.x + 1; i < checkingPiece.pos.x; i++)
                    possibleBlocks.push_back(Position(i, kingPiece->pos.y));
        //diagonal check
        else {
            //top right
            if (kingPiece->pos.y > checkingPiece.pos.y && kingPiece->pos.x < checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(1, -1); i.x < checkingPiece.pos.x; i.x++, i.y-- )
                    possibleBlocks.push_back(i);
            //top left
            else if (kingPiece->pos.y > checkingPiece.pos.y && kingPiece->pos.x > checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(-1, -1); i.x > checkingPiece.pos.x; i.x--, i.y--)
                    possibleBlocks.push_back(i);
            //down left
            else if (kingPiece->pos.y < checkingPiece.pos.y && kingPiece->pos.x > checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(-1, 1); i.x > checkingPiece.pos.x; i.x--, i.y++)
                    possibleBlocks.push_back(i);
            //down right
            else 
                for (Position i = kingPiece->pos + Position(1, 1); i.x < checkingPiece.pos.x; i.x++, i.y++)
                    possibleBlocks.push_back(i);
        }
    }

    //Find if any piece can move to a possible position that blocks the check
    for (auto& p : pieces) {
        if (p.isWhite == team && !p.isTaken) 
            SetPossibleMovementsVector(p, possibleMoves);

        for (auto& pos : possibleMoves) 
            if (std::find(begin(possibleBlocks), end(possibleBlocks), pos) != end(possibleBlocks)) {
                //This is needed for a possibility of discovered checks and double checks
                auto command = new MoveCommand(&p, pos, p.pos);
                command->execute();
                if (!IsInCheck(team)) {
                    command->undo();
                    return true;
                }
                else
                    command->undo();
            }
    }
    return false;
}

void Chess::SetPossibleMovementsVector(Piece clickedPiece, std::vector<Position>& possibleMovements) {
    possibleMovements.clear();
    switch (clickedPiece.symbol)
    {
    case pawn:
    {
        auto yOffset = clickedPiece.isWhite == playAsWhite ? -1 : 1;
        auto piecePos = clickedPiece.pos + Position(0, yOffset);
        
        //Basic movement
        if (GetPieceByCoordinate(piecePos) == nullptr)
            possibleMovements.push_back(piecePos);
        
        //Captures
        auto leftPiece = GetPieceByCoordinate(piecePos + Position(-1, 0));
        auto rightPiece = GetPieceByCoordinate(piecePos + Position(1, 0));
        if (leftPiece != nullptr && leftPiece->isWhite != clickedPiece.isWhite) 
            possibleMovements.push_back(leftPiece->pos);
        if (rightPiece != nullptr && rightPiece->isWhite != clickedPiece.isWhite) 
            possibleMovements.push_back(rightPiece->pos);

        //En Passant
        leftPiece = GetPieceByCoordinate(clickedPiece.pos + Position(-1, 0));
        rightPiece = GetPieceByCoordinate(clickedPiece.pos + Position(1, 0));
        if (leftPiece != nullptr && leftPiece->isWhite != clickedPiece.isWhite && leftPiece->isEnPassantAvailable) {
            possibleMovements.push_back(leftPiece->pos);
            possibleMovements.push_back(clickedPiece.pos + Position(-1, yOffset));
        }
        if (rightPiece != nullptr && rightPiece->isWhite != clickedPiece.isWhite && rightPiece->isEnPassantAvailable) {
            possibleMovements.push_back(rightPiece->pos);
            possibleMovements.push_back(clickedPiece.pos + Position(1, yOffset));
        }

        //First move
        if (clickedPiece.isFirstMove && GetPieceByCoordinate(piecePos + Position(0, yOffset)) == nullptr) {
            possibleMovements.push_back(piecePos + Position(0, yOffset));
        }
        break;
    }
    case rook:
        SetRookMovementVector(clickedPiece, possibleMovements);
        break;
    case bishop:
        SetBishopMovementVector(clickedPiece, possibleMovements);
        break;
    case knight:
        for (auto& p : knightMoves) {
            Position pos(clickedPiece.pos + p);
            auto piece = GetPieceByCoordinate(pos);
            if (IsWithinBoard(pos) && piece == nullptr || 
                piece != nullptr && piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
        }
        break;
    case queen:
        SetRookMovementVector(clickedPiece, possibleMovements);
        SetBishopMovementVector(clickedPiece, possibleMovements);
        break;
    case king:
        for (auto& p : kingMoves) {
            Position pos(clickedPiece.pos + p);
            if (IsWithinBoard(pos)) {
                auto piece = GetPieceByCoordinate(pos);
                if (piece == nullptr ||
                    piece != nullptr && piece->isWhite != clickedPiece.isWhite) {
                    if (!IsPositionAttacked(pos, !turn))
                        possibleMovements.push_back(pos);
                }
            }
        }

        //castling
        if (clickedPiece.isFirstMove) {
            auto leftRook = GetPieceByCoordinate(checkerboardOriginX, clickedPiece.pos.y);
            auto rightRook = GetPieceByCoordinate(checkerboardOriginX + 7, clickedPiece.pos.y);

            if (leftRook != nullptr && leftRook->isFirstMove) {
                for (int i = clickedPiece.pos.x - 1; i > 0; i--) {
                    auto piece = GetPieceByCoordinate(i, clickedPiece.pos.y);
                    if (piece == nullptr)
                    {
                        continue;
                    }
                    else if (i == leftRook->pos.x)
                    {
                        if (!IsPositionAttacked(clickedPiece.pos + Position(-1, 0), !clickedPiece.isWhite) 
                            && !IsPositionAttacked(clickedPiece.pos + Position(-2, 0), !clickedPiece.isWhite))
                        possibleMovements.push_back(clickedPiece.pos + Position(-2, 0));
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (rightRook != nullptr && rightRook->isFirstMove) {
                for (int i = clickedPiece.pos.x + 1; i < checkerboardOriginX + 7 + 1; i++) {
                    auto piece = GetPieceByCoordinate(i, clickedPiece.pos.y);
                    if (piece == nullptr)
                    {
                        continue;
                    }
                    else if (i == rightRook->pos.x)
                    {
                        if (!IsPositionAttacked(clickedPiece.pos + Position(1, 0), !clickedPiece.isWhite)
                            && !IsPositionAttacked(clickedPiece.pos + Position(2, 0), !clickedPiece.isWhite))
                        possibleMovements.push_back(clickedPiece.pos + Position(2, 0));
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        break;
    default:
        break;
    }
}

void Chess::SetRookMovementVector(Piece clickedPiece, std::vector<Position>& possibleMovements) {
    //Top
    for (auto pos = clickedPiece.pos + Position(0, -1); IsWithinBoard(pos); pos.y--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Right
    for (auto pos = clickedPiece.pos + Position(1, 0); IsWithinBoard(pos); pos.x++){
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Bottom
    for (auto pos = clickedPiece.pos + Position(0, 1); IsWithinBoard(pos); pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Left
    for (auto pos = clickedPiece.pos + Position(-1, 0); IsWithinBoard(pos); pos.x--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
}

void Chess::SetBishopMovementVector(Piece clickedPiece, std::vector<Position>& possibleMovements) {
    //Top left
    for (auto pos = clickedPiece.pos + Position(-1, -1); IsWithinBoard(pos); pos.x--, pos.y--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Top right
    for (auto pos = clickedPiece.pos + Position(1, -1); IsWithinBoard(pos); pos.x++, pos.y--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Bottom right
    for (auto pos = clickedPiece.pos + Position(1, 1); IsWithinBoard(pos); pos.x++, pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
    //Bottom left
    for (auto pos = clickedPiece.pos + Position(-1, 1); IsWithinBoard(pos); pos.x--, pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.push_back(pos);
        else {
            if (piece->isWhite != clickedPiece.isWhite)
                possibleMovements.push_back(pos);
            break;
        }
    }
}

Chess::~Chess()
{
}
