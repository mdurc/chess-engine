#include <bits/stdc++.h>
using namespace std;

struct Loc{
    Loc(int r, int c): row(r), col(c){}
    int row, col;
};



// for simplicities sake, we are including Pawns as Pieces
class Piece {
public:
    virtual ~Piece() = default;

    bool m_color; // color 0 is black, 1 is white
    string m_piece;
    list<Loc> m_attackedSquares;
    list<Loc> m_nonAttackMoves; // for pawns and kings only
    Loc m_pos{0,0};
    bool m_hasMoved = false;

    friend ostream& operator<<(ostream& os, const Piece& p){ os << p.m_piece; return os; }
    virtual bool validMove(const Loc& p);

    // for updating attacked squares
    virtual void updateMovement() = 0;
    virtual Piece* clone() const = 0;
};


// GLOBAL
vector<vector<Piece*>> BOARD(8, vector<Piece*>(8, nullptr));
list<Piece*> blackPieces, whitePieces;
int moveNumber = 1;
ofstream output("history.txt");

// <turn #, <Location of piece that can enPassant, the square it attacks>>
list<pair<int, pair<Loc,Loc>>> enPassants;


void deletePieces(list<Piece*>& pieces){
    cout << "Size: " << pieces.size() << endl;
    for(Piece*& p: pieces){
        delete p;
        p = nullptr;
    }
}

void exitProgram(){
    output.close();
    deletePieces(whitePieces);
    deletePieces(blackPieces);
    exit(1);
}

class SlidingPiece : public Piece {
protected:
    void addMovesInDirection(int rowDelta, int colDelta) {
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
    }


    void updateMovement(){
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
    Piece* clone() const {
        return new Pawn(*this);
    }
};



class Rook : public SlidingPiece {
public:
    Rook(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♖" : "♜");
        m_pos = start;
    }
    void updateMovement(){
        m_attackedSquares.clear();
        addMovesInDirection(1, 0);  // Up
        addMovesInDirection(-1, 0); // Down
        addMovesInDirection(0, 1);  // Right
        addMovesInDirection(0, -1); // Left
    }
    Piece* clone() const {
        return new Rook(*this);
    }
};

class Knight : public Piece {
public:
    Knight(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♘" : "♞");
        m_pos = start;
    }
    void updateMovement(){
        m_attackedSquares.clear();
        int row = m_pos.row;
        int col = m_pos.col;
        
        int rowChange[] = {2, 2, 1, 1, -1, -1, -2, -2};
        int colChange[] = {1, -1, 2, -2, 2, -2, 1, -1};

        for(int i=0;i<8;++i){
            int newRow = row + rowChange[i];
            int newCol = col + colChange[i];
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                if(!BOARD[newRow][newCol] || BOARD[newRow][newCol]->m_color!=m_color){
                    m_attackedSquares.push_back(Loc(newRow, newCol));
                }
            }
        }
    }
    Piece* clone() const {
        return new Knight(*this);
    }
};

class Bishop : public SlidingPiece {
public:
    Bishop(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♗" : "♝");
        m_pos = start;
    }
    void updateMovement(){
        m_attackedSquares.clear();
        addMovesInDirection(1, 1);   // Diagonal up-right
        addMovesInDirection(1, -1);  // Diagonal up-left
        addMovesInDirection(-1, 1);  // Diagonal down-right
        addMovesInDirection(-1, -1); // Diagonal down-left
    }
    Piece* clone() const {
        return new Bishop(*this);
    }
};

class Queen : public SlidingPiece {
public:
    Queen(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♕" : "♛");
        m_pos = start;
    }
    void updateMovement(){
        m_attackedSquares.clear();
        // bishop + rook movement
        addMovesInDirection(1, 1);   // Diagonal up-right
        addMovesInDirection(1, -1);  // Diagonal up-left
        addMovesInDirection(-1, 1);  // Diagonal down-right
        addMovesInDirection(-1, -1); // Diagonal down-left
        addMovesInDirection(1, 0);  // Up
        addMovesInDirection(-1, 0); // Down
        addMovesInDirection(0, 1);  // Right
        addMovesInDirection(0, -1); // Left
    }
    Piece* clone() const {
        return new Queen(*this);
    }
};

