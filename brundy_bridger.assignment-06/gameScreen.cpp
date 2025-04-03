#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "dungeon.hpp"
#include "ui.hpp"
#include "fileHandle.hpp"
#include "simulate.hpp"
#include <time.h>

int runLoadAndSave(int argc, char *argv[]);
int runTestMode(void);
int runSimulation(void);
void updateFogOfWar(Dungeon* dungeon);

void displayMonsterList(Dungeon* dungeon, WINDOW* gameWin);

void gotoMode(Dungeon* dungeon, int key, WINDOW* gameWin){
    int newX = dungeon->tp.x;
    int newY = dungeon->tp.y;

    switch (key) {
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
        case KEY_B2:
        case ' ':
        case '.':
        case '5':
            renderMessageLine("Resting in goto mode...");
            break;
        case 'r':
            renderMessageLine("Teleporting to random location.");
            dungeon->renderMapMode = 4;
            dungeon->gotoMode = 0;
            dungeon->mc.x = rand() % widthScreen;
            dungeon->mc.y = rand() % heightScreen;
            dungeon->monsterNeedUpdate = 1; // Trigger monster update after teleporting
            updateFogOfWar(dungeon); // Update fog of war at new location
            break;
    
        case 'g':
            renderMessageLine("Teleporting to targeted location.");
            dungeon->renderMapMode = 4;
            dungeon->gotoMode = 0;
            if(dungeon->tiles[dungeon->tp.y][dungeon->tp.x].hardness == 255){
                renderMessageLine("You can't teleport into immutable rock.");
            } else{
                dungeon->mc.x = dungeon->tp.x;
                dungeon->mc.y = dungeon->tp.y;
                dungeon->monsterNeedUpdate = 1; // Trigger monster update after teleporting
                updateFogOfWar(dungeon); // Update fog of war at new location
            }
            
            break;
        case 'Q':
            renderMessageLine("Quitting game...");
            break;
            
        default:
            renderMessageLine("Unrecognized command for goto mode.");
            break;
    }
    
    if (newX != dungeon->tp.x || newY != dungeon->tp.y) {
        if (newX > 0 && newX < widthScreen - 1 && newY > 0 && newY < heightScreen - 1) {
            dungeon->tp.x = newX;
            dungeon->tp.y = newY;
        }
    }
}

