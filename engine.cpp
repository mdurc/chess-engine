#include <bits/stdc++.h>
#include "shared.h"
using namespace std;


// Move will be chosen based on postion rating, value of capturing an opponents piece, checkmate
// After choosing one option, try and find what the opponent will do, and if they have move.first higher rating than our previous move, we scrap our move.
const vector<int> pawnEvalBlack = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, -20, -20, 10, 10, 5,
    5, -5, -10, 0, 0, -10, -5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, 5, 10, 25, 25, 10, 5, 5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0
};
vector<int> pawnEvalWhite = pawnEvalBlack;


const vector<int> knightEval = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -5
};

const vector<int> bishopEvalBlack = {
    -20, -10, -10, -10, -10, -10, -10, -2,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -10, 10, 10, 10, 10, 10, 1, -10,
    -10, 0, 10, 10, 10, 10, 0,-10,
    -10, 5, 5, 10, 10, 5, 5, -0,
    -10, 0, 5, 10, 10, 5, 0, -0,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -10, -10, -10, -10, -2
};
vector<int> bishopEvalWhite = bishopEvalBlack;

const vector<int> rookEvalBlack = {
    0, 0, 0, 5, 5, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    5, 10, 10, 10, 10, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};
vector<int> rookEvalWhite = rookEvalBlack;

const vector<int> queenEval = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -2
};

const vector<int> kingEvalBlack = {
    20, 30, 10, 0, 0, 10, 30, 20,
    20, 20, 0, 0, 0, 0, 20, 20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -3
};
vector<int> kingEvalWhite = kingEvalBlack;

void setupEvals(){
    reverse(pawnEvalWhite.begin(), pawnEvalWhite.end());
    reverse(bishopEvalWhite.begin(), bishopEvalWhite.end());
    reverse(rookEvalWhite.begin(), rookEvalWhite.end());
    reverse(kingEvalWhite.begin(), kingEvalWhite.end());
}


int evaluateBoard(vector<vector<Piece*>>& board, list<Piece*>& whitePieces, list<Piece*>& blackPieces, bool color, bool noMoves) {
    updateAllPieces(board,whitePieces,blackPieces);
    findValidMoves(color, 0, board, whitePieces, blackPieces);

    int score = 0;

    // checks and checkmate
    if(isInCheck(color, board, whitePieces, blackPieces)){
        score -= noMoves?100000 :50;
    }else if(isInCheck(!color, board, whitePieces, blackPieces)){
        score += noMoves?100000 :50;
    }

    for(int rank=0;rank<8;++rank){
        for(int file=0;file<8;++file){
            Piece* piece = board[rank][file];
            int position = rank*8 + file;
            if (piece != nullptr) {
                if (piece->m_color) {
                    int temp = 0;
                    // white
                    temp += piece->m_value;
                    if(piece->type == PAWN){
                        temp += pawnEvalWhite[position];
                    }else if(piece->type == KNIGHT){
                        temp += knightEval[position];
                    }else if(piece->type == BISHOP){
                        temp += bishopEvalWhite[position];
                    }else if(piece->type == ROOK){
                        temp += rookEvalWhite[position];
                    }else if(piece->type == QUEEN){
                        temp += queenEval[position];
                    }else{
                        temp += kingEvalWhite[position];
                    }
                    if(color){ score += temp; }
                    else{ score -= temp; }
                } else {
                    // black
                    int temp = 0;
                    temp += piece->m_value;
                    if(piece->type == PAWN){
                        temp += pawnEvalBlack[position];
                    }else if(piece->type == KNIGHT){
                        temp += knightEval[position];
                    }else if(piece->type == BISHOP){
                        temp += bishopEvalBlack[position];
                    }else if(piece->type == ROOK){
                        temp += rookEvalBlack[position];
                    }else if(piece->type == QUEEN){
                        temp += queenEval[position];
                    }else{
                        temp += kingEvalBlack[position];
                    }
                    if(!color){ score += temp; }
                    else{ score -= temp; }
                }
            }
        }
    }

    return score;
}


