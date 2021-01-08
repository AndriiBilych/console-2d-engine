#include "Chess.h"
#include <string>

Chess::Chess(int width, int height, int fontWidth, int fontHeight)
    : Engine(width, height, fontWidth, fontHeight)
{
    playAsWhite = false;
    whiteTurn = true;
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
    if (!IsCheckmate(whiteTurn) && GetMouse(0).pressed) {
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
                    if (whiteTurn == highlightedPiece->isWhite) {

                        //Move only to possible movements
                        if (IsMovePossible(mouseX, mouseY)) {

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
                                && (highlightedPiece->isWhite && clickedPos.y == checkerboardOriginY
                                    || !highlightedPiece->isWhite && clickedPos.y == checkerboardOriginY + 7);

                            auto promotionCaptureCondition = clickedPiece != nullptr
                                && highlightedPiece->symbol == pawn
                                && clickedPiece->isWhite != highlightedPiece->isWhite
                                && (highlightedPiece->isWhite && clickedPos.y == checkerboardOriginY
                                    || !highlightedPiece->isWhite && clickedPos.y == checkerboardOriginY + 7);

                            //Choose an appropriate move based on conditions and invoke a corresponding command
                            //En passant and promotion capture should be checked before the capture
                            if (enPassantCaptureCondition)
                                lastCommand = new EnPassantCaptureCommand(
                                    highlightedPiece,
                                    enPassantPiece,
                                    enPassantPiece->pos + Position(0, enPassantPiece->isWhite ? 1 : -1),
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
                        
                            if (IsInCheck(whiteTurn))
                                lastCommand->undo();
                            else {
                                if (highlightedPiece->isFirstMove) 
                                    highlightedPiece->isFirstMove = false;
                                
                                whiteTurn = !whiteTurn;
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
    if (!IsCheckmate(whiteTurn)) {
        DrawTextToBuffer(0, 10, "Turn: ");
        Draw(6, 10, whiteTurn ? 'W' : 'B', whiteTurn ? 0x8F : 0x70);
    }
    else
    {
        Draw(6, 10, ' ', 0x0);
        DrawTextToBuffer(0, 10, !whiteTurn ? "White" : "Black");
        DrawTextToBuffer(6, 10, "Won");
    }

    //Draw pieces
    for (auto& p : pieces)
        if (!p.isTaken) {
            if (p.symbol == king && IsInCheck(p.isWhite))
                Draw(p.pos.x, p.pos.y, p.symbol, GetChar(p.pos.x, p.pos.y).Attributes & 0xF | 0xC0);
            else
                Draw(p.pos.x, p.pos.y, p.symbol, GetChar(p.pos.x, p.pos.y).Attributes | (p.isWhite ? 0xF : 0x0));
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
        if (p.isWhite == whiteTurn && p.isEnPassantAvailable)
            p.isEnPassantAvailable = false;
}

bool Chess::IsWithinBoard(Position pos) {
    return pos.x >= checkerboardOriginX &&
        pos.x <= checkerboardOriginX + 7 &&
        pos.y >= checkerboardOriginY &&
        pos.y <= checkerboardOriginY + 7;
}

//Team means "is that position attacked by any piece of this color"
bool Chess::IsPositionAttacked(Position pos, bool team) {
    for (auto& p : pieces) {
        if (p.isWhite == team)
            switch (p.symbol)
            {
            case pawn:
            {
                auto move = p.pos;
                team ? move.y -= 1 : move.y += 1;

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
    for (auto it = pieces.begin(); it < pieces.end(); it++)
        if (it->pos == pos && !it->isTaken)
            return it._Unwrapped();
    return nullptr;
}

Piece* Chess::GetPieceByCoordinate(signed short x, signed short y){
    return GetPieceByCoordinate(Position(x, y));
}

Piece* Chess::GetKingPiece(bool team){
    for (auto it = pieces.begin(); it < pieces.end(); it++)
        if (it->isWhite == team && it->symbol == king)
            return it._Unwrapped();
    return nullptr;
}

bool Chess::IsMovePossible(short x, short y) {
    for (auto& m : possibleMovements)
        if (m.x == x && m.y == y)
            return true;
    return false;
}

bool Chess::IsInCheck(bool team) {
    for (auto& p : pieces) 
        if (p.isWhite != team && !p.isTaken && LookForChecks(p)) 
            return true;
    return false;
}

bool Chess::IsCheckmate(bool team) {
    if (IsInCheck(team))
    {
        std::vector<Position> moves;
        SetPossibleMovementsVector(*(GetKingPiece(team)), moves);

        if (moves.size() == 0)
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
        clickedPiece.isWhite ? pos.y -= 1 : pos.y += 1;

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
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite)
                return true;
        }
        break;
    case bishop:
        SetBishopMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite)
                return true;
        }
        break;
    case knight:
    {
        for (auto& p : knightMoves) {
            Position pos = { clickedPiece.pos.x + p.x, clickedPiece.pos.y + p.y };
            if (IsWithinBoard(pos)) {
                auto piece = GetPieceByCoordinate(pos);
                if (piece != nullptr && piece->isWhite != clickedPiece.isWhite && piece->symbol == 'K')
                    return true;
            }
        }
        break;
    }
    case queen:
        SetRookMovementVector(clickedPiece, movementVector);
        SetBishopMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->isWhite != clickedPiece.isWhite)
                return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void Chess::SetPossibleMovementsVector(Piece clickedPiece, std::vector<Position>& possibleMovements) {
    possibleMovements.clear();
    switch (clickedPiece.symbol)
    {
    case pawn:
    {
        auto yOffset = clickedPiece.isWhite ? -1 : 1;
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
                    if (!IsPositionAttacked(pos, !whiteTurn))
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
