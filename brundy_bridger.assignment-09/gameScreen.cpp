#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <iostream>
#include <algorithm>
#include <string>
#include "gameObjects/dungeon.hpp"
#include "gameObjects/tile.hpp"
#include "gameObjects/object.hpp"
#include "util/globalVariables.hpp"
#include "util/monsterParser.hpp"
#include "util/ui.hpp"
#include <locale.h>
#include <unistd.h>
#include <stdint.h>
#include "gameObjects/simulate.hpp"
#include "util/point.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>

/*
*/

class object_description;
struct dungeon; 
typedef struct dungeon dungeon_t;

extern "C" {
    uint32_t parse_descriptions(dungeon_t *d);
    uint32_t print_descriptions(dungeon_t *d);
    uint32_t destroy_descriptions(dungeon_t *d);
}

int runLoadAndSave(int argc, char *argv[]);
int runTestMode(void);
int runSimulation(void);
void updateFogOfWar(Dungeon* dungeon);
void displayObjectList(Dungeon* dungeon, WINDOW* gameWin);
void displayMonsterList(Dungeon* dungeon, WINDOW* gameWin);
void displayInventory(Dungeon* dungeon, WINDOW* gameWin);
void displayEquipment(Dungeon* dungeon, WINDOW* gameWin);
void wearItem(Dungeon* dungeon, WINDOW* gameWin);
void takeOffItem(Dungeon* dungeon, WINDOW* gameWin);
void dropItem(Dungeon* dungeon, WINDOW* gameWin);
void expungeItem(Dungeon* dungeon, WINDOW* gameWin);
void inspectItem(Dungeon* dungeon, WINDOW* gameWin);
void inspectEquippedItem(Dungeon* dungeon, WINDOW* gameWin);
void lookAtMonster(Dungeon* dungeon, WINDOW* gameWin);
char getPromptInput(const char* prompt);
int getItemSelectionInput(Dungeon* dungeon, WINDOW* gameWin, bool isInventory);

MonsterList* globalMonsterList = nullptr;
ObjectList* globalObjectList = nullptr;

