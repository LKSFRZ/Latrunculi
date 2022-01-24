#ifndef IO_H
#define IO_H

#define WIDTH 1600
#define HEIGHT 1600
#define SCALE 160 //px per field

#define OFFBOARD -1

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include "Board.h"

class IO
{
private:
    SDL_Surface *gScreenSurface;
    SDL_Window *gWindow;
    SDL_Surface *Assets;
    SDL_Surface *Background;
    SDL_Surface *pieces[2];
    SDL_Surface *gCurrentSurface = NULL;
    TTF_Font *Sans;
    Board *board;
    int scale;
    int selected;
    bool * quit;
    bool dragging;
    string cols[20] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U"};
    string rows[20] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "IXX", "XX"};
    bool showscore = false;
public:
    IO(bool * quit, Board * board);
    void Render();
    void loadAssets();
    void GetMove(int player);
    void WaitOnQuit();
    void plotboard();
    int Index(int x, int y);
    ~IO();
};

#endif