#include <stdio.h>
#include <stdlib.h>
#include "simulate.h"
#include "dungeon.h"
#include "rectangle.h"
#include "point.h"
#include "heap.h"
#define MAX_HALL_TILES_THRESHOLD 70
#define SURRONDING_HALL_MAX 3
#define MAX_HORIZONTAL_IN_A_ROW 10
#define INT_MAX 9999999

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

void spawnMonsters(Dungeon *dungeon, int num_monsters) {
    dungeon->numMonsters = 0;
    for (int i = 0; i < dungeon->numRooms && dungeon->numMonsters < num_monsters; i++) {
        int monsters_in_room = rand() % 4;
        for (int j = 0; j < monsters_in_room && dungeon->numMonsters < num_monsters; j++) {
            int x = dungeon->rooms[i].bottomLeft.x + (rand() % dungeon->rooms[i].width);
            int y = dungeon->rooms[i].bottomLeft.y + (rand() % dungeon->rooms[i].height);
            NPC *monster = makeNPC(createPoint(x, y));
            dungeon->monsters[dungeon->numMonsters++] = monster;
        }
    }
}

int validateDungeon(Dungeon* dungeon) {
    int hallCount = 0;
    for (int i = 0; i < heightScreen; i++) {
        for (int j = 0; j < widthScreen; j++) {
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
                        
                        if (ni >= 0 && ni < heightScreen && nj >= 0 && nj < widthScreen) {
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


    for (int i = 0; i < heightScreen; i++) {
        int rowHallCount = 0;
        for (int j = 0; j < widthScreen; j++) {
            if (dungeon->tiles[i][j].type == HALL) {
                rowHallCount++;
            }
        }
        if (rowHallCount > MAX_HORIZONTAL_IN_A_ROW) {
            return 0;
        }
    }
    
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
    for(int i = 0; i < heightScreen; i++){
        for(int j = 0; j < widthScreen; j++){
            dungeon->tiles[i][j] = createTile(ROCK);
            if(i == 0 || i == heightScreen -1 || j == 0 || j == widthScreen - 1) {
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
                            dungeon->tiles[curY][x].hardness = 0;
                    }
                    curX = nextX;
                } else {
                    int nextX = curX - step;
                    if (nextX < targetX)
                        nextX = targetX;
                    for (int x = curX - 1; x >= nextX; x--) {
                        if (dungeon->tiles[curY][x].type == ROCK)
                            dungeon->tiles[curY][x].type = HALL;
                            dungeon->tiles[curY][x].hardness = 0;
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
                            dungeon->tiles[y][curX].hardness = 0;
                    }
                    curY = nextY;
                } else {
                    int nextY = curY - step;
                    if (nextY < targetY)
                        nextY = targetY;
                    for (int y = curY - 1; y >= nextY; y--) {
                        if (dungeon->tiles[y][curX].type == ROCK)
                            dungeon->tiles[y][curX].type = HALL;
                            dungeon->tiles[y][curX].hardness = 0;
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
                        dungeon->tiles[curY][x].hardness = 0;
                }
                curX = nextX;
            } else {
                int nextX = curX - step;
                if (nextX < targetX)
                    nextX = targetX;
                for (int x = curX - 1; x >= nextX; x--) {
                    if (dungeon->tiles[curY][x].type == ROCK)
                        dungeon->tiles[curY][x].type = HALL;
                        dungeon->tiles[curY][x].hardness = 0;
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
                        dungeon->tiles[y][curX].hardness = 0;
                }
                curY = nextY;
            } else {
                int nextY = curY - step;
                if (nextY < targetY)
                    nextY = targetY;
                for (int y = curY - 1; y >= nextY; y--) {
                    if (dungeon->tiles[y][curX].type == ROCK)
                        dungeon->tiles[y][curX].type = HALL;
                        dungeon->tiles[y][curX].hardness = 0;
                }
                curY = nextY;
            }
        }
    }

    //Set hardness
    for(int i = 0; i < heightScreen; i++){
        for(int j = 0; j < widthScreen; j++){
            if(dungeon->tiles[i][j].type == HALL){
                dungeon->tiles[i][j].hardness = 0;
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
        if(numTries % 25 == 0 && extraRooms > 0){
            extraRooms--;
        }

        //cant be 0 row or col cant be 20 row or 80 col
        int row = rand() % (heightScreen - 2) + 1;
        int col = rand() % (widthScreen - 2) + 1;
        //try multiple times for each point that way there can be more small spaces
        int placed = 0;
        for(int i = 0; i < 2 && placed != 1; i++) {
            int heightRoom = rand() % 6 + 3;
            int widthRoom = rand() % 12 + 4;
            if (row + heightRoom > heightScreen - 2 || col + widthRoom > widthScreen - 2) {
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
                for (int y = row; y < row + heightRoom && y < heightScreen - 1; y++) {
                    for (int x = col; x < col + widthRoom && x < widthScreen - 1; x++) {
                        dungeon->tiles[y][x].type = FLOOR;
                        dungeon->tiles[y][x].hardness = 0;
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
        row = rand() % heightScreen;
        col = rand() % widthScreen;
        if(dungeon->tiles[row][col].type == FLOOR) {
            if(playerSet == 0) {
                //dungeon->tiles[row][col].type = PLAYER; dont do player tile do point
                dungeon->mc.x = col;
                dungeon->mc.y = row; //SPOTHERE
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

typedef struct dist_node {
  int x;
  int y;
  int distance;
} dist_node_t;


int32_t compare_dist_nodes(const void *key, const void *with) {
    const dist_node_t *a = (const dist_node_t *) key;
    const dist_node_t *b = (const dist_node_t *) with;
    return (a->distance - b->distance);
}


void renderDungeon(Dungeon* dungeon){
    for(int i = 0; i < heightScreen; i++){
        for(int j = 0; j < widthScreen; j++){
                        int printed = 0;
            if (dungeon->mc.x == j && dungeon->mc.y == i) {
                printf("@");
                printed = 1;
            }
            for (int m = 0; m < dungeon->numMonsters && !printed; m++) {
                if (dungeon->monsters[m]->cord.x == j && dungeon->monsters[m]->cord.y == i) {
                    printf("%c", dungeon->monsters[m]->texture);
                    printed = 1;
                }
            }
            if (!printed) {
                renderTile(&(dungeon->tiles[i][j]), i, j);
            }
        }
        printf("\n");
    }
}

void dungeon_dijkstra_non_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]){
    // set distances
    for (int i = 0; i < heightScreen; i++) {
        for (int j = 0; j < widthScreen; j++) {
            dist[i][j] = INT_MAX; // or some large number
        }
    }

    // set 0
    dist[dungeon->mc.y][dungeon->mc.x] = 0;

    heap_t h;
    heap_init(&h, compare_dist_nodes, NULL);

    dist_node_t *start = malloc(sizeof(*start));
    start->x = dungeon->mc.x;
    start->y = dungeon->mc.y;
    start->distance = 0;
    heap_insert(&h, start);

    // Dijkstra
    while (heap_peek_min(&h)) {
        dist_node_t *min_node = heap_remove_min(&h);
        int x = min_node->x;
        int y = min_node->y;
        int cur_dist = min_node->distance;
        free(min_node);

        // If popped isnot better than dist[y][x] skip
        if (cur_dist > dist[y][x]) continue;

        // try neighbors
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (ny < 0 || ny >= heightScreen || nx < 0 || nx >= widthScreen) continue;

                // Non-tunneling must be hardness == 0
                if (dungeon->tiles[ny][nx].hardness == 0) {
                    int alt = cur_dist + 1; // cost to step
                    if (alt < dist[ny][nx]) {
                        dist[ny][nx] = alt;
                        dist_node_t *neighbor = malloc(sizeof(*neighbor));
                        neighbor->x = nx;
                        neighbor->y = ny;
                        neighbor->distance = alt;
                        heap_insert(&h, neighbor);
                    }
                }
            }
        }
    }

    /*
    for (int i = 0; i < heightScreen; i++) {
        for (int j = 0; j < widthScreen; j++) {
            if(dungeon->mc.x == j && dungeon->mc.y == i) {
                //printf("@");
                printf("\033[1;31m@\033[0m");
            } else if (dist[i][j] == INT_MAX) {
                printf(" ");
            } else {
                int d = dist[i][j];
                if(d >= 10) {
                    d %= 10;
                }
                printf("%d", d);
            }
        }
        if(i % widthScreen == 0)
            printf("\n");

    }
    */

    heap_delete(&h);
}

void dungeon_dijkstra_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]){
    for (int i = 0; i < heightScreen; i++) {
        for (int j = 0; j < widthScreen; j++) {
            dist[i][j] = INT_MAX;
        }
    }

    // set 0
    dist[dungeon->mc.y][dungeon->mc.x] = 0;

    heap_t h;
    heap_init(&h, compare_dist_nodes, NULL);

    dist_node_t *start = malloc(sizeof(*start));
    start->x = dungeon->mc.x;
    start->y = dungeon->mc.y;
    start->distance = 0;
    heap_insert(&h, start);

    // Dijkstra
    while (heap_peek_min(&h)) {
        dist_node_t *min_node = heap_remove_min(&h);
        int x = min_node->x;
        int y = min_node->y;
        int cur_dist = min_node->distance;
        free(min_node);

        // If popped isnot better than dist[y][x] skip
        if (cur_dist > dist[y][x]) continue;

        // try neighbors
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (ny < 0 || ny >= heightScreen || nx < 0 || nx >= widthScreen) continue;

                // tunneling no must be hardness == 0
                //if (dungeon->tiles[ny][nx].hardness == 0) {
                if(dungeon->tiles[ny][nx].hardness == 255){
                    continue; //TODO Not sure what todo
                }
                int alt = cur_dist + (dungeon->tiles[ny][nx].hardness/85) + 1; // cost to step
                if (alt < dist[ny][nx]) {
                    dist[ny][nx] = alt;
                    dist_node_t *neighbor = malloc(sizeof(*neighbor));
                    neighbor->x = nx;
                    neighbor->y = ny;
                    neighbor->distance = alt;
                    heap_insert(&h, neighbor);
                }
                //}
            }
        }
    }

    /*
    for (int i = 0; i < heightScreen; i++) {
        for (int j = 0; j < widthScreen; j++) {
            if(dungeon->mc.x == j && dungeon->mc.y == i) {
                //printf("@");
                printf("\033[1;31m@\033[0m");
            } else if (dist[i][j] == INT_MAX) {
                printf("U");
            } else {
                int d = dist[i][j];
                if(d >= 10) {
                    d %= 10;
                }
                printf("%d", d);
            }
        }
        if(i % widthScreen == 0)
            printf("\n");

    }

    heap_delete(&h);
    */
}

