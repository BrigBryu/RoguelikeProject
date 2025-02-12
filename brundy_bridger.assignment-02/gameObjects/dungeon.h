#ifndef DUNGEON_H

#define DUNGEON_H

#include "rectangle.h"
#include "tile.h"

typedef struct { //increased num of stuff for asingment 2 can change later
    Rectangle rooms[50];
    int numRooms;
    Tile tiles[21][80];
    Point mc;
    Point downStairs[100];
    int numDownStairs;
    Point upStairs[100];
    int numUpStairs;

} Dungeon;

Dungeon generateDungeon();
void intiDungeon(Dungeon* dungeon);
void setTiles(Dungeon* dungeon);
void setRooms(Dungeon* dungeon);
void setHalls(Dungeon* dungeon);
void populateDungeon(Dungeon* dungeon);

void renderDungeon(Dungeon* dungeon);

#endif
