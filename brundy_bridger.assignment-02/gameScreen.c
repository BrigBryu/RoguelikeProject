#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dungeon.h"
#include "fileHandle.h"

    int main(int argc, char *argv[]) {
    srand(time(NULL));
    /*
    printf("Before all rock init:\n");
    renderDungeon(&dungeon);
    setTiles(&dungeon);
    printf("\nAfter all rock init:\n");
    renderDungeon(&dungeon);
    setRooms(&dungeon);
    printf("\nAfter room add\n");
    renderDungeon(&dungeon);
    setHalls(&dungeon);
    printf("\nAfter hall add\n");
    renderDungeon(&dungeon);
    populateDungeon(&dungeon);
    printf("\nAfter populate dungeon\n");
    renderDungeon(&dungeon);
    */
    //for(int i = 0; i < 10000; i++){
    //    printf("\n\nCreating the %d map:\n", i);
       // dungeon = generateDungeon();
        //renderDungeon(&dungeon);
    //}
    int save_flag = 0;
    int load_flag = 0;

    // Process command-line arguments.
    // Allowed switches are --save and --load. Unknown arguments will cause an error.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else {
            fprintf(stderr, "Usage: %s [--save] [--load]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    Dungeon dungeon;

    // If the load flag is set, load the dungeon from disk.
    // Otherwise, generate a new dungeon.
    if (load_flag) {
        readDungeon(&dungeon);
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
