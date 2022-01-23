#ifndef MINIMAX_H
#define MINIMAX_H

#include<stdio.h>

#include "Board.h"
#include "math.h"

class Minimax
{
private:
    Board* mind;
public:
    Minimax(/* args */);
    double GetMove(vector<Move> node, int depth, double alpha, double beta, Move * bestmove);
    double heuristic(vector<Move>);
    double heuristic();
    ~Minimax();
};



#endif