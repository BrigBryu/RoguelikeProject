#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "fileHandle.h"

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

        uint8_t roomX = (uint8_t) (dungeon->rooms[i].bottomLeft.x) + roomHeight; //needs to be top left for saving
        uint8_t roomY = (uint8_t) (dungeon->rooms[i].bottomLeft.y);

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

void readDungeon(Dungeon *dungeon) {
    const char *home_env = getenv("HOME");
    if (!home_env) {
        fprintf(stderr, "HOME environment variable not set.\n");
        exit(EXIT_FAILURE);
    }

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/.rlg327/dungeon", home_env);

    FILE *f = fopen(file_path, "rb");  // Open the file for binary reading.
    if (f == NULL) {
        fprintf(stderr, "Can't open the dungeon file for reading.\n");
        exit(EXIT_FAILURE);
    }

    /* --- Read Header --- */

    // 1. Read the 12-byte marker.
    char marker[12];
    if (fread(marker, sizeof(char), 12, f) != 12) {
        fprintf(stderr, "Error reading dungeon file marker.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    if (memcmp(marker, "RLG327-S2025", 12) != 0) {
        fprintf(stderr, "Invalid dungeon file marker.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    // 2. Read the 4-byte version and convert from network order.
    uint32_t netVersion;
    if (fread(&netVersion, sizeof(uint32_t), 1, f) != 1) {
        fprintf(stderr, "Error reading version.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint32_t version = ntohl(netVersion);
    // Optionally, verify that version is one you support.

    // 3. Read the 4-byte file size and convert.
    uint32_t netFileSize;
    if (fread(&netFileSize, sizeof(uint32_t), 1, f) != 1) {
        fprintf(stderr, "Error reading file size.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint32_t fileSize = ntohl(netFileSize);
    // Optionally, you may want to check that fileSize is as expected.

    // 4. Read the player (PC) coordinates (1 byte each for x and y).
    uint8_t playerX, playerY;
    if (fread(&playerX, sizeof(uint8_t), 1, f) != 1 ||
        fread(&playerY, sizeof(uint8_t), 1, f) != 1) {
        fprintf(stderr, "Error reading player coordinates.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    dungeon->mc.x = playerX;
    dungeon->mc.y = playerY;

    /* --- Read Dungeon Map --- */

    // The dungeon hardness map is 21 rows x 80 columns (each 1 byte).
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            uint8_t hardness;
            if (fread(&hardness, sizeof(uint8_t), 1, f) != 1) {
                fprintf(stderr, "Error reading dungeon hardness map.\n");
                fclose(f);
                exit(EXIT_FAILURE);
            }
            dungeon->tiles[i][j].hardness = hardness;
        }
    }

    /* --- Read Room Data --- */

    // 1. Read the number of rooms (16-bit, network order).
    uint16_t netNumRooms;
    if (fread(&netNumRooms, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of rooms.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numRooms = ntohs(netNumRooms);
    dungeon->numRooms = numRooms;

    // 2. Read each room's data: 4 bytes per room (x, y, width, height).
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
        // Set room data. Adjust conversion if your internal representation differs.
        dungeon->rooms[i].bottomLeft.x = roomX;
        dungeon->rooms[i].bottomLeft.y = roomY;
        dungeon->rooms[i].width = roomWidth;
        dungeon->rooms[i].height = roomHeight;
    }

    /* --- Read Staircase Data --- */

    // 1. Read the number of up staircases (16-bit, network order).
    uint16_t netNumUpStairs;
    if (fread(&netNumUpStairs, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of up staircases.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numUpStairs = ntohs(netNumUpStairs);
    dungeon->numUpStairs = numUpStairs;

    // 2. Read each up staircase's coordinates (x and y, 1 byte each).
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

    // 3. Read the number of down staircases (16-bit, network order).
    uint16_t netNumDownStairs;
    if (fread(&netNumDownStairs, sizeof(uint16_t), 1, f) != 1) {
        fprintf(stderr, "Error reading number of down staircases.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    uint16_t numDownStairs = ntohs(netNumDownStairs);
    dungeon->numDownStairs = numDownStairs;

    // 4. Read each down staircase's coordinates (x and y, 1 byte each).
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
}
