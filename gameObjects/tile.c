#include <stdio.h>
#include "tile.h"

Tile createTile(TileType type) {
    switch(type){
        case(FLOOR):
            return (Tile){FLOOR,0};
        case(ROCK):
            return (Tile){ROCK,rand() % 253 + 1};
        case(STAIR_UP):
            return (Tile){STAIR_UP, 255};
        case(STAIR_DOWN):
            return (Tile){STAIR_DOWN, 255};
        case(HALL):
            return (Tile){HALL, 0};
    }
    return (Tile){ROCK};
}

void renderTile(Tile* t){
    TileType type = t->type;
    switch(type){
        case(FLOOR):
            break;
        case(ROCK):
            break;
        case(STAIR_UP):
            break;
        case(HALL):
            break;
    }
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
    }
    return '?';
}
