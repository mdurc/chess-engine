#include <bits/stdc++.h>
#include <unistd.h>
#include "shared.h"
using namespace std;


// GLOBAL
int moveNumber = 1;
ofstream historyOutput("history.txt");

// <turn #, <Location of piece that can enPassant, the square it attacks>>
list<pair<int, pair<Loc,Loc>>> enPassants;


void deletePieces(list<Piece*>& pieces){
    cout << "Deleting " << pieces.size() << " Pieces" << endl;
    for(Piece*& p: pieces){
        delete p;
        p = nullptr;
    }
}

void exitProgram(list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    historyOutput.close();
    deletePieces(whitePieces);
    deletePieces(blackPieces);
    exit(1);
}

class SlidingPiece : public Piece {
protected:
    void addMovesInDirection(vector<vector<Piece*>>& BOARD, int rowDelta, int colDelta) {
        int newRow = m_pos.row;
        int newCol = m_pos.col;

        while (true) {
            newRow += rowDelta;
            newCol += colDelta;

            if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
                break;
            }

            if (BOARD[newRow][newCol] != nullptr) {
                if (BOARD[newRow][newCol]->m_color != m_color) {
                    m_attackedSquares.push_back({newRow, newCol}); // capture
                }
                // piece is in the way
                break;
            } else {
                m_attackedSquares.push_back({newRow, newCol}); // empty space
            }
        }
    }
};

class Pawn : public Piece {
public:
    Pawn(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♙" : "♟");
        m_pos = start;
        m_value = 100;
        type = PAWN;
    }


    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        int direction = (m_color?-1:1); // 1 for white, -1 for black
        int newRow = m_pos.row + direction;

        // DIAGONAL CAPTURES
        m_attackedSquares.clear();

        if (newRow >= 0 && newRow < 8) {

            Piece* square = m_pos.col+1<8?BOARD[newRow][m_pos.col+1]:nullptr;
            if(square && square->m_color!=m_color){
                m_attackedSquares.push_back(Loc(newRow, m_pos.col + 1));
            }
            square = m_pos.col-1>=0?BOARD[newRow][m_pos.col-1]:nullptr;
            if(square && square->m_color!=m_color){
                m_attackedSquares.push_back(Loc(newRow, m_pos.col - 1));
            }
        }


        // OTHER MOVEMENT
        m_nonAttackMoves.clear();

        if (newRow >= 0 && newRow < 8) {
            // single square move
            if (BOARD[newRow][m_pos.col] == nullptr) {
                m_nonAttackMoves.push_back(Loc(newRow, m_pos.col));

                // double square move from starting position
                if (!m_hasMoved && BOARD[m_pos.row+2*direction][m_pos.col] == nullptr) {
                    m_nonAttackMoves.push_back(Loc(m_pos.row+2*direction,m_pos.col));
                }
            }
        }
    }
};



class Rook : public SlidingPiece {
public:
    Rook(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♖" : "♜");
        m_pos = start;
        m_value = 500;
        type = ROOK;
    }
    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        m_attackedSquares.clear();
        addMovesInDirection(BOARD, 1, 0);  // Up
        addMovesInDirection(BOARD, -1, 0); // Down
        addMovesInDirection(BOARD, 0, 1);  // Right
        addMovesInDirection(BOARD, 0, -1); // Left
    }
};

class Knight : public Piece {
public:
    Knight(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♘" : "♞");
        m_pos = start;
        m_value = 320;
        type = KNIGHT;
    }
    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        m_attackedSquares.clear();
        int row = m_pos.row;
        int col = m_pos.col;
        
        int rowChange[] = {2, 2, 1, 1, -1, -1, -2, -2};
        int colChange[] = {1, -1, 2, -2, 2, -2, 1, -1};

        for(int i=0;i<8;++i){
            int newRow = row + rowChange[i];
            int newCol = col + colChange[i];
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                if(BOARD[newRow][newCol]==nullptr || BOARD[newRow][newCol]->m_color!=m_color){
                    m_attackedSquares.push_back(Loc(newRow, newCol));
                }
            }
        }
    }
};

