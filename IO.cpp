#include "IO.h"

IO::IO(bool *quit, Board *board)
{
    this->quit = quit;
    this->board = board;

    TTF_Init();
    scale = WIDTH/(max(board->width, board->height) + 2);
    Sans = TTF_OpenFont("fonts/FreeSerifBold.ttf", (scale * 62)/160);
    if (Sans == NULL)
    {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow("Latrunculi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    SDL_SetWindowResizable(gWindow,
                           SDL_TRUE);

    scale = WIDTH/(max(board->width, board->height) + 2);
    loadAssets();
    selected = -1;
}

SDL_Surface *loadSurface(std::string path)
{
    //Load image at specified path
    SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }

    return loadedSurface;
}

void IO::plotboard()
{
    SDL_Rect drect;
    int r, g, b;
    // printf("board dimensions; %i %i\n", board->height, board->width);
    for (int i = 0; i < board->width; i++)
    {
        for (int j = 0; j < board->height; j++)
        {
            drect.x = (i + 1) * scale;
            drect.y = (j + 1) * scale;
            drect.w = scale;
            drect.h = scale;
            if ((i + j) % 2)
            {
                r = 0x80;
                g = 0x4C;
                b = 0x36;
            }
            else
            {
                r = 0xC1;
                g = 0x9A;
                b = 0x6C;
            }
            if (!board->GetHistory().empty())
            {
                Move lastmove = board->GetHistory().back();
                if (board->CoordToIndex(Coord{i, j}) == lastmove.i0 || board->CoordToIndex(Coord{i, j}) == lastmove.i1)
                {
                    g += 0x30;
                    r += 0x30;
                }
            }
            if (showscore)
            {
                if (board->TerritoryAtPos(Coord{i, j}) == (BLACK | PIECE))
                {
                    r = r * 0.75;
                    g = g * 0.75;
                    b = b * 0.75;
                }
                if (board->TerritoryAtPos(Coord{i, j}) == (WHITE | PIECE))
                {
                    r = (3 * r + 0xFF) / 4;
                    g = (3 * g + 0xFF) / 4;
                    b = (3 * b + 0xFF) / 4;
                }
            }
            SDL_FillRect(gCurrentSurface, &drect, SDL_MapRGB(gCurrentSurface->format, r, g, b));
        }
    }
}

void IO::Render()
{
    // SDL_UnlockSurface(gCurrentSurface);
    if (gScreenSurface != SDL_GetWindowSurface(gWindow))
    {
        delete gScreenSurface;
        gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
    // gScreenSurface = SDL_GetWindowSurface(gWindow);
    gCurrentSurface = SDL_GetWindowSurface(gWindow);
    SDL_FillRect(gCurrentSurface, NULL, SDL_MapRGB(gCurrentSurface->format, 0xB0, 0xB0, 0xD5));
    SDL_Rect srect, drect;

    // drect.x = scale;
    // drect.y = drect.x;
    // drect.w = 8 * drect.x;
    // drect.h = drect.w;

    // SDL_BlitScaled(Background, NULL, gCurrentSurface, &drect);
    plotboard();

    if (selected != -1)
    {
        drect.x = (board->IndexToCoord(selected).i + 1) * scale + 3;
        drect.y = (board->IndexToCoord(selected).j + 1) * scale + 3;
        drect.w = scale - 6;
        drect.h = scale - 6;

        srect.w = 128;
        srect.h = 128;
        srect.x = 128 * 5;
        srect.y = 128 * 0;
    }

    SDL_BlitScaled(Assets, &srect, gCurrentSurface, &drect);


    if (selected != -1)
    {
        srect.w = 128;
        srect.h = 128;
        srect.x = 128 * 4;
        srect.y = 128 * 0;
        vector<Move> legalmoves = board->GetLegalMoves(board->IndexToCoord(selected));
        for (Move m : legalmoves)
        {
            drect.x = (board->IndexToCoord(m.i1).i + 1) * scale + 3;
            drect.y = (board->IndexToCoord(m.i1).j + 1) * scale + 3;
            drect.w = scale - 6;
            drect.h = scale - 6;
            SDL_BlitScaled(Assets, &srect, gCurrentSurface, &drect);
        }
    }
    SDL_Surface *surfaceMessage;
    for (int i = 0; i < board->width; i++)
    {
        for (int j = 0; j < board->height; j++)
        {
            drect.x = (i + 1) * scale + 3;
            drect.y = (j + 1) * scale + 3;
            drect.w = scale - 6;
            drect.h = scale - 6;

            int piece = board->AtPos(Coord{i, j});
            if (piece)
            {
                srect.w = 128;
                srect.h = 128;
                srect.x = 128 * (piece % 4);
                srect.y = 128 * (piece / 4);
                if (dragging && selected == board->CoordToIndex(Coord{i, j}))
                {
                    SDL_GetMouseState(&drect.x, &drect.y);
                    drect.x -= scale / 2;
                    drect.y -= scale / 2;
                    SDL_BlitScaled(pieces[piece & 1], NULL, gCurrentSurface, &drect);
                }
                else
                {
                    SDL_BlitScaled(pieces[piece & 1], NULL, gCurrentSurface, &drect);
                }
            }
            /*Debug for territory*/
            // surfaceMessage = TTF_RenderText_Solid(Sans, to_string(board->TerritoryAtPos(Coord{i, j})).c_str(), SDL_Color{0, 0, 0x10});
            // SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);
            // delete surfaceMessage;
        }
    }

    // SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);

    drect.x = (board->width + 1) * scale + 10;
    drect.y = scale + 3;
    for (int i = 0; i < board->height; i++)
    {
        surfaceMessage = TTF_RenderText_Solid(Sans, rows[(board->height - 1 - i)].c_str(), SDL_Color{0, 0, 0x10});
        drect.y = (i + 1 + 0.3) * scale + 3;
        SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);
        delete surfaceMessage;
    }
    drect.y = (board->height + 1) * scale + 3;
    for (int i = 0; i < board->width; i++)
    {
        surfaceMessage = TTF_RenderText_Solid(Sans, cols[i].c_str(), SDL_Color{0, 0, 0x10});
        drect.x = (i + 1 + 0.3) * scale + 3;
        SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);
        delete surfaceMessage;
    }

    if (showscore)
    {
        drect.x = scale / 2;
        drect.y = board->height * scale + 3;
        surfaceMessage = TTF_RenderText_Solid(Sans, to_string(board->score[WHITE]).c_str(), SDL_Color{0, 0, 0x10});
        SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);
        delete surfaceMessage;

        drect.x = scale / 2;
        drect.y = scale + 3;
        surfaceMessage = TTF_RenderText_Solid(Sans, to_string(board->score[BLACK]).c_str(), SDL_Color{0, 0, 0x10});
        SDL_BlitSurface(surfaceMessage, NULL, gCurrentSurface, &drect);
        delete surfaceMessage;
    }
    SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
    //Update the surface
    SDL_UpdateWindowSurface(gWindow);
    // SDL_LockSurface(gCurrentSurface);
}

