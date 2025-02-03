#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "rectangle.h"

#define width 80
#define height 21

Dungeon generateDungeon(){
    Dungeon dungeon;
    setTiles(&dungeon);
    setRooms(&dungeon);
    setHalls(&dungeon);
    populateDungeon(&dungeon);
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
/*
 * Room requireemtns
 * - area is 21 row by 80 col
 * - 3 rows by 4 cols minimum
 * - 6 room minimum
 * - there must be 1 cell of nonroom between rooms
 * - outer edge is immutable
 * - room cells should have periods
 *
 * Room choices
 * - max 8 rows 15 cols
 * - max 11 rooms min 7 ish rooms
 */
void setRooms(Dungeon* dungeon){
    int numRooms = 0;
    int extraRooms = rand() % 5 + 1;
    int numTries = 0;
    while(numRooms < (6 + extraRooms)) {
        if(numTries % 75) {
            setTiles(dungeon); //reset dungeon->tiles
        } else if(numTries % 25 == 0){
            extraRooms--;
        }

        //cant be 0 row or col cant be 20 row or 80 col
        int row = rand() % 20 + 1;
        int col = rand() % 80 + 1;
        //try multiple times for each point that way there can be more small spaces
        int placed = 0;
        for(int i = 0; i < 2 && placed != 1; i++) {
            int heightRoom = rand() % 6 + 3;
            int widthRoom = rand() % 12 + 4;
            Rectangle room = createRectangle(col, row, widthRoom, heightRoom);
            placed = 1;
            for(int r = 0; i < numRooms; i++){
                if(rectanglesTouch(&room, &(dungeon->rooms[r])) == 1) {
                    placed = 0;
                }
            }
            if(placed == 1){
                //room can fit now update dungeon with it;
                dungeon->rooms[numRooms] = room;
                dungeon-> numRooms++;

                for(int x = col; x <= widthRoom; x++){
                    for(int y = row; y <= heightRoom; y++){
                        dungeon->tiles[y][x].type = FLOOR;
                    }
                }
            }
        }
    }
}

void setHalls(Dungeon* dungeon){
    //TODO
}

void populateDungeon(Dungeon* dungeon){
    //TODO
}

void renderDungeon(Dungeon* dungeon){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            renderTile(&(dungeon->tiles[i][j]),i,j);
        }
        printf("\n");
    }
}
