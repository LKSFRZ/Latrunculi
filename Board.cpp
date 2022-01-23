#include "Board.h"

Board::Board(int width, int height)
{
    this->width = width;
    this->height = height;
    data = new int[width * height];
    territorymap = new int[width * height];
    reset();
}

Board::Board(int *data)
{
}

vector<Move> Board::GetLegalMoves()
{
    vector<Move> legalmoves;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (Owner(Coord{i, j}) == active_player)
            {
                vector<Move> local = GetLegalMoves(Coord{i, j});
                legalmoves.insert(legalmoves.end(), local.begin(), local.end());
            }
        }
    }
    return legalmoves;
}

vector<Move> Board::GetHistory()
{
    return history;
}

vector<Move> Board::GetLegalMoves(Coord start)
{
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, -1, 0, 1};
    vector<Move> legalmoves;
    for (int i = 0; i < 4; i++)
    {
        int d = 1;
        while (!AtPos(Coord{start.i + d * dx[i], start.j + d * dy[i]}))
        {
            Coord b = Coord{start.i + d * dx[i], start.j + d * dy[i]};
            Move m = Move{CoordToIndex(start), CoordToIndex(b)};
            if (true)
            {
                legalmoves.push_back(Move{CoordToIndex(start), CoordToIndex(Coord{start.i + d * dx[i], start.j + d * dy[i]})});
            }
            d++;
        }
    }
    return legalmoves;
}

bool Board::IsLegal(Move m)
{
    Coord a = IndexToCoord(m.i0);
    Coord b = IndexToCoord(m.i1);
    if ((data[m.i0] & BLACK) == active_player)
    {
        bool free = true;
        if (a.i == b.i)
        {
            int diff = abs(b.j - a.j);
            if (diff == 0)
            {
                return 0;
            }
            int sign = (b.j - a.j) / diff;
            for (int i = 1; i <= diff; i++)
            {
                if (data[CoordToIndex(Coord{a.i, a.j + sign * i})])
                {
                    free = false;
                }
            }
            return free;
        }
        else if (a.j == b.j)
        {
            int diff = abs(b.i - a.i);
            if (diff == 0)
            {
                return 0;
            }
            int sign = (b.i - a.i) / diff;
            for (int i = 1; i <= diff; i++)
            {
                if (data[CoordToIndex(Coord{a.i + sign * i, a.j})])
                {
                    free = false;
                }
            }
            return free;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void Board::Capture(Coord b, int dx, int dy)
{
    Coord anv = Coord{b.i + 2 * dx, b.j + 2 * dy};
    Coord vic = Coord{b.i + dx, b.j + dy};
    if (Anvil(anv) && Owner(vic) == (1 - active_player))
    {
        data[CoordToIndex(vic)] = 0;
    }
}

void Board::MakeMove(Move m)
{
    Coord a = IndexToCoord(m.i0);
    Coord b = IndexToCoord(m.i1);

    data[m.i1] = data[m.i0];
    data[m.i0] = 0;
    /*Capture*/
    Capture(b, 0, 1);
    Capture(b, 0, -1);
    Capture(b, 1, 0);
    Capture(b, -1, 0);

    history.push_back(m);
    updateScores();
    //Switch Player
    active_player = 1 - active_player;
}

int Board::CoordToIndex(Coord x)
{
    return x.i + 8 * x.j;
}

void Board::reset()
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            data[CoordToIndex({i, j})] = 0;
            territorymap[CoordToIndex({i, j})] = -1;
        }
    }
    for (int i = 0; i < width; i++)
    {
        data[CoordToIndex(Coord{i, 0})] = PIECE | BLACK;
        data[CoordToIndex(Coord{i, 1})] = PIECE | BLACK;
        data[CoordToIndex(Coord{i, height - 2})] = PIECE | WHITE;
        data[CoordToIndex(Coord{i, height - 1})] = PIECE | WHITE;
    }
    score[WHITE] = 16;
    score[BLACK] = 16;
    active_player = WHITE;
    winner = -1;
    history.clear();
}

vector<Coord> Board::neighbors(Coord x)
{
    vector<Coord> n;
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, -1, 0, 1};
    for (int i = 0; i < 4; i++)
    {
        if (AtPos(Coord{x.i + dx[i], x.j + dy[i]}) != -1) // is on board
        {
            n.push_back(Coord{x.i + dx[i], x.j + dy[i]});
        }
    }
    return n;
}