bool isSquareAttacked(Loc x, bool c);

class King : public Piece {
public:
    King(bool c, const Loc& start) {
        m_color = c;
        m_piece = (c? "♔" : "♚");
        m_pos = start;
    }
    bool canCastle(int rookCol, int kingCol1, int kingCol2) {
        Rook* isRook = dynamic_cast<Rook*>(BOARD[m_pos.row][rookCol]);
        if (isRook && !isRook->m_hasMoved) {
            // check if all squares between King and Rook are empty
            int lim = max(rookCol, m_pos.col);
            for (int col = min(rookCol, m_pos.col) + 1; col < lim; ++col) {
                if (BOARD[m_pos.row][col] != nullptr) {
                    return false;
                }
            }
            // check if King would pass through or end up in check
            for (int col : {m_pos.col, kingCol1, kingCol2}) {
                if (isSquareAttacked(Loc(m_pos.row, col), !m_color)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    };

    void updateMovement(){
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
        if (!m_hasMoved) {
            // make sure that all squares between rook and 
            // Kingside castling (short castling)
            if (canCastle(7, m_pos.col + 1, m_pos.col + 2)) {
                m_nonAttackMoves.push_back(Loc(m_pos.row, m_pos.col + 2));
            }

            // Queenside castling (long castling)
            if (canCastle(0, m_pos.col - 1, m_pos.col - 2)) {
                m_nonAttackMoves.push_back(Loc(m_pos.row, m_pos.col - 2));
            }
        }
    }
    Piece* clone() const {
        return new King(*this);
    }
};

void updateAllPieces(Piece* exceptThis = nullptr){
    for(Piece*& p: blackPieces){
        if(exceptThis == p){ continue; }
        p->updateMovement();
    }
    for(Piece*& p: whitePieces){
        if(exceptThis == p){ continue; }
        p->updateMovement();
    }
}

void setupBoard(){
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

void printBoard(){
    for(int i=0;i<8;++i){
        cout << "   +---+---+---+---+---+---+---+---+" << endl;
        for(int j=0;j<8;++j){
            if(j==0) cout << ' ' << 8-i << ' ';
            if(BOARD[i][j]){
                cout << "| " << *(BOARD[i][j]) << ' ';
            }else{
                cout << "|   ";
            }
        }
        cout << '|' << endl;
    }
    cout << "   +---+---+---+---+---+---+---+---+" << endl;
    cout << "     a   b   c   d   e   f   g   h  " << endl;
}


bool isSquareAttacked(Loc pos, bool color){
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

bool isInCheck(bool color) {
    Loc kingPos(-1,-1);
    for (vector<Piece*>& row : BOARD) {
        for (Piece*& piece : row) {
            if (piece && dynamic_cast<King*>(piece) && piece->m_color == color) {
                kingPos = piece->m_pos;
                break;
            }
        }
    }
    // are any opponents attacking the king
    return isSquareAttacked(kingPos,!color);
}

void simulateMoves(Piece*& p, list<Loc>& moves, bool color, const Loc& tempLoc, bool& foundMove){
    list<Piece*>& os = !color? whitePieces : blackPieces;
    list<Loc> validMovesForPiece, tempAttacks;

    list<Loc>::iterator i=moves.begin();
    for(;i!=moves.end();++i){ 

        Piece* tempPiece = BOARD[i->row][i->col];

        // pretend to capture piece
        list<Piece*>::iterator opponent = find(os.begin(),os.end(), BOARD[i->row][i->col]);
        if(opponent!=os.end()){
            //cout << "Pretend Capture" << endl;
            opponent = os.erase(opponent);
        }

        // pretend to move piece
        BOARD[tempLoc.row][tempLoc.col] = nullptr;
        p->m_pos = *i;
        BOARD[i->row][i->col] = p;

        // Make sure that moving your piece didn't lead to a discovery on your king
        // Update all of the ops
        for(Piece*& ops: os){ ops->updateMovement(); }

        if(!isInCheck(color)){
            validMovesForPiece.push_back(*i);
            //cout << "Valid move: " << tempLoc.row << ","<<tempLoc.col << " to " << i->row << ","<<i->col << endl;
            foundMove = true;
        }

        BOARD[tempLoc.row][tempLoc.col] = p;
        BOARD[tempLoc.row][tempLoc.col]->m_pos = tempLoc;
        BOARD[i->row][i->col] = tempPiece;
        if(tempPiece){
            os.push_back(BOARD[i->row][i->col]);
        }
    }
    moves = validMovesForPiece;
}

void findValidMoves(bool color, bool inCheck){
    // update them, then sort them after
    //updateAllPieces();

    //check every move that the color in check can make.
    //Re-test if they are still in check.
    //Undo if so, otherwise, keep move as an option for that piece.
    //If no moves are found, checkmate.
    bool foundMove = false;
    list<Piece*>& ps = color? whitePieces : blackPieces;
    for(Piece*& p:ps){
        Loc tempLoc = p->m_pos;
        simulateMoves(p, p->m_attackedSquares, color, tempLoc, foundMove);
        simulateMoves(p, p->m_nonAttackMoves, color, tempLoc, foundMove);
        p->m_pos = tempLoc;
    }

    if(inCheck && !foundMove){
        cout << "CHECKMATE " << (color?"BLACK":"WHITE") << " WINS!" << endl;
        exitProgram();
    }else if(!inCheck && !foundMove){
        cout << "STALEMATE " << endl;
        exitProgram();
    }
}




void handlePromotion(Piece*& p, char type, bool color){

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

void capturePiece(Piece*& op){
    if(!op) return;
    list<Piece*>& ps = op->m_color ? whitePieces: blackPieces;
    list<Piece*>::iterator i = find(ps.begin(), ps.end(), op);
    if(i!=ps.end()){
        //cout << "Successfully removed from the " << (op->m_color?"white":"black") << " pieces list" << endl;
        i = ps.erase(i);
    }
    delete op;
    op = nullptr;
}

// takes moves like: e2e4, e7e5. Promotion: e7e8q
bool movePiece(string& move, bool turn){
    if(move.size() < 4){ cout << "Error on Notation" << endl; return false; }
    //parse
    int r1,r2,c1,c2;
    r1 = 8-(move[1]-'0'); c1 = move[0]-'a';
    r2 = 8-(move[3]-'0'); c2 = move[2]-'a';

    Piece* p = BOARD[r1][c1];
    if(p){
        if(p->m_color != turn){
            cout << "Wrong turn" << endl;
            return false;
        }
        if(!p->validMove(Loc(r2,c2))){
            cout << "Invalid move" << endl;
            return false;
        }
        p->m_pos.row = r2;
        p->m_pos.col = c2;
        p->m_hasMoved = true;

        // check for promotion
        Pawn* isPawn = dynamic_cast<Pawn*>(p);
        if(isPawn && (r2==7 || r2==0)){
            if(move.size()!=5){
                cout << "Promotion types: q, b, n, r\nEx. e7e8q" << endl;
            }
            handlePromotion(p, move.back(), p->m_color);
        }

        if(BOARD[r2][c2]){
            // will not trigger in this location if this was an en passant capture
            capturePiece(BOARD[r2][c2]);
        }

        BOARD[r2][c2] = p;
        BOARD[r1][c1] = nullptr;
        return true;
    }
    return false;
}


bool Piece::validMove(const Loc& p){
    King* isKing = dynamic_cast<King*>(this);
    if(isKing){
        list<Loc>::iterator itr = isKing->m_nonAttackMoves.begin();
        for(;itr!=isKing->m_nonAttackMoves.end();++itr){
            if (itr->row==p.row && itr->col==p.col){
                // move the rook
                int side = (p.col-m_pos.col>0)?7:0;
                int newCol = (side==7)?5:3;
                Rook* isRook = dynamic_cast<Rook*>(BOARD[m_pos.row][side]);
                assert(isRook);
                isRook->m_pos.col = newCol;
                isRook->m_hasMoved = true;
                BOARD[m_pos.row][newCol] = isRook;
                BOARD[m_pos.row][side] = nullptr;
                return true;
            }
        }
    }


    // check forward movement of the pawn
    Pawn* isPawn = dynamic_cast<Pawn*>(this);
    if(isPawn){
        int direction = (m_color?-1:1); // 1 for white, -1 for black

        // PAWN FORWARD MOVEMNET
        list<Loc>::iterator itr = isPawn->m_nonAttackMoves.begin();
        for(;itr!=isPawn->m_nonAttackMoves.end();++itr){
            if (itr->row==p.row && itr->col==p.col){
                if(abs(itr->row-m_pos.row) != 2){ return true; }

                // enable en passant for neighboring opponent pawns
                Pawn* opPawn = p.col+1<8?dynamic_cast<Pawn*>(BOARD[p.row][p.col+1]):nullptr;
                if(opPawn && opPawn->m_color!=m_color){
                    enPassants.push_back({moveNumber,{opPawn->m_pos,Loc(p.row-direction,p.col)}});
                    cout << "En Passant: Move " << moveNumber 
                        << " - Opponent pawn at (" << opPawn->m_pos.row << ", " << opPawn->m_pos.col 
                        << ") can be captured en passant to (" << p.row-direction << ", " << p.col << ")" 
                        << endl;
                }
                opPawn = p.col-1>=0?dynamic_cast<Pawn*>(BOARD[p.row][p.col-1]):nullptr;
                if(opPawn && opPawn->m_color!=m_color){
                    enPassants.push_back({moveNumber,{opPawn->m_pos,Loc(p.row-direction,p.col)}});
                    cout << "En Passant: Move " << moveNumber 
                        << " - Opponent pawn at (" << opPawn->m_pos.row << ", " << opPawn->m_pos.col 
                        << ") can be captured en passant to (" << p.row-direction << ", " << p.col << ")" 
                        << endl;
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

                capturePiece(BOARD[p.row-direction][p.col]);
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

void readMoves(const string& move) {
    int r1 = 8 - (move[1] - '0'); // Convert move row to board index (8-1 to 8-8)
    int c1 = move[0] - 'a';       // Convert move column to board index (0 for 'a', 1 for 'b', etc.)

    Piece* p = BOARD[r1][c1];
    if (p) {
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

        Pawn* isPawn = dynamic_cast<Pawn*>(p);
        if(isPawn){

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
    // start with white
    bool turn = 1;

    output << '{';

    setupBoard();
    printBoard();

    // checkmate
    vector<string> moves = {"e2e4", "d7d5", "e4d5", "e7e5", "d5e6", "b8d7",
        "e6d7", "a7a5", "e8e7", "d7c8q", "g8f6","d1e2", "f6e4", "d2d3", "e4c5",
        "d8e8", "e2e4", "e7f6", "h2h4", "a7a5", "c1g5", "a5a4", "a5a4"};

    //defense to check
    //vector<string> moves = {"e2e4", ":g8", "d7d5", "f1b5", "b8d7"};

    //castling
    //vector<string> moves{"e2e4", "d7d5", "f1b5", "b8c6", "e4d5", "e7e5", "d5e6", "d8e7", "g1f3", "c8e6", ":e1", "e1g1", "e8c8", "q"};

    for(int i=0;i<moves.size();++i){
    //while(true){
        updateAllPieces();
        if(isInCheck(turn)){
            cout << "CHECK" << endl;
            findValidMoves(turn,1);
        }else{
            findValidMoves(turn,0);
        }
        string move;
        cout << '\n' << (turn?"White":"Black") << " Move #" << moveNumber << ": ";
        move = moves[i];
        //cin >> move;
        output << '"' << move << "\", ";
        cout << endl;
        if(move=="q"){ break; }
        if(move.front()==':'){
            readMoves(move.substr(1));
            continue;
        }
        else if(movePiece(move, turn)){
            ++moveNumber;
            // clear outdated en passants
            clearPassants();
            turn = !turn;
        }
        printBoard();

        cout << "white Size: " << whitePieces.size() << endl;
        cout << "black Size: " << blackPieces.size() << endl;
    }
    exitProgram();
    return 0;
}