void IO::loadAssets()
{
    Assets = loadSurface("assets.bmp");
    pieces[WHITE] = loadSurface("sprites/white.bmp");
    pieces[BLACK] = loadSurface("sprites/black.bmp");
    Background = SDL_CreateRGBSurface(0, scale * 8, scale * 8, 32,
                                      0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_FillRect(Background, NULL, SDL_MapRGB(Background->format, 0, 0, 0));
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect drect;
            drect.x = i * scale;
            drect.y = j * scale;
            drect.w = scale;
            drect.h = scale;
            int piece = board->AtPos(Coord{i, j});
            int r, g, b;
            // printf("Piece: %i\n", piece);
            // if (piece == 0)
            // {
            //     r = 0xC1;
            //     g = 0x9A;
            //     b = 0x6C;
            // }
            if ((i + j) % 2)
            {
                r = 0x80;
                g = 0x4C;
                b = 0x36;
            }
            else
            {
                r = 0xC1;
                g = 0x9A;
                b = 0x6C;
            }
            // {
            //     r = 0xE7;
            //     g = 0xB8;
            //     b = 0x8D;
            // }
            // if (piece == 6)
            // {
            //     r = 0x5C;
            //     g = 0x24;
            //     b = 0x14;
            // }

            SDL_FillRect(Background, &drect, SDL_MapRGB(Background->format, r, g, b));
        }
    }
}

int IO::Index(int x, int y)
{
    int field = -1;
    int widthoffield = scale;

    int i = x / widthoffield;
    int j = y / widthoffield;

    if (i < 1 || i > board->width || j < 1 || j > board->height)
    {
        return -1;
    }
    return (i - 1) + board->width * (j - 1);
}

void IO::WaitOnQuit()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        //User requests quit
        if (e.type == SDL_QUIT)
        {
            *quit = true;
        }
    }
    Render();
}

void IO::GetMove(int player)
{
    bool mademove = false;
    SDL_Event e;
    while (!mademove)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            // if (e.type == SDL_WINDOWEVENT_RESIZED)
            // {
            //     printf("RESIZE EVENT %i %i\n", e.window.data1, e.window.data2);
            //     SDL_SetWindowSize(gWindow, e.window.data1, e.window.data2);
            // }
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                mademove = true;
                *quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int clicked = Index(e.button.x, e.button.y);
                int piece = board->AtPos(board->IndexToCoord(clicked));
                // printf("piece: %i, player %i\n", piece, board->Player());
                if (selected != -1 && clicked != -1 && board->IsLegal(Move{selected, clicked}))
                {
                    board->MakeMove({selected, clicked});
                    mademove = true;
                    // printf("%i -> %i\n", selected, clicked);
                    selected = -1;
                }
                else if (piece && board->Player() == (piece & BLACK))
                {
                    selected = clicked;
                }
                dragging = true;
            }

            if (e.type == SDL_MOUSEBUTTONUP)
            {
                int clicked = Index(e.button.x, e.button.y);
                if (clicked != selected)
                {
                    if (selected != -1 && clicked != -1 && board->IsLegal(Move{selected, clicked}))
                    {
                        board->MakeMove({selected, clicked});
                        // printf("%i -> %i\n", selected, clicked);
                        mademove = true;
                    }
                    selected = -1;
                }
                dragging = false;
            }
            if (e.type == SDL_MOUSEWHEEL)
            {
                // printf("Scrolled: %i\n",e.wheel.y);
                if (e.wheel.y == -1)
                {
                    board->undo();
                }
            }
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.scancode == 225)
                {

                    showscore = true;
                }
            }
            if (e.type == SDL_KEYUP)
            {
                if (e.key.keysym.scancode == 225)
                {
                    showscore = false;
                }
            }
        }

        Render();
        SDL_Delay(1000 / 60);
    }
}

IO::~IO()
{
}
