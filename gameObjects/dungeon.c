#include <stdio.h>

#include "dungeon.h"
#define width 80
#define height 21

Dungeon generateDungeon(){
    Dungeon dungeon;
    setTiles(dungeon);
    setRooms(dungeon);
    setHalls(dungeon);
    populateDungeon(dungeon);
    return dungeon;
}

void setTiles(Dungeon* dungeon){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            dungeon->tiles[i][j] = createTile(ROCK);
            if(i == 0 || i == height -1 || j == 0 || j == width - 1) {
                dungeon->tiles[i][j].hardness = 255; //set edge immutable
            }
        }
    }
}