class Bishop : public SlidingPiece {
public:
    Bishop(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♗" : "♝");
        m_pos = start;
        m_value = 320;
        type = BISHOP;
    }
    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        m_attackedSquares.clear();
        addMovesInDirection(BOARD,1, 1);   // Diagonal up-right
        addMovesInDirection(BOARD,1, -1);  // Diagonal up-left
        addMovesInDirection(BOARD,-1, 1);  // Diagonal down-right
        addMovesInDirection(BOARD,-1, -1); // Diagonal down-left
    }
};

class Queen : public SlidingPiece {
public:
    Queen(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♕" : "♛");
        m_pos = start;
        m_value = 900;
        type = QUEEN;
    }
    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        m_attackedSquares.clear();
        // bishop + rook movement
        addMovesInDirection(BOARD, 1, 1);   // Diagonal up-right
        addMovesInDirection(BOARD, 1, -1);  // Diagonal up-left
        addMovesInDirection(BOARD, -1, 1);  // Diagonal down-right
        addMovesInDirection(BOARD, -1, -1); // Diagonal down-left
        addMovesInDirection(BOARD, 1, 0);  // Up
        addMovesInDirection(BOARD, -1, 0); // Down
        addMovesInDirection(BOARD, 0, 1);  // Right
        addMovesInDirection(BOARD, 0, -1); // Left
    }
};


