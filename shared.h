
#include <bits/stdc++.h>
using namespace std;

struct Loc{
    Loc(int r, int c): row(r), col(c){}
    Loc(): row(-1), col(-1){}
    int row, col;
};


enum PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

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
    int m_value;
    PieceType type;

    friend ostream& operator<<(ostream& os, const Piece& p){ os << p.m_piece; return os; }
    virtual bool validMove(const Loc& p, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces);

    // for updating attacked squares
    virtual void updateMovement(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces) = 0;
    virtual Piece* clone() const = 0;
};


void findValidMoves(bool color, bool inCheck, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces);
void updateAllPieces(vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces, Piece* exceptThis=nullptr);
void printBoard(vector<vector<Piece*>>& b);
void readMoves(const string& move, vector<vector<Piece*>>& BOARD);
bool isInCheck(bool color, vector<vector<Piece*>>& BOARD, list<Piece*>& whitePieces, list<Piece*>& blackPieces);
bool isSquareAttacked(Loc pos, bool color, list<Piece*>& whitePieces, list<Piece*>& blackPieces);

int minimax(int depth, int alpha, int beta, bool maximizingPlayer, bool color, vector<vector<Piece*>>& BOARD,
        list<Piece*>& whitePieces, list<Piece*>& blackPieces, pair<Loc, Loc>& bestMove);
void setupEvals();

