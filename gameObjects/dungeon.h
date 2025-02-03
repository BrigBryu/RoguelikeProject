#ifndef DUNGEON_H
#define DUNGEON_H

#include "rectangle.h"

typedef struct {
    Rectangle rooms[];
    int numRooms;
    Tile tiles[][];
    Point mc;
    Point downStairs[];
    int numDownStairs;
    Point upStairs[];
    int numUpStairs;

} Dungeon;

Dungeon generateDungeon();
void setTiles(Dungeon* dungeon);
void setRooms(Dungeon* dungeon);
void setHalls(Dungeon* dungeon);
void populateDungeon(Dungeon* dungeon);

#endif
