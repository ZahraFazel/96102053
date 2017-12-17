#include "physics.h"
#include "map.h"
#include "game.h"
#include <stdlib.h>

Direction decideGhost(const Map* map, Ghost* ghost)
{
    switch( abs( rand() % 4 ) )
    {
        case 1 : if( map->cells[ (int)ghost->x ][ (int)( map->height + ghost->y - 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_UP;
                 break;
        case 2 : if( map->cells[ (int)( ghost->x + 1 ) % map->width ][ (int)ghost->y ] != CELL_BLOCK )
                    return DIR_RIGHT;
                 break;
        case 3 : if( map->cells[ (int)ghost->x ][ (int)( ghost->y + 1 ) % map->height ] != CELL_BLOCK )
                    return DIR_DOWN;
                 break;
        case 0 : if( map->cells[ (int)( map->width + ghost->x - 1 ) % map->width ][ (int)ghost->y ] != CELL_BLOCK )
                    return DIR_LEFT;
                 break;
    }
    return DIR_NONE;
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
