#include "ui.hpp"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

void initCurses(void) {
    initscr();              // Start curses mode.
    cbreak();               // Disable line buffering.
    noecho();               // Prevent echoing of typed characters.
    keypad(stdscr, TRUE);   // Enable special keys (e.g., arrow keys).
    curs_set(0);            // Hide the cursor.
    
    // Initialize colors if supported
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Orange/yellow for visible area
    }
    
    clear();                // Clear the screen.
    refresh();              // Refresh the screen.
}

void endCurses(void) {
    endwin();               // Restore terminal to normal mode.
}

WINDOW* initGameMapWindow(void) {
    // Create a window for the game map that covers lines 1 to 21.
    int width = COLS;
    WINDOW* gameWin = newwin(21, width, 1, 0);
    if (gameWin == NULL) {
        endwin();
        fprintf(stderr, "Error creating game map window\n");
        exit(EXIT_FAILURE);
    }
    wrefresh(gameWin);
    return gameWin;
}

void renderCurses(Dungeon* dungeon, WINDOW* gameWin) {
    int winHeight, winWidth;
    getmaxyx(gameWin, winHeight, winWidth);
    
    // Get the current map mode from the dungeon struct
    int mode = dungeon->renderMapMode;
    
    int distances[heightScreen][widthScreen];
    if (mode == 1) {
        dungeon_dijkstra_non_tunnel(dungeon, distances);
    } else if (mode == 2) {
        dungeon_dijkstra_tunnel(dungeon, distances);
    }
    
    for (int i = 0; i < winHeight && i < heightScreen; i++) {
        for (int j = 0; j < winWidth && j < widthScreen; j++) {
            int printed = 0;
            
            // show characters for player and monsters
            if (mode == 0) {
                if (dungeon->mc.x == j && dungeon->mc.y == i) {
                    mvwaddch(gameWin, i, j, '@');
                    printed = 1;
                }
                
                for (int m = 0; m < dungeon->numMonsters && !printed; m++) {
                    if (dungeon->monsters[m]->cord->x == j &&
                        dungeon->monsters[m]->cord->y == i) {
                        mvwaddch(gameWin, i, j, dungeon->monsters[m]->texture);
                        printed = 1;
                    }
                }
                
                if (!printed) {
                    mvwaddch(gameWin, i, j, getCharacter(&dungeon->tiles[i][j]));
                }
            }
            // Non-tunneling distance map
            else if (mode == 1) {
                if (dungeon->mc.x == j && dungeon->mc.y == i) {
                    mvwaddch(gameWin, i, j, '@');
                } else if (distances[i][j] == IMMUTABLE_HARDNESS) {
                    mvwaddch(gameWin, i, j, ' ');
                } else {
                    int d = distances[i][j] % 10; // Only display last digit
                    mvwaddch(gameWin, i, j, '0' + d);
                }
            }
            // Tunneling distance map
            else if (mode == 2) {
                if (dungeon->mc.x == j && dungeon->mc.y == i) {
                    mvwaddch(gameWin, i, j, '@');
                } else if (distances[i][j] == IMMUTABLE_HARDNESS) {
                    mvwaddch(gameWin, i, j, ' ');
                } else {
                    int d = distances[i][j] % 10; // Only display last digit
                    mvwaddch(gameWin, i, j, '0' + d);
                }
            }
            // Hardness map
            else if (mode == 3) {
                if (dungeon->mc.x == j && dungeon->mc.y == i) {
                    mvwaddch(gameWin, i, j, '@');
                } else {
                    int hardness = dungeon->tiles[i][j].hardness;
                    if (hardness == 0) {
                        mvwaddch(gameWin, i, j, '0');
                    } else if (hardness == 255) {
                        mvwaddch(gameWin, i, j, '#');
                    } else {
                        int h = 1 + (hardness * 8) / 254;
                        mvwaddch(gameWin, i, j, '0' + h);
                    }
                }
            }
            //Fog of war
            else if(mode == 4){
                if (dungeon->mc.x == j && dungeon->mc.y == i) {
                    mvwaddch(gameWin, i, j, '@');
                    printed = 1;
                }
                
                for (int m = 0; m < dungeon->numMonsters && !printed; m++) {
                    if (dungeon->monsters[m]->cord->x == j &&
                        dungeon->monsters[m]->cord->y == i) {
                        // Only show monsters in visible area
                        int dx = abs(j - dungeon->mc.x);
                        int dy = abs(i - dungeon->mc.y);
                        if(dx <= 2 && dy <= 2) {
                            if (has_colors()) {
                                wattron(gameWin, COLOR_PAIR(1));
                            }
                            mvwaddch(gameWin, i, j, dungeon->monsters[m]->texture);
                            if (has_colors()) {
                                wattroff(gameWin, COLOR_PAIR(1));
                            }
                            printed = 1;
                        }
                    }
                }
                
                if (!printed) {
                    int dx = abs(j - dungeon->mc.x);
                    int dy = abs(i - dungeon->mc.y);
                    if(dx <= 2 && dy <= 2) {
                        // In visible area - show current terrain in orange
                        if (has_colors()) {
                            wattron(gameWin, COLOR_PAIR(1));
                        }
                        mvwaddch(gameWin, i, j, getCharacter(&dungeon->tiles[i][j]));
                        if (has_colors()) {
                            wattroff(gameWin, COLOR_PAIR(1));
                        }
                    } else {
                        // Outside visible area - show remembered terrain
                        if (dungeon->fogOfWar[i][j].type == ROCK) {
                            mvwaddch(gameWin, i, j, ' ');
                        } else {
                            mvwaddch(gameWin, i, j, getCharacter(&dungeon->fogOfWar[i][j]));
                        }
                    }
                }
            } else if (mode == 5) {
                if (dungeon->tp.x == j && dungeon->tp.y == i) {
                    mvwaddch(gameWin, i, j, '*');
                    printed = 1;
                }
                
                for (int m = 0; m < dungeon->numMonsters && !printed; m++) {
                    if (dungeon->monsters[m]->cord->x == j &&
                        dungeon->monsters[m]->cord->y == i) {
                        // Only show monsters in visible area
                        int dx = abs(j - dungeon->mc.x);
                        int dy = abs(i - dungeon->mc.y);
                        if(dx <= 2 && dy <= 2) {
                            if (has_colors()) {
                                wattron(gameWin, COLOR_PAIR(1));
                            }
                            mvwaddch(gameWin, i, j, dungeon->monsters[m]->texture);
                            if (has_colors()) {
                                wattroff(gameWin, COLOR_PAIR(1));
                            }
                            printed = 1;
                        }
                    }
                }
                
                if (!printed) {
                    // show remembered terrain
                    if (dungeon->fogOfWar[i][j].type == ROCK) {
                        mvwaddch(gameWin, i, j, ' ');
                    } else {
                        mvwaddch(gameWin, i, j, getCharacter(&dungeon->fogOfWar[i][j]));
                    }
                }
            }
        }
    }
    
    // display changes
    wrefresh(gameWin);
}

void renderMessageLine(const char* message) {
    // Clear line 0 and print the message.
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "%s", message);
    refresh();
}

void renderStatus(const char* status1, const char* status2) {
    // Print status on lines 22 and 23.
    mvprintw(22, 0, "%s", status1);
    clrtoeol();
    mvprintw(23, 0, "%s", status2);
    clrtoeol();
    refresh();
}
