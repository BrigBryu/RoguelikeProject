#include <stdio.h>
#include <stdlib.h> //rand is in
#include "tile.h"

Tile createTile(TileType type) {
    switch(type){
        case(FLOOR):
            return (Tile){FLOOR,0};
        case(ROCK):
            return (Tile){ROCK,((rand() % 135) + 120)};
        case(STAIR_UP):
            return (Tile){STAIR_UP, 0};
        case(STAIR_DOWN):
            return (Tile){STAIR_DOWN, 0};
        case(HALL):
            return (Tile){HALL, 0};
        case(PLAYER):
            return (Tile){PLAYER,0};
    }
    return (Tile){ROCK, ((rand() % 135) + 120)};
}

void renderTile(Tile* t, int row, int col){
    printf("%c", getCharacter(t));
}

char getCharacter(Tile* t){
    TileType type = t->type;
    switch(type){
        case(FLOOR):
            return '.';
        case(ROCK):
            return ' ';
        case(STAIR_UP):
            return '<';
        case(STAIR_DOWN):
            return '>';
        case(HALL):
            return '#';
        case(PLAYER):
            return '@';
    }
    return '?';
}
