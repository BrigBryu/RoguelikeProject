#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "fileHandle.h"

typedef struct {
    uint8_t x;
    uint8_t y;
} PointData;

static void writePoint(FILE *f, PointData *p){
    if(f == NULL || p == NULL){
        return;
    }

    fwrite(&(p->x), sizeof(uint8_t), 1, f);
    fwrite(&(p->y), sizeof(uint8_t), 1, f);
}

static void readPoint(FILE *f, PointData *p){
    if(f == NULL || p == NULL){
        return;
    }

    fread(&(p->x), sizeof(uint8_t), 1, f);
    fread(&(p->y), sizeof(uint8_t), 1, f);

}

void writeDungeon(Dungeon *dungeon){
    //Set FILE
    const char *home_env = getenv("HOME");
    if (!home_env) {
        fprintf(stderr, "HOME environment not work\n");
        exit(EXIT_FAILURE);
    }

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/.rlg327/dungeon", home_env);

    FILE *f = fopen(file_path, "wb");  // wb write


    if(f == NULL){
        fprintf(stderr, "Cant open the dungeon file for writing");
        exit(EXIT_FAILURE);
    }

    //NOTE all points are 8 bit its
    //0-12 RLG327-S2025 marker
    //12-16 version
    //16-20 size
    //20-22 player point
    //22-1702 dungeon map tiles
    //rooms 4 bytes in order x,y,xlength,ylength
    //16 bit num upstair
    //upstair points
    //16 bit num downstair
    //downstair points
}

void readDungeon(Dungeon *dungeon){
    const char *home_env = getenv("HOME");
    if (!home_env) {
        fprintf(stderr, "HOME environment not work\n");
        exit(EXIT_FAILURE);
    }

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/.rlg327/dungeon", home_env);

    FILE *f = fopen(file_path, "rb");  // rb read

    if(f == NULL){
        fprintf(stderr, "Cant open the dungeon file for reading");
        exit(EXIT_FAILURE);
    }

    //NOTE all points are 8 bit its
    //0-12 RLG327-S2025 marker
    //12-16 version
    //16-20 size
    //20-22 player point
    //22-1702 dungeon map tiles
    //rooms 4 bytes in order x,y,xlength,ylength
    //16 bit num upstair
    //upstair points
    //16 bit num downstair
    //downstair points
}