bool contains(int x, vector<int> vec)
{
    bool con = false;
    for (int i : vec)
    {
        if (i == x)
        {
            con = true;
        }
    }
    return con;
}

void Board::updateScores()
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            territorymap[CoordToIndex(Coord{i, j})] = -1;
        }
    }
    int nterritories = 10;
    score[WHITE] = 0;
    score[BLACK] = 0;
    vector<int> whiteterritories, blackterritories;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (territorymap[CoordToIndex(Coord{i, j})] == -1)
            {
                if (!AtPos(Coord{i, j}))
                {
                    bool bordersblack = false;
                    bool borderswhite = false;
                    vector<Coord> Points;
                    Points.push_back(Coord{i, j});
                    while (!Points.empty())
                    {
                        Coord current = Points.back();
                        Points.pop_back();
                        if (territorymap[CoordToIndex(current)] == -1)
                        {
                            if (AtPos(current) == 0)
                            {
                                territorymap[CoordToIndex(current)] = nterritories;
                                vector<Coord> currentneighbors = neighbors(current);
                                Points.insert(Points.end(), currentneighbors.begin(), currentneighbors.end());
                            }
                        }
                        if (AtPos(current) == (PIECE | BLACK))
                        {
                            territorymap[CoordToIndex(current)] = -3;
                            bordersblack = true;
                        }
                        if (AtPos(current) == (PIECE | WHITE))
                        {
                            territorymap[CoordToIndex(current)] = -2;
                            borderswhite = true;
                        }
                    }
                    if (bordersblack && !borderswhite)
                    {
                        blackterritories.push_back(nterritories);
                    }
                    else if (borderswhite && !bordersblack)
                    {
                        whiteterritories.push_back(nterritories);
                    }
                    nterritories++;
                }
            }
        }
    }
    // for(int i: blackterritories)
    // {
    //     printf("black territory: %i\n", i);
    // }
    // for(int i: whiteterritories)
    // {
    //     printf("white territory: %i\n", i);
    // }
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            int index = CoordToIndex(Coord{i, j});
            if (contains(territorymap[index], blackterritories) || AtPos(Coord{i, j}) == (BLACK | PIECE))
            {
                territorymap[CoordToIndex(Coord{i, j})] = BLACK | PIECE;
                score[BLACK]++;
            }
            else
            {
                if (contains(territorymap[index], whiteterritories) || AtPos(Coord{i, j}) == (WHITE | PIECE))
                {
                    territorymap[CoordToIndex(Coord{i, j})] = WHITE | PIECE;
                    score[WHITE]++;
                }
                else
                {
                    territorymap[CoordToIndex(Coord{i, j})] = 0;
                }
            }
        }
    }
    // printf("scores %i %i\n", score[WHITE], score[BLACK]);
}

void Board::undo()
{
    if (!history.empty())
    {
        vector<Move> copy;
        copy = history;

        reset();
        copy.pop_back();
        for (Move m : copy)
        {
            MakeMove(m);
        }
    }
}

void Board::copy_position(Board *other)
{
}

int Board::Owner(Coord x)
{
    if (AtPos(x) <= 0) // Empty or OffBoard
    {
        return -1;
    }
    return AtPos(x) & BLACK;
}

bool Board::Anvil(Coord x)
{
    //Throne
    // if (x.i == 4 && x.j == 4 && AtPos(x) == 0)
    // {
    //     return true;
    // }
    return Owner(x) == active_player;
}

Coord Board::IndexToCoord(int i)
{
    Coord x{i % width, i / width};
    return x;
}

int Board::AtPos(Coord x)
{
    if (x.i < 0 || x.i >= width || x.j < 0 || x.j >= height)
    {
        return -1;
    }
    return data[CoordToIndex(x)];
}

int Board::TerritoryAtPos(Coord x)
{
    if (x.i < 0 || x.i >= width || x.j < 0 || x.j >= height)
    {
        return -1;
    }
    return territorymap[CoordToIndex(x)];
}

int Board::Player()
{
    return active_player;
}

Board::~Board()
{
    delete data, territorymap;
}

bool operator==(Coord &lhs, Coord &rhs)
{
    return (lhs.i == rhs.i && lhs.j == rhs.j);
}