#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dungeon.h"
#include "fileHandle.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int save_flag = 0;
    int load_flag = 0;
    char *test_dungeon = NULL;  // For a dungeon file name provided via --test

    // Process command-line arguments.
    // Allowed switches: --save, --load, and --test <dungeon_filename>
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (strcmp(argv[i], "--test") == 0) {
            if (i + 1 < argc) {
                test_dungeon = argv[i + 1];
                i++;  // Skip the filename argument
            } else {
                fprintf(stderr, "Usage: %s [--save] [--load] [--test dungeon_filename]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Usage: %s [--save] [--load] [--test dungeon_filename]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    Dungeon dungeon;

    // If the load flag is set, load the dungeon.
    // If a test dungeon name was provided, read from saved_dungeons/<name>
    if (load_flag) {
        readDungeon(&dungeon, test_dungeon);
    } else {
        dungeon = generateDungeon();
    }

    // Display the dungeon.
    renderDungeon(&dungeon);

    // If the save flag is set, write the dungeon to disk.
    if (save_flag) {
        writeDungeon(&dungeon);
    }

    return 0;
}
