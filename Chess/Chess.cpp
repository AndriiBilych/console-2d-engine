#include "Chess.h"

Chess::Chess(int width, int height, int fontWidth, int fontHeight)
    : Engine(width, height, fontWidth, fontHeight)
{
    srand(time(NULL));
    
    //These booleans can be changed - game setting
    randomTeam = true;
    // set randomTeam to false and change second bool value in ternary to specify team
    playerColor = randomTeam ? rand() % 2 : true; 
    playWithComputer = true;

    checkerboardOriginX = 1;
    checkerboardOriginY = 0;
    highlightedX = -1;
    highlightedY = -1;
    pieces.reserve(32);
    out = std::ofstream(fileName.c_str());
    moveCounter = 1;
    whiteScore = 0;
    blackScore = 0;
    //These bools can't be changed - game tracking
    currentTurn = true;
    isGameOver = false;
}

bool Chess::Start() { 
    //Assign symbols and coordinates to pieces
    for (int i = 0; i < 8; i++) { //Pawns
        pieces.emplace_back(Piece(checkerboardOriginX + i, checkerboardOriginY + (playerColor ? 6 : 1), pawn, true, 1));
        pieces.emplace_back(Piece(checkerboardOriginX + i, checkerboardOriginY + (playerColor ? 1 : 6), pawn, false, 1));
    }

    //White figures
    pieces.emplace_back(Piece(checkerboardOriginX, checkerboardOriginY + (playerColor ? 7 : 0), rook, true, 5));
    pieces.emplace_back(Piece(checkerboardOriginX + 1, checkerboardOriginY + (playerColor ? 7 : 0), knight, true, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 2, checkerboardOriginY + (playerColor ? 7 : 0), bishop, true, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + (playerColor ? 3 : 4), checkerboardOriginY + (playerColor ? 7 : 0), queen, true, 9));
    pieces.emplace_back(Piece(checkerboardOriginX + (playerColor ? 4 : 3), checkerboardOriginY + (playerColor ? 7 : 0), king, true, 10));
    pieces.emplace_back(Piece(checkerboardOriginX + 5, checkerboardOriginY + (playerColor ? 7 : 0), bishop, true, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 6, checkerboardOriginY + (playerColor ? 7 : 0), knight, true, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 7, checkerboardOriginY + (playerColor ? 7 : 0), rook, true, 5));

    //Black figures
    pieces.emplace_back(Piece(checkerboardOriginX, checkerboardOriginY + (playerColor ? 0 : 7), rook, false, 5));
    pieces.emplace_back(Piece(checkerboardOriginX + 1, checkerboardOriginY + (playerColor ? 0 : 7), knight, false, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 2, checkerboardOriginY + (playerColor ? 0 : 7), bishop, false, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + (playerColor ? 3 : 4), checkerboardOriginY + (playerColor ? 0 : 7), queen, false, 9));
    pieces.emplace_back(Piece(checkerboardOriginX + (playerColor ? 4 : 3), checkerboardOriginY + (playerColor ? 0 : 7), king, false, 10));
    pieces.emplace_back(Piece(checkerboardOriginX + 5, checkerboardOriginY + (playerColor ? 0 : 7), bishop, false, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 6, checkerboardOriginY + (playerColor ? 0 : 7), knight, false, 3));
    pieces.emplace_back(Piece(checkerboardOriginX + 7, checkerboardOriginY + (playerColor ? 0 : 7), rook, false, 5));

    return true; 
}

