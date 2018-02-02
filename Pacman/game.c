#include <stdio.h>
#include <math.h>
#include "game.h"
#include "physics.h"

void initiateGame(char* filename, Map* outMap, Game* outGame, Pacman* outPacman, Ghost* outGhosts)
{
    FILE *map;
    int counter1 = 0 , counter2;
    char ch;
    filename = "/media/emytis/7EB6929BB692540D/University/Fundamentals of Programming/96102053/cmake-build-debug/res/map.txt";
    map = fopen(filename,"r");
    fscanf(map,"%d %d",&outMap->height,&outMap->width);
    outGame->cheeses = outGame->cherries = outGame->pineapples = 0;
    while( counter1 < outMap->height )
    {
        counter2 = 0;
        while (counter2 < outMap->width)
        {
            fscanf(map, "%c", &ch);
            if (ch != '\n' && ch != ' ' )
            {
                outMap->cells[counter2][counter1] = ch;
                switch( outMap->cells[counter2][counter1] )
                {
                    case CELL_CHEESE : outGame->cheeses++ ; break;
                    case CELL_CHERRY : outGame->cherries++ ; break;
                    case CELL_PINEAPPLE : outGame->pineapples++ ; break;
                }
                counter2++;
            }
        }
        counter1++;
    }
    fscanf(map,"%d",&outGame->score);
    fscanf(map,"%*s %d %d (%d,%d) (%lf,%lf)",&outPacman->dir,&outPacman->health,&outPacman->startY,&outPacman->startX,&outPacman->y,&outPacman->x);
    outGame->ghosts = 4;
    outGhosts[0].type = BLINKY;
    outGhosts[1].type = PINKY;
    outGhosts[2].type = CLYDE;
    outGhosts[3].type = INKY;
    for( counter1 = 0 ; counter1 < 4 ; counter1++ )
    {
        outGhosts[counter1].speed = GHOST_DEFAULT_SPEED;
        fscanf(map,"%*s %d %d ",&outGhosts[counter1].dir,&outGhosts[counter1].blue);
        outGhosts[counter1].blue = !outGhosts[counter1].blue;
        if( outGhosts[counter1].blue )
        {
            fscanf(map,"%llu ",&outGhosts[counter1].blueCounterDown);
            outGhosts[counter1].blueCounterDown *= CYCLES_PER_SEC;
            outGhosts[counter1].speed = 0.5 * GHOST_DEFAULT_SPEED;
        }
        fscanf(map,"(%d,%d) (%lf,%lf)",&outGhosts[counter1].startY,&outGhosts[counter1].startX,&outGhosts[counter1].y,&outGhosts[counter1].x);
    }
    outPacman->speed = PACMAN_DEFAULT_SPEED;
    fclose(map);
}

void checkEatables(Map* map, Game* outGame, Pacman* outPacman, Ghost* outGhosts)
{
    int counter;
    switch( map->cells[(int)round(outPacman->x)][(int)round(outPacman->y)] )
    {
        case CELL_CHEESE : outGame->score += CHEESE_SCORE;
            outGame->cheeses--;
            map->cells[(int)round(outPacman->x)][(int)round(outPacman->y)] = CELL_EMPTY;
            break;
        case CELL_CHERRY : outGame->score += CHERRY_SCORE;
            outGame->cherries--;
            map->cells[(int)round(outPacman->x)][(int)round(outPacman->y)] = CELL_EMPTY;
            break;
        case CELL_PINEAPPLE : outGame->score += PINEAPPLE_SCORE;
            outGame->pineapples--;
            for( counter = 0 ; counter < 4 ; counter++ )
            {
                outGhosts[counter].blue = true;
                outGhosts[counter].blueCounterDown = BLUE_DURATION;
                outGhosts[counter].speed = 0.5 * GHOST_DEFAULT_SPEED;
            }
            map->cells[(int)round(outPacman->x)][(int)round(outPacman->y)] = CELL_EMPTY;
            break;
    }
    for( counter = 0 ; counter < 4 ; counter++ )
        if( outGhosts[counter].blue )
            checkGhostCollision( outPacman , &outGhosts[counter] , outGame );
    for( counter = 0 ; counter < 4 ; counter++ )
        if( !outGhosts[counter].blue )
            checkGhostCollision( outPacman , &outGhosts[counter] , outGame );
    for( counter = 0 ; counter < 4 ; counter++ )
        checkGhostState( &outGhosts[counter] );


}

void checkGhostCollision(Pacman* outPacman, Ghost* outGhost, Game* outGame)
{
    if( pow( ( outPacman->x - outGhost->x ) , 2 ) + pow( ( outPacman->y - outGhost->y ) , 2 ) <= 0.5 )
    {
        if( outGhost->blue )
        {
            outGhost->blue = false;
            outGhost->x = outGhost->startX;
            outGhost->y = outGhost->startY;
            outGhost->speed = GHOST_DEFAULT_SPEED;
            outGame->score += GHOST_SCORE;
        }
        else
        {
            outPacman->health--;
            outPacman->x = outPacman->startX;
            outPacman->y = outPacman->startY;
        }
    }
}

bool isGameFinished(Game* game, Pacman* pacman)
{
    if( ( game->cheeses == 0 && game->pineapples == 0 ) || pacman->health == 0 )
        return true;
    return false;
}

void checkGhostState(Ghost* ghost)
{
    if( ( --ghost->blueCounterDown ) == 0 )
    {
        ghost->blue = false;
        ghost->speed = GHOST_DEFAULT_SPEED;
    }
}

