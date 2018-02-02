#ifndef __PHYSICS_H
#define __PHYSICS_H

#include "game.h"
#include "input.h"

#define PACMAN_DEFAULT_SPEED 2.
#define GHOST_DEFAULT_SPEED 2.

// TO WRITE
Direction decidePacman(const Map* map, Pacman* pacman, Action action);

// TO WRITE
Direction decideGhost(const Map* map, Ghost* ghost, Pacman* pacman, Ghost* blinky);

void createGraph(const Map* map);

void allShortestPaths( const Map* map, int source, int destination , int *visited , int *dist );

void BFS(const Map* map , int src , int lev , int *dist , int *visited );

void shortestPath(const Map* map , int source , int destination);

Direction randomDirection( const Map* map , Ghost* ghost );

Direction decideDirection( const Map* map , int target , Ghost* ghost );

Direction decideBlinky(const Map* map , Ghost* blinky , Pacman* pacman);

Direction decidePinky(const Map* map , Ghost* pinky , Pacman* pacman);

Direction decideInky(const Map* map , Ghost* inky , Pacman* pacman , Ghost* blinky);

Direction decideClyde(const Map* map , Ghost* clyde , Pacman* pacman);
#endif