bool Chess::Update(float deltaTime)
{
    ClearScreen();

    DisplayChess();

    //Disable EnPassants
    std::for_each(begin(pieces), end(pieces), [this](Piece& p) { if (p.color == currentTurn && p.isEnPassantAvailable) p.isEnPassantAvailable = false; });

    //Game logic
    if (!isGameOver) {
        if (currentTurn == playerColor || !playWithComputer) { 
            if (GetMouse(0).pressed) {
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
                            if (currentTurn == highlightedPiece->color) {

                                //Move only to possible movements-------------------------------------this needs to be a function for ai
                                if (IsMovePossible(clickedPos)) {

                                    bool isCapture = false;

                                    //Calculate conditions for special moves like castling, pawn double move, en passant, promotions
                                    auto captureCondition = clickedPiece != nullptr
                                        && clickedPiece->color != highlightedPiece->color;

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
                                        && enPassantPiece->color != highlightedPiece->color
                                        && enPassantPiece->isEnPassantAvailable;

                                    auto promotionCondition = clickedPiece == nullptr
                                        && highlightedPiece->symbol == pawn
                                        && (highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY
                                            || !highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY + 7);

                                    auto promotionCaptureCondition = clickedPiece != nullptr
                                        && highlightedPiece->symbol == pawn
                                        && clickedPiece->color != highlightedPiece->color
                                        && (highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY
                                            || !highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY + 7);

                                    //Choose an appropriate move based on conditions and invoke a corresponding command
                                    //En passant and promotion capture should be checked before the capture
                                    if (enPassantCaptureCondition) {
                                        lastCommand = new EnPassantCaptureCommand(
                                            highlightedPiece,
                                            enPassantPiece,
                                            enPassantPiece->pos + Position(0, enPassantPiece->color == playerColor ? 1 : -1));
                                        isCapture = true;
                                    }
                                    else if (promotionCaptureCondition) {
                                        lastCommand = new PromotionCaptureCommand(highlightedPiece, clickedPiece);
                                        isCapture = true;
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
                                            clickedPos + Position(clickedPos.x > highlightedPos.x ? -1 : 1, 0));
                                    }
                                    //if choosen to move pawn two squares on the first move
                                    else if (pawnDoubleMoveCondition) {
                                        lastCommand = new PawnDoubleMoveCommand(highlightedPiece, clickedPos);
                                    }
                                    //en passant capture
                                    else if (captureCondition) {
                                        lastCommand = new CaptureCommand(highlightedPiece, clickedPiece);
                                        isCapture = true;
                                    }
                                    //Promotion
                                    else if (promotionCondition) {
                                        lastCommand = new PromotionCommand(highlightedPiece, clickedPos);
                                    }
                                    //normal movement
                                    else {
                                        lastCommand = new MoveCommand(highlightedPiece, clickedPos);
                                    }
                            
                                    commandHistory.AddCommand(lastCommand);
                        
                                    if (IsInCheck(currentTurn)) {
                                        commandHistory.UndoLast();
                                    }
                                    else {
                                        if (highlightedPiece->isFirstMove) {
                                            highlightedPiece->isFirstMove = false;
                                        }
                                
                                        currentTurn = !currentTurn;

                                        bool isInCheck = IsInCheck(currentTurn);
                                        if (isInCheck)
                                            isGameOver = IsCheckmate(currentTurn);

                                        //Record the move
                                        char clickedFile = (char)(playerColor ? 97 + (clickedPos.x - 1) : 97 + 8 - clickedPos.x);
                                        std::string hilightedFile(1, (char)(playerColor ? 97 + (highlightedPos.x - 1) : 97 + 8 - highlightedPos.x));
                                        short line = (playerColor ? 8 - clickedPos.y : clickedPos.y + 1);
                                        std::string symbol(1, highlightedPiece->symbol);
                                        bool isPawn = highlightedPiece->symbol == pawn;

                                        out << (playerColor ? std::to_string(moveCounter++) + ". " : "");
                                        out << (isPawn ? "" : symbol);
                                        out << (isCapture && isPawn ? hilightedFile : "");
                                        out << (isCapture ? "x" : "");
                                        out << clickedFile;
                                        out << line;
                                        out << (isGameOver ? "#" : (isInCheck ? "+" : "")) << " ";
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
                
                        if (clicked != nullptr && (clicked->color == playerColor || !playWithComputer)) {
                            SetPossibleMovementsVector(*clicked, possibleMovements);
                        }

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
        }
        else {
            AIMove(!playerColor);
        }
    }
    //Stepping through the game
    else {
        if (GetKey(VK_LEFT).pressed)
            commandHistory.UndoLast();
        else if (GetKey(VK_RIGHT).pressed)
            commandHistory.RedoLast();
    }

    return true;
}

void Chess::DisplayChess() {
    //Assign coordinates for captured pieces
    for (int i = 0, x_w = checkerboardOriginX + 8, x_b = checkerboardOriginX + 8, y_w = 0, y_b = 7; i < pieces.size(); i++) {
        if (pieces[i].isTaken)
        {
            pieces[i].color ? x_w++ : x_b++;
            pieces[i].pos.x = pieces[i].color ? x_w : x_b;
            pieces[i].pos.y = pieces[i].color ? y_w : y_b;

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

    //Calculate Score
    whiteScore = 0;
    blackScore = 0;
    std::for_each(begin(pieces), end(pieces), [this](Piece& p) {if (!p.color && p.isTaken) whiteScore += p.score; });
    std::for_each(begin(pieces), end(pieces), [this](Piece& p) {if (p.color && p.isTaken) blackScore += p.score; });

    //Draw Score
    if (whiteScore != 0 && whiteScore > blackScore)
        DrawTextToBuffer(9, (playerColor ? 5 : 2), std::to_string(whiteScore - blackScore).insert(0, 1, '+'));
    if (blackScore != 0 && whiteScore < blackScore)
        DrawTextToBuffer(9, (!playerColor ? 5 : 2), std::to_string(blackScore - whiteScore).insert(0, 1, '+'));

    //Draw board
    for (int y = 0; y < 8; y++) {
        //Draw numbers
        Draw(checkerboardOriginX - 1, checkerboardOriginY + y, std::to_string(playerColor ? 8 - y : y + 1)[0], 0xF); 
        //Draw checkerboard
        for (int x = 0; x < 8; x++) 
            if (y % 2 != 0 && x % 2 != 0 || y % 2 == 0 && x % 2 == 0)
                Draw(x + checkerboardOriginX, y + checkerboardOriginY, ' ', 0x70);
            else
                Draw(x + checkerboardOriginX, y + checkerboardOriginY, ' ', 0x80);
    }

    //Draw letters
    for (int x = 0; x < 8; x++) 
        Draw(checkerboardOriginX + x, checkerboardOriginY + 8, playerColor ? 97 + x : 97 + 8 - (x + 1), 0xF);

    //Draw who's turn in title
    if (!isGameOver)
        ChangeTitle(currentTurn ? L"WHITE TURN" : L"BLACK TURN");
    else
        ChangeTitle(!currentTurn ? L"WHITE WON" : L"BLACK WON");

    //Draw pieces
    for (auto& p : pieces)
        if (!p.isTaken) {
            auto attr = GetChar(p.pos.x, p.pos.y).Attributes;
            Draw(p.pos.x, p.pos.y, p.symbol, 
                p.symbol == king && IsInCheck(p.color) ? attr & 0xF | 0xC0 : attr | (p.color ? 0xF : 0x0));
        }
        else
            Draw(p.pos.x, p.pos.y, p.symbol, 0x80 | (p.color ? 0xF : 0x0));

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

void Chess::AIMove(bool team) {
    bool isMoveLegal = false;
    std::vector<Piece> availablePieces(16);
    //Change random generator seed every ai move for true randomness
    srand(time(NULL));

    while (!isMoveLegal) {
        //get all pieces that can move
        auto it = std::copy_if(begin(pieces), end(pieces), begin(availablePieces), [this, team](Piece p) {
            std::vector<Position> availablePositions;
            SetPossibleMovementsVector(p, availablePositions);
            return availablePositions.size() > 0 
                && p.color == team
                && !p.isTaken; });
        availablePieces.resize(std::distance(begin(availablePieces), it));

        //choose random piece
        auto highlightedPiece = GetPieceByCoordinate(availablePieces[rand() % availablePieces.size()].pos) ;

        //get all possible moves of that piece
        SetPossibleMovementsVector(*highlightedPiece, possibleMovements);

        //choose random move
        auto clickedPos = possibleMovements[rand() % possibleMovements.size()];

        //Make a move
        auto highlightedPos = highlightedPiece->pos;
        auto clickedPiece = GetPieceByCoordinate(clickedPos);
        if (IsMovePossible(clickedPos)) {

            bool isCapture = false;

            //Calculate conditions for special moves like castling, pawn double move, en passant, promotions
            auto captureCondition = clickedPiece != nullptr
                && clickedPiece->color != highlightedPiece->color;

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
                && enPassantPiece->color != highlightedPiece->color
                && enPassantPiece->isEnPassantAvailable;

            auto promotionCondition = clickedPiece == nullptr
                && highlightedPiece->symbol == pawn
                && (highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY
                    || !highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY + 7);

            auto promotionCaptureCondition = clickedPiece != nullptr
                && highlightedPiece->symbol == pawn
                && clickedPiece->color != highlightedPiece->color
                && (highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY
                    || !highlightedPiece->color == playerColor && clickedPos.y == checkerboardOriginY + 7);

            //Choose an appropriate move based on conditions and invoke a corresponding command
            //En passant and promotion capture should be checked before the capture
            if (enPassantCaptureCondition) {
                lastCommand = new EnPassantCaptureCommand(
                    highlightedPiece,
                    enPassantPiece,
                    enPassantPiece->pos + Position(0, enPassantPiece->color == playerColor ? 1 : -1));
                isCapture = true;
            }
            else if (promotionCaptureCondition) {
                lastCommand = new PromotionCaptureCommand(highlightedPiece, clickedPiece);
                isCapture = true;
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
                    clickedPos + Position(clickedPos.x > highlightedPos.x ? -1 : 1, 0));
            }
            //if choosen to move pawn two squares on the first move
            else if (pawnDoubleMoveCondition) {
                lastCommand = new PawnDoubleMoveCommand(highlightedPiece, clickedPos);
            }
            //en passant capture
            else if (captureCondition) {
                lastCommand = new CaptureCommand(highlightedPiece, clickedPiece);
                isCapture = true;
            }
            //Promotion
            else if (promotionCondition) {
                lastCommand = new PromotionCommand(highlightedPiece, clickedPos);
            }
            //normal movement
            else {
                lastCommand = new MoveCommand(highlightedPiece, clickedPos);
            }

            commandHistory.AddCommand(lastCommand);

            if (IsInCheck(currentTurn)) {
                commandHistory.UndoLast();
            }
            else {
                if (highlightedPiece->isFirstMove) {
                    highlightedPiece->isFirstMove = false;
                }

                currentTurn = !currentTurn;
                isMoveLegal = true;

                //Record the move
                char clickedFile = (char)(playerColor ? 97 + (clickedPos.x - 1) : 97 + 8 - clickedPos.x);
                std::string hilightedFile(1, (char)(playerColor ? 97 + (highlightedPos.x - 1) : 97 + 8 - highlightedPos.x));
                short line = (playerColor ? 8 - clickedPos.y : clickedPos.y + 1);
                std::string symbol(1, highlightedPiece->symbol);
                bool isPawn = highlightedPiece->symbol == pawn;

                out << (!playerColor ? std::to_string(moveCounter++) + ". " : "");
                out << (isPawn ? "" : symbol);
                out << (isCapture && isPawn ? hilightedFile : "");
                out << (isCapture ? "x" : "");
                out << clickedFile;
                out << line << " ";
            }
        }
    }
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
        if (p.color == team)
            switch (p.symbol)
            {
            case pawn:
            {
                auto move = p.pos;
                team == playerColor ? move.y -= 1 : move.y += 1;

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

bool Chess::CanBeCaptured(Piece* capturePiece) {
    bool result;
    for (auto& p : pieces) {
        if (p.color != capturePiece->color) {
            switch (p.symbol)
            {
            case pawn:
            {
                auto move = p.pos;
                capturePiece->color == playerColor ? move.y -= 1 : move.y += 1;

                //Don't forget there multiple pawns
                if ((move.x - 1 == capturePiece->pos.x || move.x + 1 == capturePiece->pos.x) 
                    && move.y == capturePiece->pos.y && IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece))
                    return true;
                break;
            }
            case rook:
                //Don't forget there multiple rooks
                if (IsPositionAttackedByRook(p, capturePiece->pos) && IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece))
                    return true;
                break;
            case bishop:
                //Don't forget there multiple bishops
                if (IsPositionAttackedByBishop(p, capturePiece->pos) && IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece))
                    return true;
                break;
            case knight:
                for (auto& m : knightMoves)
                    //Don't forget there multiple knights
                    if (p.pos + m == capturePiece->pos && IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece))
                        return true;
                break;
            case queen:
                if (IsPositionAttackedByRook(p, capturePiece->pos) || IsPositionAttackedByBishop(p, capturePiece->pos))
                    return IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece);
                break;
            case king:
                for (auto& m : kingMoves)
                    if (p.pos + m == capturePiece->pos)
                        return IsCaptureLegal(GetPieceByCoordinate(p.pos), capturePiece);
                break;
            default:
                break;
            }
        }
    }
    return false;
}

bool Chess::IsCaptureLegal(Piece* piece, Piece* capturePiece) {
    Command* com = new CaptureCommand(piece, capturePiece);

    com->execute();

    bool result = IsInCheck(piece->color);

    com->undo();

    return !result;
}

bool Chess::IsMoveLegal(Piece* piece, Position newPos) {
    if (GetPieceByCoordinate(newPos) == nullptr) {
        Command* com = new MoveCommand(piece, newPos);

        com->execute();

        bool result = IsInCheck(piece->color);

        com->undo();

        return !result;
    }

    return false;
}

bool Chess::IsPositionAttackedByRook(Piece& rook, Position pos) {
    //Top
    for (auto move = rook.pos + Position(0, -1); IsWithinBoard(move); move.y--) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Right
    for (auto move = rook.pos + Position(1, 0); IsWithinBoard(move); move.x++) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom
    for (auto move = rook.pos + Position(0, 1); IsWithinBoard(move); move.y++){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Left
    for (auto move = rook.pos + Position(-1, 0); IsWithinBoard(move); move.x--){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    return false;
}

bool Chess::IsPositionAttackedByBishop(Piece& bishop, Position pos) {
    //Top left
    for (auto move = bishop.pos + Position(-1, -1); IsWithinBoard(move); move.x--, move.y--) {
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Top right
    for (auto move = bishop.pos + Position(1, -1); IsWithinBoard(move); move.x++, move.y--){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom right
    for (auto move = bishop.pos + Position(1, 1); IsWithinBoard(move); move.x++, move.y++){
        if (move == pos) return true;
        if (GetPieceByCoordinate(move) != nullptr) break;
    }
    //Bottom left
    for (auto move = bishop.pos + Position(-1, 1); IsWithinBoard(move); move.x--, move.y++){
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
    auto holder = std::find_if(begin(pieces), end(pieces), [team](Piece p) { return p.color == team && p.symbol == king; });
    return holder == end(pieces) ? nullptr : holder._Unwrapped();
}

bool Chess::IsMovePossible(Position pos) {
    return std::any_of(begin(possibleMovements), end(possibleMovements), [pos](Position m) { return m == pos; });
}

bool Chess::IsInCheck(bool team) {
    return std::any_of(begin(pieces), end(pieces), [team, this](Piece p) { return p.color != team && !p.isTaken && IsAttackingKing(p); });
}

bool Chess::IsCheckmate(bool team) {
    auto checkingPiece = std::find_if(begin(pieces), end(pieces), 
        [this, team](Piece& p) { return p.color != team && !p.isTaken && IsAttackingKing(p); });

    std::vector<Position> moves;
    Piece* king = GetKingPiece(team);
    SetPossibleMovementsVector(*king, moves);

    //Can king run, can the attacking piece be captured, can the check be blocked by another piece
    bool b1 = std::any_of(begin(moves), end(moves), [this, king](Position pos) { return IsMoveLegal(king, pos); });
    bool b2 = CanBeCaptured(checkingPiece._Unwrapped());
    bool b3 = CanCheckBeBlocked(*(checkingPiece._Unwrapped()), team);

    return !b1 && !b2 && !b3;
}

bool Chess::IsAttackingKing(Piece clickedPiece) {
    std::vector<Position> movementVector;
    switch (clickedPiece.symbol)
    {
    case pawn:
    {
        auto pos = clickedPiece.pos;
        clickedPiece.color == playerColor ? pos.y -= 1 : pos.y += 1;

        //Captures
        auto leftPiece = GetPieceByCoordinate(pos.x - 1, pos.y);
        auto rightPiece = GetPieceByCoordinate(pos.x + 1, pos.y);
        
        return (leftPiece != nullptr && leftPiece->color != clickedPiece.color && leftPiece->symbol == 'K')
            || (rightPiece != nullptr && rightPiece->color != clickedPiece.color && rightPiece->symbol == 'K');
        
        break;
    }
    case rook:
        SetRookMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->color != clickedPiece.color)
                return true;
        }
        break;
    case bishop:
        SetBishopMovementVector(clickedPiece, movementVector);
        for (auto& m : movementVector) {
            auto piece = GetPieceByCoordinate(m);
            if (piece != nullptr && piece->symbol == 'K' && piece->color != clickedPiece.color)
                return true;
        }
        break;
    case knight:
    {
        for (auto& p : knightMoves) {
            Position pos = clickedPiece.pos + p;
            if (IsWithinBoard(pos)) {
                auto piece = GetPieceByCoordinate(pos);
                if (piece != nullptr && piece->color != clickedPiece.color && piece->symbol == 'K')
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
            if (piece != nullptr && piece->symbol == 'K' && piece->color != clickedPiece.color)
                return true;
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
                    possibleBlocks.emplace_back(Position(kingPiece->pos.x, i));
            //checkingPiece under the king
            else 
                for (int i = kingPiece->pos.y + 1; i < checkingPiece.pos.y; i++)
                    possibleBlocks.emplace_back(Position(kingPiece->pos.x, i));
        //horizontal check
        else if (kingPiece->pos.y == checkingPiece.pos.y) 
            //checkingPiece left from the king
            if (kingPiece->pos.x > checkingPiece.pos.x) 
                for (int i = kingPiece->pos.x - 1; i > checkingPiece.pos.x; i--)
                    possibleBlocks.emplace_back(Position(i, kingPiece->pos.y));
            //checkingPiece right from the king
            else 
                for (int i = kingPiece->pos.x + 1; i < checkingPiece.pos.x; i++)
                    possibleBlocks.emplace_back(Position(i, kingPiece->pos.y));
        //diagonal check
        else {
            //top right
            if (kingPiece->pos.y > checkingPiece.pos.y && kingPiece->pos.x < checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(1, -1); i.x < checkingPiece.pos.x; i.x++, i.y-- )
                    possibleBlocks.emplace_back(i);
            //top left
            else if (kingPiece->pos.y > checkingPiece.pos.y && kingPiece->pos.x > checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(-1, -1); i.x > checkingPiece.pos.x; i.x--, i.y--)
                    possibleBlocks.emplace_back(i);
            //down left
            else if (kingPiece->pos.y < checkingPiece.pos.y && kingPiece->pos.x > checkingPiece.pos.x)
                for (Position i = kingPiece->pos + Position(-1, 1); i.x > checkingPiece.pos.x; i.x--, i.y++)
                    possibleBlocks.emplace_back(i);
            //down right
            else 
                for (Position i = kingPiece->pos + Position(1, 1); i.x < checkingPiece.pos.x; i.x++, i.y++)
                    possibleBlocks.emplace_back(i);
        }
    }

    //Find if any piece can move to a possible position that blocks the check
    for (auto& p : pieces) {
        if (p.color == team && !p.isTaken) 
            SetPossibleMovementsVector(p, possibleMoves);

        for (auto& pos : possibleMoves) 
            if (std::find(begin(possibleBlocks), end(possibleBlocks), pos) != end(possibleBlocks)) {
                //This is needed for a possibility of discovered checks and double checks
                auto command = new MoveCommand(&p, pos);
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
        auto yOffset = clickedPiece.color == playerColor ? -1 : 1;
        auto piecePos = clickedPiece.pos + Position(0, yOffset);
        
        //Basic movement
        if (GetPieceByCoordinate(piecePos) == nullptr)
            possibleMovements.emplace_back(piecePos);
        
        //Captures
        auto leftPiece = GetPieceByCoordinate(piecePos + Position(-1, 0));
        auto rightPiece = GetPieceByCoordinate(piecePos + Position(1, 0));
        if (leftPiece != nullptr && leftPiece->color != clickedPiece.color) 
            possibleMovements.emplace_back(leftPiece->pos);
        if (rightPiece != nullptr && rightPiece->color != clickedPiece.color) 
            possibleMovements.emplace_back(rightPiece->pos);

        //En Passant
        leftPiece = GetPieceByCoordinate(clickedPiece.pos + Position(-1, 0));
        rightPiece = GetPieceByCoordinate(clickedPiece.pos + Position(1, 0));
        if (leftPiece != nullptr && leftPiece->color != clickedPiece.color && leftPiece->isEnPassantAvailable) {
            possibleMovements.emplace_back(leftPiece->pos);
            possibleMovements.emplace_back(clickedPiece.pos + Position(-1, yOffset));
        }
        if (rightPiece != nullptr && rightPiece->color != clickedPiece.color && rightPiece->isEnPassantAvailable) {
            possibleMovements.emplace_back(rightPiece->pos);
            possibleMovements.emplace_back(clickedPiece.pos + Position(1, yOffset));
        }

        //First move
        if (clickedPiece.isFirstMove && GetPieceByCoordinate(piecePos + Position(0, yOffset)) == nullptr
            && GetPieceByCoordinate(piecePos) == nullptr) {
            possibleMovements.emplace_back(piecePos + Position(0, yOffset));
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
                piece != nullptr && piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
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
                    piece != nullptr && piece->color != clickedPiece.color) {
                    if (!IsPositionAttacked(pos, !currentTurn))
                        possibleMovements.emplace_back(pos);
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
                        if (!IsPositionAttacked(clickedPiece.pos + Position(-1, 0), !clickedPiece.color) 
                            && !IsPositionAttacked(clickedPiece.pos + Position(-2, 0), !clickedPiece.color))
                        possibleMovements.emplace_back(clickedPiece.pos + Position(-2, 0));
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
                        if (!IsPositionAttacked(clickedPiece.pos + Position(1, 0), !clickedPiece.color)
                            && !IsPositionAttacked(clickedPiece.pos + Position(2, 0), !clickedPiece.color))
                        possibleMovements.emplace_back(clickedPiece.pos + Position(2, 0));
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
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Right
    for (auto pos = clickedPiece.pos + Position(1, 0); IsWithinBoard(pos); pos.x++){
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Bottom
    for (auto pos = clickedPiece.pos + Position(0, 1); IsWithinBoard(pos); pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Left
    for (auto pos = clickedPiece.pos + Position(-1, 0); IsWithinBoard(pos); pos.x--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
}

void Chess::SetBishopMovementVector(Piece clickedPiece, std::vector<Position>& possibleMovements) {
    //Top left
    for (auto pos = clickedPiece.pos + Position(-1, -1); IsWithinBoard(pos); pos.x--, pos.y--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Top right
    for (auto pos = clickedPiece.pos + Position(1, -1); IsWithinBoard(pos); pos.x++, pos.y--) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Bottom right
    for (auto pos = clickedPiece.pos + Position(1, 1); IsWithinBoard(pos); pos.x++, pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
    //Bottom left
    for (auto pos = clickedPiece.pos + Position(-1, 1); IsWithinBoard(pos); pos.x--, pos.y++) {
        auto piece = GetPieceByCoordinate(pos);
        if (piece == nullptr)
            possibleMovements.emplace_back(pos);
        else {
            if (piece->color != clickedPiece.color)
                possibleMovements.emplace_back(pos);
            break;
        }
    }
}

Chess::~Chess() {
    out.close();
}