list<pair<Loc,Loc>> generateMoves(list<Piece*>& pieces){
    list<pair<Loc,Loc>> moves;
    for(Piece*& p: pieces){
        for(Loc l: p->m_attackedSquares){
            moves.push_back({p->m_pos, l});
        }
        for(Loc l: p->m_nonAttackMoves){
            moves.push_back({p->m_pos, l});
        }
    }
    return moves;
}

int minimax(int depth, int alpha, int beta, bool maximizingPlayer, bool color, vector<vector<Piece*>>& board,
        list<Piece*>& whitePieces, list<Piece*>& blackPieces, pair<Loc, Loc>& bestMove) {

    updateAllPieces(board,whitePieces,blackPieces);
    findValidMoves(color, 0, board, whitePieces, blackPieces);

    list<pair<Loc,Loc>> moves = color? generateMoves(whitePieces): generateMoves(blackPieces);
    if (moves.empty()) { return evaluateBoard(board, whitePieces, blackPieces, maximizingPlayer?color:!color, 1); }

    if (depth == 0) { return evaluateBoard(board, whitePieces, blackPieces, maximizingPlayer?color:!color, 0); }

    list<Piece*>& os = !color? whitePieces : blackPieces;
    if (maximizingPlayer) {
        int maxEval = numeric_limits<int>::min();
        for (const pair<Loc,Loc>& move : moves) {

            // apply move
            Piece* temp = board[move.second.row][move.second.col];
            list<Piece*>::iterator opponent = find(os.begin(),os.end(), board[move.second.row][move.second.col]);
            if(opponent!=os.end()){ opponent = os.erase(opponent); }
            board[move.second.row][move.second.col] = board[move.first.row][move.first.col];
            board[move.first.row][move.first.col] = nullptr;
            board[move.second.row][move.second.col]->m_pos = move.second;

            bool moved = board[move.second.row][move.second.col]->m_hasMoved;
            board[move.second.row][move.second.col]->m_hasMoved = true;

            int eval = minimax(depth - 1, alpha, beta, false, (color?0:1), board, whitePieces, blackPieces, bestMove);

            // undo move
            board[move.first.row][move.first.col] = board[move.second.row][move.second.col];
            board[move.first.row][move.first.col]->m_pos = move.first;
            board[move.first.row][move.first.col]->m_hasMoved = moved;
            board[move.second.row][move.second.col] = temp;
            if(temp){ os.push_back(board[move.second.row][move.second.col]); }

            if (eval > maxEval) {
                maxEval = eval;
                bestMove = move;
            }
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = numeric_limits<int>::max();

        for (const pair<Loc,Loc>& move : moves) {
            // apply move
            Piece* temp = board[move.second.row][move.second.col];
            list<Piece*>::iterator opponent = find(os.begin(),os.end(), temp);
            if(opponent!=os.end()){ opponent = os.erase(opponent); }
            board[move.second.row][move.second.col] = board[move.first.row][move.first.col];
            board[move.first.row][move.first.col] = nullptr;
            board[move.second.row][move.second.col]->m_pos = move.second;

            bool moved = board[move.second.row][move.second.col]->m_hasMoved;
            board[move.second.row][move.second.col]->m_hasMoved = true;

            int eval = minimax(depth - 1, alpha, beta, true, (color?0:1), board, whitePieces, blackPieces, bestMove);

            // undo move
            board[move.first.row][move.first.col] = board[move.second.row][move.second.col];
            board[move.first.row][move.first.col]->m_pos = move.first;
            board[move.first.row][move.first.col]->m_hasMoved = moved;
            board[move.second.row][move.second.col] = temp;
            if(temp){ os.push_back(board[move.second.row][move.second.col]); }

            if (eval < minEval) {
                minEval = eval;
            }
            beta = min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}
