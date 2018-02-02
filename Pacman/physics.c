#include "physics.h"
#include "map.h"
#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#define N 2000

int index , vertex , number_of_shortest_paths;
int graph[N][N] , paths[N][N] , shortest_paths_first_cell[N];

void createGraph(const Map* map)
{
    int i , j;
    for( i = 0 ; i < map->height * map->width ; i++ )
        for( j = 0 ; j < map->height * map->width ; j++ )
            graph[i][j] = 0;
    for( i = 0 ; i < map->width ; i++ )
    {
        for (j = 0; j < map->height; j++)
        {
            if (map->cells[i][j] != CELL_BLOCK)
            {
                if (map->cells[i][(j - 1 + map->height) % map->height] != CELL_BLOCK)
                    graph[j * map->width + i][((j - 1 + map->height) % map->height) * map->width + i] =
                    graph[((j - 1 + map->height) % map->height) * map->width + i][j * map->width + i] = 1;

                if (map->cells[i][(j + 1) % map->height] != CELL_BLOCK)
                    graph[j * map->width + i][((j + 1) % map->height) * map->width + i] =
                    graph[((j + 1) % map->height) * map->width + i][j * map->width + i] = 1;

                if (map->cells[(i - 1 + map->width) % map->width][j] != CELL_BLOCK)
                    graph[j * map->width + (i - 1 + map->width) % map->width][j * map->width + i] =
                    graph[j * map->width + i][j * map->width + (i - 1 + map->width) % map->width] = 1;

                if (map->cells[(i + 1) % map->width][j] != CELL_BLOCK)
                    graph[j * map->width + (i + 1) % map->width][j * map->width + i] =
                    graph[j * map->width + i][j * map->width + (i + 1) % map->width] = 1;
            }
        }
    }
}

void allShortestPaths( const Map* map , int source, int destination , int *visited , int *dist )
{
    visited[source] = 1;
    paths[number_of_shortest_paths][index] = source;
    index++;
    int i;
    if( index >= 2 )
        for( i = 0 ; i < index - 1 ; i++ )
            paths[number_of_shortest_paths + 1][i] = paths[number_of_shortest_paths][i];
    if( index <= dist[destination] || source == destination )
    {
        if (source == destination)
        {
            for( i = index ; i < map->width * map->height ; i++ )
                paths[number_of_shortest_paths][i] = -1;
            number_of_shortest_paths++;
        }
        else
        {
            int i;
            for (i = 0; i < map->width * map->height ; i++)
            {
                if ( ( !visited[i] ) && graph[source][i] )
                    allShortestPaths( map , i , destination , visited , dist );
            }
        }
    }
    visited[source] = 0;
    index--;
}

void BFS(const Map* map , int src , int lev , int *dist , int *visited )
{
    if( vertex == map->width + map->height )
        return;
    int i;
    for( i = 0 ; i < map->height * map->width ; i++ )
    {
        if( ( ( lev < dist[i] && visited[i] ) || ( !visited[i] ) )  && graph[src][i] )
        {
            vertex++;
            visited[i] = 1;
            dist[i] = lev;
            BFS( map , i , lev + 1 , dist , visited );
        }
    }
}

void shortestPath(const Map* map , int source , int destination)
{
    number_of_shortest_paths = index = 0;
    vertex = 0;
    int i , visited[N] = {0} , dist[N] = {INT_MAX};
    visited[source] = 1;
    dist[source] = 0;
    BFS( map , source , 1 , dist , visited );
    for( i = 0 ; i < map->height * map->width ; i++ )
        visited[i] = 0;
    allShortestPaths( map , source , destination , visited , dist);
    for( i = 0 ; i < number_of_shortest_paths ; i++ )
        shortest_paths_first_cell[i] = paths[i][1];
}

