#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dungeon.h"
#include "fileHandle.h"
#include "simulate.h"  // contains simulateMonsters and spawnMonsters prototypes

// Function prototypes for our modes.
int runLoadAndSave(int argc, char *argv[]);
int runTestMode(void);
int runSimulation(int num_monsters);

int main(int argc, char *argv[]) {
    // Default flags/values.
    int save_flag = 0;
    int load_flag = 0;
    int test_mode = 0;
    int num_monsters = 10;  // default number of monsters

    // Process command-line arguments.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
        } else if (strcmp(argv[i], "--nummon") == 0) {
            if (i + 1 < argc) {
                num_monsters = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --nummon requires an integer argument.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Usage: %s [--save] [--load] [--test] [--nummon <int>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Seed the random number generator.
    srand(time(NULL));

    // Decide which mode to run.
    if (test_mode) {
        return runTestMode();
    } else if (load_flag || save_flag) {
        return runLoadAndSave(argc, argv);
    } else {
        return runSimulation(num_monsters);
    }
}

// Mode 1: Load/Save Mode.
// Uses your existing functionality to load a dungeon, render it, and optionally save it.
int runLoadAndSave(int argc, char *argv[]) {
    Dungeon dungeon;
    int save_flag = 0;
    int load_flag = 0;
    int test_mode = 0;

    // Process arguments (reusing the same flags as in main).
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
        } else if (strcmp(argv[i], "--nummon") == 0) {
            i++; // skip monster count (not used in load/save mode)
        } else {
            fprintf(stderr, "Usage: %s [--save] [--load] [--test] [--nummon <int>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (test_mode) {
        // In test mode, iterate through the list of dungeon files.
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
        for (int i = 0; i < num_files; i++) {
            printf("=== Testing dungeon file: %s ===\n", dungeon_files[i]);
            readDungeon(&dungeon, dungeon_files[i]);
            renderDungeon(&dungeon);
            printf("\n");
        }
        return 0;
    }

    // Not in test mode.
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

// Mode 2: Test Mode.
// Iterates through a series of pre-defined dungeon files and renders them.
int runTestMode(void) {
    Dungeon dungeon;
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

    for (int i = 0; i < num_files; i++) {
        printf("=== Testing dungeon file: %s ===\n", dungeon_files[i]);
        readDungeon(&dungeon, dungeon_files[i]);
        renderDungeon(&dungeon);
        printf("\n");
    }
    return 0;
}

// Mode 3: Simulation Mode.
// Generates a new dungeon, spawns monsters based on the --nummon flag,
// and then runs the simulation (the discrete event simulation loop).
int runSimulation(int num_monsters) {
    Dungeon dungeon = generateDungeon();

    // Spawn monsters into the dungeon.
    // Here we assume you have a spawnMonsters function that takes a dungeon and the desired number of monsters.
    spawnMonsters(&dungeon, num_monsters);

    // Run the simulation loop.
    simulateMonsters(&dungeon, num_monsters);

    return 0;
}
