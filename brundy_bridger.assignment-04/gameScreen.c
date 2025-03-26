/* gameScreen.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "dungeon.h"
#include "ui.h"
#include "fileHandle.h"
#include "simulate.h"

int runLoadAndSave(int argc, char *argv[]);
int runTestMode(void);
int runSimulation(int num_monsters);

void updateDungeon(Dungeon* dungeon, int key, WINDOW* gameWin) {
    int newX = dungeon->mc.x;
    int newY = dungeon->mc.y;
    
    // Movement keys - handle all directional movement
    switch (key) {
        // Cardinal directions
        case KEY_UP:
        case '8':
        case 'k':
            newY--; // Move up
            break;
        case KEY_DOWN:
        case '2':
        case 'j':
            newY++; // Move down
            break;
        case KEY_LEFT:
        case '4':
        case 'h':
            newX--; // Move left
            break;
        case KEY_RIGHT:
        case '6':
        case 'l':
            newX++; // Move right
            break;
            
        // Diagonal directions
        case KEY_HOME:
        case '7':
        case 'y':
            newY--; // Move up-left
            newX--;
            break;
        case KEY_PPAGE:
        case '9':
        case 'u':
            newY--; // Move up-right
            newX++;
            break;
        case KEY_END:
        case '1':
        case 'b':
            newY++; // Move down-left
            newX--;
            break;
        case KEY_NPAGE:
        case '3':
        case 'n':
            newY++; // Move down-right
            newX++;
            break;
            
        // Rest commands
        case KEY_B2:
        case ' ':
        case '.':
        case '5':
            renderMessageLine("Resting...");
            break;
            
        // Stairs
        case '>':
            // Go down stairs if player is on them
            for (int i = 0; i < dungeon->numDownStairs; i++) {
                if (dungeon->mc.x == dungeon->downStairs[i].x && 
                    dungeon->mc.y == dungeon->downStairs[i].y) {
                    renderMessageLine("You descend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode; // Save the current mode
                    
                    // Generate new dungeon level
                    freeDungeon(dungeon);
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode; // Restore the current mode
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    spawnMonsters(dungeon, 10);
                    
                    return;
                }
            }
            renderMessageLine("There are no down stairs (>) here.");
            break;
            
        case '<':
            // Go up stairs if player is on them
            for (int i = 0; i < dungeon->numUpStairs; i++) {
                if (dungeon->mc.x == dungeon->upStairs[i].x && 
                    dungeon->mc.y == dungeon->upStairs[i].y) {
                    renderMessageLine("You ascend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode; 
                    
                    // Generate new dungeon level
                    freeDungeon(dungeon);
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode; // Restore the current mode
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    spawnMonsters(dungeon, 10);
                    
                    return;
                }
            }
            renderMessageLine("There are no up stairs(<) here.");
            break;
            
        // Display commands
        case 'c':
            renderMessageLine("Character information would be displayed here.");
            break;
        case 'e':
            renderMessageLine("Equipment would be displayed here.");
            break;
        case 'i':
            renderMessageLine("Inventory would be displayed here.");
            break;
        case 'm':
            renderMessageLine("Monster list would be displayed here.");
            break;
        case 's':
            renderMessageLine("Displaying default terrain map.");
            dungeon->renderMapMode = 0;
            break;
        case 'D':
            renderMessageLine("Displaying non-tunneling distance map.");
            dungeon->renderMapMode = 1;
            break;
        case 'T':
            renderMessageLine("Displaying tunneling distance map.");
            dungeon->renderMapMode = 2;
            break;
        case 'H':
            renderMessageLine("Displaying hardness map.");
            dungeon->renderMapMode = 3;
            break;
            
        // Item commands
        case 'd':
            renderMessageLine("Drop item command would be processed here.");
            break;
        case 'w':
            renderMessageLine("Wear item command would be processed here.");
            break;
        case 't':
            renderMessageLine("Take off item command would be processed here.");
            break;
        case 'x':
            renderMessageLine("Expunge item command would be processed here.");
            break;
        case 'I':
            renderMessageLine("Inspect inventory item command would be processed here.");
            break;
        case 'E':
            renderMessageLine("Inspect equipped item command would be processed here.");
            break;
            
        // Other commands
        case 'f':
            renderMessageLine("Fog of war command would be processed here.");
            break;
        case 'g':
            renderMessageLine("Teleport command would be processed here.");
            break;
        case 'L':
            renderMessageLine("Look at monster command would be processed here.");
            break;
        case 'Q':
            renderMessageLine("Quitting game...");
            // main loop already checks for 'Q' to exit
            break;
            
        default:
            renderMessageLine("Unrecognized command.");
            break;
    }
    
    // Check new position
    if (newX != dungeon->mc.x || newY != dungeon->mc.y) {
        // Check bounds
        if (newX > 0 && newX < widthScreen - 1 && newY > 0 && newY < heightScreen - 1) {
            if (dungeon->tiles[newY][newX].hardness == 0) {
                dungeon->mc.x = newX;
                dungeon->mc.y = newY;
                
                // Check for collision with monsters
                for (int i = 0; i < dungeon->numMonsters; i++) {
                    if (dungeon->monsters[i]->cord.x == newX && 
                        dungeon->monsters[i]->cord.y == newY) {
                        renderMessageLine("You attack the monster!");
                        // Combat logic here
                        break;
                    }
                }
            } else {
                renderMessageLine("You can't move there silly adventuerer");
            }
        }
    }
}

int runLoadAndSave(int argc, char *argv[]) {
    return 0;
}

int runTestMode(void) {
    return 0;
}

int runSimulation(int num_monsters) {
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        Dungeon* dungeon = malloc(sizeof(Dungeon));
        if (dungeon == NULL) {
            fprintf(stderr, "Failed to allocate memory for dungeon\n");
            return EXIT_FAILURE;
        }
        
        initDungeon(dungeon);
        dungeon->renderMapMode = 0; // Default map view
        setTiles(dungeon);
        setRooms(dungeon);
        setHalls(dungeon);
        populateDungeon(dungeon);
        
        spawnMonsters(dungeon, 10);           

        initCurses();

        renderMessageLine("Welcome to the dungeon!");

        WINDOW* gameWin = initGameMapWindow();

        renderCurses(dungeon, gameWin);

        renderStatus("Health: 100/100", "Level: 1  Turn: 0");

        int ch;
        while ((ch = getch()) != 'Q') {
            updateDungeon(dungeon, ch, gameWin);
            renderCurses(dungeon, gameWin);  // Now uses dungeon->renderMapMode
            /* Optionally, update the message or status lines here. */
        }

        /* Clean up ncurses and dungeon resources */
        endCurses();
        freeDungeon(dungeon);
        free(dungeon);
    } else {
        /* Process command-line arguments (old flags) */
        int save_flag = 0, load_flag = 0, test_mode = 0;
        int num_monsters = 10;
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
        
        /* Call the appropriate mode based on flags */
        if (save_flag || load_flag) {
            return runLoadAndSave(argc, argv);
        } else if (test_mode) {
            return runTestMode();
        } else {
            return runSimulation(num_monsters);
        }
    }
    return 0;
}
