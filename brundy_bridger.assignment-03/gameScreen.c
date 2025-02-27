#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dungeon.h"
#include "fileHandle.h"

int runLoadAndSave(int argc, char *argv[]) {
    srand(time(NULL));
    int save_flag = 0;
    int load_flag = 0;
    int test_mode = 0;
    int use_test_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (use_test_mode && strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                i++;
            }
        } else {
            fprintf(stderr, "Usage: %s [--save] [--load] [--test]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (test_mode) {
        char *dungeon_files[] = {
            "01.rlg327",
            "02.rlg327",
            "03.rlg327",
            "04.rlg327",
            "05.rlg327",
            "06.rlg327",
            "07.rlg327",
            "08.rlg327",
            "09.rlg327",
            "10.rlg327",
            "welldone.rlg327",
            "adventure.rlg327",
            "hello.rlg327"
        };
        int num_files = sizeof(dungeon_files) / sizeof(dungeon_files[0]);
        Dungeon dungeon;

        for (int i = 0; i < num_files; i++) {
            printf("=== Testing dungeon file: %s ===\n", dungeon_files[i]);
            readDungeon(&dungeon, dungeon_files[i]);
            renderDungeon(&dungeon);
            printf("\n"); 
        }
        return 0;
    }

    // not in test mode
    Dungeon dungeon;

    if (load_flag) {
        readDungeon(&dungeon, NULL);
    } else {
        dungeon = generateDungeon();
    }

    renderDungeon(&dungeon);

    if (save_flag) {
        writeDungeon(&dungeon);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    Dungeon dungeon;
    srand(time(NULL));
    dungeon = generateDungeon();

    renderDungeon(&dungeon);

    dungeon_dijkstra_non_tunnel(&dungeon);


    dungeon_dijkstra_tunnel(&dungeon);

    return 0;
}

