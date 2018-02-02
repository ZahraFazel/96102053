#include <stdio.h>
#include "map.h"
#include "input.h"
#include "game.h"
#include "physics.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "SDL2/SDL.h"
#include <math.h>

#define SIZE 50
#define HEIGHT ( SIZE * map->height )


void drawGame( const Game* game , const Map* map , const Pacman* pacman , const Ghost* ghosts );
void drawMap( const Map* map );
void drawPacman( const Pacman* pacman );
void drawGhosts( const Ghost* ghosts );
void gameLoop( Game* game , Map* map , Pacman* pacman , Ghost* ghosts );
void pause();
int menu();

SDL_Window* window;
SDL_Renderer* renderer;

int main()
{
    Map map;
    Pacman pacman;
    Ghost ghosts[4];
    Game game;
    char filename[3000];
    SDL_Event e;
    if( menu() )
    {
        initiateGame( filename , &map , &game , &pacman , ghosts );
        if( SDL_Init(SDL_INIT_VIDEO) )
            return 1;
        window = SDL_CreateWindow("Pacman", 100, 100, SIZE * map.width, SIZE * map.height + 200, SDL_WINDOW_OPENGL);
        if (window == NULL)
        {
            SDL_Quit();
            return 2;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == NULL)
        {
            SDL_Quit();
            return 3;
        }
        drawGame( &game , &map , &pacman , ghosts );
        gameLoop( &game , &map , &pacman ,ghosts );
        SDL_RenderClear(renderer);
        if( game.cheeses == 0 && game.pineapples == 0 )
            stringRGBA( renderer , 190 , 100 , "YOU WON :)" , 0xFF , 0xFF , 0xFF , 0xFF );
        else
            stringRGBA( renderer , 190 , 100 , "YOU LOST :(" , 0xFF , 0xFF , 0xFF , 0xFF );
        stringRGBA( renderer , 170 , 150 , "PRESS Q TO QUIT" , 0xFF , 0xFF , 0xFF , 0xFF );
        SDL_RenderClear(renderer);
        while(1)
            if( SDL_PollEvent(&e) )
                if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q )
                    break;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return 0;
}

void drawGame( const Game* game , const Map* map , const Pacman* pacman , const Ghost* ghosts )
{
    int i;
    char str[100];
    drawMap( map );
    drawPacman( pacman );
    drawGhosts( ghosts );
    for( i = 0 ; i < pacman->health ; i++ )
        filledPieRGBA( renderer , 30 + 50 * i , HEIGHT + 70 , 15 , 20 , 340 , 0xFF , 0xFF , 0x00 , 0xFF );
    stringRGBA( renderer , 15 , HEIGHT + 150 , "PRESS SPACE TO PAUSE THE GAME" , 0xFF , 0xFF , 0xFF , 0xFF );
    stringRGBA( renderer , 15 , HEIGHT + 175 , "PRESS Q TO QUIT" , 0xFF , 0xFF , 0xFF , 0xFF );
    sprintf(str,"Score:%d",game->score);
    stringRGBA( renderer , 15 , HEIGHT + 10 , str , 0xFF , 0xFF , 0xFF , 0xFF );
}

void drawMap( const Map* map )
{
    int i , j;
    for( i = 0 ; i < map->width ; i++ )
    {
        for( j = 0 ; j < map->height ; j++ )
        {
            switch( map->cells[i][j] )
            {
                case CELL_BLOCK:
                    boxRGBA( renderer , i * SIZE , j *  SIZE , ( i + 1 ) * SIZE , ( j + 1 ) * SIZE , 0x94 , 0x94 , 0xB8 , 0xFF );
                    break;
                case CELL_CHEESE:
                    filledCircleRGBA( renderer , ( 2 * i + 1 ) * SIZE / 2 , ( 2 * j + 1 ) * SIZE / 2 , 3 , 0xFF , 0xFF , 0x33 , 0xFF );
                    break;
                case CELL_PINEAPPLE:
                    filledCircleRGBA( renderer , ( 2 * i + 1 ) * SIZE / 2 , ( 2 * j + 1 ) * SIZE / 2 , 12 , 0xFF , 0xFF , 0x33 , 0xFF );
                    break;
                case CELL_CHERRY:
                    filledCircleRGBA( renderer , ( 2 * i + 1 ) * SIZE / 2 , ( 2 * j + 1 ) * SIZE / 2 , 8 , 0xCC , 0x00 , 0x33 , 0xFF );
                    break;
            }
        }
    }
}

