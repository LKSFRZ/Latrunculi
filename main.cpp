#include <iostream>
#include "Board.h"
#include "IO.h"
#include "Minimax.h"

using namespace std;

int main(int argc, char const *argv[])
{
    Board *board;
    if (argc == 3)
    {
        board = new Board(atoi(argv[1]), atoi(argv[2]));
    }
    else if (argc == 4)
    {
        board = new Board(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    }
    else
    {
        board = new Board(8, 8);
    }

    for (int i = 0; i < board->width; i++)
    {
        for (int j = 0; j < board->height; j++)
        {
            cout << board->AtPos(Coord{i, j}) << " ";
        }
        cout << endl;
    }

    bool quit = false;

    IO *renderer = new IO(&quit, board);

    cout << "Created Board and Renderer" << endl;
    // Minimax * Player2 = new Minimax();
    // Minimax * Player1 = new Minimax();
    cout << "Created Players" << endl;
    while (!quit)
    {
        if (board->winner == -1)
        {
            if (board->Player() == WHITE)
            {
                renderer->GetMove(board->Player());
                // Move m;
                // Player1->GetMove(board->GetHistory(),4, -1, 1, &m);
                // board->MakeMove(m);
            }
            else if (board->Player() == BLACK)
            {
                renderer->GetMove(board->Player());
            }
            // for(Move m: board->GetLegalMoves())
            // {
            //     // cout << m.i0 << "->" << m.i1 << endl;
            // }
        }
        renderer->WaitOnQuit();
    }

    return 0;
}