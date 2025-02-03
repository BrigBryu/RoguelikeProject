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
    int numRoomsPlaced = 0;
    int extraRooms = rand() % 5 + 1;
    int numTries = 0;
    while(numRoomsPlaced < (6 + extraRooms)) {
        numTries++;
        if(numTries % 75 == 0) {
            setTiles(dungeon); //reset dungeon->tiles
        } else if(numTries % 25 == 0 && extraRooms > 0){
            extraRooms--;
        }

        //cant be 0 row or col cant be 20 row or 80 col
        int row = rand() % (height - 2) + 1;
        int col = rand() % (width - 2) + 1;
        //try multiple times for each point that way there can be more small spaces
        int placed = 0;
        for(int i = 0; i < 2 && placed != 1; i++) {
            int heightRoom = rand() % 6 + 3;
            int widthRoom = rand() % 12 + 4;
            if (row + heightRoom > height - 2 || col + widthRoom > width - 2) {
                continue;  // Skip this try location + dimensions make illegal
            }
            Rectangle room = createRectangle(col, row, widthRoom, heightRoom);
            placed = 1;
            for(int r = 0; r < dungeon->numRooms; r++){
                if(rectanglesTouch(&room, &(dungeon->rooms[r])) == 1) {
                    placed = 0;
                    break;
                }
            }
            if(placed == 1){
                //room can fit now update dungeon with it;
                dungeon->rooms[dungeon->numRooms] = room;
                dungeon->numRooms++;
                numRoomsPlaced++;
                for (int y = row; y < row + heightRoom && y < height - 1; y++) {
                    for (int x = col; x < col + widthRoom && x < width - 1; x++) {
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