void updateDungeon(Dungeon* dungeon, int key, WINDOW* gameWin) {
    if (dungeon->gotoMode == 1) {
        gotoMode(dungeon, key, gameWin);
        return;
    }
    
    int newX = dungeon->mc.x;
    int newY = dungeon->mc.y;
        
    switch (key) {
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
            
        case KEY_B2:
        case ' ':
        case '.':
        case '5':
            renderMessageLine("Resting...");
            dungeon->monsterNeedUpdate = 1;
            break;
        case '>': {
            int onStairs = 0;
            for (int i = 0; i < dungeon->numDownStairs; i++) {
                if (dungeon->mc.x == dungeon->downStairs[i].x && 
                    dungeon->mc.y == dungeon->downStairs[i].y) {
                    renderMessageLine("You descend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode; // update view mode
                    
                    // new dungeon level
                    freeDungeon(dungeon);
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode;
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    
                    onStairs = 1;
                    break;
                }
            }
            if (!onStairs) {
                renderMessageLine("There are no down stairs (>) here.");
            }
            return; 
        }
        case '<': {
            int onStairs = 0;
            for (int i = 0; i < dungeon->numUpStairs; i++) {
                if (dungeon->mc.x == dungeon->upStairs[i].x && 
                    dungeon->mc.y == dungeon->upStairs[i].y) {
                    renderMessageLine("You ascend the staircase to a new level.");
                    
                    int currentMode = dungeon->renderMapMode;
                    
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
            displayMonsterList(dungeon, gameWin);
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
        case 'f':
            renderMessageLine("Toggling fog of war...");
            dungeon->renderMapMode = (dungeon->renderMapMode == 4) ? 0 : 4;
            break;
            
        //Item commands
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
        case 'g':
            renderMessageLine("Goto mode press 'g' to go to '*' or press 'r' to go to a random location.");
            dungeon->renderMapMode = 5;
            dungeon->gotoMode = 1;
            dungeon->tp.x = dungeon->mc.x;
            dungeon->tp.y = dungeon->mc.y;
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
    
    if (newX != dungeon->mc.x || newY != dungeon->mc.y) {
        if (newX > 0 && newX < widthScreen - 1 && newY > 0 && newY < heightScreen - 1) {
            if (dungeon->tiles[newY][newX].hardness == 0) {
                dungeon->mc.x = newX;
                dungeon->mc.y = newY;
                

                dungeon->monsterNeedUpdate = 1;

                for (int i = 0; i < dungeon->numMonsters; i++) {
                    if (dungeon->monsters[i]->cord.x == newX && 
                        dungeon->monsters[i]->cord.y == newY) {
                        renderMessageLine("You attack the monster!");
                        break;
                    }
                }

                updateFogOfWar(dungeon);
            } else {
                renderMessageLine("You can't move there, silly adventurer.");
            }
        }
    }
}


void updateFogOfWar(Dungeon* dungeon){
    // First time initialization - set all to rock
    static int initialized = 0;
    if (!initialized) {
        for(int y = 0; y < heightScreen; y++){
            for(int x = 0; x < widthScreen; x++){
                dungeon->fogOfWar[y][x] = createTile(ROCK);
                dungeon->fogOfWar[y][x].hardness = 1;
            }
        }
        initialized = 1;
    }
    
    // Update visible area around player - 5×5 region centered on PC
    for(int y = 0; y < heightScreen; y++){
        for(int x = 0; x < widthScreen; x++){
            // Check if within 2 units in each direction from PC (creating 5×5 region)
            int dx = abs(x - dungeon->mc.x);
            int dy = abs(y - dungeon->mc.y);
            if(dx <= 2 && dy <= 2){
                // Update visible area with current terrain
                dungeon->fogOfWar[y][x] = dungeon->tiles[y][x];
            }
        }
    }
}

void displayMonsterList(Dungeon* dungeon, WINDOW* gameWin) {
    int i;
    int screenHeight, screenWidth;
    int currentLine = 0;
    int scrollOffset = 0;
    int maxDisplay;
    int key;
    char relativePos[50];
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    maxDisplay = screenHeight - 4; 
    
    WINDOW* monsterPad = newpad(dungeon->numMonsters + 5, screenWidth - 2);
    
    // Display loop
    while (1) {
        wclear(monsterPad);
        
        // Header
        wprintw(monsterPad, "Monster List (ESC to exit, arrow keys to scroll)\n");
        wprintw(monsterPad, "----------------------------------------------\n");
        
        // Display each monster
        for (i = 0; i < dungeon->numMonsters; i++) {
            int relX = dungeon->monsters[i]->cord.x - dungeon->mc.x;
            int relY = dungeon->monsters[i]->cord.y - dungeon->mc.y;
            
            if (relY < 0) {
                if (relX < 0) {
                    sprintf(relativePos, "%d north and %d west", abs(relY), abs(relX));
                } else if (relX > 0) {
                    sprintf(relativePos, "%d north and %d east", abs(relY), relX);
                } else {
                    sprintf(relativePos, "%d north", abs(relY));
                }
            } else if (relY > 0) {
                if (relX < 0) {
                    sprintf(relativePos, "%d south and %d west", relY, abs(relX));
                } else if (relX > 0) {
                    sprintf(relativePos, "%d south and %d east", relY, relX);
                } else {
                    sprintf(relativePos, "%d south", relY);
                }
            } else {
                if (relX < 0) {
                    sprintf(relativePos, "%d west", abs(relX));
                } else if (relX > 0) {
                    sprintf(relativePos, "%d east", relX);
                } else {
                    sprintf(relativePos, "same position");
                }
            }
            
            wprintw(monsterPad, "%c, %s\n", dungeon->monsters[i]->texture, relativePos);
        }
        
        prefresh(monsterPad, scrollOffset, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        // Get input
        key = getch();
        
        // Process input
        if (key == 27) { // ESC key
            break;
        } else if (key == KEY_UP) {
            if (scrollOffset > 0) {
                scrollOffset--;
            }
        } else if (key == KEY_DOWN) {
            if (scrollOffset < dungeon->numMonsters - maxDisplay && 
                dungeon->numMonsters > maxDisplay) {
                scrollOffset++;
            }
        }
    }
    
    // Clean up
    delwin(monsterPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

int runTestMode(void) {
    return 0;
}

int runSimulation(void) {
    return 0;
}

int runLoadAndSave(int argc, char *argv[]) {
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        Dungeon* dungeon = (Dungeon*) malloc(sizeof(Dungeon));
        if (dungeon == NULL) {
            fprintf(stderr, "Failed to allocate memory for dungeon\n");
            return EXIT_FAILURE;
        }
        
        initDungeon(dungeon);
        srand(time(NULL));
        dungeon->renderMapMode = 0;
        dungeon->monsterNeedUpdate = 0;
        setTiles(dungeon);
        setRooms(dungeon);
        setHalls(dungeon);
        populateDungeon(dungeon);
        
        spawnMonsters(dungeon, 10);
        
        // Initialize fog of war with the player's starting view
        updateFogOfWar(dungeon);
        
        initCurses();
        renderMessageLine("Welcome to the dungeon!");
        WINDOW* gameWin = initGameMapWindow();
        renderCurses(dungeon, gameWin);
        renderStatus("status line 1", "status line 2");
        
        int ch;
        while ((ch = getch()) != 'Q') {
            updateDungeon(dungeon, ch, gameWin);
            
            // Always render the game after updating the dungeon state
            renderCurses(dungeon, gameWin);
            
            // Handle monster movement if needed
            if (dungeon->monsterNeedUpdate) {
                renderStatus("Monsters moving...", "");
                if (simulateMonsters(dungeon)) {
                    renderStatus("Monsters moved!", "");
                } else {
                    renderStatus("No monsters moved", "");
                }
                dungeon->monsterNeedUpdate = 0;
                renderCurses(dungeon, gameWin);
            }
        }
        
        endCurses();
        freeDungeon(dungeon);
        free(dungeon);
    } else {
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