void drawPacman( const Pacman* pacman )
{
    double x = ( 2 * pacman->x + 1 ) * SIZE / 2 , y = ( 2 * pacman->y + 1 ) * SIZE / 2;
    switch( pacman->dir )
    {
        case DIR_UP:
            filledPieRGBA( renderer , (int)x , (int)y , 20 , -50 , 230 , 0xFF , 0xFF , 0x00 , 0xFF );
            break;
        case DIR_NONE:
        case DIR_RIGHT:
            filledPieRGBA( renderer , (int)x , (int)y , 20 , 40 , 320 , 0xFF , 0xFF , 0x00 , 0xFF );
            break;
        case DIR_DOWN:
            filledPieRGBA( renderer , (int)x , (int)y , 20 , -230 , 50 , 0xFF , 0xFF , 0x00 , 0xFF );
            break;
        case DIR_LEFT:
            filledPieRGBA( renderer , (int)x , (int)y , 20 , -140 , 140 , 0xFF , 0xFF , 0x00 , 0xFF );
            break;
    }
}

void drawGhosts( const Ghost* ghosts )
{
    int i;
    double x , y;
    for( i = 0 ; i < 4 ; i++ )
    {
        y = ( 2 * ghosts[i].y + 1) * SIZE / 2;
        x = ( 2 * ghosts[i].x + 1) * SIZE / 2;
        if( ghosts[i].blue )
        {
            filledPieRGBA( renderer , (int)x , (int)y , SIZE / 2 , 180 , 360 , 0x00 , 0x33 , 0xCC ,0xFF );
            boxRGBA( renderer , (int)x - SIZE / 2 , (int)y , (int)x + SIZE / 2 , (int)y + 20 , 0x00 , 0x33 , 0xCC ,0xFF );
        }
        else
        {
            switch( ghosts[i].type )
            {
                case BLINKY:
                    filledPieRGBA( renderer , (int)x , (int)y , SIZE / 2 , 180 , 360 , 0xFF , 0x1A , 0x1A ,0xFF );
                    boxRGBA( renderer , (int)x - SIZE / 2 , (int)y , (int)x + SIZE / 2 , (int)y + 20 , 0xFF , 0x1A , 0x1A ,0xFF );
                    break;
                case PINKY:
                    filledPieRGBA( renderer , (int)x , (int)y , SIZE / 2 , 180 , 360 , 0xFF , 0x33 , 0x99 ,0xFF );
                    boxRGBA( renderer , (int)x - SIZE / 2 , (int)y , (int)x + SIZE / 2 , (int)y + 20 , 0xFF , 0x33 , 0x99 ,0xFF );
                    break;
                case INKY:
                    filledPieRGBA( renderer , (int)x , (int)y , SIZE / 2 , 180 , 360 , 0x33 , 0xCC , 0xFF ,0xFF );
                    boxRGBA( renderer , (int)x - SIZE / 2 , (int)y , (int)x + SIZE / 2 , (int)y + 20 , 0x33 , 0xCC , 0xFF ,0xFF );
                    break;
                case CLYDE:
                    filledPieRGBA( renderer , (int)x , (int)y , SIZE / 2 , 180 , 360 , 0xFF , 0x99 , 0x33 ,0xFF );
                    boxRGBA( renderer , (int)x - SIZE / 2 , (int)y , (int)x + SIZE / 2 , (int)y + 20 , 0xFF , 0x99 , 0x33 ,0xFF );
                    break;
            }
        }
        filledCircleRGBA( renderer , (int)x - 7 , (int)y - SIZE / 4 , 5 , 0xFF , 0xFF , 0xFF , 0xFF );
        filledCircleRGBA( renderer , (int)x + 7 , (int)y - SIZE / 4 , 5 , 0xFF , 0xFF , 0xFF , 0xFF );
        filledCircleRGBA( renderer , (int)x + 7 , (int)y - SIZE / 4 , 2 , 0x00 , 0x00 , 0x00 , 0xFF );
        filledCircleRGBA( renderer , (int)x - 7 , (int)y - SIZE / 4 , 2 , 0x00 , 0x00 , 0x00 , 0xFF );
        arcRGBA( renderer , (int)x , (int)y , 10 , 50 , 130 , 0x00 , 0x00 , 0x00 , 0xFF );
    }
}

