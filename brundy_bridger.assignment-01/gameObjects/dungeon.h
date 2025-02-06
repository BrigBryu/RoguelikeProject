#ifndef DUNGEON_H

#define DUNGEON_H

#include "rectangle.h"
#include "tile.h"

typedef struct {
    Rectangle rooms[25];
    int numRooms;
    Tile tiles[21][80];
    Point mc;
    Point downStairs[10];
    int numDownStairs;
    Point upStairs[10];
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
