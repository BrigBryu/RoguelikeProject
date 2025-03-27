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
int runSimulation(void);  /* Removed num_monsters parameter */

void updateDungeon(Dungeon* dungeon, int key, WINDOW* gameWin) {
    int newX = dungeon->mc.x;
    int newY = dungeon->mc.y;
    
    /* Process movement and other commands */
    switch (key) {
        /* Cardinal directions */
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
            
        /* Diagonal directions */
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
            
        /* Rest commands */
        case KEY_B2:
        case ' ':
        case '.':
        case '5':
            renderMessageLine("Resting...");
            break;
            
        /* Stairs */
        case '>': {
            int onStairs = 0;
            for (int i = 0; i < dungeon->numDownStairs; i++) {
                if (dungeon->mc.x == dungeon->downStairs[i].x && 
                    dungeon->mc.y == dungeon->downStairs[i].y) {
                    renderMessageLine("You descend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode; // Save current view mode
                    
                    /* Generate new dungeon level */
                    freeDungeon(dungeon);
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode;
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    /* Note: initial monster placement now happens in simulateMonsters if needed */
                    
                    onStairs = 1;
                    break;
                }
            }
            if (!onStairs) {
                renderMessageLine("There are no down stairs (>) here.");
            }
            return;  /* Return early since level changed */
        }
        case '<': {
            int onStairs = 0;
            for (int i = 0; i < dungeon->numUpStairs; i++) {
                if (dungeon->mc.x == dungeon->upStairs[i].x && 
                    dungeon->mc.y == dungeon->upStairs[i].y) {
                    renderMessageLine("You ascend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode;
                    
                    /* Generate new dungeon level */
                    freeDungeon(dungeon);
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode;
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    spawnMonsters(dungeon, 10);
                    
                    onStairs = 1;
                    break;
                }
            }
            if (!onStairs) {
                renderMessageLine("There are no up stairs (<) here.");
            }
            return;
        }
            
        /* Display commands */
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
            
        /* Item commands */
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
            
        /* Other commands */
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
            break;
            
        default:
            renderMessageLine("Unrecognized command.");
            break;
    }
    
    /* Check if the new position is different than the old one */
    if (newX != dungeon->mc.x || newY != dungeon->mc.y) {
        /* Check bounds */
        if (newX > 0 && newX < widthScreen - 1 && newY > 0 && newY < heightScreen - 1) {
            if (dungeon->tiles[newY][newX].hardness == 0) {
                dungeon->mc.x = newX;
                dungeon->mc.y = newY;
                
                /* Set flag to update monster movement */
                dungeon->monsterNeedUpdate = 1;
                
                /* Check for collision with monsters */
                for (int i = 0; i < dungeon->numMonsters; i++) {
                    if (dungeon->monsters[i]->cord.x == newX && 
                        dungeon->monsters[i]->cord.y == newY) {
                        renderMessageLine("You attack the monster!");
                        /* Combat logic here */
                        break;
                    }
                }
            } else {
                renderMessageLine("You can't move there, silly adventurer.");
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

int runSimulation(void) {
    return 0;
}

int main(int argc, char *argv[]) {
    /* For interactive play, we ignore command-line flags and use a default of 10 monsters. */
    if (argc == 1) {
        Dungeon* dungeon = malloc(sizeof(Dungeon));
        if (dungeon == NULL) {
            fprintf(stderr, "Failed to allocate memory for dungeon\n");
            return EXIT_FAILURE;
        }
        
        initDungeon(dungeon);
        dungeon->renderMapMode = 0;   /* Default map view */
        dungeon->monsterNeedUpdate = 0; /* Initialize monster update flag */
        setTiles(dungeon);
        setRooms(dungeon);
        setHalls(dungeon);
        populateDungeon(dungeon);
        
        spawnMonsters(dungeon, 10);
        
        initCurses();
        renderMessageLine("Welcome to the dungeon!");
        WINDOW* gameWin = initGameMapWindow();
        renderCurses(dungeon, gameWin);
        renderStatus("status line 1", "status line 2");
        
        int ch;
        int gameOver = 0;
        while ((ch = getch()) != 'Q') {
            if (!gameOver) {
            updateDungeon(dungeon, ch, gameWin);
            /* If the player moved, update monster movement */
            if (dungeon->monsterNeedUpdate) {
                if (simulateMonsters(dungeon)) {
                    renderMessageLine("You have been slain by a monster! Press Q to quit.");
                    gameOver = 1;
                }
                dungeon->monsterNeedUpdate = 0;
                }
                renderCurses(dungeon, gameWin);
            } else {
                renderMessageLine("You have been slain by a monster! Press Q to quit.");
            }
        }
        
        /* Clean up ncurses and dungeon resources */
        endCurses();
        freeDungeon(dungeon);
        free(dungeon);
    } else {
        /* For non-interactive modes, we remove the --nummon logic.
           Other command-line options could be handled as needed. */
        int save_flag = 0, load_flag = 0, test_mode = 0;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--save") == 0) {
                save_flag = 1;
            } else if (strcmp(argv[i], "--load") == 0) {
                load_flag = 1;
            } else if (strcmp(argv[i], "--test") == 0) {
                test_mode = 1;
            } else {
                fprintf(stderr, "Usage: %s [--save] [--load] [--test]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        
        if (save_flag || load_flag) {
            return runLoadAndSave(argc, argv);
        } else if (test_mode) {
            return runTestMode();
        } else {
            return runSimulation();
        }
    }
    return 0;
}