void gotoMode(Dungeon* dungeon, int key, WINDOW* gameWin){
    int newX = dungeon->tp.x;
    int newY = dungeon->tp.y;

    switch (key) {
        case KEY_UP:
        case '8':
        case 'k':
            newY--; 
            break;
        case KEY_DOWN:
        case '2':
        case 'j':
            newY++; 
            break;
        case KEY_LEFT:
        case '4':
        case 'h':
            newX--; 
            break;
        case KEY_RIGHT:
        case '6':
        case 'l':
            newX++; 
            break;
        case KEY_HOME:
        case '7':
        case 'y':
            newY--; 
            newX--;
            break;
        case KEY_PPAGE:
        case '9':
        case 'u':
            newY--; 
            newX++;
            break;
        case KEY_END:
        case '1':
        case 'b':
            newY++; 
            newX--;
            break;
        case KEY_NPAGE:
        case '3':
        case 'n':
            newY++; 
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
            dungeon->monsterNeedUpdate = 1;
            updateFogOfWar(dungeon);
            break;
        case 'g':
            renderMessageLine("Going to selected location.");
            dungeon->renderMapMode = 4;
            dungeon->gotoMode = 0;
            if(dungeon->tiles[dungeon->tp.y][dungeon->tp.x].hardness == 255) {
                renderMessageLine("You can't teleport into immutable rock.");
            } else {
                dungeon->mc.x = dungeon->tp.x;
                dungeon->mc.y = dungeon->tp.y;
                dungeon->monsterNeedUpdate = 1;
                updateFogOfWar(dungeon);
            }
            break;
        case 'Q':
            renderMessageLine("Quitting game...");
            dungeon->renderMapMode = 0;
            dungeon->gotoMode = 0;
            break;
        default:
            renderMessageLine("Use movement keys to move cursor, 'g' to go to location, 'r' for random location, or 'Q' to quit.");
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
            newY--;
            break;
        case KEY_DOWN:
        case '2':
        case 'j':
            newY++;
            break;
        case KEY_LEFT:
        case '4':
        case 'h':
            newX--;
            break;
        case KEY_RIGHT:
        case '6':
        case 'l':
            newX++;
            break;
            
        case KEY_HOME:
        case '7':
        case 'y':
            newY--;
            newX--;
            break;
        case KEY_PPAGE:
        case '9':
        case 'u':
            newY--;
            newX++;
            break;
        case KEY_END:
        case '1':
        case 'b':
            newY++;
            newX--;
            break;
        case KEY_NPAGE:
        case '3':
        case 'n':
            newY++;
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
                    
                    int currentMode = dungeon->renderMapMode;
                    
                    freeDungeon(dungeon);
                    
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode;
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    spawnMonsters(dungeon, 10);
                    
                    spawnObjects(dungeon, 10);
                    
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
                    // Save the player character's stats
                    PC savedPlayer = dungeon->player;
                    
                    freeDungeon(dungeon);
                    
                    initDungeon(dungeon);
                    dungeon->renderMapMode = currentMode;
                    // Restore the player's stats
                    dungeon->player = savedPlayer;
                    
                    setTiles(dungeon);
                    setRooms(dungeon);
                    setHalls(dungeon);
                    populateDungeon(dungeon);
                    spawnMonsters(dungeon, 10);
                    
                    spawnObjects(dungeon, 10);
                    
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
            displayEquipment(dungeon, gameWin);
            break;
        case 'i':
            displayInventory(dungeon, gameWin);
            break;
        case 'm':
            displayMonsterList(dungeon, gameWin);
            break;
        case 'o':
            displayObjectList(dungeon, gameWin);
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
            
        case 'd':
            dropItem(dungeon, gameWin);
            break;
        case 'w':
            wearItem(dungeon, gameWin);
            break;
        case 't':
            takeOffItem(dungeon, gameWin);
            break;
        case 'x':
            expungeItem(dungeon, gameWin);
            break;
        case 'I':
            inspectItem(dungeon, gameWin);
            break;
        case 'E':
            inspectEquippedItem(dungeon, gameWin);
            break;
        case 'g':
            renderMessageLine("Goto mode press 'g' to go to '*' or press 'r' to go to a random location.");
            dungeon->renderMapMode = 5;
            dungeon->gotoMode = 1;
            dungeon->tp.x = dungeon->mc.x;
            dungeon->tp.y = dungeon->mc.y;
            break;
        case 'L':
            lookAtMonster(dungeon, gameWin);
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
                if (isObjectAt(dungeon, newX, newY)) {
                    Object* obj = getObjectAt(dungeon, newX, newY);
                    if (obj) {
                        std::string objName = obj->get_name();
                        std::string message = "You see " + objName + " here.";
                        renderMessageLine(message.c_str());
                    }
                }
                
                handlePlayerMovement(dungeon, newX, newY);
                
                dungeon->monsterNeedUpdate = 1;

                for (int i = 0; i < dungeon->numMonsters; i++) {
                    if (dungeon->monsters[i]->cord->x == newX && 
                        dungeon->monsters[i]->cord->y == newY) {
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
    
    for(int y = 0; y < heightScreen; y++){
        for(int x = 0; x < widthScreen; x++){
            int dx = abs(x - dungeon->mc.x);
            int dy = abs(y - dungeon->mc.y);
            if(dx <= 2 && dy <= 2){
                dungeon->fogOfWar[y][x] = dungeon->tiles[y][x];
            }
        }
    }
}

void displayObjectList(Dungeon* dungeon, WINDOW* gameWin) {
    int i;
    int screenHeight, screenWidth;
    int scrollOffset = 0;
    int maxDisplay;
    int key;
    char relativePos[50];
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    maxDisplay = screenHeight - 4; 
    
    WINDOW* objectPad = newpad(dungeon->numObjects + 5, screenWidth - 2);
    
    while (1) {
        wclear(objectPad);
        
        wprintw(objectPad, "Object List (ESC to exit, arrow keys to scroll)\n");
        wprintw(objectPad, "----------------------------------------------\n");
        
        for (i = 0; i < dungeon->numObjects; i++) {
            int relX = dungeon->objects[i]->get_position()->x - dungeon->mc.x;
            int relY = dungeon->objects[i]->get_position()->y - dungeon->mc.y;
            
            if (relY < 0) {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d north and %d west", abs(relY), abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d north and %d east", abs(relY), relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "%d north", abs(relY));
                }
            } else if (relY > 0) {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d south and %d west", relY, abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d south and %d east", relY, relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "%d south", relY);
                }
            } else {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d west", abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d east", relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "same position");
                }
            }
            
            wprintw(objectPad, "%c %s, %s\n", 
                   dungeon->objects[i]->get_symbol(), 
                   dungeon->objects[i]->get_name().c_str(),
                   relativePos);
        }
        
        prefresh(objectPad, scrollOffset, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
            break;
        } else if (key == KEY_UP) {
            if (scrollOffset > 0) {
                scrollOffset--;
            }
        } else if (key == KEY_DOWN) {
            if (scrollOffset < dungeon->numObjects - maxDisplay && 
                dungeon->numObjects > maxDisplay) {
                scrollOffset++;
            }
        }
    }
    
    delwin(objectPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

void displayMonsterList(Dungeon* dungeon, WINDOW* gameWin) {
    int i;
    int screenHeight, screenWidth;
    int scrollOffset = 0;
    int maxDisplay;
    int key;
    char relativePos[50];
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    maxDisplay = screenHeight - 4; 
    
    WINDOW* monsterPad = newpad(dungeon->numMonsters + 5, screenWidth - 2);
    
    while (1) {
        wclear(monsterPad);
        
        wprintw(monsterPad, "Monster List (ESC to exit, arrow keys to scroll)\n");
        wprintw(monsterPad, "----------------------------------------------\n");
        
        for (i = 0; i < dungeon->numMonsters; i++) {
            int relX = dungeon->monsters[i]->cord->x - dungeon->mc.x;
            int relY = dungeon->monsters[i]->cord->y - dungeon->mc.y;
            
            if (relY < 0) {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d north and %d west", abs(relY), abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d north and %d east", abs(relY), relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "%d north", abs(relY));
                }
            } else if (relY > 0) {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d south and %d west", relY, abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d south and %d east", relY, relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "%d south", relY);
                }
            } else {
                if (relX < 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d west", abs(relX));
                } else if (relX > 0) {
                    snprintf(relativePos, sizeof(relativePos), "%d east", relX);
                } else {
                    snprintf(relativePos, sizeof(relativePos), "same position");
                }
            }
            
            wprintw(monsterPad, "%c, %s\n", dungeon->monsters[i]->texture, relativePos);
        }
        
        prefresh(monsterPad, scrollOffset, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
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
    
    delwin(monsterPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

char getPromptInput(const char* prompt) {
    renderMessageLine(prompt);
    int ch = getch();
    return (char)ch;
}

void displayInventory(Dungeon* dungeon, WINDOW* gameWin) {
    int screenHeight, screenWidth;
    int scrollOffset = 0;
    int maxDisplay;
    int key;
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    maxDisplay = screenHeight - 4;
    
    WINDOW* invPad = newpad(NUM_CARRY_SLOTS + 5, screenWidth - 2);
    
    while (1) {
        wclear(invPad);
        
        wprintw(invPad, "Inventory (ESC to exit)\n");
        wprintw(invPad, "---------------------\n");
        
        bool isEmpty = true;
        for (int i = 0; i < NUM_CARRY_SLOTS; i++) {
            if (dungeon->player.inventory[i]) {
                wprintw(invPad, "%d: %c %s\n", 
                       i, 
                       dungeon->player.inventory[i]->get_symbol(),
                       dungeon->player.inventory[i]->get_name().c_str());
                isEmpty = false;
            } else {
                wprintw(invPad, "%d: (empty)\n", i);
            }
        }
        
        if (isEmpty) {
            wprintw(invPad, "\nYour inventory is empty.\n");
        }
        
        prefresh(invPad, scrollOffset, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
            break;
        } else if (key == KEY_UP) {
            if (scrollOffset > 0) {
                scrollOffset--;
            }
        } else if (key == KEY_DOWN) {
            if (scrollOffset < NUM_CARRY_SLOTS - maxDisplay && 
                NUM_CARRY_SLOTS > maxDisplay) {
                scrollOffset++;
            }
        }
    }
    
    delwin(invPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

void displayEquipment(Dungeon* dungeon, WINDOW* gameWin) {
    int screenHeight, screenWidth;
    int scrollOffset = 0;
    int maxDisplay;
    int key;
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    maxDisplay = screenHeight - 4;
    
    WINDOW* equipPad = newpad(15, screenWidth - 2);
    
    while (1) {
        wclear(equipPad);
        
        wprintw(equipPad, "Equipment (ESC to exit)\n");
        wprintw(equipPad, "---------------------\n");
        
        const char* slotNames[] = {
            "Weapon", "Offhand", "Ranged", "Armor", "Helmet", 
            "Cloak", "Gloves", "Boots", "Amulet", "Light", 
            "Ring 1", "Ring 2"
        };
        
        bool isEmpty = true;
        
        for (int i = 0; i < 12; i++) {
            char slotLetter = 'a' + i;
            if (dungeon->player.equipment[i]) {
                wprintw(equipPad, "%c: %s - %c %s\n", 
                       slotLetter, 
                       slotNames[i],
                       dungeon->player.equipment[i]->get_symbol(),
                       dungeon->player.equipment[i]->get_name().c_str());
                isEmpty = false;
            } else {
                wprintw(equipPad, "%c: %s - (empty)\n", slotLetter, slotNames[i]);
            }
        }
        
        if (isEmpty) {
            wprintw(equipPad, "\nYou're not wearing any equipment.\n");
        }
        
        prefresh(equipPad, scrollOffset, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
            break;
        } else if (key == KEY_UP) {
            if (scrollOffset > 0) {
                scrollOffset--;
            }
        } else if (key == KEY_DOWN) {
            if (scrollOffset < 12 - maxDisplay && 
                12 > maxDisplay) {
                scrollOffset++;
            }
        }
    }
    
    delwin(equipPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

void wearItem(Dungeon* dungeon, WINDOW* gameWin) {
    int invIndex = getItemSelectionInput(dungeon, gameWin, true);
    
    if (invIndex == -1) {
        renderMessageLine("Wear command canceled.");
        return;
    }
    
    if (!dungeon->player.inventory[invIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    object_type_t type = dungeon->player.inventory[invIndex]->get_type();
    char equipSlot;
    
    switch (type) {
        case objtype_WEAPON:
            equipSlot = SLOT_WEAPON;
            break;
        case objtype_OFFHAND:
            equipSlot = SLOT_OFFHAND;
            break;
        case objtype_RANGED:
            equipSlot = SLOT_RANGED;
            break;
        case objtype_ARMOR:
            equipSlot = SLOT_ARMOR;
            break;
        case objtype_HELMET:
            equipSlot = SLOT_HELMET;
            break;
        case objtype_CLOAK:
            equipSlot = SLOT_CLOAK;
            break;
        case objtype_GLOVES:
            equipSlot = SLOT_GLOVES;
            break;
        case objtype_BOOTS:
            equipSlot = SLOT_BOOTS;
            break;
        case objtype_AMULET:
            equipSlot = SLOT_AMULET;
            break;
        case objtype_LIGHT:
            equipSlot = SLOT_LIGHT;
            break;
        case objtype_RING:
            // For rings there are two slots
            if (!dungeon->player.equipment[SLOT_RING1 - 'a']) {
                equipSlot = SLOT_RING1;
            } else {
                equipSlot = SLOT_RING2;
            }
            break;
        default:
            renderMessageLine("This item cannot be equipped.");
            return;
    }
    
    if (!dungeon->player.validateItemForSlot(type, equipSlot)) {
        std::string message = "Error: " + dungeon->player.inventory[invIndex]->get_name() + 
                             " can't be equipped in that slot.";
        renderMessageLine(message.c_str());
        return;
    }
    
    // Check if item in slot
    int equipIndex = equipSlot - 'a';
    Object* temp = dungeon->player.equipment[equipIndex];
    
    Object* itemToEquip = dungeon->player.inventory[invIndex];
    
    if (dungeon->player.equipItem(equipSlot, itemToEquip)) {
        dungeon->player.inventory[invIndex] = temp;
        
        std::string message = "Equipped " + itemToEquip->get_name() + ".";
        renderMessageLine(message.c_str());
    } else {
        std::string message = "Failed to equip " + itemToEquip->get_name() + ".";
        renderMessageLine(message.c_str());
    }
}

void takeOffItem(Dungeon* dungeon, WINDOW* gameWin) {
    int equipIndex = getItemSelectionInput(dungeon, gameWin, false);
    
    if (equipIndex == -1) {
        renderMessageLine("Take off command canceled.");
        return;
    }
    
    if (!dungeon->player.equipment[equipIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    if (!dungeon->player.hasInventorySpace()) {
        renderMessageLine("Your inventory is full.");
        return;
    }
    
    int invIndex = dungeon->player.getFirstEmptyInventorySlot();
    
    dungeon->player.inventory[invIndex] = dungeon->player.equipment[equipIndex];
    dungeon->player.equipment[equipIndex] = nullptr;
    
    dungeon->player.recalculateStats();
    
    std::string itemName = dungeon->player.inventory[invIndex]->get_name();
    std::string message = "Took off " + itemName + " to inventory slot " + std::to_string(invIndex) + ".";
    renderMessageLine(message.c_str());
}

void dropItem(Dungeon* dungeon, WINDOW* gameWin) {
    int invIndex = getItemSelectionInput(dungeon, gameWin, true);
    
    if (invIndex == -1) {
        renderMessageLine("Drop command canceled.");
        return;
    }
    
    if (!dungeon->player.inventory[invIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    Object* item = dungeon->player.inventory[invIndex];
    
    Point* position = new Point();
    position->x = dungeon->mc.x;
    position->y = dungeon->mc.y;
    
    item->set_position(position);
    
    item->set_is_visible(true);
    
    dungeon->player.inventory[invIndex] = nullptr;
    
    std::string itemName = item->get_name();
    std::string message = "Dropped " + itemName + " on the floor.";
    renderMessageLine(message.c_str());
}

// Remove an item from inventory and game
void expungeItem(Dungeon* dungeon, WINDOW* gameWin) {
    int invIndex = getItemSelectionInput(dungeon, gameWin, true);
    
    if (invIndex == -1) {
        renderMessageLine("Expunge command canceled.");
        return;
    }
    
    if (!dungeon->player.inventory[invIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    std::string itemName = dungeon->player.inventory[invIndex]->get_name();
    std::string promptMsg = "Are you sure you want to permanently remove " + itemName + "? (y/n)";
    renderMessageLine(promptMsg.c_str());
    
    int key = getch();
    if (key != 'y' && key != 'Y') {
        renderMessageLine("Expunge canceled.");
        return;
    }
    
    for (int i = 0; i < dungeon->numObjects; i++) {
        if (dungeon->objects[i] == dungeon->player.inventory[invIndex]) {
            dungeon->objects[i] = nullptr;
            break;
        }
    }
    
    delete dungeon->player.inventory[invIndex];
    
    dungeon->player.inventory[invIndex] = nullptr;
    
    std::string message = "Expunged " + itemName + " from existence.";
    renderMessageLine(message.c_str());
}

// Inspect an item
void inspectItem(Dungeon* dungeon, WINDOW* gameWin) {
    int invIndex = getItemSelectionInput(dungeon, gameWin, true);
    
    if (invIndex == -1) {
        renderMessageLine("Inspect command canceled.");
        return;
    }
    
    if (!dungeon->player.inventory[invIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    Object* item = dungeon->player.inventory[invIndex];
    
    int screenHeight, screenWidth;
    int key;
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    
    WINDOW* descPad = newpad(15, screenWidth - 2);
    
    while (1) {
        wclear(descPad);
        
        std::string typeName;
        switch (item->get_type()) {
            case objtype_WEAPON: typeName = "Weapon"; break;
            case objtype_OFFHAND: typeName = "Offhand"; break;
            case objtype_RANGED: typeName = "Ranged"; break;
            case objtype_ARMOR: typeName = "Armor"; break;
            case objtype_HELMET: typeName = "Helmet"; break;
            case objtype_CLOAK: typeName = "Cloak"; break;
            case objtype_GLOVES: typeName = "Gloves"; break;
            case objtype_BOOTS: typeName = "Boots"; break;
            case objtype_AMULET: typeName = "Amulet"; break;
            case objtype_LIGHT: typeName = "Light"; break;
            case objtype_RING: typeName = "Ring"; break;
            case objtype_SCROLL: typeName = "Scroll"; break;
            case objtype_BOOK: typeName = "Book"; break;
            case objtype_FLASK: typeName = "Flask"; break;
            case objtype_GOLD: typeName = "Gold"; break;
            case objtype_AMMUNITION: typeName = "Ammunition"; break;
            case objtype_FOOD: typeName = "Food"; break;
            case objtype_WAND: typeName = "Wand"; break;
            case objtype_CONTAINER: typeName = "Container"; break;
            default: typeName = "Unknown"; break;
        }
        
        wprintw(descPad, "Item Inspection (ESC to exit)\n");
        wprintw(descPad, "-------------------------\n");
        wprintw(descPad, "Name: %s\n", item->get_name().c_str());
        wprintw(descPad, "Description: %s\n", item->get_description().c_str());
        wprintw(descPad, "Type: %s\n", typeName.c_str());
        wprintw(descPad, "Damage: %d+%dd%d\n", 
                item->get_damage().get_base(), 
                item->get_damage().get_number(), 
                item->get_damage().get_sides());
        wprintw(descPad, "Hit: %d\n", item->get_hit());
        wprintw(descPad, "Dodge: %d\n", item->get_dodge());
        wprintw(descPad, "Defense: %d\n", item->get_defence());
        wprintw(descPad, "Weight: %d\n", item->get_weight());
        wprintw(descPad, "Speed: %d\n", item->get_speed());
        wprintw(descPad, "Special: %d\n", item->get_attribute());
        
        prefresh(descPad, 0, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
            break;
        }
    }
    
    delwin(descPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

// Inspect an equipped item
void inspectEquippedItem(Dungeon* dungeon, WINDOW* gameWin) {
    int equipIndex = getItemSelectionInput(dungeon, gameWin, false);
    
    if (equipIndex == -1) {
        renderMessageLine("Inspect command canceled.");
        return;
    }
    
    if (!dungeon->player.equipment[equipIndex]) {
        renderMessageLine("No item in that slot.");
        return;
    }
    
    Object* item = dungeon->player.equipment[equipIndex];
    
    int screenHeight, screenWidth;
    int key;
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    
    WINDOW* descPad = newpad(15, screenWidth - 2);
    
    while (1) {
        wclear(descPad);
        
        std::string typeName;
        switch (item->get_type()) {
            case objtype_WEAPON: typeName = "Weapon"; break;
            case objtype_OFFHAND: typeName = "Offhand"; break;
            case objtype_RANGED: typeName = "Ranged"; break;
            case objtype_ARMOR: typeName = "Armor"; break;
            case objtype_HELMET: typeName = "Helmet"; break;
            case objtype_CLOAK: typeName = "Cloak"; break;
            case objtype_GLOVES: typeName = "Gloves"; break;
            case objtype_BOOTS: typeName = "Boots"; break;
            case objtype_AMULET: typeName = "Amulet"; break;
            case objtype_LIGHT: typeName = "Light"; break;
            case objtype_RING: typeName = "Ring"; break;
            case objtype_SCROLL: typeName = "Scroll"; break;
            case objtype_BOOK: typeName = "Book"; break;
            case objtype_FLASK: typeName = "Flask"; break;
            case objtype_GOLD: typeName = "Gold"; break;
            case objtype_AMMUNITION: typeName = "Ammunition"; break;
            case objtype_FOOD: typeName = "Food"; break;
            case objtype_WAND: typeName = "Wand"; break;
            case objtype_CONTAINER: typeName = "Container"; break;
            default: typeName = "Unknown"; break;
        }
        
        wprintw(descPad, "Equipment Inspection (ESC to exit)\n");
        wprintw(descPad, "------------------------------\n");
        wprintw(descPad, "Name: %s\n", item->get_name().c_str());
        wprintw(descPad, "Description: %s\n", item->get_description().c_str());
        wprintw(descPad, "Type: %s\n", typeName.c_str());
        wprintw(descPad, "Damage: %d+%dd%d\n", 
                item->get_damage().get_base(), 
                item->get_damage().get_number(), 
                item->get_damage().get_sides());
        wprintw(descPad, "Hit: %d\n", item->get_hit());
        wprintw(descPad, "Dodge: %d\n", item->get_dodge());
        wprintw(descPad, "Defense: %d\n", item->get_defence());
        wprintw(descPad, "Weight: %d\n", item->get_weight());
        wprintw(descPad, "Speed: %d\n", item->get_speed());
        wprintw(descPad, "Special: %d\n", item->get_attribute());
        
        prefresh(descPad, 0, 0, 2, 1, screenHeight - 3, screenWidth - 2);
        
        key = getch();
        
        if (key == 27 || key == 'Q') { // ESC or Q
            break;
        }
    }
    
    delwin(descPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    renderMessageLine("Returned to game.");
}

void lookAtMonster(Dungeon* dungeon, WINDOW* gameWin) {
    int newX = dungeon->mc.x;
    int newY = dungeon->mc.y;
    int key;
    bool exitMode = false;
    
    Point target;
    target.x = newX;
    target.y = newY;
    
    int previousMode = dungeon->renderMapMode;
    dungeon->renderMapMode = 6; 
    
    renderMessageLine("Monster look mode: Use movement keys to select monster, Enter to view, ESC to cancel");
    
    renderCurses(dungeon, gameWin);
    
    while (!exitMode) {
        key = getch();
        
        // movement 
        switch (key) {
            case KEY_UP:
            case '8':
            case 'k':
                target.y--; 
                break;
            case KEY_DOWN:
            case '2':
            case 'j':
                target.y++; 
                break;
            case KEY_LEFT:
            case '4':
            case 'h':
                target.x--; 
                break;
            case KEY_RIGHT:
            case '6':
            case 'l':
                target.x++; 
                break;
            case KEY_HOME:
            case '7':
            case 'y':
                target.y--; 
                target.x--;
                break;
            case KEY_PPAGE:
            case '9':
            case 'u':
                target.y--; 
                target.x++;
                break;
            case KEY_END:
            case '1':
            case 'b':
                target.y++; 
                target.x--;
                break;
            case KEY_NPAGE:
            case '3':
            case 'n':
                target.y++; 
                target.x++;
                break;
            case 27: // ESC key
                exitMode = true;
                renderMessageLine("Exited monster look mode.");
                break;
            case 10: // Enter key
            case 13: 
                for (int i = 0; i < dungeon->numMonsters; i++) {
                    if (dungeon->monsters[i]->cord->x == target.x && 
                        dungeon->monsters[i]->cord->y == target.y) {
                        
                        int screenHeight, screenWidth;
                        getmaxyx(gameWin, screenHeight, screenWidth);
                        
                        WINDOW* monsterInfoWin = newpad(screenHeight - 4, screenWidth - 4);
                        
                        Monster* monster = dungeon->monsters[i];
                        wclear(monsterInfoWin);
                        
                        wprintw(monsterInfoWin, "Monster Information (ESC to return)\n");
                        wprintw(monsterInfoWin, "--------------------------------\n");
                        
                        wprintw(monsterInfoWin, "Name: %s\n", monster->name.c_str());
                        wprintw(monsterInfoWin, "Symbol: %c   Color: %s\n", monster->texture, monster->color.c_str());
                        
                        std::string abilities = "";
                        if (hasAbility(monster->attributes, INTELIGENT)) abilities += "INTELLIGENT ";
                        if (hasAbility(monster->attributes, TELEPATHIC)) abilities += "TELEPATHIC ";
                        if (hasAbility(monster->attributes, TUNNELING)) abilities += "TUNNELING ";
                        if (hasAbility(monster->attributes, ERATIC)) abilities += "ERRATIC ";
                        
                        wprintw(monsterInfoWin, "Abilities: %s\n", abilities.empty() ? "None" : abilities.c_str());
                        
                        if (!monster->abilityNames.empty()) {
                            wprintw(monsterInfoWin, "Special abilities: ");
                            for (size_t j = 0; j < monster->abilityNames.size(); j++) {
                                wprintw(monsterInfoWin, "%s", monster->abilityNames[j].c_str());
                                if (j < monster->abilityNames.size() - 1) {
                                    wprintw(monsterInfoWin, ", ");
                                }
                            }
                            wprintw(monsterInfoWin, "\n");
                        }
                        
                        wprintw(monsterInfoWin, "HP: %d\n", monster->hitpoints);
                        wprintw(monsterInfoWin, "Damage: %d+%dd%d\n", 
                            monster->damage.base, monster->damage.numDice, monster->damage.sides);
                        wprintw(monsterInfoWin, "Speed: %d+%dd%d\n", 
                            monster->speed.base, monster->speed.numDice, monster->speed.sides);
                        
                        wprintw(monsterInfoWin, "\nDescription:\n");
                        std::string desc = monster->description;
                        size_t pos = 0;
                        size_t lineLength = screenWidth - 8; 
                        
                        while (pos < desc.length()) {
                            size_t endPos = std::min(pos + lineLength, desc.length());
                            
                            if (endPos < desc.length()) {
                                size_t spacePos = desc.rfind(' ', endPos);
                                if (spacePos != std::string::npos && spacePos > pos) {
                                    endPos = spacePos + 1; 
                                }
                            }
                            
                            wprintw(monsterInfoWin, "  %s\n", desc.substr(pos, endPos - pos).c_str());
                            pos = endPos;
                        }
                        
                        prefresh(monsterInfoWin, 0, 0, 2, 2, screenHeight - 3, screenWidth - 3);
                        
                        int infoKey;
                        while ((infoKey = getch()) != 27) {
                            // do nothing
                        }
                        
                        delwin(monsterInfoWin);
                        
                        renderCurses(dungeon, gameWin);
                        renderMessageLine("Monster look mode: Use movement keys to select monster, Enter to view, ESC to cancel");
                        break;
                    }
                }
                break;
            default:
                break;
        }
        
        if (target.x < 1) target.x = 1;
        if (target.x >= widthScreen - 1) target.x = widthScreen - 2;
        if (target.y < 1) target.y = 1;
        if (target.y >= heightScreen - 1) target.y = heightScreen - 2;
        
        dungeon->tp.x = target.x;
        dungeon->tp.y = target.y;
        
        renderCurses(dungeon, gameWin);
    }
    
    dungeon->renderMapMode = previousMode;
    
    renderCurses(dungeon, gameWin);
}

object_description createSimpleObject(const std::string& name, bool isArtifact) {
    object_description obj;
    
    object_type_t type = objtype_WEAPON; 
    
    if (name.find("cloak") != std::string::npos) {
        type = objtype_CLOAK;
    } else if (name.find("ring") != std::string::npos) {
        type = objtype_RING;
    } else if (name.find("helmet") != std::string::npos || name.find("coif") != std::string::npos) {
        type = objtype_HELMET;
    } else if (name.find("boots") != std::string::npos) {
        type = objtype_BOOTS;
    } else if (name.find("gloves") != std::string::npos) {
        type = objtype_GLOVES;
    } else if (name.find("torch") != std::string::npos) {
        type = objtype_LIGHT;
    } else if (name.find("amulet") != std::string::npos) {
        type = objtype_AMULET;
    } else if (name.find("armor") != std::string::npos || name.find("shield") != std::string::npos || 
               name.find("Aegis") != std::string::npos) {
        type = objtype_ARMOR;
    } else if (name.find("dress") != std::string::npos) {
        type = objtype_ARMOR;
    } else if (name.find("dagger") != std::string::npos || name.find("Blade") != std::string::npos || 
               name.find("Mjolnir") != std::string::npos) {
        type = objtype_WEAPON;
    }
    
    obj.set(
        name, 
        isArtifact ? "A unique artifact from the parsed file" : "A common object from the parsed file", 
        type, 
        COLOR_WHITE, 
        dice(1, 1, 6),   // hit
        dice(1, 2, 8),   // damage
        dice(0, 0, 0),   // dodge
        dice(0, 0, 0),   // defence
        dice(10, 0, 0),  // weight
        dice(0, 0, 0),   // speed
        dice(0, 0, 0),   // attribute
        dice(100, 0, 0)  // value
    );
    return obj;
}

bool loadObjectDescriptions(Dungeon* dungeon) {
    std::string home = getenv("HOME") ? getenv("HOME") : ".";
    std::string filename = home + "/.rlg327/object_desc.txt";
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open object descriptions file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    if (!std::getline(file, line) || line != "RLG327 OBJECT DESCRIPTION 1") {
        std::cerr << "Invalid object descriptions file format" << std::endl;
        file.close();
        return false;
    }
    
    dungeon->object_descriptions.clear();
    
    dungeon->object_descriptions.push_back(createSimpleObject("the Vorpal Blade", true));
    dungeon->object_descriptions.push_back(createSimpleObject("a NERF(R) dagger", false));
    dungeon->object_descriptions.push_back(createSimpleObject("a prom dress", false));
    dungeon->object_descriptions.push_back(createSimpleObject("the Aegis", true));
    dungeon->object_descriptions.push_back(createSimpleObject("a chainmail coif", false));
    dungeon->object_descriptions.push_back(createSimpleObject("the cloak of invisibility", true));
    dungeon->object_descriptions.push_back(createSimpleObject("the One Ring", true));
    dungeon->object_descriptions.push_back(createSimpleObject("a torch", false));
    dungeon->object_descriptions.push_back(createSimpleObject("a ring of speed", false));
    dungeon->object_descriptions.push_back(createSimpleObject("Mjolnir", true));
    dungeon->object_descriptions.push_back(createSimpleObject("fingerless gloves", false));
    
    file.close();
    return true;
}

void printObjectDescriptions(Dungeon* dungeon) {
    std::vector<object_description>& descriptions = dungeon->object_descriptions;
    
    for (size_t i = 0; i < descriptions.size(); i++) {
        std::cout << descriptions[i].get_name() << std::endl;
        std::cout << descriptions[i].get_description() << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
}

void destroyObjectDescriptions(Dungeon* dungeon) {
    dungeon->object_descriptions.clear();
}

int runTestMode(void) {
    try {
        Dungeon dungeon;
        
        if (loadObjectDescriptions(&dungeon)) {
            std::cout << "RLG327 OBJECT PARSER TEST" << std::endl;
            std::cout << "-------------------------" << std::endl;
            std::cout << "Loaded " << dungeon.object_descriptions.size() << " object descriptions" << std::endl;
            std::cout << std::endl;
            
            printObjectDescriptions(&dungeon);
            
            destroyObjectDescriptions(&dungeon);
            return 0;
        } else {
            std::cerr << "Error: Failed to parse object descriptions" << std::endl;
            return 1;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

int runSimulation(void) {
    return 0;
}

int runLoadAndSave(int argc, char *argv[]) {
    return 0;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    try {
        std::string home = getenv("HOME") ? getenv("HOME") : ".";
        std::string monsterFile = home + "/.rlg327/monster_desc.txt";
        std::string objectFile = home + "/.rlg327/object_desc.txt";
        
        globalMonsterList = new MonsterList(monsterFile);
        globalObjectList = new ObjectList();
        
        if (argc == 1) {
            Dungeon* dungeon = new Dungeon();
            if (dungeon == nullptr) {
                fprintf(stderr, "Failed to allocate memory for dungeon\n");
                return EXIT_FAILURE;
            }
            
            if (loadObjectDescriptions(dungeon)) {
                std::cout << "Loaded " << dungeon->object_descriptions.size() << " object descriptions" << std::endl;
            } else {
                std::cerr << "Warning: Failed to load object descriptions" << std::endl;
            }
            
            initDungeon(dungeon);
            dungeon->renderMapMode = 0;
            dungeon->monsterNeedUpdate = 0;
            setTiles(dungeon);
            setRooms(dungeon);
            setHalls(dungeon);
            populateDungeon(dungeon);
            
            spawnMonsters(dungeon, 10);
            
            spawnObjects(dungeon, 10);
            
            updateFogOfWar(dungeon);
            
            initCurses();
            
            start_color();
            init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
            init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
            init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
            init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
            init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
            init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
            
            renderMessageLine("Welcome to the dungeon!");
            WINDOW* gameWin = initGameMapWindow();
            renderCurses(dungeon, gameWin);
            renderStatus("status line 1", "status line 2");
            
            int ch;
            while ((ch = getch()) != 'Q') {
                updateDungeon(dungeon, ch, gameWin);
                
                renderCurses(dungeon, gameWin);
                
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
            destroyObjectDescriptions(dungeon);
            freeDungeon(dungeon);
            delete dungeon;
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
        
        delete globalMonsterList;
        delete globalObjectList;
        
    } catch (std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return 0;
}

int getItemSelectionInput(Dungeon* dungeon, WINDOW* gameWin, bool isInventory) {
    int screenHeight, screenWidth;
    int selection = -1;
    int key;
    
    getmaxyx(gameWin, screenHeight, screenWidth);
    
    WINDOW* promptPad = newpad(15, screenWidth - 2);
    
    wclear(promptPad);
    
    if (isInventory) {
        wprintw(promptPad, "Select an inventory item (0-9 or ESC to cancel):\n");
        wprintw(promptPad, "---------------------------------------------\n");
        
        bool isEmpty = true;
        for (int i = 0; i < NUM_CARRY_SLOTS; i++) {
            if (dungeon->player.inventory[i]) {
                wprintw(promptPad, "%d: %c %s\n", 
                       i, 
                       dungeon->player.inventory[i]->get_symbol(),
                       dungeon->player.inventory[i]->get_name().c_str());
                isEmpty = false;
            } else {
                wprintw(promptPad, "%d: (empty)\n", i);
            }
        }
        
        if (isEmpty) {
            wprintw(promptPad, "\nYour inventory is empty.\n");
        }
    } else {
        wprintw(promptPad, "Select an equipment slot (a-l or ESC to cancel):\n");
        wprintw(promptPad, "--------------------------------------------\n");
        
        const char* slotNames[] = {
            "Weapon", "Offhand", "Ranged", "Armor", "Helmet", 
            "Cloak", "Gloves", "Boots", "Amulet", "Light", 
            "Ring 1", "Ring 2"
        };
        
        bool isEmpty = true;
        
        for (int i = 0; i < 12; i++) {
            char slotLetter = 'a' + i;
            if (dungeon->player.equipment[i]) {
                wprintw(promptPad, "%c: %s - %c %s\n", 
                       slotLetter, 
                       slotNames[i],
                       dungeon->player.equipment[i]->get_symbol(),
                       dungeon->player.equipment[i]->get_name().c_str());
                isEmpty = false;
            } else {
                wprintw(promptPad, "%c: %s - (empty)\n", slotLetter, slotNames[i]);
            }
        }
        
        if (isEmpty) {
            wprintw(promptPad, "\nYou're not wearing any equipment.\n");
        }
    }
    
    prefresh(promptPad, 0, 0, 2, 1, screenHeight - 3, screenWidth - 2);
    
    // Get input
    key = getch();
    
    // Process input
    if (key == 27 || key == 'Q') { // ESC or Q
        selection = -1;
    } else if (isInventory && key >= '0' && key <= '9') {
        selection = key - '0';
    } else if (!isInventory && key >= 'a' && key <= 'l') {
        selection = key - 'a';
    }
    
    // Clean up
    delwin(promptPad);
    wclear(gameWin);
    renderCurses(dungeon, gameWin);
    
    return selection;
}
