#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "fileHandle.h"

static void postProcessDungeon(Dungeon* d) {
    //set all 0 to hall
    for (int row = 0; row < 21; row++) {
        for (int col = 0; col < 80; col++) {
            int h = d->tiles[row][col].hardness;
            if (h == 0) {
                d->tiles[row][col].type = HALL;
            } else {
                d->tiles[row][col].type = ROCK;
            }
        }
    }

    //set all 0 in a room to floor
    for (int r = 0; r < d->numRooms; r++) {
        Rectangle *room = &d->rooms[r];
        for (int row = 0; row < 21; row++) {
            for (int col = 0; col < 80; col++) {
                if (rectangleContainsCord(room, col, row)) {
                    d->tiles[row][col].type = FLOOR;
                    d->tiles[row][col].hardness = 0;
                }
            }
        }
    }


    //Place stairs
    for (int i = 0; i < d->numUpStairs; i++) {
        int sx = d->upStairs[i].x;
        int sy = d->upStairs[i].y;
        d->tiles[sy][sx].type = STAIR_UP;
    }

    for (int i = 0; i < d->numDownStairs; i++) {
        int sx = d->downStairs[i].x;
        int sy = d->downStairs[i].y;
        d->tiles[sy][sx].type = STAIR_DOWN;
    }

    //player
    int px = d->mc.x;
    int py = d->mc.y;
    d->tiles[py][px].type = PLAYER;

}


