#ifndef BOARD_H
#define BOARD_H

#define WHITE 0
#define BLACK 1
#define PIECE 2
#define KING 4

#include <vector>
#include <stdio.h>
#include "math.h"

using namespace std;

struct Move
{
    int i0, i1;
};

struct Coord
{
    int i, j;
};

bool operator==(Coord &lhs, Coord &rhs);

class Board
{
private:
    int width, height;
    int *data;
    int active_player;

    int *territorymap;
    vector<Move> history;

public:
    int score[2];
    int winner = -1;
    Board(int width, int height);
    Board(int *data);
    vector<Move> GetLegalMoves();
    vector<Move> GetLegalMoves(Coord start);
    bool IsLegal(Move m);
    void MakeMove(Move m);
    int CoordToIndex(Coord x);
    Coord IndexToCoord(int i);
    int AtPos(Coord x);
    int Player();
    bool Anvil(Coord);
    int Owner(Coord);
    void Capture(Coord, int, int);
    void undo();
    void reset();
    void copy_position(Board *);
    vector<Move> GetHistory();
    void updateScores();
    vector<Coord> neighbors(Coord x);
    int TerritoryAtPos(Coord x);
    ~Board();
};

#endif