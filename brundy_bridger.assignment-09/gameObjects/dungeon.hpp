#ifndef DUNGEON_HPP
#define DUNGEON_HPP

#include "rectangle.hpp"
#include "tile.hpp"
#include "npc.hpp"
#include "object.hpp"
#include "pc.hpp"
#include "../util/monsterParser.hpp"
#include <vector>

// Forward declarations for object description
class object_description;

// Forward declaration
class Dungeon;

// DO NOT define a typedef for dungeon_t here - it's defined in object_descriptions.h
// typedef Dungeon dungeon_t;

#define MAX_MONSTERS 100
#define MAX_OBJECTS 100
#define widthScreen 80
#define heightScreen 21
#define IMMUTABLE_HARDNESS 255

class Dungeon {
public:
    Rectangle rooms[50];
    int numRooms;
    Tile tiles[heightScreen][widthScreen];
    Tile fogOfWar[heightScreen][widthScreen];
    Point mc;
    Point tp;
    int gotoMode;
    Point downStairs[100];
    int numDownStairs;
    Point upStairs[100];
    int numUpStairs;
    Monster* monsters[MAX_MONSTERS];
    int numMonsters;
    Object* objects[MAX_OBJECTS];  // Array of objects in the dungeon
    int numObjects;                // Number of objects
    int renderMapMode; // 0=default, 1=non-tunneling, 2=tunneling, 3=hardness
    int monsterNeedUpdate;
    PC player;  // The player character
    std::vector<object_description> object_descriptions; // Object descriptions from file

    Dungeon() : numRooms(0), numDownStairs(0), numUpStairs(0), numMonsters(0), numObjects(0) {}
};

Dungeon generateDungeon();
void initDungeon(Dungeon* dungeon);
void setTiles(Dungeon* dungeon);
void setRooms(Dungeon* dungeon);
void setHalls(Dungeon* dungeon);
void populateDungeon(Dungeon* dungeon);

// Object-related functions
void spawnObjects(Dungeon *dungeon, int num_objects);
void clearObjects(Dungeon *dungeon);
Object* getObjectAt(Dungeon *dungeon, int x, int y);
bool isObjectAt(Dungeon *dungeon, int x, int y);

// Player functions
void handlePlayerMovement(Dungeon *dungeon, int newX, int newY);

void dungeon_dijkstra_non_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void dungeon_dijkstra_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void renderDungeon(Dungeon* dungeon);
void freeDungeon(Dungeon* dungeon);
void spawnMonsters(Dungeon *dungeon, int num_monsters);
#endif
