#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "rectangle.h"
#include "point.h"

#define width 80
#define height 21

void initDungeon(Dungeon*);

Dungeon generateDungeon(){
    Dungeon dungeon;
    initDungeon(&dungeon);
    setTiles(&dungeon);
    setRooms(&dungeon);
    setHalls(&dungeon);
    populateDungeon(&dungeon);
    return dungeon;
}

void initDungeon(Dungeon* dungeon){
    dungeon->numRooms = 0;
    dungeon->mc = createPoint(0,0);
    dungeon->numDownStairs = 0;
    dungeon->numUpStairs = 0;
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

void carveCorridor(Dungeon* dungeon, Point* p1, Point* p2) {
    int minX, minY, maxX, maxY;
    if(p1->x < p2->x){
        minX = p1->x;
        maxX = p2->x;
    } else {
        minX = p2->x;
        maxX = p1->x;
    }

    if(p1->y < p2->y){
        minY = p1->y;
        maxY = p2->y;
    } else {
        minY = p2->y;
        maxY = p1->y;
    }

    if (rand() % 2 == 0) {
        // Carve horizontal then vertical
        for (int x = minX; x <= maxX; x++) {
            if (dungeon->tiles[p1->y][x].type == ROCK)
                dungeon->tiles[p1->y][x].type = HALL;
        }
        for (int y = minY; y <= maxY; y++) {
            if (dungeon->tiles[y][p2->x].type == ROCK)
                dungeon->tiles[y][p2->x].type = HALL;
        }
    } else {
        // Carve vertical then horizontal
        for (int y = minY; y <= maxY; y++) {
            if (dungeon->tiles[y][p1->x].type == ROCK)
                dungeon->tiles[y][p1->x].type = HALL;
        }
        for (int x = minX; x <= maxX; x++) {
            if (dungeon->tiles[p2->y][x].type == ROCK)
                dungeon->tiles[p2->y][x].type = HALL;
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
        if(numTries % 25 == 0 && extraRooms > 0){
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
    for(int i = 1; i < dungeon->numRooms; i++){
        Point mid1 = getRectangleMid(&(dungeon->rooms[i]));
        Point mid2;
        double closestDist = 9999;
        int closestIndex = 0;
        for(int j = 0; j < i; j++){
            mid2 = getRectangleMid(&(dungeon->rooms[j]));
            double dist = getDistance(&mid1, &mid2);
            if(dist < closestDist){
                closestDist = dist;
                closestIndex = j;
            }
        }
        mid2 = getRectangleMid(&(dungeon->rooms[closestIndex]));
        carveCorridor(dungeon, &mid1, &mid2);
    }
}
//Requirements
//atelast one stair case up and down no more than 5 each
//Imposed rules
//no stairs down where the player starts
//1 stair up in the room where the player starts
//only one stair in each room
void populateDungeon(Dungeon* dungeon){
    int chance = rand() % 100 + 1;
    int chanceStair;
    if(chance < 50) {
        chanceStair = 3;
    } else if(chance < 75) {
        chanceStair = 4;
    } else {
        chanceStair = 5;
    }
    int row;
    int col;
    int neededUpStairs = rand() % chanceStair + 1;
    int neededDownStairs = rand() % chanceStair + 1;
    int playerSet = 0;

    Rectangle playerRoom;
    while(
            //all cases need for a successful generation
            //stairs
            dungeon->numUpStairs < neededUpStairs &&
            dungeon->numDownStairs < neededDownStairs &&
            //player location valid
            playerSet != 1
         ) {
        row = rand() % height;
        col = rand() % width;
        if(dungeon->tiles[row][col].type == FLOOR) {
            if(playerSet == 0) {
                dungeon->tiles[row][col].type = PLAYER;
                //Find the room the player starts in
                for(int r = 0; r < dungeon-> numRooms; r++){
                    if(rectangleContainsCord(&(dungeon->rooms[r]), col, row) == 1){
                        playerRoom = dungeon->rooms[r];
                    }
                }
                for(int i = -1; i < 2; i++){
                    for(int j = -1; j < 2; j++){
                        if(dungeon->tiles[row + i][col + j].type == FLOOR){
                            dungeon->tiles[row + i][col + j].type= STAIR_UP;
                            dungeon->upStairs[dungeon->numUpStairs] = createPoint(col + j, row + i);
                            dungeon->numUpStairs++;
                            break;
                        }
                    }
                }
                playerSet = 1;
            } else {
                if(dungeon->numUpStairs < neededUpStairs) {
                    dungeon->tiles[row][col].type= STAIR_UP;
                            dungeon->upStairs[dungeon->numUpStairs] = createPoint(col, row);
                            dungeon->numUpStairs++;
                } else if(dungeon->numDownStairs < neededDownStairs &&
                        rectangleContainsCord(&playerRoom, row, col) != 1 //dont let the player spawn room have a down stair case
                        ) {
                            dungeon->tiles[row][col].type= STAIR_DOWN;
                            dungeon->downStairs[dungeon->numDownStairs] = createPoint(col, row);
                            dungeon->numDownStairs++;
                } else {
//Can maybe spawn monsters here????
                }
            }
        }
    }
}

void renderDungeon(Dungeon* dungeon){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            renderTile(&(dungeon->tiles[i][j]),i,j);
        }
        printf("\n");
    }
}