void gameLoop( Game* game , Map* map , Pacman* pacman , Ghost* ghosts )
{
    SDL_Event e;
    int i , cycle = 0;
    double d;
    Action action;
    while(1)
    {
        if( SDL_PollEvent(&e) )
        {
            if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_UP:action = ACTION_UP;break;
                    case SDLK_RIGHT:action = ACTION_RIGHT;break;
                    case SDLK_DOWN:action = ACTION_DOWN;break;
                    case SDLK_LEFT:action = ACTION_LEFT;break;
                    case SDLK_q:
                        return;
                    case SDLK_SPACE:pause();
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        if( pacman->x - floor(pacman->x) < 0.1  && pacman->y - floor(pacman->y) < 0.1 )
            pacman->dir = decidePacman( map , pacman , action );
        switch( pacman->dir )
        {
            case DIR_UP:pacman->y = fmod( pacman->y - 0.02 + map->height , map->height );break;
            case DIR_RIGHT:pacman->x = fmod( pacman->x + 0.02 , map->width );break;
            case DIR_DOWN:pacman->y = fmod( pacman->y + 0.02 , map->height );break;
            case DIR_LEFT:pacman->x = fmod( pacman->x - 0.02 + map->width , map->width );break;
        }
        checkEatables( map , game , pacman , ghosts );
        if( isGameFinished( game , pacman) )
            break;
        if( cycle > 0 )
            for( i = 0 ; i < 4 ; i++ )
            {
                if( ghosts[i].x - floor(ghosts[i].x) < 0.1 && ghosts[i].y - floor(ghosts[i].y) < 0.1 )
                    ghosts[i].dir = decideGhost( map , &ghosts[i] , pacman , &ghosts[0] );
                if( ghosts[i].blue )
                    d = 0.0125;
                 else
                    d = 0.025;
                switch( ghosts[i].dir )
                {
                    case DIR_UP:ghosts[i].y = fmod( ghosts[i].y - d + map->height , map->height );break;
                    case DIR_RIGHT:ghosts[i].x = fmod( ghosts[i].x + d , map->width );break;
                    case DIR_DOWN:ghosts[i].y = fmod( ghosts[i].y + d , map->height );break;
                    case DIR_LEFT:ghosts[i].x = fmod( ghosts[i].x - d + map->width , map->width );break;
                }
            }
        drawGame( game , map , pacman , ghosts );
        SDL_RenderPresent(renderer);
        SDL_Delay(5);
        cycle++;
    }
}

void pause()
{
    SDL_Event e;
    if( SDL_Init(SDL_INIT_VIDEO) )
        return;
    SDL_Window* window1 = SDL_CreateWindow("Pause", 100, 100, 400, 400, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        SDL_Quit();
        return;
    }
    SDL_Renderer* renderer1 = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_Quit();
        return;
    }
    SDL_SetRenderDrawColor(renderer1, 0, 0, 0, 0);
    stringRGBA( renderer1 , 170 , 200 , "PRESS SPACE TO RESUME" , 0xFF , 0xFF , 0xFF , 0xFF );
    while(1)
    {
        if( SDL_PollEvent(&e) )
            if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE )
            {
                SDL_DestroyRenderer(renderer1);
                SDL_DestroyWindow(window1);
                break;
            }
    }
}

