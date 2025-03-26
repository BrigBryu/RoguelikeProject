#ifndef DUNGEON_H

#define DUNGEON_H

#include "rectangle.h"
#include "tile.h"
#include "npc.h"

#define MAX_MONSTERS 100
#define widthScreen 80
#define heightScreen 21
#define IMMUTABLE_HARDNESS 255


typedef struct { //increased num of stuff for asingment 2 can change later
    Rectangle rooms[50];
    int numRooms;
    Tile tiles[21][80];
    Point mc;
    Point downStairs[100];
    int numDownStairs;
    Point upStairs[100];
    int numUpStairs;
    NPC* monsters[MAX_MONSTERS];
    int numMonsters;
    int renderMapMode; // 0=default, 1=non-tunneling, 2=tunneling, 3=hardness
} Dungeon;

Dungeon generateDungeon();
void initDungeon(Dungeon* dungeon);
void setTiles(Dungeon* dungeon);
void setRooms(Dungeon* dungeon);
void setHalls(Dungeon* dungeon);
void populateDungeon(Dungeon* dungeon);

void dungeon_dijkstra_non_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void dungeon_dijkstra_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void renderDungeon(Dungeon* dungeon);
void freeDungeon(Dungeon* dungeon);
#endif