Direction randomDirection( const Map* map , Ghost* ghost )
{
    srand(time(0));
    while( true )
    {
        switch( abs( rand() % 4 ) + 1 )
        {
            case 1 :
                if( map->cells[ (int)ghost->x ][ (int)( map->height + ghost->y - 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_UP;
                break;
            case 2 :
                if( map->cells[ (int)( ghost->x + 1 ) % map->width ][ (int)ghost->y ] != CELL_BLOCK )
                    return DIR_RIGHT;
                break;
            case 3 :
                if( map->cells[ (int)ghost->x ][ (int)( ghost->y + 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_DOWN;
                break;
            case 4 :
                if( map->cells[ (int)( map->width + ghost->x - 1 ) % map->width ][ (int)ghost->y ] != CELL_BLOCK )
                    return DIR_LEFT;
                break;
        }
    }

}

Direction decideDirection( const Map* map , int target , Ghost* ghost )
{
    int target_y = target / map->width;
    int target_x = target % map->width;
    if( target_x == (int)( map->width + ghost->x - 1 ) % map->width )
        return DIR_LEFT;
    if( target_x == (int)( ghost->x + 1 ) % map->width )
        return DIR_RIGHT;
    if( target_y == (int)( ghost->y + 1 ) % map->height )
        return DIR_DOWN;
    if( target_y == (int)( map->height + ghost->y - 1 ) % map->height )
        return DIR_UP;
}

Direction decideBlinky(const Map* map , Ghost* blinky , Pacman* pacman)
{
    int source = ( int )( blinky->y * map->width + blinky->x ) , destination;
    if( blinky->blue )
        destination = blinky->startY * map->width + blinky->startX;
    else
        destination = ( int )( pacman->y * map->width + pacman->x);
    if( source == destination )
        return randomDirection( map , blinky );
    shortestPath( map , source , destination );
    if( number_of_shortest_paths != 0 )
        return decideDirection( map , shortest_paths_first_cell[ abs( rand() ) % number_of_shortest_paths ] , blinky );
}

Direction decidePinky(const Map* map , Ghost* pinky , Pacman* pacman)
{
    int source = (int)( pinky->y * map->width + pinky->x ) , destination , i;
    if( pinky->blue )
        destination = pinky->startY * map->width + pinky->startX;
    else
    {
        switch(pacman->dir)
        {
            case DIR_UP:
                i = (int)pacman->y;
                while( map->cells[ ( i + map->height ) % map->height ][(int)pacman->x] != CELL_BLOCK && abs( i - (int)pacman->y ) < 4 )
                    i--;
                destination = ( i + map->height ) % map->height * map->width + (int)pacman->x;
                break;

            case DIR_DOWN:
                i = (int)pacman->y;
                while( map->cells[ i % map->height ][(int)pacman->x] != CELL_BLOCK && abs( i - (int)pacman->y ) < 4 )
                    i++;
                destination = i % map->height * map->width + (int)pacman->x;
                break;

            case DIR_RIGHT:
                i = (int)pacman->x;
                while( map->cells[(int)pacman->y][i % map->width] != CELL_BLOCK && abs( i - (int)pacman->y ) < 4 )
                    i++;
                destination = (int)pacman->y * map->width + i % map->width;
                break;

            case DIR_LEFT:
                i = (int)pacman->y;
                while( map->cells[(int)pacman->y][( i + map->width ) % map->width] != CELL_BLOCK && abs( i - (int)pacman->y ) < 4 )
                    i--;
                destination = (int)pacman->y * map->width + ( i + map->width ) % map->width;
                break;
            case DIR_NONE:
                destination = (int)pacman->y * map->width + (int)pacman->x;
        }
    }
    if( source == destination )
        return randomDirection( map , pinky );
    shortestPath( map , source , destination );
    if( number_of_shortest_paths != 0 )
        return decideDirection( map , shortest_paths_first_cell[ abs( rand() ) % number_of_shortest_paths ] , pinky );
}

Direction decideInky(const Map* map , Ghost* inky , Pacman* pacman , Ghost* blinky)
{
    int source = ( int )( inky->y * map->width + inky->x ) , destination;
    if( inky->blue )
        destination = inky->startY * map->width + inky->startX;
    else
    {
        int x = (int)pacman->x , y = (int)pacman->y;
        switch(pacman->dir)
        {
            case DIR_UP:y = y - 2 + map->height;break;
            case DIR_DOWN:y = y + 2;break;
            case DIR_LEFT:x = x - 2 + map->width;break;
            case DIR_RIGHT:x = x + 2;break;
        }
        destination = ( 2 * y - (int)blinky->y  + map->height ) % map->height * map->width + ( 2 * x - (int)blinky->x + map->width ) % map->width;
    }
    if( source == destination || map->cells[ destination / map->width ][ destination % map->width ] == CELL_BLOCK )
        return randomDirection( map , inky );
    shortestPath( map , source , destination );
    if( number_of_shortest_paths != 0 )
        return decideDirection( map , shortest_paths_first_cell[ abs( rand() ) % number_of_shortest_paths ] , inky );
}

Direction decideClyde(const Map* map , Ghost* clyde , Pacman* pacman)
{
    int source = ( int )( clyde->y * map->width + clyde->x ) , destination;
    if( clyde->blue )
        destination = clyde->startY * map->width + clyde->startX;
    else
    {
        if( abs( (int)clyde->x - (int)pacman->x ) + abs( (int)clyde->x - (int)pacman->x ) <= 8 )
            if( map->cells[map->height - 1][map->width - 1] != CELL_BLOCK )
                destination = map->width * map->height - 1;
            else
                return decideBlinky( map , clyde , pacman );
    }
    if( source == destination || map->cells[ destination / map->width ][ destination % map->width ] == CELL_BLOCK )
        return randomDirection( map , clyde );
    shortestPath( map , source , destination );
    if( number_of_shortest_paths != 0 )
        return decideDirection( map , shortest_paths_first_cell[ abs( rand() ) % number_of_shortest_paths ] , clyde );
}

Direction decideGhost(const Map* map, Ghost* ghost, Pacman* pacman, Ghost* blinky)
{
    createGraph(map);
    switch(ghost->type)
    {
        case BLINKY:
            return decideBlinky( map , ghost , pacman );
        case PINKY:
            return decidePinky( map , ghost , pacman );
        case INKY:
            return decideInky( map , ghost , pacman , blinky );
        case CLYDE:
            return decideClyde( map , ghost , pacman );
    }
}

Direction decidePacman(const Map* map, Pacman* pacman, Action action)
{
        switch( action )
        {
            case ACTION_UP : if( map->cells[ (int)pacman->x ][ (int)( map->height + pacman->y - 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_UP;
                else if( pacman->dir != DIR_UP )
                    return pacman->dir;
                break;
            case ACTION_RIGHT : if( map->cells[ (int)( pacman->x + 1 ) % map->width ][ (int)pacman->y ] != CELL_BLOCK )
                    return DIR_RIGHT;
                else if( pacman->dir != DIR_RIGHT )
                    return pacman->dir;
                break;
            case ACTION_DOWN : if( map->cells[ (int)pacman->x ][ (int)( pacman->y + 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_DOWN;
                else if( pacman->dir != DIR_DOWN )
                    return pacman->dir;
                break;
            case ACTION_LEFT : if( map->cells[ (int)( map->width + pacman->x - 1 ) % map->width ][ (int)pacman->y ] != CELL_BLOCK )
                    return DIR_LEFT;
                else if( pacman->dir != DIR_LEFT )
                    return pacman->dir;
                break;
        }
        return DIR_NONE;
}