class King : public Piece {
public:
    King(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♔" : "♚");
        m_pos = start;
        m_value = 20000;
        type = KING;
    }
    bool canCastle(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces, int rookCol, int kingCol1, int kingCol2) {
        Piece* isRook = BOARD[m_pos.row][rookCol];
        if (isRook && isRook->type==ROOK && !isRook->m_hasMoved) {
            // check if all squares between King and Rook are empty
            int lim = max(rookCol, m_pos.col);
            for (int col = min(rookCol, m_pos.col) + 1; col < lim; ++col) {
                if (BOARD[m_pos.row][col] != nullptr) {
                    return false;
                }
            }
            // check if King would pass through or end up in check
            for (int col : {m_pos.col, kingCol1, kingCol2}) {
                if (isSquareAttacked(Loc(m_pos.row, col), !m_color, whitePieces, blackPieces)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    };

    void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
        m_attackedSquares.clear();
        int directions[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for(int i=0;i<8;++i){
            int newRow = m_pos.row + directions[i][0];
            int newCol = m_pos.col + directions[i][1];

            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                if (BOARD[newRow][newCol] == nullptr || BOARD[newRow][newCol]->m_color != m_color) {
                    m_attackedSquares.push_back(Loc(newRow, newCol));
                }
            }
        }

        m_nonAttackMoves.clear();
        // CASTLING

        if (!m_hasMoved && !isInCheck(m_color, BOARD, whitePieces, blackPieces)) {
            // Kingside castling (short castling)
            if (canCastle(BOARD, whitePieces, blackPieces, 7, m_pos.col + 1, m_pos.col + 2)) {
                m_nonAttackMoves.push_back(Loc(m_pos.row, m_pos.col + 2));
            }

            // Queenside castling (long castling)
            if (canCastle(BOARD, whitePieces, blackPieces, 0, m_pos.col - 1, m_pos.col - 2)) {
                m_nonAttackMoves.push_back(Loc(m_pos.row, m_pos.col - 2));
            }
        }
    }
};

void updateAllPieces(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces, Piece* exceptThis){
    for(Piece*& p: blackPieces){
        if(exceptThis == p){ continue; }
        p->updateMovement(BOARD, whitePieces, blackPieces);
    }
    for(Piece*& p: whitePieces){
        if(exceptThis == p){ continue; }
        p->updateMovement(BOARD, whitePieces, blackPieces);
    }
}

void setupBoard(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    // black pieces
    BOARD[0][0] = new Rook(0, Loc(0, 0)); BOARD[0][7] = new Rook(0, Loc(0,7));
    BOARD[0][1] = new Knight(0, Loc(0, 1)); BOARD[0][6] = new Knight(0, Loc(0, 6));
    BOARD[0][2] = new Bishop(0, Loc(0, 2)); BOARD[0][5] = new Bishop(0, Loc(0, 5));
    BOARD[0][3] = new Queen(0, Loc(0, 3));
    BOARD[0][4] = new King(0, Loc(0, 4));

    // white pieces
    BOARD[7][0] = new Rook(1, Loc(7, 0)); BOARD[7][7] = new Rook(1, Loc(7, 7));
    BOARD[7][1] = new Knight(1, Loc(7, 1)); BOARD[7][6] = new Knight(1, Loc(7, 6));
    BOARD[7][2] = new Bishop(1, Loc(7, 2)); BOARD[7][5] = new Bishop(1, Loc(7, 5));
    BOARD[7][3] = new Queen(1, Loc(7, 3));
    BOARD[7][4] = new King(1, Loc(7, 4));

    // Pawns and saving all pieces into respective vectors
    for(int i=0;i<8;++i){
        //black:
        BOARD[1][i] = new Pawn(0, Loc(1, i)); 
        blackPieces.push_back(BOARD[1][i]);
        blackPieces.push_back(BOARD[0][i]);
        //white:
        BOARD[6][i] = new Pawn(1, Loc(6, i));
        whitePieces.push_back(BOARD[6][i]);
        whitePieces.push_back(BOARD[7][i]);
    }
}

void printBoard(vector<vector<Piece*>>& b){
    for(int i=0;i<8;++i){
        cout << "   +---+---+---+---+---+---+---+---+" << endl;
        for(int j=0;j<8;++j){
            if(j==0) cout << ' ' << 8-i << ' ';
            if(b[i][j]){
                cout << "| " << *(b[i][j]) << ' ';
            }else{
                cout << "|   ";
            }
        }
        cout << '|' << endl;
    }
    cout << "   +---+---+---+---+---+---+---+---+" << endl;
    cout << "     a   b   c   d   e   f   g   h  " << endl;
}


bool isSquareAttacked(Loc pos, bool color, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    list<Piece*>& ps = color ? whitePieces : blackPieces;
    for (Piece*& piece : ps) {
        for (const Loc& attacked : piece->m_attackedSquares) {
            if (attacked.row == pos.row && attacked.col == pos.col) {
                return true;
            }
        }
    }
    return false;
}

bool isInCheck(bool color, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces) {
    Loc kingPos;
    for (vector<Piece*>& row : BOARD) {
        for (Piece*& piece : row) {
            if (piece && piece->type==KING && piece->m_color == color) {
                kingPos = piece->m_pos;
                break;
            }
        }
    }
    // are any opponents attacking the king
    return isSquareAttacked(kingPos,!color, whitePieces, blackPieces);
}

void simulateMoves(Piece*& p, list<Loc>& moves, bool color, const Loc& tempLoc, bool& foundMove,
        vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    list<Piece*>& os = !color? whitePieces : blackPieces;
    list<Loc> validMovesForPiece, tempAttacks;

    list<Loc>::iterator i=moves.begin();
    for(;i!=moves.end();++i){ 

        Piece* tempPiece = BOARD[i->row][i->col];
        if(tempPiece){ assert(tempPiece->m_color != color); }

        // pretend to capture piece
        list<Piece*>::iterator opponent = find(os.begin(),os.end(), BOARD[i->row][i->col]);
        if(opponent!=os.end()){
            opponent = os.erase(opponent);
        }

        // pretend to move piece
        BOARD[i->row][i->col] = BOARD[tempLoc.row][tempLoc.col];
        BOARD[tempLoc.row][tempLoc.col] = nullptr;
        BOARD[i->row][i->col]->m_pos = *i;


        // Make sure that moving your piece didn't lead to a discovery on your king
        // Update all of the ops
        for(Piece*& ops: os){ ops->updateMovement(BOARD, whitePieces, blackPieces); }

        if(!isInCheck(color, BOARD, whitePieces, blackPieces)){
            validMovesForPiece.push_back(*i);
            foundMove = true;
        }

        BOARD[tempLoc.row][tempLoc.col] = BOARD[i->row][i->col];
        BOARD[tempLoc.row][tempLoc.col]->m_pos = tempLoc;
        BOARD[i->row][i->col] = tempPiece;
        if(tempPiece){ os.push_back(BOARD[i->row][i->col]); }
    }
    moves = validMovesForPiece;
}

void findValidMoves(bool color, int inCheck, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){

    //check every move that the color in check can make.
    //Re-test if they are still in check.
    //Undo if so, otherwise, keep move as an option for that piece.
    //If no moves are found, checkmate.
    bool foundMove = false;
    list<Piece*>& ps = color? whitePieces : blackPieces;
    for(Piece*& p:ps){
        Loc tempLoc = p->m_pos;
        simulateMoves(p, p->m_attackedSquares, color, tempLoc, foundMove, BOARD, whitePieces, blackPieces);
        simulateMoves(p, p->m_nonAttackMoves, color, tempLoc, foundMove, BOARD, whitePieces, blackPieces);
        p->m_pos = tempLoc;
    }

    if(inCheck==1 && !foundMove){
        cout << "CHECKMATE " << (color?"BLACK":"WHITE") << " WINS!" << endl;
        exitProgram(whitePieces, blackPieces);
    }else if(inCheck==0 && !foundMove){
        cout << "STALEMATE " << endl;
        printBoard(BOARD);
        exitProgram(whitePieces, blackPieces);
    }
}




void handlePromotion(Piece*& p, char type, bool color, list<Piece*>& whitePieces, list<Piece*>& blackPieces){

    list<Piece*>& ps = color? whitePieces: blackPieces;
    list<Piece*>::iterator i = find(ps.begin(), ps.end(), p);
    ps.erase(i);

    Loc position = p->m_pos;
    delete p;
    switch(type){
        case 'q':
            p = new Queen(color, position);
            break;
        case 'b':
            p = new Bishop(color, position);
            break;
        case 'n':
            p = new Knight(color, position);
            break;
        case 'r':
            p = new Rook(color, position);
            break;
        default:
            p = new Queen(color, position);
            break;
    }
    
    ps.push_back(p);
}

void capturePiece(Piece*& op, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    if(!op) return;
    list<Piece*>& ps = op->m_color ? whitePieces: blackPieces;
    list<Piece*>::iterator i = find(ps.begin(), ps.end(), op);
    if(i!=ps.end()){
        i = ps.erase(i);
    }
    delete op;
    op = nullptr;
}

pair<Loc,Loc> parseInput(string& move){
    if(move.size() < 4){
        cout << "Error on Notation" << endl;
        return {Loc(),Loc()};
    }
    return {Loc(8-(move[1]-'0'),move[0]-'a'), Loc(8-(move[3]-'0'),move[2]-'a')};
}

// takes moves like: e2e4, e7e5. Promotion: e7e8q
bool movePiece(pair<Loc,Loc>& move, bool turn, char promotion, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    int r1=move.first.row; int c1=move.first.col;
    int r2=move.second.row; int c2=move.second.col;
    Piece* p = BOARD[r1][c1];
    if(p){
        if(p->m_color != turn){
            cout << "Wrong turn" << endl;
            return false;
        }
        if(!p->validMove(move.second, BOARD, whitePieces, blackPieces)){
            cout << "Invalid move" << endl;
            return false;
        }
        p->m_pos.row = r2;
        p->m_pos.col = c2;
        p->m_hasMoved = true;

        // check for promotion
        if(p->type==PAWN && (r2==7 || r2==0)){
            cout << "Promotion types: q, b, n, r\nEx. e7e8q" << endl;
            handlePromotion(p, promotion, p->m_color, whitePieces, blackPieces);
        }

        if(BOARD[r2][c2]){
            // will not trigger in this location if this was an en passant capture
            capturePiece(BOARD[r2][c2], whitePieces, blackPieces);
        }

        BOARD[r2][c2] = p;
        BOARD[r1][c1] = nullptr;
        return true;
    }
    return false;
}


bool Piece::validMove(const Loc& p, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces){
    if(type==KING){
        list<Loc>::iterator itr = m_nonAttackMoves.begin();
        for(;itr!=m_nonAttackMoves.end();++itr){
            if (itr->row==p.row && itr->col==p.col){
                // move the rook
                int side = (p.col-m_pos.col>0)?7:0;
                int newCol = (side==7)?5:3;

                Piece* isRook = BOARD[m_pos.row][side];
                assert(isRook->type==ROOK);
                isRook->m_pos.col = newCol;
                isRook->m_hasMoved = true;
                BOARD[m_pos.row][newCol] = isRook;
                BOARD[m_pos.row][side] = nullptr;
                return true;
            }
        }
    }


    // check forward movement of the pawn
    if(type==PAWN){
        int direction = (m_color?-1:1); // 1 for white, -1 for black

        // PAWN FORWARD MOVEMNET
        list<Loc>::iterator itr = m_nonAttackMoves.begin();
        for(;itr!=m_nonAttackMoves.end();++itr){
            if (itr->row==p.row && itr->col==p.col){
                if(abs(itr->row-m_pos.row) != 2){ return true; }

                // enable en passant for neighboring opponent pawns
                Pawn* opPawn = p.col+1<8?dynamic_cast<Pawn*>(BOARD[p.row][p.col+1]):nullptr;
                if(opPawn && opPawn->m_color!=m_color){
                    enPassants.push_back({moveNumber,{opPawn->m_pos,Loc(p.row-direction,p.col)}});
                    //cout << "En Passant: Move " << moveNumber 
                    //    << " - Opponent pawn at (" << opPawn->m_pos.row << ", " << opPawn->m_pos.col 
                    //    << ") can be captured en passant to (" << p.row-direction << ", " << p.col << ")" 
                    //    << endl;
                }
                opPawn = p.col-1>=0?dynamic_cast<Pawn*>(BOARD[p.row][p.col-1]):nullptr;
                if(opPawn && opPawn->m_color!=m_color){
                    enPassants.push_back({moveNumber,{opPawn->m_pos,Loc(p.row-direction,p.col)}});
                    //cout << "En Passant: Move " << moveNumber 
                    //    << " - Opponent pawn at (" << opPawn->m_pos.row << ", " << opPawn->m_pos.col 
                    //    << ") can be captured en passant to (" << p.row-direction << ", " << p.col << ")" 
                    //    << endl;
                }
                return true;
            }
        }


        // check if move is in the enPassants List
        list<pair<int, pair<Loc,Loc>>>::iterator i = enPassants.begin();
        for(;i!=enPassants.end();++i){
            if (m_pos.row==i->second.first.row && m_pos.col==i->second.first.col &&
                p.row == i->second.second.row && p.col == i->second.second.col){
                i = enPassants.erase(i);

                capturePiece(BOARD[p.row-direction][p.col], whitePieces, blackPieces);
                return true;
            }
        }
    }

    // check attacking options
    list<Loc>::iterator i = m_attackedSquares.begin();
    for(;i!=m_attackedSquares.end();++i){
        if (i->row==p.row && i->col==p.col){
            return true;
        }
    }
    return false;
}

void clearPassants(){
    list<pair<int, pair<Loc,Loc>>>::iterator i = enPassants.begin();
    for(;i!=enPassants.end();++i){
        if(moveNumber - i->first >= 2){
            i = enPassants.erase(i);
        }
    }
}

void readMoves(const string& move, vector<vector<Piece*>>& BOARD) {
    int r1 = 8 - (move[1] - '0'); // Convert move row to board index (8-1 to 8-8)
    int c1 = move[0] - 'a';       // Convert move column to board index (0 for 'a', 1 for 'b', etc.)

    Piece* p = BOARD[r1][c1];
    if (p) {
        cout << "PIECE: " << *p << endl;
        cout << "Piece at " << move << " can attack the following squares:" << endl;
        for (const Loc& loc : p->m_attackedSquares) {
            // Convert board indices to chess notation
            char col = 'a' + loc.col;
            int row = 8 - loc.row;
            cout << "  " << col << row << endl;
        }
        cout << "Piece at " << move << " can move to the following squares:" << endl;
        for (const Loc& loc : p->m_nonAttackMoves) {
            // Convert board indices to chess notation
            char col = 'a' + loc.col;
            int row = 8 - loc.row;
            cout << "  " << col << row << endl;
        }

        if(p->type==PAWN){

            cout << "Pawn at " << move << " can capture En Passant to the following squares:" << endl;
            for (const auto& enPassant : enPassants) {
                if (p->m_pos.row == enPassant.second.first.row && p->m_pos.col == enPassant.second.first.col) {
                    // Convert board indices to chess notation
                    char col = 'a' + enPassant.second.second.col;
                    int row = 8 - enPassant.second.second.row;
                    cout << "  " << col << row << endl;
                }
            }
        }
    } else {
        cout << "No piece in this location." << endl;
    }
}



int main(){
    vector<vector<Piece*>> BOARD(8, vector<Piece*>(8, nullptr));
    list<Piece*> blackPieces, whitePieces;

    // for engine:
    setupEvals();

    // start with white
    bool turn = 1;

    historyOutput << '{';

    setupBoard(BOARD, whitePieces, blackPieces);
    printBoard(BOARD);
    int count = 0;

    while(true){
        updateAllPieces(BOARD, whitePieces, blackPieces);
        if(isInCheck(turn, BOARD, whitePieces, blackPieces)){
            cout << "CHECK" << endl;
            findValidMoves(turn,1, BOARD, whitePieces, blackPieces);
        }else{
            findValidMoves(turn,0, BOARD, whitePieces, blackPieces);
        }

        pair<Loc,Loc> chosenMove;
        string move;
        cout << '\n' << (turn?"White":"Black") << " Move #" << moveNumber << ": ";
        char cont;
        if(1){
            if(count > 10){
                count = 0;
                cin >> cont;
                if(cont=='q') break;
            }else{
                ++count;
            }
            // AI is black (change to if(turn) to play as black)
            int eval = run_engine(BOARD, whitePieces, blackPieces, turn, chosenMove, 3);
            move = string(1, 'a' + chosenMove.first.col) + to_string(8-chosenMove.first.row) +
                string(1, 'a' + chosenMove.second.col) + to_string(8-chosenMove.second.row);
            cout << move << endl;
            cout << "Minimax eval: " << eval << endl;

            // one last update after searching and editing the board in minimax algorithm
            updateAllPieces(BOARD,whitePieces,blackPieces);
            bool inCheck = isInCheck(!turn, BOARD, whitePieces, blackPieces);
            findValidMoves(turn, inCheck, BOARD, whitePieces, blackPieces);
        }else{
            cin >> move;
            if(move=="q"){ break; }
            if(move.front()==':'){
                readMoves(move.substr(1), BOARD);
                continue;
            }
            chosenMove = parseInput(move);
        }

        if(chosenMove.first.row == -1){ continue; }

        historyOutput << '"' << move << "\", ";
        cout << endl;
        if(movePiece(chosenMove, turn, move.back(), BOARD, whitePieces, blackPieces)){
            ++moveNumber;
            // clear outdated en passants
            clearPassants();
            turn = !turn;
        }else{
            readMoves(move.substr(0,2), BOARD);
        }
        printBoard(BOARD);
        usleep(10000);
    }
    exitProgram(whitePieces, blackPieces);
    return 0;
}
