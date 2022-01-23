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
    int blackstones = 0;
    int whitestones = 0;
    int kingi = 0;
    int kingj = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int piece = mind->AtPos(Coord{i, j});
            if (piece == (PIECE | WHITE))
                whitestones++;
            if (piece == (PIECE | BLACK))
                blackstones++;
            if (piece & KING)
            {
                kingi = i;
                kingj = j;
            }
        }
    }
    int kingdist = min(kingi, min(kingj, min(8 - kingi, 8 - kingj)));
    if (mind->winner == WHITE)
        return 1;
    if (mind->winner == BLACK)
        return -1;

    return M_2_PI * atan(whitestones - blackstones + (8 - 2 * kingdist));
}

Minimax::~Minimax()
{
}