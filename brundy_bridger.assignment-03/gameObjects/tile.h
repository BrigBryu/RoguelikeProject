#ifndef TILE_H
#define TILE_H

typedef enum{
    FLOOR = 0,
    ROCK,
    STAIR_UP,
    STAIR_DOWN,
    HALL,
} TileType;

typedef struct{
    TileType type;
    int hardness;
} Tile;
Tile createTile(TileType type);
void renderTile(Tile* t, int row, int col);
char getCharacter(Tile* t);

#endif
