#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "rectangle.h"
#include "point.h"

#define width 80
#define height 21
#define MAX_HALL_TILES_THRESHOLD 70
#define SURRONDING_HALL_MAX 3
#define MAX_HORIZONTAL_IN_A_ROW 10

void initDungeon(Dungeon*);
int validateDungeon(Dungeon* dungeon);

Dungeon generateDungeon(){
    Dungeon dungeon;
    int ranOnce = 0;
    while(ranOnce == 0 || validateDungeon(&dungeon) != 1) {
        initDungeon(&dungeon);
        setTiles(&dungeon);
        setRooms(&dungeon);
        setHalls(&dungeon);
        populateDungeon(&dungeon);
        ranOnce = 1;
    }
    return dungeon;
}

int validateDungeon(Dungeon* dungeon) {
    int hallCount = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (dungeon->tiles[i][j].type == HALL) {
                hallCount++;
                int surroundingHallCount = 0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (di == 0 && dj == 0)
                            continue;
                        
                        int ni = i + di;
                        int nj = j + dj;
                        //check bounds 
                        
                        if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                            if (dungeon->tiles[ni][nj].type == HALL)
                                surroundingHallCount++;
                        }
                    }
                }
                if (surroundingHallCount > SURRONDING_HALL_MAX) {
                    return 0;
                }
            }
        }
    }


    for (int i = 0; i < height; i++) {
        int rowHallCount = 0;
        for (int j = 0; j < width; j++) {
            if (dungeon->tiles[i][j].type == HALL) {
                rowHallCount++;
            }
        }
        if (rowHallCount > MAX_HORIZONTAL_IN_A_ROW) {
            return 0;
        }
    }
    
    //printf("Hall tile count: %d\n", hallCount);
    //renderDungeon(dungeon);
    if(hallCount <= MAX_HALL_TILES_THRESHOLD){
        return 1;
    } else {
        return 0;
    }
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
    int curX = p1->x;
    int curY = p1->y;
    int targetX = p2->x;
    int targetY = p2->y;

    if (dungeon->tiles[curY][curX].type == ROCK)
        dungeon->tiles[curY][curX].type = HALL;

    while (curX != targetX || curY != targetY) {
        if (curX != targetX && curY != targetY) {
            if (rand() % 2 == 0) {
                int step = (rand() % 2) + 1;
                if (targetX > curX) {
                    int nextX = curX + step;
                    if (nextX > targetX)
                        nextX = targetX;
                    for (int x = curX + 1; x <= nextX; x++) {
                        if (dungeon->tiles[curY][x].type == ROCK)
                            dungeon->tiles[curY][x].type = HALL;
                    }
                    curX = nextX;
                } else {
                    int nextX = curX - step;
                    if (nextX < targetX)
                        nextX = targetX;
                    for (int x = curX - 1; x >= nextX; x--) {
                        if (dungeon->tiles[curY][x].type == ROCK)
                            dungeon->tiles[curY][x].type = HALL;
                    }
                    curX = nextX;
                }
            } else {
                // Step vertically.
                int step = (rand() % 2) + 1;
                if (targetY > curY) {
                    int nextY = curY + step;
                    if (nextY > targetY)
                        nextY = targetY;
                    for (int y = curY + 1; y <= nextY; y++) {
                        if (dungeon->tiles[y][curX].type == ROCK)
                            dungeon->tiles[y][curX].type = HALL;
                    }
                    curY = nextY;
                } else {
                    int nextY = curY - step;
                    if (nextY < targetY)
                        nextY = targetY;
                    for (int y = curY - 1; y >= nextY; y--) {
                        if (dungeon->tiles[y][curX].type == ROCK)
                            dungeon->tiles[y][curX].type = HALL;
                    }
                    curY = nextY;
                }
            }
        }
        // only horizontal movement need
        else if (curX != targetX) {
            int step = (rand() % 2) + 1;
            if (targetX > curX) {
                int nextX = curX + step;
                if (nextX > targetX)
                    nextX = targetX;
                for (int x = curX + 1; x <= nextX; x++) {
                    if (dungeon->tiles[curY][x].type == ROCK)
                        dungeon->tiles[curY][x].type = HALL;
                }
                curX = nextX;
            } else {
                int nextX = curX - step;
                if (nextX < targetX)
                    nextX = targetX;
                for (int x = curX - 1; x >= nextX; x--) {
                    if (dungeon->tiles[curY][x].type == ROCK)
                        dungeon->tiles[curY][x].type = HALL;
                }
                curX = nextX;
            }
        }
        // Only vertical movement need
        else if (curY != targetY) {
            int step = (rand() % 2) + 1;
            if (targetY > curY) {
                int nextY = curY + step;
                if (nextY > targetY)
                    nextY = targetY;
                for (int y = curY + 1; y <= nextY; y++) {
                    if (dungeon->tiles[y][curX].type == ROCK)
                        dungeon->tiles[y][curX].type = HALL;
                }
                curY = nextY;
            } else {
                int nextY = curY - step;
                if (nextY < targetY)
                    nextY = targetY;
                for (int y = curY - 1; y >= nextY; y--) {
                    if (dungeon->tiles[y][curX].type == ROCK)
                        dungeon->tiles[y][curX].type = HALL;
                }
                curY = nextY;
            }
        }
    }
}
/*
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
*/
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
    int roomHasStair[dungeon->numRooms];
    for(int i = 0; i < dungeon->numRooms; i++){
        roomHasStair[i] = 0; //false all have no
    }
    int playerRoomHasStairIndex = -1;
    int counter = 0;
    while(
            //all cases need for a successful generation
            //stairs
            dungeon->numUpStairs < neededUpStairs ||
            dungeon->numDownStairs < neededDownStairs ||
            //player location valid
            playerSet != 1
//monster validation later ???????
         ) {
        counter++;
        if(counter > 200) {
            for(int i = 0; i < dungeon-> numRooms; i++){
                if(i != playerRoomHasStairIndex) {
                    roomHasStair[i] = 0; //allow all rooms to place multiple stairs accept the room the player is started in                       
                }
            }
        }
        row = rand() % height;
        col = rand() % width;
        if(dungeon->tiles[row][col].type == FLOOR) {
            if(playerSet == 0) {
                dungeon->tiles[row][col].type = PLAYER;
                //Find the room the player starts in
                for(int r = 0; r < dungeon-> numRooms; r++){
                    if(rectangleContainsCord(&(dungeon->rooms[r]), col, row) == 1){
                        //playerRoom = dungeon->rooms[r];
                        roomHasStair[r] = 1;
                        playerRoomHasStairIndex = r;
                    }
                }
                int stairPlaced = 0;
                for (int i = -1; i <= 1 && stairPlaced != 1; i++) {
                    for (int j = -1; j <= 1 && stairPlaced != 1; j++) {
                        if (i == 0 && j == 0)
                            continue;
                        if (dungeon->tiles[row + i][col + j].type == FLOOR) {
                            dungeon->tiles[row + i][col + j].type = STAIR_UP;
                            dungeon->upStairs[dungeon->numUpStairs] = createPoint(col + j, row + i);
                            dungeon->numUpStairs++;
                            stairPlaced = 1;
                        }
                    }
                }
                playerSet = 1;
            } else {
                int roomIndexAddingStairTo;
                roomIndexAddingStairTo = -1;
                for(int i = 0; i < dungeon->numRooms; i++){
                    if(roomHasStair[i] == 0 && 
                            rectangleContainsCord(&(dungeon->rooms[i]),col,row) == 1) {
                        roomIndexAddingStairTo = i;
                    }
                }
                if(roomIndexAddingStairTo != -1 &&
                        dungeon->numUpStairs < neededUpStairs) {
                    roomHasStair[roomIndexAddingStairTo] = 1;
                    dungeon->tiles[row][col].type= STAIR_UP;
                    dungeon->upStairs[dungeon->numUpStairs] = createPoint(col, row);
                    dungeon->numUpStairs++;
                } else if(roomIndexAddingStairTo != -1 &&
                    dungeon->numDownStairs < neededDownStairs) {
                    roomHasStair[roomIndexAddingStairTo] = 1;
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