void writeDungeon(Dungeon *dungeon){
    //Set FILE
    char *home;
    char *dungeonPath;
    int dungeonLeng;
    FILE *f;

    home = getenv("HOME");
    dungeonLeng = strlen(home) + strlen("/.rlg327/dungeon") + 1;
    dungeonPath = malloc(dungeonLeng * sizeof(char));
    strcpy(dungeonPath, home);
    strcat(dungeonPath, "/.rlg327/dungeon");

    f = fopen(dungeonPath, "wb");  // wb write


    if(f == NULL){
        fprintf(stderr, "Cant open the dungeon file for writing");
        free(dungeonPath);
        exit(EXIT_FAILURE);
    }

    //NOTE all points are 8 bit its
    //0-12 RLG327-S2025 marker
    char marker[12] = "RLG327-S2025";
    if(fwrite(marker, sizeof(char),12,f) != 12){
        fprintf(stderr, "wrote not enough or too many for marker");
        free(dungeonPath);
        fclose(f);
        exit(EXIT_FAILURE);

    }

    //12-16 version uint32_t for 32 bit bumber
    uint32_t version = 0;
    uint32_t bigVersion = htonl(version);
    fwrite(&bigVersion, sizeof(uint32_t), 1, f);
    //16-20 size
    uint32_t fileSize = 1704 +
                           (dungeon->numRooms * 4) +
                           2 + (dungeon->numUpStairs * 2) +
                           2 + (dungeon->numDownStairs * 2);
    uint32_t bigFileSize = htonl(fileSize);
    fwrite(&bigFileSize, sizeof(uint32_t), 1, f);
    //20-22 player point 1 byte for x and one for y
    uint8_t playerX = dungeon -> mc.x;
    uint8_t playerY = dungeon -> mc.y;
    if(fwrite(&playerX, sizeof(uint8_t), 1, f) != 1){
        fprintf(stderr, "issue writing playerx");
        free(dungeonPath);
        fclose(f);
        exit(EXIT_FAILURE);

    }

    if(fwrite(&playerY, sizeof(uint8_t), 1, f) != 1){
        fprintf(stderr, "issue writing playerY");
        free(dungeonPath);
        fclose(f);
        exit(EXIT_FAILURE);

    }
    //22-1702 dungeon map tiles row major so do all 1 row all 2 row all 3 row
    uint8_t hardMap[21][80];

    for(int i = 0; i < 21; i++){
        for(int j = 0; j < 80; j++){
            hardMap[i][j] = (uint8_t) (dungeon->tiles[i][j].hardness); //hardness is just an int not sure if the cast works
        }
    }

    for(int i = 0; i < 21; i++){ //now row worder add so just all of each row at a time
        if(fwrite(&hardMap[i], sizeof(uint8_t), 80, f) != 80){
            fprintf(stderr, "Writing the hardMap not working");
            free(dungeonPath);
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    //num rooms 2 bites so 16 bit int
    uint16_t numRooms = (uint16_t) (dungeon->numRooms);
    uint16_t bigNumRooms = htons(numRooms);
    if (fwrite(&bigNumRooms, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error writing number of rooms.\n");
        fclose(f);
        free(dungeonPath);
        exit(EXIT_FAILURE);
    }
    //rooms 4 bytes in order x,y,xlength,ylength
    for(int i = 0; i < numRooms; i++){
        uint8_t roomHeight = (uint8_t) (dungeon -> rooms[i].height); 
        uint8_t roomWidth = (uint8_t) (dungeon->rooms[i].width);
        uint8_t roomX = (uint8_t) (dungeon->rooms[i].bottomLeft.x);

        //uint8_t roomX = (uint8_t) (dungeon->rooms[i].bottomLeft.x); //needs to be top left for saving

        uint8_t roomY = (uint8_t) (dungeon->rooms[i].bottomLeft.y); //+ roomHeight;


        if(fwrite(&roomX, sizeof(uint8_t), 1, f) != 1){
            fprintf(stderr, "issue writing room x");
            free(dungeonPath);
            fclose(f);
            exit(EXIT_FAILURE);

        }
        if(fwrite(&roomY, sizeof(uint8_t), 1, f) != 1){
            fprintf(stderr, "issue writing room y");
            free(dungeonPath);
            fclose(f);
            exit(EXIT_FAILURE);

        }
        if(fwrite(&roomWidth, sizeof(uint8_t), 1, f) != 1){
            fprintf(stderr, "issue writing room width");
            free(dungeonPath);
            fclose(f);
            exit(EXIT_FAILURE);

        }
        if(fwrite(&roomHeight, sizeof(uint8_t), 1, f) != 1){
            fprintf(stderr, "issue writing room height");
            free(dungeonPath);
            fclose(f);
            exit(EXIT_FAILURE);

        }
    }
    //16 bit num upstair
    uint16_t numUpStairs = (uint16_t) (dungeon->numUpStairs);
    uint16_t bigNumUpStairs = htons(numUpStairs);
    if(fwrite(&bigNumUpStairs,sizeof(uint16_t), 1, f) != 1){
        fprintf(stderr, "issue writing upStaiir amt");
        free(dungeonPath);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    //upstair points

    for(int i = 0; i < numUpStairs; i++){
            uint8_t stairX = (uint8_t) (dungeon->upStairs[i].x);
            uint8_t stairY = (uint8_t) (dungeon->upStairs[i].y);
            if(fwrite(&stairX, sizeof(uint8_t), 1, f) != 1){
                fprintf(stderr, "issue writing upstair x");
                free(dungeonPath);
                fclose(f);
                exit(EXIT_FAILURE);

            }
            if(fwrite(&stairY, sizeof(uint8_t), 1, f) != 1){
                fprintf(stderr, "issue writing upstair y");
                free(dungeonPath);
                fclose(f);
                exit(EXIT_FAILURE);

            }
    }

    //16 bit num downstair
    uint16_t numDownStairs = (uint16_t) (dungeon->numDownStairs);
    uint16_t bigNumDownStairs = htons(numDownStairs);
    if(fwrite(&bigNumDownStairs,sizeof(uint16_t), 1, f) != 1){
        fprintf(stderr, "issue writing downStaiir amt");
        free(dungeonPath);
        fclose(f);
        exit(EXIT_FAILURE);

    }
    //upstair points

    for(int i = 0; i < numDownStairs; i++){
            uint8_t stairX = (uint8_t) (dungeon->downStairs[i].x);
            uint8_t stairY = (uint8_t) (dungeon->downStairs[i].y);
            if(fwrite(&stairX, sizeof(uint8_t), 1, f) != 1){
                fprintf(stderr, "issue writing down stair x");
                free(dungeonPath);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            if(fwrite(&stairY, sizeof(uint8_t), 1, f) != 1){
                fprintf(stderr, "issue writing down stair y");
                free(dungeonPath);
                fclose(f);
                exit(EXIT_FAILURE);

            }
    }
    //free stuff
    free(dungeonPath);
    fclose(f);
}

void readDungeon(Dungeon *dungeon, char *testDungeon) {
    char file_path[256];

    if (testDungeon != NULL) {
        snprintf(file_path, sizeof(file_path), "saved_dungeons/%s", testDungeon);
    } else {
        const char *home_env = getenv("HOME");
        if (!home_env) {
            fprintf(stderr, "HOME environment variable not set.\n");
            exit(EXIT_FAILURE);
        }
        snprintf(file_path, sizeof(file_path), "%s/.rlg327/dungeon", home_env);
    }

    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Can't open the dungeon file for reading: %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    //Header
    char marker[12];
    if (fread(marker, sizeof(char), 12, f) != 12) {
        fprintf(stderr, "Error reading dungeon file marker.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    //printf("Marker read: %.*s\n", 12, marker);



    uint32_t netVersion;
    if (fread(&netVersion, sizeof(uint32_t), 1, f) != 1) {
        fprintf(stderr, "Error reading version.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint32_t version = ntohl(netVersion);

    //printf("Version (net): 0x%08x, after ntohl: %u\n",
     //  netVersion, version);


    uint32_t netFileSize;
    if (fread(&netFileSize, sizeof(uint32_t), 1, f) != 1) {
        fprintf(stderr, "Error reading file size.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint32_t fileSize = ntohl(netFileSize);

   // printf("File size (net): 0x%08x, after ntohl: %u\n",
   //    netFileSize, fileSize);


    uint8_t playerX, playerY;
    if (fread(&playerX, sizeof(uint8_t), 1, f) != 1 ||
        fread(&playerY, sizeof(uint8_t), 1, f) != 1) {
        fprintf(stderr, "Error reading player coordinates.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    dungeon->mc.x = playerX;
    dungeon->mc.y = playerY;

    //printf("PC coords read: x=%u, y=%u\n", (unsigned) playerX, (unsigned) playerY);

    //hardnesses
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            uint8_t hardness;
            if (fread(&hardness, sizeof(uint8_t), 1, f) != 1) {
                fprintf(stderr, "Error reading dungeon hardness map.\n");
                fclose(f);
                exit(EXIT_FAILURE);
            }
            dungeon->tiles[i][j].hardness = hardness;
            //printf("Hardness row0 col%2d = %u\n", j, (unsigned) hardness);
        }
    }

    //rooms
    uint16_t netNumRooms;
    if (fread(&netNumRooms, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of rooms.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numRooms = ntohs(netNumRooms);
    dungeon->numRooms = numRooms;

    for (int i = 0; i < numRooms; i++) {
        uint8_t roomX, roomY, roomWidth, roomHeight;
        if (fread(&roomX, sizeof(uint8_t), 1, f) != 1 ||
            fread(&roomY, sizeof(uint8_t), 1, f) != 1 ||
            fread(&roomWidth, sizeof(uint8_t), 1, f) != 1 ||
            fread(&roomHeight, sizeof(uint8_t), 1, f) != 1) {
            fprintf(stderr, "Error reading room data.\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
        //dungeon->rooms[i].bottomLeft.x = roomX;
        //dungeon->rooms[i].bottomLeft.y = roomY - roomHeight;
        dungeon->rooms[i].bottomLeft.x = roomX;
        //dungeon->rooms[i].bottomLeft.y = roomY + (roomHeight - 1);
        dungeon->rooms[i].bottomLeft.y = roomY;//(20 - roomY) - (roomHeight - 1);
        dungeon->rooms[i].width = roomWidth;
        dungeon->rooms[i].height = roomHeight;
    }

    //stairs
    uint16_t netNumUpStairs;
    if (fread(&netNumUpStairs, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of up staircases.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numUpStairs = ntohs(netNumUpStairs);
    dungeon->numUpStairs = numUpStairs;

    for (int i = 0; i < numUpStairs; i++) {
        uint8_t stairX, stairY;
        if (fread(&stairX, sizeof(uint8_t), 1, f) != 1 ||
            fread(&stairY, sizeof(uint8_t), 1, f) != 1) {
            fprintf(stderr, "Error reading up staircase coordinates.\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
        dungeon->upStairs[i].x = stairX;
        dungeon->upStairs[i].y = stairY;
    }

    uint16_t netNumDownStairs;
    if (fread(&netNumDownStairs, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of down staircases.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numDownStairs = ntohs(netNumDownStairs);
    dungeon->numDownStairs = numDownStairs;

    for (int i = 0; i < numDownStairs; i++) {
        uint8_t stairX, stairY;
        if (fread(&stairX, sizeof(uint8_t), 1, f) != 1 ||
            fread(&stairY, sizeof(uint8_t), 1, f) != 1) {
            fprintf(stderr, "Error reading down staircase coordinates.\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
        dungeon->downStairs[i].x = stairX;
        dungeon->downStairs[i].y = stairY;
    }

    fclose(f);
    //set the tile type enums
    postProcessDungeon(dungeon);
}

