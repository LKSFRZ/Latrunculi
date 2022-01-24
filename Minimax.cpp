#include "Minimax.h"

Minimax::Minimax()
{
    mind = new Board(8,8);
}

double Minimax::GetMove(vector<Move> node, int depth, double alpha, double beta, Move *bestmove)
{
    mind->reset();
    for (Move m : node)
    {
        mind->MakeMove(m);
    }
    if (depth == 0)
    {
        return heuristic();
    }
    vector<Move> legalmoves = mind->GetLegalMoves();
    if (mind->Player() == WHITE)
    {
        double value = -1;
        vector<Move> child;
        for (Move m : legalmoves)
        {
            child = node;
            child.push_back(m);
            double childvalue = GetMove(child, depth - 1, alpha, beta, NULL);
            if (value < childvalue)
            {
                value = childvalue;
                if (bestmove)
                    *bestmove = m;
            }
            if (value >= beta)
            {
                break;
            }
            if (alpha <= value)
            {
                alpha = value;
            }
        }
        return value;
    }
    else
    {
        double value = 1;
        vector<Move> child;
        for (Move m : legalmoves)
        {
            child = node;
            child.push_back(m);
            double childvalue = GetMove(child, depth - 1, alpha, beta, NULL);
            if (value > childvalue)
            {
                value = childvalue;
                if (bestmove)
                    *bestmove = m;
            }
            if (value <= alpha)
            {
                break;
            }
            if (beta >= value)
            {
                beta = value;
            }
        }
        return value;
    }
}

double Minimax::heuristic(vector<Move> node)
{
    mind->reset();
    for (Move m : node)
    {
        mind->MakeMove(m);
    }
    return heuristic();
}

double Minimax::heuristic()
{
    
    return M_2_PI *  atan(mind->score[WHITE] - mind->score[BLACK]);
}

Minimax::~Minimax()
{
}