int menu()
{
    SDL_Event e;
    if (SDL_Init(SDL_INIT_VIDEO))
        return 2;
    SDL_Window* window = SDL_CreateWindow("Menu", 100, 100, 500, 500, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        SDL_Quit();
        return 3;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_Quit();
        return 4;
    }
    ////Pacman
    filledPieRGBA( renderer , 250 , 125 , 100 , 30 , 330 , 0xFF , 0xFF , 0x00 , 0xFF );

    ////Blinky
    filledPieRGBA( renderer , 100 , 250 , SIZE , 180 , 360 , 0xFF , 0x1A , 0x1A ,0xFF );
    boxRGBA( renderer , 100 - SIZE , 250 , 100 + SIZE , 300 , 0xFF , 0x1A , 0x1A ,0xFF );
    filledCircleRGBA( renderer , 90 , 250 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 110 , 250 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 110 , 250 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    filledCircleRGBA( renderer , 90 , 250 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    arcRGBA( renderer , 100 , 250 , 20 , 60 , 120 , 0x00 , 0x00 , 0x00 , 0xFF );

    ////Pinky
    filledPieRGBA( renderer , 400 , 250 , SIZE , 180 , 360 , 0xFF , 0x33 , 0x99 ,0xFF );
    boxRGBA( renderer , 400 - SIZE , 250 , 400 + SIZE , 300 , 0xFF , 0x33 , 0x99 ,0xFF );
    filledCircleRGBA( renderer , 390 , 250 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 410 , 250 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 410 , 250 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    filledCircleRGBA( renderer , 390 , 250 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    arcRGBA( renderer , 400 , 250 , 20 , 60 , 120 , 0x00 , 0x00 , 0x00 , 0xFF );

    ////Inky
    filledPieRGBA( renderer , 100 , 400 , SIZE , 180 , 360 , 0x33 , 0xCC , 0xFF ,0xFF );
    boxRGBA( renderer , 100 - SIZE , 400 , 100 + SIZE , 450 , 0x33 , 0xCC , 0xFF ,0xFF );
    filledCircleRGBA( renderer , 90 , 400 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 110 , 400 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 110 , 400 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    filledCircleRGBA( renderer , 90 , 400 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    arcRGBA( renderer , 100 , 400 , 20 , 60 , 120 , 0x00 , 0x00 , 0x00 , 0xFF );

    ////Clyde
    filledPieRGBA( renderer , 400 , 400 , SIZE , 180 , 360 , 0xFF , 0x99 , 0x33 ,0xFF );
    boxRGBA( renderer , 400 - SIZE , 400 , 400 + SIZE , 450 , 0xFF , 0x99 , 0x33 ,0xFF );
    filledCircleRGBA( renderer , 390 , 400 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 410 , 400 - SIZE / 2 , 10 , 0xFF , 0xFF , 0xFF , 0xFF );
    filledCircleRGBA( renderer , 410 , 400 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    filledCircleRGBA( renderer , 390 , 400 - SIZE / 2 , 4 , 0x00 , 0x00 , 0x00 , 0xFF );
    arcRGBA( renderer , 400 , 400 , 20 , 60 , 120 , 0x00 , 0x00 , 0x00 , 0xFF );

    ////Menu
    stringRGBA( renderer , 235 , 250 , "PlAY" , 0xFF , 0xFF , 0xFF , 0xFF );
    stringRGBA( renderer , 235 , 275 , "QUIT" , 0xFF , 0xFF , 0xFF , 0xFF );
    stringRGBA( renderer , 185 , 400 , "PRESS P TO PLAY" , 0xFF , 0xFF , 0xFF , 0xFF );
    stringRGBA( renderer , 185 , 425 , "PRESS Q TO QUIT" , 0xFF , 0xFF , 0xFF , 0xFF );
    SDL_RenderPresent(renderer);
    while(1)
    {
        if(SDL_PollEvent(&e))
            if( e.type == SDL_KEYDOWN )
                if( e.key.keysym.sym == SDLK_p || e.key.keysym.sym == SDLK_q )
                {
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    if( e.key.keysym.sym == SDLK_p )
                        return 1;
                    else
                        return 0;
                }
    }
}